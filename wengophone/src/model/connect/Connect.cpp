/*
 * WengoPhone, a voice over Internet phone
 * Copyright (C) 2004-2006  Wengo
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

#include "Connect.h"

#include <imwrapper/IMWrapperFactory.h>

#include <model/network/NetworkObserver.h>
#include <model/profile/UserProfile.h>

#include <util/Logger.h>

const unsigned RECONNECT_INTERVAL = 20000;
const unsigned RECONNECT_RETRY = 10;

Connect::Connect(IMAccount & account, UserProfile & userProfile)
	: _imAccount(account),
	_userProfile(userProfile) {

	_imConnect = IMWrapperFactory::getFactory().createIMConnect(account);

	_imConnect->connectedEvent += connectedEvent;
	_imConnect->connectedEvent +=
		boost::bind(&Connect::connectedEventHandler, this, _1);

	_imConnect->disconnectedEvent += disconnectedEvent;
	_imConnect->disconnectedEvent +=
		boost::bind(&Connect::disconnectedEventHandler, this, _1, _2, _3);

	_imConnect->connectionProgressEvent += connectionProgressEvent;

	_timerIsRunning = false;
	_timer.timeoutEvent +=
		boost::bind(&Connect::timeoutEventHandler, this, _1);
}

Connect::~Connect() {
	_timer.stop();
	delete _imConnect;
}

void Connect::connect() {
	_connectionRetryCount = 0;

	autoConnect();
}

void Connect::disconnect(bool now) {
	_timer.stop();
	_timerIsRunning = false;
	if ((_imAccount.getProtocol() == EnumIMProtocol::IMProtocolSIPSIMPLE)
		|| (_imAccount.getProtocol() == EnumIMProtocol::IMProtocolWengo)) {
        _userProfile.disconnectSipAccounts(now);
	}
	_imConnect->disconnect();
}

void Connect::timeoutEventHandler(Timer & sender) {
	if (_connectionRetryCount < RECONNECT_RETRY && NetworkObserver::getInstance().isConnected()) {
		++_connectionRetryCount;
		if ((_imAccount.getProtocol() == EnumIMProtocol::IMProtocolSIPSIMPLE)
			|| (_imAccount.getProtocol() == EnumIMProtocol::IMProtocolWengo)) {
            _userProfile.connectSipAccounts();
		}
		_imConnect->connect();
	} else {
		// Either the user as cancelled the connection
		// nor the retry count has been exceeded
		_timer.stop();
		_timerIsRunning = false;
		_connectionRetryCount = 0;
	}
}

void Connect::autoConnect() {
	if (!_timerIsRunning && (_connectionRetryCount < RECONNECT_RETRY)) {
		_timerIsRunning = true;
		_timer.start(0, RECONNECT_INTERVAL, 0);
	}
}

void Connect::connectedEventHandler(IMConnect & sender) {
	_timer.stop();
	_timerIsRunning = false;
	_connectionRetryCount = 0;
}

void Connect::disconnectedEventHandler(IMConnect & sender, bool connectionError, const std::string & reason) {
	// We check if this is a user disconnection or a error disconnection.
	// If so, we try to reconnect
	if (connectionError) {
		autoConnect();
	}
}
