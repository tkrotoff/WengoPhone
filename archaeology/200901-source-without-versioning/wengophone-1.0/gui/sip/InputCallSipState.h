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

#ifndef INPUTCALLSIPSTATE_H
#define INPUTCALLSIPSTATE_H

#include "SipState.h"
#include "callhistory/CallLog.h"

#include <qobject.h>
#include <qstring.h>

class SipAddress;
class PhonePageWidget;
class Sound;

/**
 * A call has been received from outside state of the state machine.
 *
 * Design Pattern State.
 *
 * @author Tanguy Krotoff
 */
class InputCallSipState : public SipState {

	Q_OBJECT
public:

	InputCallSipState(AudioCall & audioCall, const SipAddress & sipAddress);

	virtual ~InputCallSipState();

	virtual void handle(int callId, const CallStateInfo * info) const;

public slots:

	void acceptAudioCall();

	void acceptVideoCall();

	/**
	 * The user rejects the input call by clicking on the hang up button.
	 */
	void rejectCall();

private:

	/**
	 * The user accepts the input call by clicking on the call button.
	 */
	void acceptCall(bool video);

	/**
	* Inits buttons
	*/
	void initButtons();

	InputCallSipState();
	InputCallSipState(const InputCallSipState &);
	InputCallSipState & operator=(const InputCallSipState &);

	/**
	 * SIP code to reject an input call.
	 */
	static const int REJECT_INPUT_CALL_SIP_CODE;

	static const QString SOUND_RINGIN;

	Sound * _soundRingin;

	CallLog * _callLog;
};

#endif	//INPUTCALLSIPSTATE_H
