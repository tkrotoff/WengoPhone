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

#include "WaitingSipState.h"
#include "InitSipState.h"
#include "ConversationSipState.h"
#include "SipPrimitives.h"
#include "AudioCall.h"
#include "AudioCallManager.h"
#include "PhonePageWidget.h"
#include "callhistory/CallHistory.h"
#include "config/Audio.h"
#include "util/emit_signal.h"

#include <Sound.h>

#include <qpushbutton.h>

#include <cassert>
#include <iostream>
#include <phapi.h>

using namespace std;

const QString WaitingSipState::SOUND_OCCUPIED = "sounds/occupied.wav";

WaitingSipState::WaitingSipState(AudioCall & audioCall)
: SipState(audioCall) {

	_kind = SipState::waiting_state;
	//_audioCall.notify(AudioCallEvent(AudioCallEvent::use));

	_callLog = new CallLog();
	_callLog->setType(CallLog::CallOutgoing);
	if (_phonePageWidget.getContactName().isEmpty()) {
		_callLog->setContactName(_phonePageWidget.getPhoneNumberToCall());
	} else {
		_callLog->setContactName(_phonePageWidget.getContactName());
	}
	_callLog->setPhoneNumber(_phonePageWidget.getPhoneNumberToCall());

	_phonePageWidget.setStatusText(tr("Waiting..."));

	_soundOccupied = new Sound(SOUND_OCCUPIED);
	_soundOccupied->setWaveOutDevice(Audio::getInstance().getRingingDeviceName());
	_soundOccupied->setLoops(4);

	QPushButton * hangUpButton = _phonePageWidget.getHangUpButton();
	connect(hangUpButton, SIGNAL(clicked()), this, SLOT(closeCall()));

	//Emits the signal: outputCall
	//Used by the Wenbox
	emit_signal(&AudioCallManager::getInstance(), SIGNAL(outputCall()));
}

WaitingSipState::~WaitingSipState() {
}

void WaitingSipState::handle(int callId, const CallStateInfo * info) const {
	//If the event corresponds to the call managed by this class
	if (callId == _audioCall.getCallId()) {
		switch (info->event) {
		case phDIALING:
			break;

		case phRINGING:
			break;

		case phNOANSWER:
			_soundOccupied->stop();
			_audioCall.setSipState(new InitSipState(_audioCall));
			break;

		case phCALLBUSY:
			//Plays busy sound
			_soundOccupied->play();

			_audioCall.setSipState(new InitSipState(_audioCall));
			break;

		case phCALLREDIRECTED:
			break;

		case phCALLOK:
			_audioCall.setSipState(new ConversationSipState(_audioCall, *_callLog));
			break;

		case phCALLHELD:
			break;

		case phCALLRESUMED:
			break;

		case phHOLDOK:
			break;

		case phRESUMEOK:
			break;

		case phINCALL:
			break;

		case phCALLCLOSED:
			_soundOccupied->stop();
			_audioCall.setSipState(new InitSipState(_audioCall));
			break;

		case phCALLERROR:
			_soundOccupied->stop();
			_audioCall.setSipState(new InitSipState(_audioCall));
			break;

        case phDTMF:
        case phXFERPROGRESS:
        case phXFEROK:
        case phXFERFAIL:
        case phXFERREQ:
        case phCALLREPLACED:
        /* XXX *FIXME* !  */
            break;

		default:
			assert(NULL && "This phapi event is not handled by this class");
		}
	}
}

void WaitingSipState::closeCall() {
	_phonePageWidget.setStatusText(QString::null);
	SipPrimitives::closeCall(_audioCall.getCallId());
}
