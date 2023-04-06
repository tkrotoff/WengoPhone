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

#ifndef OWTCONNECTMANAGER_H
#define OWTCONNECTMANAGER_H

#include <coipmanager_threaded/tcoipmanagerdll.h>

#include <coipmanager/connectmanager/EnumConnectionError.h>

#include <coipmanager_base/EnumConnectionState.h>

#include <networkdiscovery/NetworkProxyDiscovery.h>

#include <util/NonCopyable.h>

#include <QtCore/QObject>

#include <string>

class Account;
class ConnectManager;
class TCoIpManager;

/**
 * Provides connection/disconnection service of Account.
 *
 * @see ConnectManager
 * @ingroup TCoIpManager
 * @author Tanguy Krotoff
 */
class COIPMANAGER_THREADED_API TConnectManager : public QObject, NonCopyable {
	Q_OBJECT
public:

	TConnectManager(TCoIpManager & tCoIpManager);

	~TConnectManager();

	/**
	 * @see ConnectManager::checkValidity()
	 */
	bool checkValidity(const Account & account);

	/**
	 * @see ConnectManager::connect()
	 */
	void connect(const std::string & accountId);

	/**
	 * @see ConnectManager::disconnect()
	 */
	void disconnect(const std::string & accountId);

	/**
	 * @see ConnectManager::connectAllAccounts()
	 */
	void connectAllAccounts();

	/**
	 * @see ConnectManager::disconnectAllAccounts()
	 */
	void disconnectAllAccounts();

	/**
	 * Sets the proxy information.
	 *
	 * Call this method after receiving the proxyNeedsAuthenticationSignal.
	 */
	void setHttpProxy(NetworkProxy networkProxy);

Q_SIGNALS:

	/**
	 * Emitted when http proxy settings must be detected automatically
	 * and when the detected proxy needs authentication or when login/password
	 * are incorrect.
	 * Once received, information must be filled in the CoIpManagerConfig
	 */
	void proxyNeedsAuthenticationSignal(NetworkProxy networkProxy);

	/**
	 * @see ConnectManager::accountConnectedSignal()
	 */
	void accountConnectionStateSignal(std::string accountId,
		EnumConnectionState::ConnectionState state);

	/**
	 * @see ConnectManager::accountConnectionErrorSignal()
	 */
	void accountConnectionErrorSignal(std::string accountId,
		EnumConnectionError::ConnectionError errorCode);

	/**
	 * @see ConnectManager::accountConnectionProgressSignal()
	 */
	void accountConnectionProgressSignal(std::string accountId,
		unsigned currentStep, unsigned totalSteps, std::string infoMessage);

private:

	/**
	 * Code factorization.
	 */
	ConnectManager * getConnectManager() const;

	TCoIpManager & _tCoIpManager;
};

#endif	//OWTCONNECTMANAGER_H
