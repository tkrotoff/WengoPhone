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

#include <networkdiscovery/NetworkObserver.h>

#include <util/Logger.h>
#include <util/SafeConnect.h>

#include <netlib.h>

NetworkObserver::NetworkObserver() {
	_isConnected = is_connection_available();

	SAFE_CONNECT(&_timer, SIGNAL(timeout()), SLOT(timeoutSlot()));
	_timer.start(500);

	timeoutSlot();
}

NetworkObserver * NetworkObserver::_staticInstance = NULL;

NetworkObserver & NetworkObserver::getInstance() {
	if (!_staticInstance) {
		_staticInstance = new NetworkObserver();
	}

	return * _staticInstance;
}

NetworkObserver::~NetworkObserver() {
	_timer.stop();
}

bool NetworkObserver::isConnected() const {
	return _isConnected;
}

void NetworkObserver::timeoutSlot() {
	if (is_connection_available()) {
		if (!_isConnected) {
			LOG_DEBUG("connection is up");
			_isConnected = true;
			connectionIsUpSignal();
		}
	} else {
		if (_isConnected) {
			LOG_DEBUG("connection is down");
			_isConnected = false;
			connectionIsDownSignal();
		}
	}
}
