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

#include <model/connect/Connect.h>
#include <model/presence/Presence.h>
#include <model/presence/PresenceHandler.h>
#include <model/profile/UserProfile.h>

#include <imwrapper/IMAccount.h>
#include <imwrapper/EnumIMProtocol.h>

#include <util/Logger.h>

using namespace std;

ConnectHandler::ConnectHandler(UserProfile & userProfile) {
	userProfile.newIMAccountAddedEvent +=
		boost::bind(&ConnectHandler::newIMAccountAddedEventHandler, this, _1, _2);
	userProfile.imAccountRemovedEvent +=
		boost::bind(&ConnectHandler::imAccountRemovedEventHandler, this, _1, _2);
}

ConnectHandler::~ConnectHandler() {
}

void ConnectHandler::connect(IMAccount & imAccount) {
	IMAccount * actIMAccount = findIMAccount(_actualIMAccount, imAccount);
	if (!actIMAccount) {
		LOG_DEBUG("This IMAccount has not yet been added. Pending connection.");
		_pendingConnections.insert(imAccount);
		return;
	}

	Connect * connect = NULL;
	ConnectMap::const_iterator it = _connectMap.find(actIMAccount);

	if (it == _connectMap.end()) {
		connect = new Connect(*actIMAccount);
		connect->loginStatusEvent +=
			boost::bind(&ConnectHandler::loginStatusEventHandler, this, _1, _2);
		_connectMap.insert(pair<IMAccount *, Connect *>(actIMAccount, connect));
	} else {
		connect = (*it).second;
	}

	if (!imAccount.isConnected()) {
		connect->connect();
	}
}

void ConnectHandler::disconnect(IMAccount & imAccount) {
	ConnectMap::const_iterator it = _connectMap.find(&imAccount);

	if (it != _connectMap.end()) {
		if (imAccount.isConnected()) {
			(*it).second->disconnect();
		}
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

void ConnectHandler::newIMAccountAddedEventHandler(UserProfile & sender, IMAccount & imAccount) {
	IMAccountSet::const_iterator it = _pendingConnections.find(imAccount);
	if (it != _pendingConnections.end()) {
		LOG_DEBUG("A connection was pending for this IMAccount. Releasing connection.");
		_actualIMAccount.insert(&imAccount);
		connect(imAccount);
		_pendingConnections.erase(it);
	} else {
		_actualIMAccount.insert(&imAccount);
	}
}

void ConnectHandler::imAccountRemovedEventHandler(UserProfile & sender, IMAccount & imAccount) {
	ConnectMap::iterator it = _connectMap.find(&imAccount);

	if (it != _connectMap.end()) {
		delete (*it).second;
		_connectMap.erase(it);
	} else {
		LOG_DEBUG("IMAccount not in ConnectHandler");
	}
}

IMAccount * ConnectHandler::findIMAccount(const IMAccountPtrSet & set, const IMAccount & imAccount) {
	IMAccount * result = NULL;

	for (IMAccountPtrSet::const_iterator it = set.begin();
		it != set.end();
		++it) {
		if (*(*it) == imAccount) {
			result = *it;
			break;
		}
	}

	return result;
}
