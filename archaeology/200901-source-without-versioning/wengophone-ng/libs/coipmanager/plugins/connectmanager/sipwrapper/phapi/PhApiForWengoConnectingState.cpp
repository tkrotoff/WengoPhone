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

#include "PhApiForWengoConnectingState.h"

#include <coipmanager/CoIpManager.h>
#include <coipmanager/CoIpManagerConfig.h>
#include <coipmanager/connectmanager/IConnectPlugin.h>

#include <coipmanager_base/account/SipAccount.h>
#include <coipmanager_base/account/WengoAccount.h>

#include <util/Logger.h>
#include <util/SafeConnect.h>
#include <util/String.h>

#include <QtCore/QTimer>

static const unsigned MAX_UDPCONNECTION_ATTEMPT = 3;
static const unsigned MAX_HTTP80CONNECTION_ATTEMPT = 1;
static const unsigned MAX_HTTP443CONNECTION_ATTEMPT = 1;
static const unsigned MAX_HTTPS443CONNECTION_ATTEMPT = 1;
static const std::string STUN_SERVER = "stun.wengo.fr";

PhApiForWengoConnectingState::PhApiForWengoConnectingState(IConnectPlugin & context, SipWrapper & sipWrapper)	
	: SipWrapperBasedConnectingState(context, sipWrapper) {

	resetAttemptCounter();
}

void PhApiForWengoConnectingState::resetAttemptCounter() {
	_udpConnectionAttempt = 0;
	_http80ConnectionAttempt = 0;
	_http443ConnectionAttempt = 0;
	_https443ConnectionAttempt = 0;
}

void PhApiForWengoConnectingState::execute() {
	IConnectPlugin & iConnect = static_cast<IConnectPlugin &>(_stateContext);
	Account & account = iConnect.getAccount();
	WengoAccount * wengoAccount = static_cast<WengoAccount *>(account.getPrivateAccount());

	if (!wengoAccount->isSSORequestMade()) {
		SSORequest::SSORequestError error = doSsoRequest();
		if (error == SSORequest::SSORequestErrorInvalidLoginPassword) {
			LOG_DEBUG("Wrong wengo login/password.");
			iConnect.setState(EnumConnectionState::ConnectionStateDisconnected);
			iConnect.emitAccountConnectionErrorSignal(iConnect.getAccount().getUUID(),
				EnumConnectionError::ConnectionErrorInvalidLoginPassword);
			return;
		} else if (error != SSORequest::SSORequestErrorOk) {
			LOG_ERROR("Error while doing SSO request.");
			iConnect.setState(EnumConnectionState::ConnectionStateDisconnected);
			iConnect.emitAccountConnectionErrorSignal(iConnect.getAccount().getUUID(),
				EnumConnectionError::ConnectionErrorServer);
			return;
		} else {
			// error == SSORequest::SSORequestErrorOk
			AccountMutator::getInstance().emitAccountUpdatedSignal(account, EnumUpdateSection::UpdateSectionUnknown);
		}
	}

	SipWrapperBasedConnectingState::execute();
}

SSORequest::SSORequestError PhApiForWengoConnectingState::doSsoRequest() {
	IConnectPlugin & iConnect = static_cast<IConnectPlugin &>(_stateContext);
	Account & account = iConnect.getAccount();

	unsigned currentConnectionStep = iConnect.getCurrentConnectionStep();
	iConnect.emitAccountConnectionProgressSignal(account.getUUID(),
		++currentConnectionStep, iConnect.getConnectionSteps(), "getting SSO parameters");
	iConnect.setCurrentConnectionStep(currentConnectionStep);

	WengoAccount * wengoAccount = static_cast<WengoAccount *>(account.getPrivateAccount());
	NetworkProxy networkProxy;
	networkProxy.setServer(iConnect.getCoIpManager().getCoIpManagerConfig().getHttpProxyServer());
	networkProxy.setServerPort(iConnect.getCoIpManager().getCoIpManagerConfig().getHttpProxyServerPort());
	networkProxy.setLogin(iConnect.getCoIpManager().getCoIpManagerConfig().getHttpProxyLogin());
	networkProxy.setPassword(iConnect.getCoIpManager().getCoIpManagerConfig().getHttpProxyPassword());

	SSORequest ssoRequest(networkProxy, *wengoAccount);
	return ssoRequest.launch();
}

void PhApiForWengoConnectingState::connectionProcess() {
	IConnectPlugin & iConnect = static_cast<IConnectPlugin &>(_stateContext);
	Account & account = iConnect.getAccount();
	WengoAccount * wengoAccount = static_cast<WengoAccount *>(account.getPrivateAccount());

	if (account.isConnectionConfigurationSet()) {
		// Connection configuration is available.
		// Trying to use these settings.
		if (wengoAccount->isHttpTunnelNeeded()) {
			_udpConnectionAttempt = MAX_UDPCONNECTION_ATTEMPT;
			if (wengoAccount->useSSLForHttpTunnel()) {
				_http80ConnectionAttempt = MAX_HTTP80CONNECTION_ATTEMPT;
				_http443ConnectionAttempt = MAX_HTTP443CONNECTION_ATTEMPT;
			} else if (wengoAccount->getHttpTunnelServerPort() == 443) {
				_http80ConnectionAttempt = MAX_HTTP80CONNECTION_ATTEMPT;
			}
		}
	}

	if (_udpConnectionAttempt < MAX_UDPCONNECTION_ATTEMPT) {
		// Tries to connect via UDP
		LOG_DEBUG("trying UDP connection");
		wengoAccount->setHttpTunnelNeeded(false);
		_udpConnectionAttempt++;
		connectAccount();
	} else if (_http80ConnectionAttempt < MAX_HTTP80CONNECTION_ATTEMPT) {
		// Tries with HTTP tunnel 80
		LOG_DEBUG("trying HTTP 80 connection");
		wengoAccount->setHttpTunnelNeeded(true);
		wengoAccount->setHttpTunnelServerPort(80);
		wengoAccount->setUseSSLForHttpTunnel(false);
		_http80ConnectionAttempt++;
		connectAccount();
	} else if (_http443ConnectionAttempt < MAX_HTTP443CONNECTION_ATTEMPT) {
		// Tries with HTTP tunnel 443
		LOG_DEBUG("trying HTTP 443 connection");
		wengoAccount->setHttpTunnelNeeded(true);
		wengoAccount->setHttpTunnelServerPort(443);
		wengoAccount->setUseSSLForHttpTunnel(false);
		_http443ConnectionAttempt++;
		connectAccount();
	} else if (_https443ConnectionAttempt < MAX_HTTPS443CONNECTION_ATTEMPT) {
		// Tries with HTTPS tunnel 443
		LOG_DEBUG("trying HTTPS 443 connection");
		wengoAccount->setHttpTunnelNeeded(true);
		wengoAccount->setHttpTunnelServerPort(443);
		wengoAccount->setUseSSLForHttpTunnel(true);
		_https443ConnectionAttempt++;
		connectAccount();
	} else {
		wengoAccount->setConnectionConfigurationSet(false);
		wengoAccount->setSSORequestMade(false);
		AccountMutator::emitAccountUpdatedSignal(account, EnumUpdateSection::UpdateSectionUnknown);

		// Restarting all connection process.
		iConnect.setState(EnumConnectionState::ConnectionStateDisconnected);
		iConnect.execute();
	}
}
