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

#ifndef CCONNECTHANDLER_H
#define CCONNECTHANDLER_H

#include <imwrapper/IMConnect.h>

#include <util/NonCopyable.h>
#include <util/Event.h>

class IMChat;
class IMConnect;
class ConnectHandler;
class PConnectHandler;

/**
 *
 * @ingroup model
 * @author Tanguy Krotoff
 * @author Philippe Bernery
 */
class CConnectHandler : NonCopyable {
public:

	CConnectHandler(ConnectHandler & connectHandler);

	~CConnectHandler();

	/**
	 * @see IMConnect::loginStatusEvent
	 */
	Event<void (IMConnect & sender, IMConnect::LoginStatus status)> loginStatusEvent;

private:

	ConnectHandler & _connectHandler;

	PConnectHandler * _pConnectHandler;

};

#endif	//CCONNECTHANDLER_H
