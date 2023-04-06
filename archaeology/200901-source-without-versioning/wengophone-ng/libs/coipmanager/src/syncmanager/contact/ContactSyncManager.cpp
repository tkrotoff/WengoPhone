/*
 * WengoPhone, a voice over Internet phone
 * Copyright (C) 2004-2006  Wengo
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

#include <coipmanager/syncmanager/contact/ContactSyncManager.h>

#include <coipmanager/CoIpManager.h>
#include <coipmanager/CoIpManagerPluginLoader.h>
#include <coipmanager/datamanager/UserProfileManager.h>
#include <coipmanager/syncmanager/contact/IContactSyncManagerPlugin.h>
#include <coipmanager/syncmanager/contact/IIMContactSyncManagerPlugin.h>

#include <util/Logger.h>
#include <util/Path.h>
#include <util/SafeDelete.h>
#include <util/SafeConnect.h>

static const std::string CONTACTSYNCMANAGER_PLUGIN_DIR = "coip-plugins/syncmanager/contact/";
static const std::string CONTACTSYNC_PLUGIN_NAME = "contactsync";
static const std::string IMCONTACTSYNC_PLUGIN_NAME = "imcontactsync";

ContactSyncManager::ContactSyncManager(CoIpManager & coIpManager)
	: SyncManager(coIpManager) {

	SAFE_CONNECT(&_coIpManager.getUserProfileManager().getContactManager(),
		SIGNAL(contactAddedSignal(std::string)),
		SLOT(cmContactAddedSlot(std::string)));
	SAFE_CONNECT(&_coIpManager.getUserProfileManager().getContactManager(),
		SIGNAL(contactRemovedSignal(std::string)),
		SLOT(cmContactRemovedSlot(std::string)));
	SAFE_CONNECT(&_coIpManager.getUserProfileManager().getContactManager(),
		SIGNAL(contactUpdatedSignal(std::string, EnumUpdateSection::UpdateSection)),
		SLOT(cmContactUpdatedSlot(std::string, EnumUpdateSection::UpdateSection)));
	SAFE_CONNECT(&_coIpManager.getUserProfileManager().getContactManager(),
		SIGNAL(imContactAddedSignal(std::string, IMContact)),
		SLOT(cmIMContactAddedSlot(std::string, IMContact)));
	SAFE_CONNECT(&_coIpManager.getUserProfileManager().getContactManager(),
		SIGNAL(imContactRemovedSignal(std::string, IMContact)),
		SLOT(cmIMContactRemovedSlot(std::string, IMContact)));

	typedef ISyncManagerPlugin * (*coip_plugin_init_function)(CoIpManager &);

	// IContactSyncManagerPlugin plugins
	std::list<void *> pluginList =
		CoIpManagerPluginLoader::loadPlugins(_coIpManager.getCoIpManagerConfig().getCoIpPluginsPath(),
			CONTACTSYNCMANAGER_PLUGIN_DIR, CONTACTSYNC_PLUGIN_NAME);

	for (std::list<void *>::const_iterator it = pluginList.begin(); it != pluginList.end(); ++it) {
		coip_plugin_init_function coip_plugin_init = (coip_plugin_init_function) (*it);
		if (coip_plugin_init) {
			IContactSyncManagerPlugin * plugin =
				static_cast<IContactSyncManagerPlugin *>(coip_plugin_init(_coIpManager));
			//TODO: safe_connect
			_iCoIpManagerPluginList.push_back(plugin);
		}
	}
	////
	
	// IIMContactSyncManagerPlugin plugins
	pluginList = CoIpManagerPluginLoader::loadPlugins(_coIpManager.getCoIpManagerConfig().getCoIpPluginsPath(),
			CONTACTSYNCMANAGER_PLUGIN_DIR, IMCONTACTSYNC_PLUGIN_NAME);

	for (std::list<void *>::const_iterator it = pluginList.begin(); it != pluginList.end(); ++it) {
		coip_plugin_init_function coip_plugin_init = (coip_plugin_init_function) (*it);
		if (coip_plugin_init) {
			IIMContactSyncManagerPlugin * plugin =
				static_cast<IIMContactSyncManagerPlugin *>(coip_plugin_init(_coIpManager));
			SAFE_CONNECT(plugin, SIGNAL(imContactAddedSignal(IMContact)), SLOT(icsIMContactAddedSlot(IMContact)));
			SAFE_CONNECT(plugin, SIGNAL(imContactRemovedSignal(IMContact)), SLOT(icsIMContactRemovedSlot(IMContact)));
			SAFE_CONNECT(plugin, SIGNAL(imContactUpdatedSignal(IMContact)), SLOT(icsIMContactUpdatedSlot(IMContact)));
			_iCoIpManagerPluginList.push_back(plugin);
		}
	}
	////
}

ContactSyncManager::~ContactSyncManager() {
}

void ContactSyncManager::cmContactAddedSlot(std::string contactId) {
/*	Contact * contact = _coIpManager.getUserProfileManager().getUserProfile().getContactList().getContact(contactId);
	if (contact) {
		// Synchronizing Contact
		StringList phoneBookList = contact->getPhoneBookList();
		for (StringList::const_iterator it = phoneBookList.begin();
			it != phoneBookList.end();
			++it) {
			IContactSyncManagerPlugin * manager = getIContactSyncManagerPlugin(*it);
			manager->contactAddedEvent.connect(this,
				boost::bind(&ContactSyncManager::icsContactAddedEventHandler, this, _1, _2), NULL);
			manager->contactRemovedEvent.connect(this,
				boost::bind(&ContactSyncManager::icsContactRemovedEventHandler, this, _1, _2), NULL);
			manager->contactUpdatedEvent.connect(this,
				boost::bind(&ContactSyncManager::icsContactUpdatedEventHandler, this, _1, _2), NULL);
			if (manager) {
				manager->add(*contact);
			} else {
				// TODO: Should we update the Contact?
				LOG_ERROR("synchronizer " + (*it) + " does not exist anymore");
			}
		}
		/////
		OWSAFE_DELETE(contact);
	}*/
}

void ContactSyncManager::cmContactRemovedSlot(std::string contactId) {
/*	Contact * contact = _coIpManager.getUserProfileManager().getUserProfile().getContactList().getContact(contactId);
	if (contact) {
		// Synchronizing Contact
		StringList phoneBookList = contact->getPhoneBookList();
		for (StringList::const_iterator it = phoneBookList.begin();
			it != phoneBookList.end();
			++it) {
			IContactSyncManagerPlugin * manager = getIContactSyncManagerPlugin(*it);
			if (manager) {
				manager->remove(*contact);
			} else {
				// TODO: Should we update the Contact?
				LOG_ERROR("synchronizer " + (*it) + " does not exist anymore");
			}
		}
		/////
		OWSAFE_DELETE(contact);
	}*/
}

void ContactSyncManager::cmContactUpdatedSlot(std::string contactId, EnumUpdateSection::UpdateSection section) {
/*	Contact * contact = _coIpManager.getUserProfileManager().getUserProfile().getContactList().getContact(contactId);
	if (contact) {
		// Synchronizing Contact
		StringList phoneBookList = contact->getPhoneBookList();
		for (StringList::const_iterator it = phoneBookList.begin();
			it != phoneBookList.end();
			++it) {
			IContactSyncManagerPlugin * manager = getIContactSyncManagerPlugin(*it);
			if (manager) {
				manager->update(*contact);
			} else {
				// TODO: Should we update the Contact?
				LOG_ERROR("synchronizer " + (*it) + " does not exist anymore");
			}
		}
		/////
		OWSAFE_DELETE(contact);
	}*/
}

void ContactSyncManager::cmIMContactAddedSlot(std::string contactId, IMContact imContact) {
/*	StringList phoneBookList = imContact.getPhoneBookList();
	for (StringList::const_iterator it = phoneBookList.begin();
		it != phoneBookList.end();
		++it) {
		IIMContactSyncManagerPlugin * manager = getIIMContactSyncManagerPlugin(*it);
		manager->imContactAddedEvent.connect(this,
			boost::bind(&ContactSyncManager::icsIMContactAddedEventHandler, this, _1, _2), NULL);
		manager->imContactRemovedEvent.connect(this,
			boost::bind(&ContactSyncManager::icsIMContactRemovedEventHandler, this, _1, _2), NULL);
		manager->imContactUpdatedEvent.connect(this,
			boost::bind(&ContactSyncManager::icsIMContactUpdatedEventHandler, this, _1, _2), NULL);
		if (manager) {
			manager->add(imContact);
		} else {
			LOG_ERROR("synchronizer " + (*it) + " does not exist anymore");
		}
	}*/
}

void ContactSyncManager::cmIMContactRemovedSlot(std::string contactId, IMContact imContact) {
/*	StringList phoneBookList = imContact.getPhoneBookList();
	for (StringList::const_iterator it = phoneBookList.begin();
		it != phoneBookList.end();
		++it) {
		IIMContactSyncManagerPlugin * manager = getIIMContactSyncManagerPlugin(*it);
		if (manager) {
			manager->remove(imContact);
		} else {
			LOG_ERROR("synchronizer " + (*it) + " does not exist anymore");
		}
	}*/
}

void ContactSyncManager::icsContactAddedSlot(Contact contact) {
	//TODO: retrieve the Contact in ContactList to see if it is already there. Add it otherwhise
	LOG_DEBUG("Contact added");
}

void ContactSyncManager::icsContactRemovedSlot(Contact contact) {
	//TODO: look for the Contact in ContactList and remove it
	LOG_DEBUG("Contact removed");
}

void ContactSyncManager::icsContactUpdatedSlot(Contact contact) {
	//TODO: look for the Contact and update it
	LOG_DEBUG("Contact updated");
}

void ContactSyncManager::icsIMContactAddedSlot(IMContact imContact) {
	//TODO: Create a new Contact and put this IMContact in it
	LOG_DEBUG("IMContact added");
}

void ContactSyncManager::icsIMContactRemovedSlot(IMContact imContact) {
	//TODO: Look for the Contact owning this IMContact and update it
	LOG_DEBUG("IMContact removed");
}

void ContactSyncManager::icsIMContactUpdatedSlot(IMContact imContact) {
	LOG_DEBUG("IMContact updated");

	ContactList contactList = _coIpManager.getUserProfileManager().getCopyOfUserProfile().getContactList();
	Contact * contact = ContactListHelper::getCopyOfFirstContactThatOwns(contactList, imContact);
	if (!contact) {
		contact = new Contact();
		contact->addIMContact(imContact);
		_coIpManager.getUserProfileManager().getContactManager().add(*contact);
	} else {
		contact->updateIMContact(imContact);
		_coIpManager.getUserProfileManager().getContactManager().update(*contact);
	}

	OWSAFE_DELETE(contact);
}
