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

#ifndef WAITINGSIPSTATE_H
#define WAITINGSIPSTATE_H

#include "SipState.h"
#include "callhistory/CallLog.h"

#include <qobject.h>
#include <qstring.h>

class PhonePageWidget;
class Sound;

/**
 * Waiting state of the state machine.
 *
 * Design Pattern State.
 *
 * @author Tanguy Krotoff
 */
class WaitingSipState : public SipState {
	Q_OBJECT
public:

	WaitingSipState(AudioCall & audioCall);

	virtual ~WaitingSipState();

	virtual void handle(int callId, const CallStateInfo * info) const;

private slots:

	/**
	 * The user clicked on the hang up button to close the current call.
	 */
	void closeCall();

private:

	WaitingSipState();
	WaitingSipState(const WaitingSipState &);
	WaitingSipState & operator=(const WaitingSipState &);

	static const QString SOUND_OCCUPIED;

	Sound * _soundOccupied;

	CallLog * _callLog;
};

#endif	//WAITINGSIPSTATE_H
