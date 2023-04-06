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

#include <coipmanager/CoIpManager.h>

#include <coipmanager/CoIpManagerPluginLoader.h>
#include <coipmanager/CoIpManagerSaver.h>
#include <coipmanager/CoIpManagerUser.h>
#include <coipmanager/callsessionmanager/CallSessionManager.h>
#include <coipmanager/chatsessionmanager/ChatSessionManager.h>
#include <coipmanager/chatsessionmanager/EnumChatStatusMessage.h>
#include <coipmanager/connectmanager/ConnectManager.h>
#include <coipmanager/datamanager/UserProfileManager.h>
#include <coipmanager/filesessionmanager/FileSessionManager.h>
#include <coipmanager/initplugin/ICoIpManagerInitPlugin.h>
#include <coipmanager/notification/Notification.h>
#include <coipmanager/presencemanager/account/AccountPresenceManager.h>
#include <coipmanager/presencemanager/contact/ContactPresenceManager.h>
#include <coipmanager/smssessionmanager/EnumSMSState.h>
#include <coipmanager/smssessionmanager/SMSSessionManager.h>
#include <coipmanager/syncmanager/contact/ContactSyncManager.h>

#include <coipmanager_base/EnumChatTypingState.h>
#include <coipmanager_base/storage/UserProfileFileStorage.h>

#include <networkdiscovery/NetworkProxyDiscovery.h>

#include <sipwrapper/EnumPhoneCallState.h>
#include <sipwrapper/EnumPhoneLineState.h>

#include <util/Logger.h>
#include <util/Path.h>
#include <util/SafeDelete.h>

#include <QtCore/QMetaType>

static bool qtMetaTypeDeclared = false;
static void declareQtMetaType() {
	if (!qtMetaTypeDeclared) {
		qtMetaTypeDeclared	= true;

		//Basic types
		qRegisterMetaType<std::string>("std::string");
		qRegisterMetaType<unsigned>("unsigned");

		// SipWrapper type that should be in base
		qRegisterMetaType<EnumChatTypingState::ChatTypingState>("EnumChatTypingState::ChatTypingState");
		qRegisterMetaType<EnumPhoneCallState::PhoneCallState>("EnumPhoneCallState::PhoneCallState");
		qRegisterMetaType<EnumPhoneLineState::PhoneLineState>("EnumPhoneLineState::PhoneLineState");

		// networkdiscovery
		qRegisterMetaType<NetworkProxy>("NetworkProxy");

		// CoIpManager base
		qRegisterMetaType<Account>("Account");
		qRegisterMetaType<Contact>("Contact");
		qRegisterMetaType<EnumChatTypingState::ChatTypingState>("EnumChatTypingState::ChatTypingState");
		qRegisterMetaType<EnumConnectionState::ConnectionState>("EnumConnectionState::ConnectionState");
		qRegisterMetaType<EnumPresenceState::PresenceState>("EnumPresenceState::PresenceState");
		qRegisterMetaType<IMContact>("IMContact");
		qRegisterMetaType<UserProfile>("UserProfile");

		//CoIpManager classes
		qRegisterMetaType<EnumChatStatusMessage::ChatStatusMessage>("EnumChatStatusMessage::ChatStatusMessage");
		qRegisterMetaType<EnumConnectionError::ConnectionError>("EnumConnectionError::ConnectionError");
		qRegisterMetaType<EnumNotificationType::NotificationType>("EnumNotificationType::NotificationType");
		qRegisterMetaType<EnumUpdateSection::UpdateSection>("EnumUpdateSection::UpdateSection");
		qRegisterMetaType<EnumSMSState::SMSState>("EnumSMSState::SMSState");
		qRegisterMetaType<IFileSession::IFileSessionEvent>("IFileSession::IFileSessionEvent");
	}
}

CoIpManager::CoIpManager(const CoIpManagerConfig & coIpManagerConfig) {
	_coIpManagerConfig = coIpManagerConfig;
	_userProfileManager = NULL;
	_connectManager = NULL;
	_contactSyncManager = NULL;
	_fileSessionManager = NULL;
	_callSessionManager = NULL;
	_chatSessionManager = NULL;
	_smsSessionManager = NULL;
	_coIpManagerSaver = NULL;
	_accountPresenceManager = NULL;
	_contactPresenceManager = NULL;

	_mutex = new QMutex(QMutex::Recursive);
	_conditionMutex = new QMutex();

	Notification::getInstance();
	
	declareQtMetaType();
}

CoIpManager::~CoIpManager() {
	releaseCoIpManager();

	OWSAFE_DELETE(_contactPresenceManager);
	OWSAFE_DELETE(_accountPresenceManager);
	OWSAFE_DELETE(_coIpManagerSaver);
	OWSAFE_DELETE(_contactSyncManager);

	OWSAFE_DELETE(_smsSessionManager);
	OWSAFE_DELETE(_fileSessionManager);
	OWSAFE_DELETE(_chatSessionManager);
	OWSAFE_DELETE(_callSessionManager);

	OWSAFE_DELETE(_connectManager);

	OWSAFE_DELETE(_userProfileManager);

	for (std::vector<ICoIpManagerInitPlugin *>::iterator it = _coIpManagerInitPluginVector.begin();
		it != _coIpManagerInitPluginVector.end();
		++it) {
		delete (*it);
	}

	OWSAFE_DELETE(_mutex);
	OWSAFE_DELETE(_conditionMutex);
}

void CoIpManager::initialize() {
	loadExtLibraries();

	_userProfileManager = new UserProfileManager(*this);

	_connectManager = new ConnectManager(*this);

	_callSessionManager  = new CallSessionManager(*this);
	_chatSessionManager = new ChatSessionManager(*this);
	_fileSessionManager = new FileSessionManager(*this);
	_smsSessionManager = new SMSSessionManager(*this);

	_contactSyncManager = new ContactSyncManager(*this);
	_coIpManagerSaver = new CoIpManagerSaver(*this);
	_accountPresenceManager = new AccountPresenceManager(*this);
	_contactPresenceManager = new ContactPresenceManager(*this);
}

CoIpManagerConfig & CoIpManager::getCoIpManagerConfig() {
	return _coIpManagerConfig;
}

CallSessionManager & CoIpManager::getCallSessionManager() {
	return *_callSessionManager;
}

ChatSessionManager & CoIpManager::getChatSessionManager() {
	return *_chatSessionManager;
}

ConnectManager & CoIpManager::getConnectManager() {
	return *_connectManager;
}

FileSessionManager & CoIpManager::getFileSessionManager() {
	return *_fileSessionManager;
}

SMSSessionManager & CoIpManager::getSMSSessionManager() {
	return *_smsSessionManager;
}

UserProfileManager & CoIpManager::getUserProfileManager() {
	return *_userProfileManager;
}

static const std::string INITPLUGIN_PLUGIN_DIR = "coip-plugins/initplugin/";
static const std::string INIT_PLUGIN_NAME = "init";

void CoIpManager::loadExtLibraries() {
	typedef ICoIpManagerInitPlugin * (*coip_plugin_init_function)(CoIpManager &);

	std::list<void *> pluginList =
		CoIpManagerPluginLoader::loadPlugins(_coIpManagerConfig.getCoIpPluginsPath(),
			INITPLUGIN_PLUGIN_DIR, INIT_PLUGIN_NAME);

	for (std::list<void *>::const_iterator it = pluginList.begin(); it != pluginList.end(); ++it) {
		coip_plugin_init_function coip_plugin_init = (coip_plugin_init_function) (*it);
		if (coip_plugin_init) {
			ICoIpManagerInitPlugin * plugin = coip_plugin_init(*this);
			_coIpManagerInitPluginVector.push_back(plugin);
		}
	}
}

void CoIpManager::initExtLibraries() {
	for (std::vector<ICoIpManagerInitPlugin *>::iterator it = _coIpManagerInitPluginVector.begin();
		it != _coIpManagerInitPluginVector.end();
		++it) {
		(*it)->initialize();
	}
}

void CoIpManager::uninitExtLibraries() {
	for (std::vector<ICoIpManagerInitPlugin *>::iterator it = _coIpManagerInitPluginVector.begin();
		it != _coIpManagerInitPluginVector.end();
		++it) {
		(*it)->uninitialize();
	}
}

void CoIpManager::initializeCoIpManager() {
	initExtLibraries();

	CoIpManagerUserList list = _coIpManagerUserList;
	for (CoIpManagerUserList::const_iterator it = list.begin();
		it != list.end();
		++it) {
		if (!(*it).second) {
			(*it).first->coIpManagerIsInitializing();
		}
	}
}

void CoIpManager::releaseCoIpManager() {
	QMutexLocker lock(_mutex);
	QMutexLocker conditionLock(_conditionMutex);

	CoIpManagerUserList list = _coIpManagerUserList;
	for (CoIpManagerUserList::const_iterator it = list.begin();
		it != list.end();
		++it) {
		if ((*it).second) {
			(*it).first->releaseCoIpManager();
		}
	}

	while (!isReleased()) {
		_condition.wait(_conditionMutex);
	}

	uninitExtLibraries();
}

bool CoIpManager::isReleased() const {
	QMutexLocker lock(_mutex);

	bool result = true;

	for (CoIpManagerUserList::const_iterator it = _coIpManagerUserList.begin();
		it != _coIpManagerUserList.end();
		++it) {
		if ((*it).second) {
			result = false;
			break;
		}
	}

	return result;
}

void CoIpManager::retained(CoIpManagerUser * user) {
	QMutexLocker lock(_mutex);

	CoIpManagerUserList::iterator it = findCoIpManagerUser(_coIpManagerUserList, user);
	if (it != _coIpManagerUserList.end()) {
		(*it).second = true;
	}
}

void CoIpManager::released(CoIpManagerUser * user) {
	QMutexLocker lock(_mutex);

	CoIpManagerUserList::iterator it = findCoIpManagerUser(_coIpManagerUserList, user);
	if (it != _coIpManagerUserList.end()) {
		(*it).second = false;
	}

	_condition.wakeAll();
}

void CoIpManager::registerCoIpManagerUser(CoIpManagerUser * user) {
	QMutexLocker lock(_mutex);

	CoIpManagerUserList::iterator it = findCoIpManagerUser(_coIpManagerUserList, user);
	if (it == _coIpManagerUserList.end()) {
		_coIpManagerUserList.push_back(std::pair<CoIpManagerUser *, bool>(user, false));
	}
}

void CoIpManager::unregisterCoIpManagerUser(CoIpManagerUser * user) {
	QMutexLocker lock(_mutex);

	CoIpManagerUserList::iterator it = findCoIpManagerUser(_coIpManagerUserList, user);
	if (it != _coIpManagerUserList.end()) {
		_coIpManagerUserList.erase(it);
	}
}

CoIpManager::CoIpManagerUserList::iterator
	CoIpManager::findCoIpManagerUser(CoIpManagerUserList & list, const CoIpManagerUser * user) {

	CoIpManagerUserList::iterator result = list.end();
	for (CoIpManagerUserList::iterator it = list.begin();
		it != list.end();
		++it) {
		if ((*it).first == user) {
			result = it;
			break;
		}
	}

	return result;
}
 