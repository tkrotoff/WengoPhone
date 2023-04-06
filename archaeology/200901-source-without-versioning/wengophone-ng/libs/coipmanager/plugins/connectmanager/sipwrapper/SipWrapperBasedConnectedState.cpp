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

#include "SipWrapperBasedConnectedState.h"

#include <coipmanager/connectmanager/IConnectPlugin.h>

#include <coipmanager_base/account/SipAccount.h>

#include <sipwrapper/SipWrapper.h>

#include <util/Logger.h>
#include <util/SafeConnect.h>

SipWrapperBasedConnectedState::SipWrapperBasedConnectedState(IConnectPlugin & context, SipWrapper & sipWrapper)
	: QObject(),
	IConnectState(context),
	_sipWrapper(sipWrapper) {

	SAFE_CONNECT(&_sipWrapper,
		SIGNAL(phoneLineStateChangedSignal(int, EnumPhoneLineState::PhoneLineState)),
		SLOT(phoneLineStateChangedSlot(int, EnumPhoneLineState::PhoneLineState)));
}

void SipWrapperBasedConnectedState::connect() {
	//Do nothing, already connected.
}

void SipWrapperBasedConnectedState::disconnect() {
	IConnectPlugin & iConnect = static_cast<IConnectPlugin &>(_stateContext);

	iConnect.setState(EnumConnectionState::ConnectionStateDisconnecting);
	iConnect.execute();

	Account & account = iConnect.getAccount();
	SipAccount *sipAccount = static_cast<SipAccount *>(account.getPrivateAccount());
	_sipWrapper.removeVirtualLine(sipAccount->getVirtualLineId());
}

void SipWrapperBasedConnectedState::execute() {
	// Nothing to do. Wait for an event in phoneLineStateChangedSlot
}

void SipWrapperBasedConnectedState::retryToConnect() {
	IConnectPlugin & iConnect = static_cast<IConnectPlugin &>(_stateContext);
	iConnect.setState(EnumConnectionState::ConnectionStateConnecting);
	iConnect.execute();
}

void SipWrapperBasedConnectedState::phoneLineStateChangedSlot(int lineId, EnumPhoneLineState::PhoneLineState state) {
	IConnectPlugin & iConnect = static_cast<IConnectPlugin &>(_stateContext);
	Account & account = iConnect.getAccount();
	SipAccount * sipAccount = static_cast<SipAccount *>(account.getPrivateAccount());

	if (iConnect.getStateId() != ((int) EnumConnectionState::ConnectionStateConnected)) {
		// Current state is not ConnectionStateConnected, so this event is not for us
		return;
	}

	if (lineId != sipAccount->getVirtualLineId()) {
		// This is certainly not for our line so ignore
		return;
	}

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
	////

	default:
		LOG_ERROR("should not receive this line state here");
	}
}
