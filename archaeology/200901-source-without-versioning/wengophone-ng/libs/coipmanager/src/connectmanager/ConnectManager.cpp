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

#include <coipmanager/connectmanager/ConnectManager.h>

#include <coipmanager/AccountMutator.h>
#include <coipmanager/CoIpManager.h>
#include <coipmanager/CoIpManagerConfig.h>
#include <coipmanager/CoIpManagerPluginLoader.h>
#include <coipmanager/connectmanager/IConnectManagerPlugin.h>
#include <coipmanager/datamanager/UserProfileManager.h>

#include <coipmanager_base/account/Account.h>
#include <coipmanager_base/account/AccountList.h>
#include <coipmanager_base/userprofile/UserProfile.h>

#include <networkdiscovery/NetworkDiscovery.h>

#include <util/Logger.h>
#include <util/Path.h>
#include <util/SafeConnect.h>
#include <util/SafeDelete.h>

static const std::string CONNECTMANAGER_PLUGIN_DIR = "coip-plugins/connectmanager/";
static const std::string CONNECTSESSION_PLUGIN_NAME = "connect";

ConnectManager::ConnectManager(CoIpManager & coIpManager)
	: ICoIpManager(coIpManager) {

	_httpProxySet = false;

	typedef IConnectManagerPlugin * (*coip_plugin_init_function)(CoIpManager &);
	std::list<void *> pluginList =
		CoIpManagerPluginLoader::loadPlugins(_coIpManager.getCoIpManagerConfig().getCoIpPluginsPath(),
			CONNECTMANAGER_PLUGIN_DIR, CONNECTSESSION_PLUGIN_NAME);

	for (std::list<void *>::const_iterator it = pluginList.begin(); it != pluginList.end(); ++it) {
		coip_plugin_init_function coip_plugin_init = (coip_plugin_init_function) (*it);
		if (coip_plugin_init) {
			IConnectManagerPlugin * plugin = coip_plugin_init(_coIpManager);
			_iCoIpManagerPluginList.push_back(plugin);
		}
	}
}

ConnectManager::~ConnectManager() {
	uninitialize();
}

void ConnectManager::initialize() {
	ICoIpManager::initialize();

	getHttpProxy();
}

void ConnectManager::uninitialize() {
	disconnectAllAccounts();

	for (std::map<std::string, IConnectPlugin *>::iterator it = _iConnectMap.begin();
		it != _iConnectMap.end();
		++it) {
		OWSAFE_DELETE((*it).second);
	}
	_iConnectMap.clear();

	_httpProxySet = false;

	ICoIpManager::uninitialize();
}

void ConnectManager::getHttpProxy() {
	if (_coIpManager.getCoIpManagerConfig().autoDetectHttpProxy()) {
		NetworkProxy networkProxy;

		networkProxy.setLogin(_coIpManager.getCoIpManagerConfig().getHttpProxyLogin());
		networkProxy.setPassword(_coIpManager.getCoIpManagerConfig().getHttpProxyPassword());
		networkProxy.setProxyAuthType(_coIpManager.getCoIpManagerConfig().getHttpProxyAuthType());

		NetworkProxyDiscovery::NetworkProxyDiscoveryError error = 
			NetworkProxyDiscovery::getInstance().getNetworkProxy(networkProxy);
		
		if (error == NetworkProxyDiscovery::NetworkProxyDiscoveryErrorNeedsAuthentication) {
			proxyNeedsAuthenticationSignal(networkProxy);
			_httpProxySet = false;
			return;
		}
	}

	_httpProxySet = true;
}

void ConnectManager::setHttpProxy(NetworkProxy networkProxy) {
	_coIpManager.getCoIpManagerConfig().set(CoIpManagerConfig::HTTP_PROXY_SERVER_KEY,
		networkProxy.getServer());
	_coIpManager.getCoIpManagerConfig().set(CoIpManagerConfig::HTTP_PROXY_SERVER_PORT_KEY,
		networkProxy.getServerPort());
	_coIpManager.getCoIpManagerConfig().set(CoIpManagerConfig::HTTP_PROXY_LOGIN_KEY,
		networkProxy.getLogin());
	_coIpManager.getCoIpManagerConfig().set(CoIpManagerConfig::HTTP_PROXY_PASSWORD_KEY,
		networkProxy.getPassword());
	_coIpManager.getCoIpManagerConfig().set(CoIpManagerConfig::HTTP_PROXY_AUTH_TYPE_KEY, 
		EnumProxyAuthType::toString(networkProxy.getProxyAuthType()));

	NetworkDiscovery::getInstance().setNetworkProxy(networkProxy);

	// Restart proxy detection.
	getHttpProxy();
}

bool ConnectManager::checkValidity(const Account & account) {
	//TODO
	return true;
}

void ConnectManager::connect(const std::string & accountId) {
	if (!_httpProxySet) {
		LOG_ERROR("Proxy is not set. You cannot connect.");
		return;
	}

	IConnectPlugin * iConnect = NULL;

	// Look for an existing IConnectPlugin associated with this accountId
	std::map<std::string, IConnectPlugin *>::const_iterator it = _iConnectMap.find(accountId);
	if (it != _iConnectMap.end()) {
		iConnect = (*it).second;
	} else {
		AccountList accountList = _coIpManager.getUserProfileManager().getCopyOfUserProfile().getAccountList();
		Account * account = AccountListHelper::getCopyOfAccount(accountList, accountId);
		if (account) {
			for (ICoIpManagerPluginList::const_iterator icmIt = _iCoIpManagerPluginList.begin();
				icmIt != _iCoIpManagerPluginList.end();
				++icmIt) {
				IConnectManagerPlugin * iConnectManagerPlugin = static_cast<IConnectManagerPlugin *>(*icmIt);
				if (iConnectManagerPlugin->isProtocolSupported(account->getAccountType())) {
					iConnect = iConnectManagerPlugin->createIConnectPlugin(*account);
					_iConnectMap[accountId] = iConnect;

					SAFE_CONNECT(iConnect,
						SIGNAL(accountConnectionStateSignal(std::string, EnumConnectionState::ConnectionState)),
						SLOT(accountConnectionStateSlot(std::string, EnumConnectionState::ConnectionState)));
					SAFE_CONNECT(iConnect,
						SIGNAL(accountConnectionErrorSignal(std::string, EnumConnectionError::ConnectionError)),
						SLOT(accountConnectionErrorSlot(std::string, EnumConnectionError::ConnectionError)));
					SAFE_CONNECT(iConnect,
						SIGNAL(accountConnectionProgressSignal(std::string, unsigned, unsigned, std::string)),
						SLOT(accountConnectionProgressSlot(std::string, unsigned, unsigned, std::string)));
					break;
				}
			}
		} else {
			LOG_FATAL("account not in AccountList, cannot connect");
		}
		////
	}

	if (!iConnect) {
		LOG_DEBUG("no IConnectPlugin implementation available for this account");
		accountConnectionErrorSlot(accountId, EnumConnectionError::ConnectionErrorNoImplementation);
		return;
	}
	////

	// Connecting
	iConnect->connect();
	////
}

void ConnectManager::disconnect(const std::string & accountId) {
	std::map<std::string, IConnectPlugin *>::const_iterator it = _iConnectMap.find(accountId);
	if (it != _iConnectMap.end()) {
		(*it).second->disconnect();
	}
}

void ConnectManager::connectAllAccounts() {
	AccountList accountList = _coIpManager.getUserProfileManager().getCopyOfUserProfile().getAccountList();

	for (AccountList::const_iterator it = accountList.begin();
		it != accountList.end();
		++it) {
		connect((*it).getUUID());
	}
}

void ConnectManager::disconnectAllAccounts() {
	AccountList accountList = _coIpManager.getUserProfileManager().getCopyOfUserProfile().getAccountList();

	for (AccountList::const_iterator it = accountList.begin();
		it != accountList.end();
		++it) {
		disconnect((*it).getUUID());
	}
}

void ConnectManager::accountConnectionStateSlot(std::string accountId,
	EnumConnectionState::ConnectionState state) {

	QMutexLocker lock(_mutex);

	AccountList accountList = _coIpManager.getUserProfileManager().getCopyOfUserProfile().getAccountList();
	Account * account = AccountListHelper::getCopyOfAccount(accountList, accountId);
	if (account) {
		account->setConnectionState(state);

		switch(state) {
		case EnumConnectionState::ConnectionStateConnected:
			account->setPresenceState(EnumPresenceState::PresenceStateOnline);
			break;
		default:
			account->setPresenceState(EnumPresenceState::PresenceStateOffline);
			break;
		}

		AccountMutator::emitAccountUpdatedSignal(*account, EnumUpdateSection::UpdateSectionPresenceState);
		OWSAFE_DELETE(account);
	}

	LOG_DEBUG("account: " + accountId + " change its state - " + EnumConnectionState::toString(state));

	accountConnectionStateSignal(accountId, state);
}

void ConnectManager::accountConnectionErrorSlot(std::string accountId,
	EnumConnectionError::ConnectionError errorCode) {

	QMutexLocker lock(_mutex);

	LOG_DEBUG("account: " + accountId
		+ " had error during connection: " + EnumConnectionError::toString(errorCode));

	accountConnectionErrorSignal(accountId, errorCode);
}

void ConnectManager::accountConnectionProgressSlot(std::string accountId,
	unsigned currentStep, unsigned totalSteps, std::string infoMessage) {
	QMutexLocker lock(_mutex);

	LOG_DEBUG("connection progress of account " + accountId + ": step "
		+ String::fromNumber(currentStep) + "/" + String::fromNumber(totalSteps)
		+ ": " + infoMessage);

	accountConnectionProgressSignal(accountId, currentStep, totalSteps, infoMessage);
}
