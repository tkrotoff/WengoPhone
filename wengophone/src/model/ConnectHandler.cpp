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

#include <model/Connect.h>
#include <model/Chat.h>
#include <model/WengoPhone.h>
#include <model/imwrapper/IMAccount.h>
#include <model/imwrapper/EnumIMProtocol.h>
#include <model/presence/Presence.h>
#include <model/presence/PresenceHandler.h>

#include <Logger.h>

ConnectHandler::ConnectHandler(WengoPhone & wengoPhone, WengoAccount & wengoAccount)
	: _wengoPhone(wengoPhone),
	_presenceHandler(PresenceHandler::getInstance()) {

	wengoAccount.loginEvent += boost::bind(&ConnectHandler::wengoLoginEventHandler, this, _1, _2, _3, _4);

	//Read the configuration file
	/*
	for () {
		IMAccount * imAccount = new IMAccount(login, password, IMAccount::IMProtocolSIPSIMPLE);
		_accountList += imAccount;

		IMConnect * connect = IMWrapperFactory::getFactory().createIMConnect(*imAccount);
		_connectList += connect;
		connect->loginStatusEvent += boost::bind(&ConnectHandler::loginStatusEventHandler, this, _1, _2);
		connect->connect();
	}
	*/
}

ConnectHandler::~ConnectHandler() {
}

void ConnectHandler::wengoLoginEventHandler(WengoAccount & sender, WengoAccount::LoginState state,
			const std::string & login, const std::string & password) {

	switch (state) {
	case WengoAccount::LoginOk: {
		presenceHandlerCreatedEvent(*this, _presenceHandler);
		chatHandlerCreatedEvent(*this, _chatHandler);

		//Creates the connection to the IM service
		IMAccount * imAccount = new IMAccount(login, password, EnumIMProtocol::IMProtocolSIPSIMPLE);
		_accountList += imAccount;

		Connect * connect = new Connect(*imAccount);
		_connectList += connect;
		connect->loginStatusEvent += boost::bind(&ConnectHandler::loginStatusEventHandler, this, _1, _2);
		connect->connect();

		break;
	}

	case WengoAccount::LoginNetworkError:
		break;

	case WengoAccount::LoginPasswordError:
		break;

	default:
		LOG_FATAL("WengoAccount::LoginState unknown state");
		break;
	}
}

void ConnectHandler::loginStatusEventHandler(IMConnect & sender, IMConnect::LoginStatus status) {
	switch(status) {
	case IMConnect::LoginStatusConnected: {
		_chatHandler.connected(sender.getIMAccount());
		_presenceHandler.connected(sender.getIMAccount());
		break;
	}

	case IMConnect::LoginStatusPasswordError:
		break;

	case IMConnect::LoginStatusDisconnected:
		_chatHandler.disconnected(sender.getIMAccount());
		_presenceHandler.disconnected(sender.getIMAccount());
		break;

	default:
		LOG_FATAL("unknown status");
	}
}
