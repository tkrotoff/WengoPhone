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

#ifndef OWSIPWRAPPERBASEDCONNECTINGSTATE_H
#define OWSIPWRAPPERBASEDCONNECTINGSTATE_H

#include <coipmanager/connectmanager/IConnectState.h>

#include <sipwrapper/EnumPhoneLineState.h>

#include <QtCore/QObject>

class SipWrapper;

/**
 * Sip account connection state.
 *
 * - connect does nothing.
 * - disconnect does nothing.
 * - execute connects the Account.
 *
 * @author Philippe Bernery
 */
class SipWrapperBasedConnectingState : public QObject, public IConnectState {
	Q_OBJECT
public:

	SipWrapperBasedConnectingState(IConnectPlugin & context, SipWrapper & sipWrapper);

	virtual void connect();

	virtual void disconnect();

	virtual void execute();

protected Q_SLOTS:

		/**
		 * Launches the connection process.
		 * connectionProcess will try 3 times to connect via UDP
		 * to the Sip server. If unsuccessful, it will try to connect
		 * via Http (and Https) if the protocol supports it.
		 */
		virtual void connectionProcess();

private Q_SLOTS:

	void phoneLineStateChangedSlot(int lineId, EnumPhoneLineState::PhoneLineState state);

protected:

	/** Resets attempt counters. */
	virtual void resetAttemptCounter();

	/**
	 * Launches the connection of the Account with parameters set on SipAccount.
	 */
	void connectAccount();

private:

	/**
	 * Retry to connect.
	 * Schedule a reconnection process.
	 */
	void retryToConnect();

	/** Number of connection attempt. */
	unsigned _udpConnectionAttempt;

	SipWrapper & _sipWrapper;
};

#endif //OWSIPWRAPPERBASEDCONNECTINGSTATE_H
