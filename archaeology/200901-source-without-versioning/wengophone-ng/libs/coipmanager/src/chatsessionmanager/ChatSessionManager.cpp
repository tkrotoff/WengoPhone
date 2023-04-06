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

#include <coipmanager/chatsessionmanager/ChatSessionManager.h>

#include <coipmanager/CoIpManager.h>
#include <coipmanager/CoIpManagerPluginLoader.h>
#include <coipmanager/chatsessionmanager/ChatSession.h>
#include <coipmanager/chatsessionmanager/IChatSessionManagerPlugin.h>

#include <util/Logger.h>
#include <util/Path.h>
#include <util/SafeConnect.h>
#include <util/SafeDelete.h>

#include <QtCore/QString>

static const std::string CHATSESSIONMANAGER_PLUGIN_DIR = "coip-plugins/chatsessionmanager/";
static const std::string CHATSESSION_PLUGIN_NAME = "chatsession";

ChatSessionManager::ChatSessionManager(CoIpManager & coIpManager)
	: ICoIpSessionManager(coIpManager) {

	typedef IChatSessionManagerPlugin * (*coip_plugin_init_function)(CoIpManager &);

	std::list<void *> pluginList =
		CoIpManagerPluginLoader::loadPlugins(_coIpManager.getCoIpManagerConfig().getCoIpPluginsPath(),
			CHATSESSIONMANAGER_PLUGIN_DIR, CHATSESSION_PLUGIN_NAME);

	for (std::list<void *>::const_iterator it = pluginList.begin(); it != pluginList.end(); ++it) {
		coip_plugin_init_function coip_plugin_init = (coip_plugin_init_function) (*it);
		if (coip_plugin_init) {
			IChatSessionManagerPlugin * plugin = coip_plugin_init(_coIpManager);
			_iCoIpManagerPluginList.push_back(plugin);

			SAFE_CONNECT(plugin, SIGNAL(newIChatSessionPluginCreatedSignal(IChatSessionPlugin *)),
				SLOT(newIChatSessionPluginCreatedSlot(IChatSessionPlugin *)));
		}
	}
}

ChatSessionManager::~ChatSessionManager() {
}

void ChatSessionManager::initialize() {
	ICoIpSessionManager::initialize();
}

void ChatSessionManager::uninitialize() {
	closeAllChatSession();
	ICoIpSessionManager::uninitialize();
}

void ChatSessionManager::closeAllChatSession() {
}

ChatSession * ChatSessionManager::createChatSession() {
	return new ChatSession(_coIpManager);
}

void ChatSessionManager::newIChatSessionPluginCreatedSlot(IChatSessionPlugin * iChatSession) {
	newChatSessionCreatedSignal(new ChatSession(_coIpManager, iChatSession));
}

IChatSessionManagerPlugin * ChatSessionManager::getICoIpManagerPlugin(const Account & account, const ContactList & contactList) const {
	return static_cast<IChatSessionManagerPlugin *>(ICoIpSessionManager::getICoIpManagerPlugin(account, contactList));
}
