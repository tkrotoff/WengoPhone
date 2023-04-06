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

#include <coipmanager/connectmanager/DisconnectedState.h>

#include <coipmanager/AccountMutator.h>
#include <coipmanager/connectmanager/IConnectPlugin.h>

#include <coipmanager_base/EnumConnectionState.h>

#include <util/Logger.h>
#include <util/String.h>

static const unsigned MAX_CONNECTION_ATTEMPT = 5;

DisconnectedState::DisconnectedState(IConnectPlugin & context)
	: IConnectState(context) {
}

void DisconnectedState::connect() {
	_connectionAttempt = 0;
	execute();
}

void DisconnectedState::disconnect() {
	//Nothing to do in this state.
}

void DisconnectedState::execute() {
	IConnectPlugin & iConnect = static_cast<IConnectPlugin &>(_stateContext);

	_connectionAttempt++;
	if (_connectionAttempt <= MAX_CONNECTION_ATTEMPT) {
		LOG_DEBUG("starting connection process. Attempt " 
			+ String::fromNumber(_connectionAttempt) + "/"
			+ String::fromNumber(MAX_CONNECTION_ATTEMPT));

		_stateContext.setState(EnumConnectionState::ConnectionStateConnecting);
		_stateContext.execute();
	} else {
		// Stopping connection process. Resetting connection parameters.
		Account & account = iConnect.getAccount();
		account.setConnectionConfigurationSet(false);
		AccountMutator::emitAccountUpdatedSignal(account, EnumUpdateSection::UpdateSectionUnknown);
		iConnect.emitAccountConnectionErrorSignal(account.getUUID(),
			EnumConnectionError::ConnectionErrorTooMuchRetry);
	}
}
