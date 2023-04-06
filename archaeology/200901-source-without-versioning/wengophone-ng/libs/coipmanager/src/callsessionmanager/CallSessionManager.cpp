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

#include <coipmanager/callsessionmanager/CallSessionManager.h>

#include <coipmanager/CoIpManager.h>
#include <coipmanager/CoIpManagerPluginLoader.h>
#include <coipmanager/callsessionmanager/CallSession.h>
#include <coipmanager/callsessionmanager/ICallSessionManagerPlugin.h>

#include <coipmanager_base/account/Account.h>

#include <util/Logger.h>
#include <util/Path.h>
#include <util/SafeDelete.h>
#include <util/SafeConnect.h>

static const std::string CALLSESSIONMANAGER_PLUGIN_DIR = "coip-plugins/callsessionmanager/";
static const std::string CALLSESSION_PLUGIN_NAME = "callsession";

CallSessionManager::CallSessionManager(CoIpManager & coIpManager)
	: ICoIpSessionManager(coIpManager) {

	typedef ICallSessionManagerPlugin * (*coip_plugin_init_function)(CoIpManager &);

	std::list<void *> pluginList =
		CoIpManagerPluginLoader::loadPlugins(_coIpManager.getCoIpManagerConfig().getCoIpPluginsPath(),
			CALLSESSIONMANAGER_PLUGIN_DIR, CALLSESSION_PLUGIN_NAME);

	for (std::list<void *>::const_iterator it = pluginList.begin(); it != pluginList.end(); ++it) {
		coip_plugin_init_function coip_plugin_init = (coip_plugin_init_function) (*it);
		if (coip_plugin_init) {
			ICallSessionManagerPlugin * plugin = coip_plugin_init(_coIpManager);
			_iCoIpManagerPluginList.push_back(plugin);

			SAFE_CONNECT(plugin, SIGNAL(newICallSessionPluginCreatedSignal(ICallSessionPlugin *)),
				SLOT(newICallSessionPluginCreatedSlot(ICallSessionPlugin *)));
		}
	}
}

CallSessionManager::~CallSessionManager() {
	uninitialize();
}

void CallSessionManager::initialize() {
	ICoIpSessionManager::initialize();
}

void CallSessionManager::uninitialize() {
	ICoIpSessionManager::uninitialize();
}

ICallSessionManagerPlugin * CallSessionManager::getICoIpManagerPlugin(const Account & account,
	const ContactList & contactList) const {
	return static_cast<ICallSessionManagerPlugin *>(ICoIpSessionManager::getICoIpManagerPlugin(account, contactList));
}

CallSession * CallSessionManager::createCallSession() {
	CallSession * callSession = new CallSession(_coIpManager);

	SAFE_CONNECT(callSession,
		SIGNAL(phoneCallStateChangedSignal(EnumPhoneCallState::PhoneCallState)),
		SLOT(phoneCallStateChangedSlot(EnumPhoneCallState::PhoneCallState)));

	return callSession;
}

void CallSessionManager::newICallSessionPluginCreatedSlot(ICallSessionPlugin * iCallSession) {
	CallSession * callSession = new CallSession(_coIpManager, iCallSession);

	SAFE_CONNECT(callSession,
		SIGNAL(phoneCallStateChangedSignal(EnumPhoneCallState::PhoneCallState)),
		SLOT(phoneCallStateChangedSlot(EnumPhoneCallState::PhoneCallState)));

	newCallSessionCreatedSignal(callSession);
}

void CallSessionManager::phoneCallStateChangedSlot(EnumPhoneCallState::PhoneCallState state) {
}
