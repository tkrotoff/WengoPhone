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

#include "SipWrapperBasedDisconnectingState.h"

#include <coipmanager/connectmanager/IConnectPlugin.h>

#include <coipmanager_base/account/SipAccount.h>

#include <sipwrapper/SipWrapper.h>

#include <util/Logger.h>
#include <util/SafeConnect.h>

SipWrapperBasedDisconnectingState::SipWrapperBasedDisconnectingState(IConnectPlugin & context, SipWrapper & sipWrapper)
	: QObject(),
	IConnectState(context),
	_sipWrapper(sipWrapper) {

	SAFE_CONNECT(&_sipWrapper,
		SIGNAL(phoneLineStateChangedSignal(int, EnumPhoneLineState::PhoneLineState)),
		SLOT(phoneLineStateChangedSlot(int, EnumPhoneLineState::PhoneLineState)));
}

void SipWrapperBasedDisconnectingState::connect() {
	//Do nothing, already connected.
}

void SipWrapperBasedDisconnectingState::disconnect() {
	//Do nothing.
}

void SipWrapperBasedDisconnectingState::execute() {
	// Nothing to do. Wait for an event in phoneLineStateChangedSlot
}

void SipWrapperBasedDisconnectingState::phoneLineStateChangedSlot(int lineId, EnumPhoneLineState::PhoneLineState state) {
	IConnectPlugin & iConnect = static_cast<IConnectPlugin &>(_stateContext);
	Account & account = iConnect.getAccount();
	SipAccount * sipAccount = static_cast<SipAccount *>(account.getPrivateAccount());

	if (iConnect.getStateId() != ((int) EnumConnectionState::ConnectionStateDisconnecting)) {
		// Current state is not ConnectionStateConnected, so this event is not for us
		return;
	}

	if (lineId != sipAccount->getVirtualLineId()) {
		// This is certainly not for our line so ignore
		return;
	}

	switch (state) {
	// Disconnection
	case EnumPhoneLineState::PhoneLineStateClosed:
		sipAccount->setVirtualLineId(0);
		AccountMutator::emitAccountUpdatedSignal(account, EnumUpdateSection::UpdateSectionVolatileData);
		iConnect.setState(EnumConnectionState::ConnectionStateDisconnected);
		break;
	////

	default:
		LOG_ERROR("should not receive this line state here");
	}
}
