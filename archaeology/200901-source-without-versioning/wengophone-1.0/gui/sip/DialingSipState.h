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

#ifndef DIALINGSIPSTATE_H
#define DIALINGSIPSTATE_H

#include "SipState.h"
#include "callhistory/CallLog.h"

class Sound;

/**
 * Dialing state of the state machine.
 *
 * Design Pattern State.
 *
 * @author Philippe Kajmar
 */
class DialingSipState : public SipState {
	Q_OBJECT
public:

	DialingSipState(AudioCall & audioCall);

	virtual ~DialingSipState();

	virtual void handle(int callId, const CallStateInfo * info) const;

private slots:

	/**
	 * The user cancelled the call by clicking on the hang up button.
	 */
	void cancelCall();

private:

	DialingSipState(const DialingSipState &);
	DialingSipState & operator=(const DialingSipState &);

	/**
	 * Inits the buttons call and hang up.
	 */
	void initButtons();

	static const QString SOUND_OCCUPIED;

	Sound * _soundOccupied;

	CallLog * _callLog;
};

#endif	//DIALINGSIPSTATE_H
