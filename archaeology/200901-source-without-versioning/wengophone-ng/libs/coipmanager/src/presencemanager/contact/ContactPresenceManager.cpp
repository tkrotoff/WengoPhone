/*
 * WengoPhone, a voice over Internet phone
 * Copyright (C) 2004-2007  Wengo
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <coipmanager/presencemanager/contact/ContactPresenceManager.h>

#include <coipmanager/CoIpManager.h>
#include <coipmanager/CoIpManagerPluginLoader.h>
#include <coipmanager/connectmanager/ConnectManager.h>
#include <coipmanager/datamanager/ContactManager.h>
#include <coipmanager/datamanager/UserProfileManager.h>
#include <coipmanager/presencemanager/contact/IContactPresenceManagerPlugin.h>

#include <util/Logger.h>
#include <util/Path.h>
#include <util/SafeConnect.h>
#include <util/SafeDelete.h>

static const std::string CONTACTPRESENCEMANAGER_PLUGIN_DIR = "coip-plugins/presencemanager/contact/";
static const std::string CONTACTPRESENCE_PLUGIN_NAME = "contactpresence";
	
ContactPresenceManager::ContactPresenceManager(CoIpManager & coIpManager)
	: ICoIpManager(coIpManager) {

	// Load Plugins
	typedef IContactPresenceManagerPlugin * (*coip_plugin_init_function)(CoIpManager &);

	std::list<void *> pluginList =
		CoIpManagerPluginLoader::loadPlugins(_coIpManager.getCoIpManagerConfig().getCoIpPluginsPath(),
			CONTACTPRESENCEMANAGER_PLUGIN_DIR, CONTACTPRESENCE_PLUGIN_NAME);

	for (std::list<void *>::const_iterator it = pluginList.begin(); it != pluginList.end(); ++it) {
		coip_plugin_init_function coip_plugin_init = (coip_plugin_init_function) (*it);
		if (coip_plugin_init) {
			IContactPresenceManagerPlugin * plugin = coip_plugin_init(_coIpManager);
			SAFE_CONNECT(plugin, SIGNAL(imContactUpdated(IMContact)), SLOT(remoteIMContactUpdatedSlot(IMContact)));
			_iCoIpManagerPluginList.push_back(plugin);
		}
	}
	////

	// Register to contact list signals.
	SAFE_CONNECT(&coIpManager.getUserProfileManager().getContactManager(), 
		SIGNAL(imContactAddedSignal(std::string, IMContact)),
		SLOT(localIMContactAddedSlot(std::string, IMContact)));
	SAFE_CONNECT(&coIpManager.getUserProfileManager().getContactManager(), 
		SIGNAL(imContactRemovedSignal(std::string, IMContact)),
		SLOT(localIMContactRemovedSlot(std::string, IMContact)));
	////

	// Register to ConnectManager signals.
	SAFE_CONNECT(&_coIpManager.getConnectManager(),
		SIGNAL(accountConnectionStateSignal(std::string, EnumConnectionState::ConnectionState)),
		SLOT(accountConnectionStateSlot(std::string, EnumConnectionState::ConnectionState)));
	////
}

ContactPresenceManager::~ContactPresenceManager() {
	uninitialize();
}

void ContactPresenceManager::initialize() {
	ICoIpManager::initialize();
}

void ContactPresenceManager::uninitialize() {
	ICoIpManager::uninitialize();
}

void ContactPresenceManager::localIMContactAddedSlot(std::string contactId, IMContact imContact) {
	for (ICoIpManagerPluginList::const_iterator it = _iCoIpManagerPluginList.begin();
		it != _iCoIpManagerPluginList.end();
		++it) {
		IContactPresenceManagerPlugin * plugin = static_cast<IContactPresenceManagerPlugin *>(*it);
		if (plugin->isProtocolSupported(imContact.getAccountType())) {
			plugin->imContactAdded(imContact);
		}
	}
}

void ContactPresenceManager::localIMContactRemovedSlot(std::string contactId, IMContact imContact) {
	for (ICoIpManagerPluginList::const_iterator it = _iCoIpManagerPluginList.begin();
		it != _iCoIpManagerPluginList.end();
		++it) {
		IContactPresenceManagerPlugin * plugin = static_cast<IContactPresenceManagerPlugin *>(*it);
		if (plugin->isProtocolSupported(imContact.getAccountType())) {
			plugin->imContactRemoved(imContact);
		}
	}
}

void ContactPresenceManager::remoteIMContactUpdatedSlot(IMContact imContact) {
	ContactList contactList = _coIpManager.getUserProfileManager().getCopyOfUserProfile().getContactList();
	Contact *contact = ContactListHelper::getCopyOfFirstContactThatOwns(contactList, imContact);
	if (contact) {
		contact->updateIMContact(imContact, EnumUpdateSection::UpdateSectionPresenceState);
		_coIpManager.getUserProfileManager().getContactManager().update(*contact, EnumUpdateSection::UpdateSectionPresenceState);
		OWSAFE_DELETE(contact);
	} else {
		LOG_ERROR("No contact associated with this IMContact in ContactList");
	}
}

void ContactPresenceManager::accountConnectionStateSlot(std::string accountId,
	EnumConnectionState::ConnectionState state) {

	// Set presence state of contact associated with accountId to Unknown.
	switch (state) {
	case EnumConnectionState::ConnectionStateDisconnected: {
		UserProfile userProfile = _coIpManager.getUserProfileManager().getCopyOfUserProfile();
		ContactList contactList = userProfile.getContactList();
		for (ContactList::const_iterator cIt = contactList.begin();
			cIt != contactList.end();
			cIt++) {
			IMContactList imContactList = IMContactListHelper::getCopyOfAssociatedIMContacts((*cIt).getIMContactList(), accountId);
			for (IMContactList::iterator imIt = imContactList.begin();
				imIt != imContactList.end();
				++imIt) {
				(*imIt).setPresenceState(EnumPresenceState::PresenceStateUnknown);
				remoteIMContactUpdatedSlot(*imIt);
			}
		}
		break;
	}
	default:
		break;
	}
	////
}

void ContactPresenceManager::authorizationRequestedSlot(IMContact imContact, const std::string & message) {
}
