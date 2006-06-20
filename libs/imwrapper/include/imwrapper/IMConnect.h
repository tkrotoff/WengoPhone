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

#include "IMAccount.h"

#include <util/Event.h>
#include <util/Interface.h>
#include <util/Trackable.h>

/**
 * Wrapper for Instant Messaging connection.
 *
 * @author Philippe Bernery
 * @author Tanguy Krotoff
 */
class IMConnect : Interface, public Trackable {
public:

	/**
	 * Connection event.
	 *
	 * @param sender this class
	 */
	Event<void (IMConnect & sender)> connectedEvent;

	/**
	 * Disconnection event.
	 *
	 * @param sender this class
	 * @param connectionError true if an error occured; false otherwise
	 * @param reason reason for the connection failure
	 */
	Event<void (IMConnect & sender, bool connectionError, const std::string & reason)> disconnectedEvent;

	/**
	 * Connection progression status changed.
	 *
	 * @param sender this class
	 * @param currentStep the connection current step
	 * @param totalSteps number of steps during the connection
	 * @param infoMessage description of the current step
	 */
	Event<void (IMConnect & sender, int currentStep, int totalSteps, const std::string & infoMessage)> connectionProgressEvent;

	virtual ~IMConnect() { }

	virtual void connect() = 0;

	virtual void disconnect() = 0;

	IMAccount & getIMAccount() const {
		return _imAccount;
	}

protected:

	IMConnect(IMAccount & account) : _imAccount(account) {
		connectedEvent += boost::bind(&IMConnect::connectedEventHandler, this, _1);
		disconnectedEvent += boost::bind(&IMConnect::disconnectedEventHandler, this, _1, _2, _3);
	}

	void connectedEventHandler(IMConnect & sender) {
		_imAccount.setConnected(true);
	}

	void disconnectedEventHandler(IMConnect & sender,
		bool connectionError, const std::string & reason) {
		_imAccount.setConnected(false);
	}

	IMAccount & _imAccount;
};

#endif	//IMCONNECT_H
