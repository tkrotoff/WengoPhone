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

#ifndef OWPURPLECONNECTINGSTATE_H
#define OWPURPLECONNECTINGSTATE_H

#include <coipmanager/connectmanager/IConnectState.h>

extern "C" {
#include <libpurple/account.h>
}

#include <QtCore/QMutex>

/**
 * LibPurple account connection state.
 *
 * - connect does nothing.
 * - disconnect does nothing.
 * - execute connects the Account.
 *
 * @author Philippe Bernery
 */
class PurpleConnectingState : public IConnectState {
public:

	PurpleConnectingState(IConnectPlugin & context);
	~PurpleConnectingState();

	virtual void connect();

	virtual void disconnect();

	virtual void execute();

	static bool executeCbk(void * data);

private:

	/**
	 * Checks connection configuration of Account.
	 *
	 * This methods will look into Account which connection type
	 * has been used for the last connection.
	 * If this is the normal connection (usual port), nothing is done.
	 * If this is the alternate one, _nbConnectionTry is set to 1. This will
	 * force the use of the alternate connection type. 
	 */
	void checkConnectionConfiguration();

	/**
	 * Used to selected the connection type (normal or alternate).
	 */
	int _nbConnectionTry;

	static QMutex * _mutex;
	
};

#endif //OWPURPLECONNECTINGSTATE_H
