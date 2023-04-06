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

#include <coipmanager_threaded/connectmanager/TConnectManager.h>

#include <coipmanager_threaded/TCoIpManager.h>

#include <coipmanager/CoIpManager.h>
#include <coipmanager/connectmanager/ConnectManager.h>

#include <util/SafeConnect.h>
#include <util/PostEvent.h>

TConnectManager::TConnectManager(TCoIpManager & tCoIpManager)
	: _tCoIpManager(tCoIpManager) {

	ConnectManager * connectManager = getConnectManager();

	SAFE_CONNECT(connectManager,
		SIGNAL(accountConnectionStateSignal(std::string, EnumConnectionState::ConnectionState)),
		SIGNAL(accountConnectionStateSignal(std::string, EnumConnectionState::ConnectionState)));
	SAFE_CONNECT(connectManager,
		SIGNAL(accountConnectionErrorSignal(std::string, EnumConnectionError::ConnectionError)),
		SIGNAL(accountConnectionErrorSignal(std::string, EnumConnectionError::ConnectionError)));
	SAFE_CONNECT(connectManager,
		SIGNAL(accountConnectionProgressSignal(std::string, unsigned, unsigned, std::string)),
		SIGNAL(accountConnectionProgressSignal(std::string, unsigned, unsigned, std::string)));
	SAFE_CONNECT(connectManager,
		SIGNAL(proxyNeedsAuthenticationSignal(NetworkProxy)),
		SIGNAL(proxyNeedsAuthenticationSignal(NetworkProxy)));
}

TConnectManager::~TConnectManager() {
}

ConnectManager * TConnectManager::getConnectManager() const {
	return &_tCoIpManager.getCoIpManager().getConnectManager();
}

bool TConnectManager::checkValidity(const Account & account) {
	return getConnectManager()->checkValidity(account);
}

void TConnectManager::connect(const std::string & accountId) {
	PostEvent::invokeMethod(getConnectManager(), "connect",
		Q_ARG(std::string, accountId));
}

void TConnectManager::disconnect(const std::string & accountId) {
	PostEvent::invokeMethod(getConnectManager(), "disconnect",
		Q_ARG(std::string, accountId));
}

void TConnectManager::connectAllAccounts() {
	PostEvent::invokeMethod(getConnectManager(), "connectAllAccounts");
}

void TConnectManager::disconnectAllAccounts() {
	PostEvent::invokeMethod(getConnectManager(), "disconnectAllAccounts");
}

void TConnectManager::setHttpProxy(NetworkProxy networkProxy) {
	PostEvent::invokeMethod(getConnectManager(), "setHttpProxy",
		Q_ARG(NetworkProxy, networkProxy));
}
