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

#ifndef OWSIPWRAPPERBASEDCONNECTEDSTATE_H
#define OWSIPWRAPPERBASEDCONNECTEDSTATE_H

#include <coipmanager/connectmanager/IConnectState.h>

#include <sipwrapper/EnumPhoneLineState.h>

#include <QtCore/QObject>

class SipWrapper;

/**
 * "Account is connected" state.
 *
 * This state monitors the connection and tries to reconnect 
 * an account if necessary.
 *
 * - connect does nothing.
 * - disconnect disconnects the Account.
 * - execute does nothing.
 *
 * @author Philippe Bernery
 */
class SipWrapperBasedConnectedState : public QObject, public IConnectState {
	Q_OBJECT
public:

	SipWrapperBasedConnectedState(IConnectPlugin & context, SipWrapper & _sipWrapper);

	virtual void connect();

	virtual void disconnect();

	virtual void execute();

private Q_SLOTS:

	void phoneLineStateChangedSlot(int lineId, EnumPhoneLineState::PhoneLineState state);

private:

	void retryToConnect();

	SipWrapper & _sipWrapper;

};

#endif //OWSIPWRAPPERBASEDCONNECTEDSTATE_H
