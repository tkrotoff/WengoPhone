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

#ifndef OWCONNECTMANAGER_H
#define OWCONNECTMANAGER_H

#include <coipmanager/ICoIpManager.h>
#include <coipmanager/connectmanager/EnumConnectionError.h>
#include <coipmanager/connectmanager/IConnectManagerPlugin.h>

#include <coipmanager_base/EnumConnectionState.h>
#include <coipmanager_base/contact/ContactList.h>

#include <networkdiscovery/NetworkProxyDiscovery.h>

#include <map>
#include <string>

class Account;
class IConnectPlugin;
class IConnectManagerPlugin;

/**
 * Provides connection/disconnection service of Account.
 *
 * @ingroup CoIpManager
 * @author Philippe Bernery
 * @author Tanguy Krotoff
 */
class COIPMANAGER_API ConnectManager : public ICoIpManager {
	Q_OBJECT
public:

	ConnectManager(CoIpManager & coIpManager);

	~ConnectManager();

	/**
	 * Checks the validity of the account.
	 *
	 * @return true if the Account is valid
	 */
	bool checkValidity(const Account & account);

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
	 * Emitted when an Account is connected.
	 *
	 * @param accountId UUID of the connected Account
	 */
	void accountConnectionStateSignal(std::string accountId,
		EnumConnectionState::ConnectionState state);

	/**
	 * Emitted when an error occured while connecting.
	 *
	 * @param errorCode @see ConnectionError
	 * @param accountId UUID of the Account
	 */
	void accountConnectionErrorSignal(std::string accountId,
		EnumConnectionError::ConnectionError errorCode);

	/**
	 * Emitted for information about Account connection progression.
	 *
	 * @param accountId UUID of the Account
	 * @param currentStep
	 * @param totalSteps
	 * @param infoMessage
	 */
	void accountConnectionProgressSignal(std::string accountId,
		unsigned currentStep, unsigned totalSteps, std::string infoMessage);

public Q_SLOTS:

	/**
	 * Connects an Account given its id.
	 */
	void connect(const std::string & accountId);

	/**
	 * Disconnects an Account given its id.
	 */
	void disconnect(const std::string & accountId);

	/**
	 * Connects all Account of UserProfile.
	 */
	void connectAllAccounts();

	/**
	 * Disconnects all Account of UserProfile.
	 */
	void disconnectAllAccounts();

private Q_SLOTS:

	/**
	 * @see IConnectPlugin::accountConnectionStateSignal()
	 */
	void accountConnectionStateSlot(std::string accountId,
		EnumConnectionState::ConnectionState state);

	/**
	 * @see IConnectPlugin::accountConnectionErrorSignal()
	 */
	void accountConnectionErrorSlot(std::string accountId,
		EnumConnectionError::ConnectionError errorCode);

	/**
	 * @see IConnectPlugin::accountConnectionProgressSignal()
	 */
	void accountConnectionProgressSlot(std::string accountId,
		unsigned currentStep, unsigned totalSteps, std::string infoMessage);

private:

	virtual void initialize();

	virtual void uninitialize();

	/**
	 * Gets Http Proxy information.
	 *
	 * If CoIpManagerConfig::autoDetectHttpProxy is true, 
	 * CoIpManager gets Http proxy information automatically.
	 * Otherwise it takes information from its configuration.
	 * If CoIpManagerConfig::autoDetectHttpProxy is true and
	 * if proxy needs authentication, the proxyNeedsAuthenticationSignal
	 * is emitted, and no connection will be made until proxy is 
	 * correctly set.
	 */
	void getHttpProxy();

	/** Maps between Account UUID and IConnectPlugin instance. */
	std::map<std::string, IConnectPlugin *> _iConnectMap;

	/** True if http proxy parameters are set. */
	bool _httpProxySet;
};

#endif //OWCONNECTMANAGER_H
