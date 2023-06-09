/*
 * WengoPhone, a voice over Internet phone
 * Copyright (C) 2004-2007  Wengo
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

#include <thread/Mutex.h>
#include <thread/Timer.h>

#include <util/Event.h>
#include <util/NonCopyable.h>
#include <util/Trackable.h>

class UserProfile;

/**
 * Wrapper for Instant Messaging connection.
 *
 * @ingroup model
 * @author Tanguy Krotoff
 * @author Philippe Bernery
 */
class Connect : NonCopyable, public Trackable {
public:

	/**
	 * @see IMConnect::connectedEvent
	 */
	Event<void (IMConnect & sender)> connectedEvent;

	/**
	 * @see IMConnect::disconnectedEvent
	 */
	Event<void (IMConnect & sender,
		bool connectionError, std::string reason)> disconnectedEvent;

	/**
	 * @see IMConnect::connectionProgressEvent
	 */
	Event<void (IMConnect & sender, int currentStep,
		int totalSteps, std::string infoMessage)> connectionProgressEvent;

	Connect(IMAccount account, UserProfile & userProfile);

	virtual ~Connect();

	void connect();

	void disconnect();

	IMAccount getIMAccount() const {
		return _imAccount;
	}

	/**
	 * Tells Connect to unregister to all registered Events.
	 */
	void unregisterAllEvents();

protected:

	/**
	 * Used disconnectedEventHandler to launch reconnection
	 * without resetting _connectionRetryCount to 0.
	 */
	void autoConnect();

	/**
	 * @see IMConnect::connectedEvent
	 */
	void connectedEventHandler(IMConnect & sender);

	/**
	 * @see IMConnect::disconnectedEvent
	 */
	void disconnectedEventHandler(IMConnect & sender,
		bool connectionError, std::string reason);

	/**
	 * @see IMConnect::connectionProgressEvent
	 */
	void connectionProgressEventHandler(IMConnect & sender,
		int currentStep, int totalSteps, std::string infoMessage);

	void timeoutEventHandler(Timer & sender);

	bool _timerIsRunning;

	bool _isConnecting;

	bool _isDisconnecting;

	/** Count how many time we retry to connect. */
	unsigned _connectionRetryCount;

	IMAccount _imAccount;

	IMConnect * _imConnect;

	UserProfile & _userProfile;

	Mutex _mutex;

	Timer _timer;
};

#endif	//CONNECT_H
