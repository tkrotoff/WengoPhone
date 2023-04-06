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

#ifndef OWDISCONNECTEDSTATE_H
#define OWDISCONNECTEDSTATE_H

#include <coipmanager/connectmanager/IConnectState.h>

#include <coipmanager/coipmanagerdll.h>

/**
 * Account not connected state.
 *
 * This state counts the number of connection attempt, restart connection
 * if attempt < MAX_ATTEMPT or stop it telling IConnectPlugin to emit
 * ConnectionErrorTooMuchRetry error.
 *
 * - connect launches execute.
 * - disconnect does nothing.
 * - execute launches the connection process (switch to 'ConnectingState').
 *
 * @author Philippe Bernery
 */
class COIPMANAGER_API DisconnectedState : public IConnectState {
public:

	DisconnectedState(IConnectPlugin & context);

	virtual void connect();

	virtual void disconnect();

	virtual void execute();

private:
	
	/** Number of full connection attempt. */
	unsigned _connectionAttempt;

};

#endif //OWDISCONNECTEDSTATE_H
