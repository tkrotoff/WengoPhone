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

#include <coipmanager/filesessionmanager/FileSessionManager.h>

#include <coipmanager/CoIpManager.h>
#include <coipmanager/CoIpManagerPluginLoader.h>
#include <coipmanager/datamanager/UserProfileManager.h>
#include <coipmanager/filesessionmanager/IFileSessionManagerPlugin.h>
#include <coipmanager/filesessionmanager/ISendFileSessionPlugin.h>
#include <coipmanager/filesessionmanager/ReceiveFileSession.h>
#include <coipmanager/filesessionmanager/SendFileSession.h>

#include <util/Logger.h>
#include <util/Path.h>
#include <util/SafeConnect.h>
#include <util/SafeDelete.h>

static const std::string FILESESSIONMANAGER_PLUGIN_DIR = "coip-plugins/filesessionmanager/";
static const std::string FILESESSION_PLUGIN_NAME = "filesession";

FileSessionManager::FileSessionManager(CoIpManager & coIpManager)
	: ICoIpSessionManager(coIpManager) {

	typedef IFileSessionManagerPlugin * (*coip_plugin_init_function)(CoIpManager &);

	std::list<void *> pluginList =
		CoIpManagerPluginLoader::loadPlugins(_coIpManager.getCoIpManagerConfig().getCoIpPluginsPath(),
			FILESESSIONMANAGER_PLUGIN_DIR, FILESESSION_PLUGIN_NAME);

	for (std::list<void *>::const_iterator it = pluginList.begin(); it != pluginList.end(); ++it) {
		coip_plugin_init_function coip_plugin_init = (coip_plugin_init_function) (*it);
		if (coip_plugin_init) {
			IFileSessionManagerPlugin * plugin = coip_plugin_init(_coIpManager);
			_iCoIpManagerPluginList.push_back(plugin);

			SAFE_CONNECT(plugin,
				SIGNAL(newIReceiveFileSessionPluginCreatedSignal(IReceiveFileSessionPlugin *)),
				SLOT(newIReceiveFileSessionPluginCreatedSlot(IReceiveFileSessionPlugin *)));
			SAFE_CONNECT(plugin,
				SIGNAL(needsUpgradeSignal()),
				SLOT(needsUpgradeSlot()));
			SAFE_CONNECT(plugin,
				SIGNAL(peerNeedsUpgradeSignal(IMContact)),
				SLOT(peerNeedsUpgradeSlot(IMContact)));
		}
	}
}

FileSessionManager::~FileSessionManager() {
	uninitialize();
}

void FileSessionManager::initialize() {
	ICoIpSessionManager::initialize();
}

void FileSessionManager::uninitialize() {
	ICoIpSessionManager::uninitialize();
}

SendFileSession * FileSessionManager::createSendFileSession() {
	return new SendFileSession(_coIpManager);
}

IFileSessionManagerPlugin * FileSessionManager::getICoIpManagerPlugin(const Account & account, const ContactList & contactList) const {
	return static_cast<IFileSessionManagerPlugin *>(ICoIpSessionManager::getICoIpManagerPlugin(account, contactList));
}

void FileSessionManager::newIReceiveFileSessionPluginCreatedSlot(IReceiveFileSessionPlugin * iReceiveFileSession) {
	LOG_DEBUG("new IReceiveFileSessionPlugin created event");

	ReceiveFileSession *newFileSession = new ReceiveFileSession(_coIpManager, iReceiveFileSession);

	newReceiveFileSessionCreatedSignal(newFileSession);
}

void FileSessionManager::needsUpgradeSlot() {
	needsUpgradeSignal();
}

void FileSessionManager::peerNeedsUpgradeSlot(IMContact imContact) {
	ContactList contactList = _coIpManager.getUserProfileManager().getCopyOfUserProfile().getContactList();
	Contact * contact = ContactListHelper::getCopyOfFirstContactThatOwns(contactList, imContact);
	if (contact) {
		peerNeedsUpgradeSignal(*contact);
		OWSAFE_DELETE(contact);
	}
}
