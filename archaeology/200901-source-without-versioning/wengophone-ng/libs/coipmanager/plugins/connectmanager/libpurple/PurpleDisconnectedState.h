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

#ifndef OWPURPLEDISCONNECTEDSTATE_H
#define OWPURPLEDISCONNECTEDSTATE_H

#include <coipmanager/connectmanager/DisconnectedState.h>

#include <QtCore/QObject>

/**
 * Account not connected state.
 *
 * - connect launches execute.
 * - disconnect does nothing.
 * - execute launches the connection process (switch to 'ConnectingState').
 *
 * @author Philippe Bernery
 */
class PurpleDisconnectedState : public QObject, public DisconnectedState {
	Q_OBJECT
public:

	PurpleDisconnectedState(IConnectPlugin & context);

	virtual void connect();

private Q_SLOTS:

	void accountReadySlot();

private:

	bool _canConnect;

	bool _mustConnect;

};

#endif //OWPURPLEDISCONNECTEDSTATE_H
