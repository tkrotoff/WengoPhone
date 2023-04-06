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

#include "SipWrapperBasedConnectingState.h"

#include <coipmanager/CoIpManager.h>
#include <coipmanager/CoIpManagerConfig.h>
#include <coipmanager/connectmanager/IConnectPlugin.h>

#include <coipmanager_base/account/SipAccount.h>

#include <networkdiscovery/NetworkDiscovery.h>
#include <networkdiscovery/NetworkProxyDiscovery.h>

#include <sipwrapper/SipWrapper.h>

#include <util/Logger.h>
#include <util/SafeConnect.h>
#include <util/String.h>

#include <QtCore/QTimer>

static const unsigned MAX_UDPCONNECTION_ATTEMPT = 3;
static const unsigned INTERVAL_BETWEEN_CONNECTION = 5000;
static const std::string STUN_SERVER = "stun.wengo.fr";

SipWrapperBasedConnectingState::SipWrapperBasedConnectingState(IConnectPlugin & context, SipWrapper & sipWrapper)
	: QObject(), IConnectState(context), _sipWrapper(sipWrapper) {

	resetAttemptCounter();

	SAFE_CONNECT(&_sipWrapper,
		SIGNAL(phoneLineStateChangedSignal(int, EnumPhoneLineState::PhoneLineState)),
		SLOT(phoneLineStateChangedSlot(int, EnumPhoneLineState::PhoneLineState)));
}

void SipWrapperBasedConnectingState::resetAttemptCounter() {
	_udpConnectionAttempt = 0;
}

void SipWrapperBasedConnectingState::connect() {
	// Do nothing in this state.
}

void SipWrapperBasedConnectingState::disconnect() {
	// Do nothing in this state.
}

void SipWrapperBasedConnectingState::execute() {
	IConnectPlugin & iConnect = static_cast<IConnectPlugin &>(_stateContext);
	Account & account = iConnect.getAccount();

	unsigned currentConnectionStep = iConnect.getCurrentConnectionStep();
	iConnect.emitAccountConnectionProgressSignal(account.getUUID(),
		++currentConnectionStep, iConnect.getConnectionSteps(), "connecting (register)");
	iConnect.setCurrentConnectionStep(currentConnectionStep);

	resetAttemptCounter();

	connectionProcess();
}

void SipWrapperBasedConnectingState::connectionProcess() {
	IConnectPlugin & iConnect = static_cast<IConnectPlugin &>(_stateContext);
	Account & account = iConnect.getAccount();
	SipAccount * sipAccount = static_cast<SipAccount *>(account.getPrivateAccount());

	if (_udpConnectionAttempt < MAX_UDPCONNECTION_ATTEMPT) {
		// Tries to connect via UDP
		LOG_DEBUG("trying UDP connection");
		sipAccount->setHttpTunnelNeeded(false);
		_udpConnectionAttempt++;
		connectAccount();
	} else {
		// Restarting all connection process.
		iConnect.setState(EnumConnectionState::ConnectionStateDisconnected);
		iConnect.execute();
	}
}

void SipWrapperBasedConnectingState::retryToConnect() {
	LOG_DEBUG("scheduling a connection retry");
	QTimer::singleShot(INTERVAL_BETWEEN_CONNECTION, this, SLOT(connectionProcess()));
}

void SipWrapperBasedConnectingState::connectAccount() {
	IConnectPlugin & iConnect = static_cast<IConnectPlugin &>(_stateContext);
	Account & account = iConnect.getAccount();
	SipAccount * sipAccount = static_cast<SipAccount *>(account.getPrivateAccount());

	_sipWrapper.setHTTPProxy(iConnect.getCoIpManager().getCoIpManagerConfig().getHttpProxyServer(),
		iConnect.getCoIpManager().getCoIpManagerConfig().getHttpProxyServerPort(),
		iConnect.getCoIpManager().getCoIpManagerConfig().getHttpProxyLogin(),
		iConnect.getCoIpManager().getCoIpManagerConfig().getHttpProxyPassword());

	if (sipAccount->isHttpTunnelNeeded()) {
		std::string server = sipAccount->getHttpTunnelServerHostname();
		if (sipAccount->useSSLForHttpTunnel()) {
			server = sipAccount->getHttpsTunnelServerHostname();
		}
		_sipWrapper.setTunnel(server,
			sipAccount->getHttpTunnelServerPort(),
			sipAccount->useSSLForHttpTunnel());
	}

	// Getting Nat Type
	EnumNatType::NatType natType = EnumNatType::NatTypeFullCone;
	bool natFailed = true;
 	if (NetworkDiscovery::getInstance().testNAT(STUN_SERVER, natType)) {
		if ((natType != EnumNatType::NatTypeBlocked) &&
			(natType != EnumNatType::NatTypeFailure)) {
			natFailed = false;
		}
	}

	if (natFailed) {
		_sipWrapper.setNatType(EnumNatType::NatTypeFullCone);
	} else {
		_sipWrapper.setNatType(natType);
	}
	////

	int lineId = _sipWrapper.addVirtualLine(
		sipAccount->getDisplayName(), //displayname
		sipAccount->getUsername(), //username
		sipAccount->getIdentity(), //identity
		sipAccount->getPassword(), //pwd
		sipAccount->getRealm(), //realm
		sipAccount->getSIPProxyServerHostname() + ":" + String::fromNumber(sipAccount->getSIPProxyServerPort()),
		sipAccount->getRegisterServerHostname() /*+ ":" + String::fromNumber(sipAccount->getRegisterServerPort())*/ //registerserver
		);

	if (lineId == SipWrapper::VirtualLineIdError) {
		LOG_ERROR("couldn't create the phone line");
		retryToConnect();
	} else {
		LOG_DEBUG("virtual line with lineId=" + String::fromNumber(lineId) + " created");
		sipAccount->setVirtualLineId(lineId);
		AccountMutator::emitAccountUpdatedSignal(account, EnumUpdateSection::UpdateSectionVolatileData);
		int ret = _sipWrapper.registerVirtualLine(lineId);
		if (!ret) {
			LOG_ERROR("couldn't register lineId=" + String::fromNumber(lineId));
		}
	}
}

void SipWrapperBasedConnectingState::phoneLineStateChangedSlot(int lineId, EnumPhoneLineState::PhoneLineState state) {
	IConnectPlugin & iConnect = static_cast<IConnectPlugin &>(_stateContext);
	Account & account = iConnect.getAccount();
	SipAccount * sipAccount = static_cast<SipAccount *>(account.getPrivateAccount());

	if (iConnect.getStateId() != ((int) EnumConnectionState::ConnectionStateConnecting)) {
		// Current state is not ConnectionState, so this event is not for us
		return;
	}

	if (lineId != sipAccount->getVirtualLineId()) {
		// This is certainly not for our line so ignore
		return;
	}

	unsigned currentConnectionStep = iConnect.getCurrentConnectionStep();
	unsigned connectionSteps = iConnect.getConnectionSteps(); 

	switch (state) {
	// Error cases
	case EnumPhoneLineState::PhoneLineStateUnknown:
		LOG_ERROR(EnumConnectionError::toString(EnumConnectionError::ConnectionErrorUnknown));
		retryToConnect();
		break;
	case EnumPhoneLineState::PhoneLineStateServerError:
		LOG_ERROR(EnumConnectionError::toString(EnumConnectionError::ConnectionErrorServer));
		retryToConnect();
		break;
	case EnumPhoneLineState::PhoneLineStateTimeout:
		LOG_ERROR(EnumConnectionError::toString(EnumConnectionError::ConnectionErrorTimeout));
		retryToConnect();
		break;
	case EnumPhoneLineState::PhoneLineStateAuthenticationError:
		iConnect.emitAccountConnectionErrorSignal(account.getUUID(),
			EnumConnectionError::ConnectionErrorInvalidLoginPassword);
		iConnect.setState(EnumConnectionState::ConnectionStateDisconnected);
		break;
	////

	// Connection Progress
	case EnumPhoneLineState::PhoneLineStateProgress:
		LOG_DEBUG("registering");
		//TODO: do we have information about the progress here?
		/*iConnect.emitAccountConnectionProgressSignal(account.getUUID(),
			++currentConnectionStep, connectionSteps, "connecting to server");
		iConnect.setCurrentConnectionStep(currentConnectionStep);*/
		break;
	////

	// Connected
	case EnumPhoneLineState::PhoneLineStateOk:
		account.setConnectionConfigurationSet(true);
		// Save data set in connectionProcess
		AccountMutator::emitAccountUpdatedSignal(account, EnumUpdateSection::UpdateSectionUnknown);

		iConnect.setState(EnumConnectionState::ConnectionStateConnected);
		break;
	////

	default:
		LOG_ERROR("should not receive this line state here");
	}
}
