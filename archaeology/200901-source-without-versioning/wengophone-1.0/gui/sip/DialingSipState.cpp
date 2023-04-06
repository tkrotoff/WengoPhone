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

#include "DialingSipState.h"

#include "WaitingSipState.h"
#include "InitSipState.h"
#include "ConversationSipState.h"
#include "SipPrimitives.h"
#include "AudioCall.h"
#include "PhonePageWidget.h"
#include "config/Audio.h"

#include <Sound.h>

#include <qstring.h>
#include <qpushbutton.h>

#include <cassert>
#include <phapi.h>

const QString DialingSipState::SOUND_OCCUPIED = "sounds/occupied.wav";

DialingSipState::DialingSipState(AudioCall & audioCall) : SipState(audioCall) {
	QString phoneNumber;

	_kind = SipState::dialing_state;
	//_audioCall.notify(AudioCallEvent(AudioCallEvent::use));

	phoneNumber = _phonePageWidget.getPhoneNumberToCall().latin1();
	_phonePageWidget.setPhoneNumberCalled(phoneNumber);
	_phonePageWidget.setStatusText(tr("Dialing..."));

	initButtons();

	//TODO play dialing sound

	_soundOccupied = new Sound(SOUND_OCCUPIED);
	_soundOccupied->setWaveOutDevice(Audio::getInstance().getRingingDeviceName());
	//Plays the sound 10 times maximum
	_soundOccupied->setLoops(10);

	_callLog = new CallLog();
	_callLog->setType(CallLog::CallOutgoing);
	if (_phonePageWidget.getContactName().isEmpty()) {
		_callLog->setContactName(_phonePageWidget.getPhoneNumberToCall());
	} else {
		_callLog->setContactName(_phonePageWidget.getContactName());
	}
	_callLog->setPhoneNumber(_phonePageWidget.getPhoneNumberToCall());
}

DialingSipState::~DialingSipState() {
	//Cannot do that, otherwise it does not play the sound
	//delete _soundOccupied;
}

void DialingSipState::cancelCall() {
	SipPrimitives::closeCall(_audioCall.getCallId());
}

void DialingSipState::handle(int, const CallStateInfo * info) const {
	switch (info->event) {
	case phDIALING:
		//TODO play dialing sound
		break;

	case phRINGING:
		_audioCall.setSipState(new WaitingSipState(_audioCall));
		break;

	case phNOANSWER:
		_audioCall.setSipState(new InitSipState(_audioCall));
		break;

	case phCALLBUSY:
		_soundOccupied->play();
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

	default:
		assert(NULL && "This phapi event is not handled by this class");
	}
}

void DialingSipState::initButtons() {
	QPushButton * audioCallButton = _phonePageWidget.getAudioCallButton();
	audioCallButton->setEnabled(false);
	QPushButton * videoCallButton = _phonePageWidget.getVideoCallButton();
	videoCallButton->setEnabled(false);

	QPushButton * hangUpButton = _phonePageWidget.getHangUpButton();
	hangUpButton->setEnabled(true);
	connect(hangUpButton, SIGNAL(clicked()),
		this, SLOT(cancelCall()));
}
