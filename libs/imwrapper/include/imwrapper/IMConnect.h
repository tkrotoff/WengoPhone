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

#ifndef IMCONNECT_H
#define IMCONNECT_H

#include <Interface.h>
#include <Event.h>

class IMAccount;

/**
 * Wrapper for Instant Messaging connection.
 *
 * @author Philippe Bernery
 * @author Tanguy Krotoff
 */
class IMConnect : Interface {
public:

	enum LoginStatus {
		/** Connected, login is OK. */
		LoginStatusConnected,

		/** login/password is incorrect. */
		LoginStatusPasswordError,

		/** A network error occured. */
		LoginStatusDisconnected
	};

	/**
	 * Status changed.
	 *
	 * @param sender this class
	 * @param status new status
	 */
	Event<void (IMConnect & sender, LoginStatus status)> loginStatusEvent;

	virtual ~IMConnect() { }

	virtual void connect() = 0;

	virtual void disconnect() = 0;

	IMAccount & getIMAccount() const {
		return _account;
	}

protected:

	IMConnect(IMAccount & account) : _account(account) {}

	IMAccount & _account;
};

#endif	//IMCONNECT_H
