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

#include "PurpleConnectManager.h"

#include "PurpleConnect.h"

#include <purplewrapper/PurpleEnumIMProtocol.h>
#include <purplewrapper/PurpleWrapper.h>

#include <coipmanager_base/account/Account.h>
#include <coipmanager_base/account/AccountList.h>
#include <coipmanager_base/userprofile/UserProfile.h>

#include <util/Logger.h>
#include <util/SafeDelete.h>
#include <util/String.h>

IConnectManagerPlugin * coip_connect_plugin_init(CoIpManager & coIpManager) {
	return new PurpleConnectManager(coIpManager);
}

// LibPurple Callbacks table
static PurpleConnectionUiOps connUiOps = {
	PurpleConnectManager::connProgressCbk,
	PurpleConnectManager::connConnectedCbk,
	PurpleConnectManager::connDisconnectedCbk,
	PurpleConnectManager::connNoticeCbk,
	PurpleConnectManager::connReportDisconnectCbk,
	NULL, // network_connected
	NULL, // network_disconnected
	NULL, // purple_reserved1
	NULL, // purple_reserved2
	NULL, // purple_reserved3
	NULL, // purple_reserved4
};
////

PurpleConnectManager * PurpleConnectManager::_instance = NULL;

PurpleConnectManager::PurpleConnectManager(CoIpManager & coIpManager)
	: IConnectManagerPlugin(coIpManager) {

	_instance = this;

	_mutex = new QMutex(QMutex::Recursive);

	_supportedAccountType.push_back(EnumAccountType::AccountTypeAIM);
	_supportedAccountType.push_back(EnumAccountType::AccountTypeGTalk);
	_supportedAccountType.push_back(EnumAccountType::AccountTypeICQ);
	_supportedAccountType.push_back(EnumAccountType::AccountTypeJabber);
	_supportedAccountType.push_back(EnumAccountType::AccountTypeMSN);
	_supportedAccountType.push_back(EnumAccountType::AccountTypeYahoo);

	PurpleWrapper::getInstance().setConnectionCallbacks(&connUiOps);
}

PurpleConnectManager::~PurpleConnectManager() {
	OWSAFE_DELETE(_mutex);
	_instance = NULL;
}

void PurpleConnectManager::initialize() {
	
}

void PurpleConnectManager::uninitialize() {
	QMutexLocker lock(_mutex);
	_iConnectVector.clear();
}

IConnectPlugin * PurpleConnectManager::createIConnectPlugin(const Account & account)  {
	IConnectPlugin * result = NULL;

	switch (account.getAccountType()) {
	case EnumAccountType::AccountTypeAIM:
	case EnumAccountType::AccountTypeICQ:
	case EnumAccountType::AccountTypeGTalk:
	case EnumAccountType::AccountTypeJabber:
	case EnumAccountType::AccountTypeMSN:
	case EnumAccountType::AccountTypeYahoo:
		result = new PurpleConnect(_coIpManager, account);
		break;
	default:
		LOG_FATAL("Protocol not supported by this implementation");
	};

	{
		QMutexLocker lock(_mutex);
		_iConnectVector.push_back(result);
	}

	return result;
}

IConnectPlugin * PurpleConnectManager::findIConnectByPurpleConnection(PurpleConnection *gc) {
	IConnectPlugin * result = NULL;

	PurpleAccount * pAccount = purple_connection_get_account(gc);
	if (pAccount) {
		EnumAccountType::AccountType accountType = PurpleIMPrcl::getEnumIMProtocol(pAccount);
		std::string accountLogin = PurpleWrapper::cleanLogin(pAccount);

		{
			QMutexLocker lock(_mutex);
			for (std::vector<IConnectPlugin *>::const_iterator it = _iConnectVector.begin();
				it != _iConnectVector.end();
				++it) {
				Account account = (*it)->getAccount();
				if ((account.getLogin() == accountLogin)
					&& (account.getAccountType() == accountType)) {
					result = *it;
					break;
				}
			}
		}
	} else {
		LOG_ERROR("No Account found for this PurpleConnection");
	}

	return result;
}

void PurpleConnectManager::connProgressCbk(PurpleConnection *gc, const char *text, size_t step, size_t step_count) {
	IConnectPlugin * iConnect = _instance->findIConnectByPurpleConnection(gc);
	if (iConnect) {
		iConnect->emitAccountConnectionProgressSignal(
			iConnect->getAccount().getUUID(), step, step_count, std::string(text));
	} else {
		LOG_ERROR("Could not find associated IConnectPlugin");
	}
}

void PurpleConnectManager::connConnectedCbk(PurpleConnection *gc) {
	IConnectPlugin * iConnect = _instance->findIConnectByPurpleConnection(gc);
	if (iConnect) {
		iConnect->setState(EnumConnectionState::ConnectionStateConnected);
		iConnect->execute();
	} else {
		LOG_ERROR("Could not find associated IConnectPlugin");
	}
}

void PurpleConnectManager::connDisconnectedCbk(PurpleConnection *gc) {
	LOG_DEBUG("account disconnection");

	IConnectPlugin * iConnect = _instance->findIConnectByPurpleConnection(gc);
	if (iConnect) {
		if (gc->wants_to_die) {
			// Disconnection has been asked or account is connected
			// On another machine.
			iConnect->setState(EnumConnectionState::ConnectionStateDisconnected);
		} else {
			// Connection error. Retrying connection.
			iConnect->setState(EnumConnectionState::ConnectionStateConnecting);
			iConnect->execute();
		}
	} else {
		LOG_ERROR("Could not find associated IConnectPlugin");
	}
}

void PurpleConnectManager::connNoticeCbk(PurpleConnection *gc, const char *text) {
}

void PurpleConnectManager::connReportDisconnectCbk(PurpleConnection *gc, const char *text) {
	LOG_DEBUG("connection error: " + std::string(text));
	// Connection error are treated in connDisconnectedCbk
}
