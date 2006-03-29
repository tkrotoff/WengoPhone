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

#include <util/Interface.h>
#include <util/Event.h>

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


	/**
	 * Status changed.
	 *
	 * @param sender this class
	 * @param totalSteps number of steps during the connection
	 * @param curStep the connection current step
	 * @param infoMsg description of the current step
	 */
	Event<void (IMConnect & sender, int totalSteps, int curStep, const std::string & infoMsg)> connectionStatusEvent;

	virtual ~IMConnect() { }

	virtual void connect() = 0;

	virtual void disconnect() = 0;

	IMAccount & getIMAccount() const {
		return _imAccount;
	}

protected:

	IMConnect(IMAccount & account) : _imAccount(account) {}

	IMAccount & _imAccount;
};

#endif	//IMCONNECT_H
