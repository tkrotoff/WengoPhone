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

ConnectHandler::ConnectHandler(UserProfile & userProfile)
	: _userProfile(userProfile) {

	_userProfile.newIMAccountAddedEvent +=
		boost::bind(&ConnectHandler::newIMAccountAddedEventHandler, this, _1, _2);
}

ConnectHandler::~ConnectHandler() {
	_userProfile.newIMAccountAddedEvent -=
		boost::bind(&ConnectHandler::newIMAccountAddedEventHandler, this, _1, _2);
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
		connect = new Connect(*actIMAccount, _userProfile);

		connect->connectedEvent +=
			boost::bind(&ConnectHandler::connectedEventHandler, this, _1);
		connect->disconnectedEvent +=
			boost::bind(&ConnectHandler::disconnectedEventHandler, this, _1, _2, _3);
		connect->connectionProgressEvent +=
			boost::bind(&ConnectHandler::connectionProgressEventHandler, this, _1, _2, _3, _4);

		_connectMap.insert(pair<IMAccount *, Connect *>(actIMAccount, connect));
	}

	else {
		connect = (*it).second;
	}

	if (!imAccount.isConnected()) {
		if (imAccount.getProtocol() == EnumIMProtocol::IMProtocolSIPSIMPLE) {
			// FIXME: currently there is only one SIP account so we are sure that
			// the connectSipAccounts will connect the Wengo account
			_userProfile.connectSipAccounts();
		}
		connect->connect();
	}
}

void ConnectHandler::disconnect(IMAccount & imAccount, bool now) {
	ConnectMap::const_iterator it = _connectMap.find(&imAccount);

	if (it != _connectMap.end()) {
		if (imAccount.isConnected()) {
			if (imAccount.getProtocol() == EnumIMProtocol::IMProtocolSIPSIMPLE) {
				// FIXME: currently there is only one SIP account so we are sure that
				// the connectSipAccounts will disconnect the Wengo account
				_userProfile.disconnectSipAccounts(now);
			}
			(*it).second->disconnect();
		}
	}
}

void ConnectHandler::connectedEventHandler(IMConnect & sender) {
	connectedEvent(*this, sender.getIMAccount());
}

void ConnectHandler::disconnectedEventHandler(IMConnect & sender, bool connectionError, const std::string & reason) {
	disconnectedEvent(*this, sender.getIMAccount(), connectionError, reason);
}

void ConnectHandler::connectionProgressEventHandler(IMConnect & sender, int currentStep, int totalSteps,
				const std::string & infoMessage) {

	connectionProgressEvent(*this, sender.getIMAccount(), currentStep, totalSteps, infoMessage);
}

void ConnectHandler::newIMAccountAddedEventHandler(UserProfile & sender, IMAccount & imAccount) {
	IMAccountSet::iterator it = _pendingConnections.find(imAccount);
	if (it != _pendingConnections.end()) {
		LOG_DEBUG("A connection was pending for this IMAccount. Releasing connection.");
		_actualIMAccount.insert(&imAccount);
		connect(imAccount);
		_pendingConnections.erase(it);
	} else {
		_actualIMAccount.insert(&imAccount);
	}

	imAccount.imAccountDeadEvent +=
		boost::bind(&ConnectHandler::imAccountDeadEventHandler, this, _1);
}

void ConnectHandler::imAccountDeadEventHandler(IMAccount & sender) {
	ConnectMap::iterator it = _connectMap.find(&sender);

	if (it != _connectMap.end()) {
		delete (*it).second;
		_connectMap.erase(it);
	} else {
		LOG_DEBUG("IMAccount not in ConnectHandler");
	}

	IMAccountPtrSet::iterator actIt = _actualIMAccount.find(&sender);
	if (actIt != _actualIMAccount.end()) {
		_actualIMAccount.erase(actIt);
	}

	IMAccountSet::iterator pendIt = _pendingConnections.find(sender);
	if (pendIt != _pendingConnections.end()) {
		_pendingConnections.erase(pendIt);
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
