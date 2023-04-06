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

#include "PhApiForSipConnect.h"

#include "../SipWrapperBasedConnectingState.h"
#include "../SipWrapperBasedConnectedState.h"
#include "../SipWrapperBasedDisconnectingState.h"

#include <coipmanager/connectmanager/DisconnectedState.h>

#include <coipmanager_base/EnumConnectionState.h>

#include <PhApiWrapper.h>

PhApiForSipConnect::PhApiForSipConnect(CoIpManager & coIpManager, const Account & account)
	: IConnectPlugin(coIpManager, account) {

	SipWrapper *sipWrapper = PhApiWrapper::getInstance();
	addState(EnumConnectionState::ConnectionStateDisconnected, new DisconnectedState(*this));
	addState(EnumConnectionState::ConnectionStateConnecting, new SipWrapperBasedConnectingState(*this, *sipWrapper));
	addState(EnumConnectionState::ConnectionStateConnected, new SipWrapperBasedConnectedState(*this, *sipWrapper));
	addState(EnumConnectionState::ConnectionStateDisconnecting, new SipWrapperBasedDisconnectingState(*this, *sipWrapper));

	setState(EnumConnectionState::ConnectionStateDisconnected);
}

PhApiForSipConnect::~PhApiForSipConnect() {
}

bool PhApiForSipConnect::checkValidity() {
	return true;
}
