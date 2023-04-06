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

#include <coipmanager_threaded/TCoIpManager.h>

#include <coipmanager_threaded/callsessionmanager/TCallSessionManager.h>
#include <coipmanager_threaded/chatsessionmanager/TChatSessionManager.h>
#include <coipmanager_threaded/connectmanager/TConnectManager.h>
#include <coipmanager_threaded/datamanager/TUserProfileManager.h>
#include <coipmanager_threaded/filesessionmanager/TFileSessionManager.h>
#include <coipmanager_threaded/smssessionmanager/TSMSSessionManager.h>

#include <coipmanager/CoIpManager.h>

#include <util/SafeDelete.h>
#include <util/Logger.h>

TCoIpManager::TCoIpManager(const CoIpManagerConfig & coIpManagerConfig)
	: _coIpManagerConfig(coIpManagerConfig) {

	_coIpManager = NULL;
	_tUserProfileManager = NULL;
	_tConnectManager = NULL;
	_tCallSessionManager = NULL;
	_tChatSessionManager = NULL;
	_tFileSessionManager = NULL;
	_tSMSSessionManager = NULL;

	//Starts TCoIpManager thread
	start();
}

TCoIpManager::~TCoIpManager() {
	release();
}

void TCoIpManager::release() {
	quit();
	wait();
 	//FIXME: there is sometimes a deadlock but don't know why. We
	// should maybe put a timeout in wait.	
}

void TCoIpManager::run() {
	_coIpManager = new CoIpManager(_coIpManagerConfig);
	_coIpManager->initialize();

	_tUserProfileManager = new TUserProfileManager(*this);
	_tConnectManager = new TConnectManager(*this);
	_tCallSessionManager = new TCallSessionManager(*this);
	_tChatSessionManager = new TChatSessionManager(*this);
	_tFileSessionManager = new TFileSessionManager(*this);
	_tSMSSessionManager = new TSMSSessionManager(*this);

	initialized();

	exec();

	OWSAFE_DELETE(_tSMSSessionManager);
	OWSAFE_DELETE(_tFileSessionManager);
	OWSAFE_DELETE(_tChatSessionManager);
	OWSAFE_DELETE(_tCallSessionManager);
	OWSAFE_DELETE(_tConnectManager);
	OWSAFE_DELETE(_tUserProfileManager);

	OWSAFE_DELETE(_coIpManager);
}

CoIpManager & TCoIpManager::getCoIpManager() const {
	if (!_coIpManager) {
		LOG_FATAL("_coIpManager not initialized");
	}

	return *_coIpManager;
}

CoIpManagerConfig & TCoIpManager::getCoIpManagerConfig() {
	return _coIpManagerConfig;
}

TCallSessionManager & TCoIpManager::getTCallSessionManager() {
	if (!_tCallSessionManager) {
		LOG_FATAL("_tCallSessionManager not initialized");
	}

	return *_tCallSessionManager;
}

TChatSessionManager & TCoIpManager::getTChatSessionManager() {
	if (!_tChatSessionManager) {
		LOG_FATAL("_tChatSessionManager not initialized");
	}

	return *_tChatSessionManager;
}

TConnectManager & TCoIpManager::getTConnectManager() {
	if (!_tConnectManager) {
		LOG_FATAL("_tConnectManager not initialized");
	}

	return *_tConnectManager;
}

TFileSessionManager & TCoIpManager::getTFileSessionManager() {
	if (!_tFileSessionManager) {
		LOG_FATAL("_tFileSessionManager not initialized");
	}

	return *_tFileSessionManager;
}

TSMSSessionManager & TCoIpManager::getTSMSSessionManager() {
	if (!_tSMSSessionManager) {
		LOG_FATAL("_tSMSProfileManager not initialized");
	}

	return *_tSMSSessionManager;
}

TUserProfileManager & TCoIpManager::getTUserProfileManager() {
	if (!_tUserProfileManager) {
		LOG_FATAL("_tUserProfileManager not initialized");
	}

	return *_tUserProfileManager;
}

/*
TUserProfileSyncManager & TCoIpManager::getTUserProfileSyncManager() {
}
*/
