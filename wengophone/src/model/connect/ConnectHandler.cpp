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

#include "ConnectHandler.h"

#include <model/WengoPhone.h>
#include <model/connect/Connect.h>
#include <model/presence/Presence.h>
#include <model/presence/PresenceHandler.h>

#include <imwrapper/IMAccount.h>
#include <imwrapper/EnumIMProtocol.h>

#include <Logger.h>

using namespace std;

ConnectHandler::ConnectHandler(WengoPhone & wengoPhone) {
	wengoPhone.newIMAccountAddedEvent +=
		boost::bind(&ConnectHandler::newIMAccountAddedEventHandler, this, _1, _2);
}

ConnectHandler::~ConnectHandler() {
}

void ConnectHandler::connect(const IMAccount & imAccount) {
	IMAccountSet::const_iterator actIt = _actualIMAccount.find(imAccount);
	if (actIt == _actualIMAccount.end()) {
		LOG_DEBUG("This IMAccount has not yet been added. Pending connection.");
		_pendingConnections.insert(imAccount);
		return;
	}

	Connect * connect = NULL;
	ConnectMap::const_iterator it = _connectMap.find(imAccount);

	if (it == _connectMap.end()) {
		connect = new Connect((IMAccount &)imAccount);
		connect->loginStatusEvent +=
			boost::bind(&ConnectHandler::loginStatusEventHandler, this, _1, _2);
		_connectMap.insert(pair<IMAccount, Connect *>(imAccount, connect));
	} else {
		connect = (*it).second;	
	}

	connect->connect();
}

void ConnectHandler::disconnect(const IMAccount & imAccount) {
	ConnectMap::const_iterator it = _connectMap.find(imAccount);

	if (it != _connectMap.end()) {
		(*it).second->disconnect();
	}
}

void ConnectHandler::loginStatusEventHandler(IMConnect & sender,
	IMConnect::LoginStatus status) {
	switch(status) {
	case IMConnect::LoginStatusConnected: {
		connectedEvent(*this, sender.getIMAccount());
		break;
	}

	case IMConnect::LoginStatusPasswordError:
		break;

	case IMConnect::LoginStatusDisconnected:
		disconnectedEvent(*this, sender.getIMAccount());
		break;

	default:
		LOG_FATAL("unknown status");
	}
}

void ConnectHandler::newIMAccountAddedEventHandler(WengoPhone & sender, IMAccount & imAccount) {
	IMAccountSet::iterator it = _pendingConnections.find(imAccount);
	if (it != _pendingConnections.end()) {
		LOG_DEBUG("A connection was pending for this IMAccount. Releasing connection.");
		_actualIMAccount.insert(imAccount);
		connect(imAccount);
		_pendingConnections.erase(it);
	} else {
		_actualIMAccount.insert(imAccount);
	}
}

