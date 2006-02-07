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

#ifndef CONNECT_H
#define CONNECT_H

#include <imwrapper/IMConnect.h>

#include <Interface.h>
#include <Event.h>

class IMAccount;

/**
 * Wrapper for Instant Messaging connection.
 *
 * @ingroup model
 * @author Tanguy Krotoff
 */
class Connect : Interface {
public:

	/**
	 * @see IMConnect::loginStatusEvent
	 */
	Event<void (IMConnect & sender, IMConnect::LoginStatus status)> loginStatusEvent;

	/**
	 * Status changed event handler.
	 *
	 * @param sender the sender
	 * @param status new status
	 */
	void loginStatusEventHandler(IMConnect & sender, IMConnect::LoginStatus status);


	Connect(IMAccount & account);

	virtual ~Connect() { }

	void connect();

	void disconnect();

	bool isConnected() { return _connected; }
	
	const IMAccount & getIMAccount() const {
		return _account;
	}

protected:

	IMAccount & _account;

	IMConnect * _imConnect;
	
	bool _connected;
};

#endif	//CONNECT_H
