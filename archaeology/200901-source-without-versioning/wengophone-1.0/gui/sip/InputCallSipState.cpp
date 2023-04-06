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

#include "InputCallSipState.h"

#include "InitSipState.h"
#include "ConversationSipState.h"
#include "SipPrimitives.h"
#include "SessionWindow.h"
#include "AudioCallManager.h"
#include "contact/ContactList.h"
#include "PhonePageWidget.h"
#include "callhistory/CallHistory.h"
#include "Softphone.h"
#include "systray/Systray.h"
#include "util/emit_signal.h"
#include "AudioCall.h"
#include "config/Audio.h"
#include "SipAddress.h"

#include <Sound.h>

#include <qmainwindow.h>
#include <qcombobox.h>
#include <qpushbutton.h>
#include <qsound.h>
#include <qmessagebox.h>
#include <qapplication.h>

#include <cassert>
#include <iostream>
#include <phapi.h>

using namespace std;

const QString InputCallSipState::SOUND_RINGIN = "sounds/ringin.wav";

const int InputCallSipState::REJECT_INPUT_CALL_SIP_CODE = 486;

InputCallSipState::InputCallSipState(AudioCall & audioCall, const SipAddress & sipAddress)
: SipState(audioCall) {

	_kind = SipState::input_state;
	SipPrimitives::ringingCall(_audioCall.getCallId());

	_callLog = new CallLog();
	_callLog->setType(CallLog::CallIncoming);

	_phonePageWidget.setStatusText(tr("Input call..."));

	/*if (AudioCallManager::getInstance().getAudioCallUsed() != audioCall.getCallId()) {
		//FIXME replace by double-call notification
		_soundRingin = new Sound(SOUND_RINGIN);
	} else {*/
		_soundRingin = new Sound(qApp->applicationDirPath() + QDir::separator() + SOUND_RINGIN);
	//}
	_soundRingin->setWaveOutDevice(Audio::getInstance().getRingingDeviceName());
	//Play the sound indefinitely
	_soundRingin->setLoops(-1);
	_soundRingin->play();

	initButtons();

	//Shows the SessionWindow
	SessionWindow & sessionWindow = _audioCall.getSessionWindow();
	Contact * contact = sessionWindow.showIncomingCallPhonePage(sipAddress);

	//Contact already present inside the ContactList
	if (!_phonePageWidget.getContactName().isEmpty()) {
		_callLog->setContactName(_phonePageWidget.getContactName());
	} else {
		_callLog->setContactName(_phonePageWidget.getCallerPhoneNumber());
	}
	_callLog->setPhoneNumber(_phonePageWidget.getCallerPhoneNumber());

	Softphone::getInstance().getSystray().setCallIncoming(_phonePageWidget.getCallerPhoneNumber(),
								contact);

	//Emits the signal: inputCall
	//Used by the Wenbox
	emit_signal(&AudioCallManager::getInstance(), SIGNAL(inputCall()));
}

InputCallSipState::~InputCallSipState() {
	delete _soundRingin;
}

void InputCallSipState::handle(int /*callId*/, const CallStateInfo * info) const {
	/* If the event corresponds to the call managed by this class */
	//if (callId == _audioCall->getCallId()) {
		switch (info->event) {
		case phDIALING:
			break;

		case phRINGING:
			break;

		case phNOANSWER:
			Softphone::getInstance().getSystray().setCallMissed();
			_callLog->setType(CallLog::CallMissed);
			_audioCall.getCallHistory().addCallLog(*_callLog);
			_soundRingin->stop();
			_audioCall.setSipState(new InitSipState(_audioCall));
			emit_signal(&AudioCallManager::getInstance(), SIGNAL(endCall()));
			break;

		case phCALLBUSY:
			break;

		case phCALLREDIRECTED:
			break;

		case phCALLOK:
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
			Softphone::getInstance().getSystray().setCallMissed();
			_callLog->setType(CallLog::CallMissed);
			_audioCall.getCallHistory().addCallLog(*_callLog);
			_soundRingin->stop();
			_audioCall.setSipState(new InitSipState(_audioCall));
			emit_signal(&AudioCallManager::getInstance(), SIGNAL(endCall()));
			break;

		case phCALLERROR:
			Softphone::getInstance().getSystray().setCallMissed();
			_callLog->setType(CallLog::CallMissed);
			_audioCall.getCallHistory().addCallLog(*_callLog);
			_soundRingin->stop();
			_audioCall.setSipState(new InitSipState(_audioCall));
			emit_signal(&AudioCallManager::getInstance(), SIGNAL(endCall()));
			break;

		default:
			assert("This phapi event is not handled by this class");
		}
	//}

	/*
	If the event does not correspond to a call managed by this class.
	We should show the PhonePageWidget corresponding to an unknow Contact.
	The user can then fill up the form of this Contact and saves it in its ContactList.
	*/
	//else {
	//}
}

void InputCallSipState::acceptAudioCall() {
	acceptCall(false);
}

void InputCallSipState::acceptVideoCall() {
	acceptCall(true);
}

void InputCallSipState::acceptCall(bool video) {
	if (AudioCallManager::getInstance().isConversationOccuring()) {
		_audioCall.notify(AudioCallEvent(AudioCallEvent::call_action));
		return;
	}
	_soundRingin->stop();

	if (video) {
		SipPrimitives::acceptVideoCall(_audioCall.getCallId());
	} else {
		SipPrimitives::acceptAudioCall(_audioCall.getCallId());
	}

	disconnect(_phonePageWidget.getAudioCallButton(), SIGNAL(clicked()), this, SLOT(rejectCall()));
	disconnect(_phonePageWidget.getVideoCallButton(), SIGNAL(clicked()), this, SLOT(rejectCall()));
	_audioCall.setSipState(new ConversationSipState(_audioCall, *_callLog));
}

void InputCallSipState::rejectCall() {
	Softphone::getInstance().getSystray().setCallMissed();
	_callLog->setType(CallLog::CallMissed);
	_audioCall.getCallHistory().addCallLog(*_callLog);

	SipPrimitives::rejectCall(_audioCall.getCallId(), REJECT_INPUT_CALL_SIP_CODE);
	emit_signal(&AudioCallManager::getInstance(), SIGNAL(endCall()));
}

void InputCallSipState::initButtons() {
	QPushButton * audioCallButton = _phonePageWidget.getAudioCallButton();
	audioCallButton->setEnabled(true);
	connect(audioCallButton, SIGNAL(clicked()), this, SLOT(acceptAudioCall()));
	QPushButton * videoCallButton = _phonePageWidget.getVideoCallButton();
	videoCallButton->setEnabled(true);
	connect(videoCallButton, SIGNAL(clicked()), this, SLOT(acceptVideoCall()));

	QPushButton * hangUpButton = _phonePageWidget.getHangUpButton();
	hangUpButton->setEnabled(true);
	connect(hangUpButton, SIGNAL(clicked()), this, SLOT(rejectCall()));
}
