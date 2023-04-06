/*
 * WengoPhone, a voice over Internet phone
 * Copyright (C) 2004-2005  Wengo
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

#ifndef INITSIPSTATE_H
#define INITSIPSTATE_H

#include "SipState.h"

#include <qobject.h>
#include <qstring.h>

/**
 * Initial state of the state machine.
 *
 * Design Pattern State.
 *
 * @author Tanguy Krotoff
 */
class InitSipState : public SipState {
	Q_OBJECT
public:

	InitSipState(AudioCall & audioCall);

	virtual ~InitSipState();

	virtual void handle(int callId, const CallStateInfo * info) const;

private slots:

	/**
	 * The user clicked on the call button in order to make a call.
	 */
	void audioCall();

	void videoCall();

	/**
	 * @see MainWindow::callButtonEnabled()
	 */
	void callButtonEnabled(const QString &);

private:

	void call(bool video);

	InitSipState();
	InitSipState(const InitSipState &);
	InitSipState & operator=(const InitSipState &);

	/**
	 * Inits the buttons call and hang up.
	 */
	void initButtons() const;
};

#endif	//INITSIPSTATE_H
