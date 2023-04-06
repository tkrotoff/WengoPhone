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

#include "PurpleDisconnectedState.h"

#include "PurpleConnect.h"

#include <util/SafeConnect.h>

PurpleDisconnectedState::PurpleDisconnectedState(IConnectPlugin & context)
	: DisconnectedState(context) {

	_canConnect = false;
	_mustConnect = false;

	PurpleConnect & purpleConnect = static_cast<PurpleConnect &>(context);
	SAFE_CONNECT(&purpleConnect, SIGNAL(accountReadySignal()), SLOT(accountReadySlot()));
}

void PurpleDisconnectedState::connect() {
	if (_canConnect) {
		DisconnectedState::connect();
	} else {
		_mustConnect = true;
	}
}

void PurpleDisconnectedState::accountReadySlot() {
	_canConnect = true;

	if (_mustConnect) {
		DisconnectedState::connect();
		_mustConnect = false;
	}
}
