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

#ifndef CONNECTHANDLER_H
#define CONNECTHANDLER_H

#include <model/imwrapper/IMConnect.h>
#include <model/account/wengo/WengoAccount.h>
#include <model/ChatHandler.h>

#include <NonCopyable.h>
#include <List.h>
#include <Event.h>

class IMAccount;
class Connect;
class Chat;
class PresenceHandler;
class WengoPhone;
class PresenceHandler;
class ChatHandler;

/**
 *
 * @ingroup model
 * @author Tanguy Krotoff
 * @author Philippe Bernery
 */
class ConnectHandler : NonCopyable {
public:

	/**
	 * @see IMConnect::loginStatusEvent
	 */
	Event<void (IMConnect & sender, IMConnect::LoginStatus status)> loginStatusEvent;

	/**
	 * PresenceHandler has been created.
	 *
	 * @param sender this class
	 * @param imHandler ConnectHandler created
	 */
	Event<void (ConnectHandler & sender, PresenceHandler & presenceHandler)> presenceHandlerCreatedEvent;

	/**
	 * ChatHandler has been created.
	 *
	 * @param sender this class
	 * @param imHandler ConnectHandler created
	 */
	Event<void (ConnectHandler & sender, ChatHandler & chatHandler)> chatHandlerCreatedEvent;

	ConnectHandler(WengoPhone & wengoPhone, WengoAccount & wengoAccount);

	~ConnectHandler();

private:

	void wengoLoginEventHandler(WengoAccount & sender, WengoAccount::LoginState state,
				const std::string & login, const std::string & password);

	void loginStatusEventHandler(IMConnect & sender, IMConnect::LoginStatus status);

	List<IMAccount *> _accountList;

	List<Connect *> _connectList;

	WengoPhone & _wengoPhone;
	
	PresenceHandler  & _presenceHandler;
	
	ChatHandler _chatHandler;
	
};

#endif	//CONNECTHANDLER_H
