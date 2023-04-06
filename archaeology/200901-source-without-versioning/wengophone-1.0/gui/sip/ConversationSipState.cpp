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

#include "ConversationSipState.h"
#ifdef ENABLE_VIDEO
#include "config/Video.h"
#endif

#include "InitSipState.h"
#include "SipPrimitives.h"
#include "AudioCall.h"
#include "AudioCallManager.h"
#include "SessionWindow.h"
#include "PhonePageWidget.h"
#include "callhistory/CallHistory.h"
#include "callhistory/CallLog.h"
#include "Softphone.h"
#include "systray/Systray.h"
#include "util/emit_signal.h"
#include "config/Audio.h"

#include <Sound.h>

#include <qpushbutton.h>

#include <cassert>
#include <iostream>

#include <phapi.h>

using namespace std;

const QString ConversationSipState::SOUND_OCCUPIED = "sounds/occupied.wav";

ConversationSipState::ConversationSipState(AudioCall & audioCall, CallLog & callLog)
: SipState(audioCall), _callLog(callLog) {

#ifdef ENABLE_VIDEO
	AudioCallManager::getInstance().getActiveAudioCall()->getSessionWindow().getVisioPageWidget().initDefaultQuality();
#endif
	_kind = SipState::conversation_state;
	//_audioCall.notify(AudioCallEvent(AudioCallEvent::use));

	_phonePageWidget.startCallDurationTimer();
	_callLog.startTimer();

	Softphone::getInstance().getSystray().setCallOutgoing();

	QPushButton * audioCallButton = _phonePageWidget.getAudioCallButton();
	QPushButton * videoCallButton = _phonePageWidget.getVideoCallButton();
	audioCallButton->setEnabled(false);
	videoCallButton->setEnabled(false);

	QPushButton * hangUpButton = _phonePageWidget.getHangUpButton();
	hangUpButton->setEnabled(true);
	connect(hangUpButton, SIGNAL(clicked()),
		this, SLOT(closeCall()));

	_soundOccupied = new Sound(SOUND_OCCUPIED);
	_soundOccupied->setWaveOutDevice(Audio::getInstance().getRingingDeviceName());
	//Plays the sound 4 times only
	_soundOccupied->setLoops(4);
}

ConversationSipState::~ConversationSipState() {
	//Cannot do that, otherwise it does not play the sound
	//delete _soundOccupied;
}

void ConversationSipState::handle(int , const CallStateInfo * info) const {
	switch (info->event) {
	case phDIALING:
		break;

	case phRINGING:
		break;

	case phNOANSWER:
		_soundOccupied->play();
		Softphone::getInstance().getSystray().setToDefault();
		_phonePageWidget.stopCallDurationTimer();
		_callLog.stopTimer();
		_audioCall.getCallHistory().addCallLog(_callLog);
		emit_signal(&AudioCallManager::getInstance(), SIGNAL(endCall()));
		_audioCall.setSipState(new InitSipState(_audioCall));
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
		_soundOccupied->play();
		Softphone::getInstance().getSystray().setToDefault();
		_phonePageWidget.stopCallDurationTimer();
		_callLog.stopTimer();
		_audioCall.getCallHistory().addCallLog(_callLog);
		emit_signal(&AudioCallManager::getInstance(), SIGNAL(endCall()));
		_audioCall.setSipState(new InitSipState(_audioCall));
		break;

	case phCALLERROR:
		_soundOccupied->play();
		Softphone::getInstance().getSystray().setToDefault();
		_phonePageWidget.stopCallDurationTimer();
		_callLog.stopTimer();
		_audioCall.getCallHistory().addCallLog(_callLog);
		emit_signal(&AudioCallManager::getInstance(), SIGNAL(endCall()));
		_audioCall.setSipState(new InitSipState(_audioCall));
		break;

	default:
		assert(NULL && "This phapi event is not handled by this class");
	}
}

void ConversationSipState::closeCall() {
	_phonePageWidget.setStatusText(QString::null);

	SipPrimitives::closeCall(_audioCall.getCallId());
	emit_signal(&AudioCallManager::getInstance(), SIGNAL(endCall()));
}
