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

#include "AudioCall.h"

#include "sip/SipState.h"
#include "sip/InitSipState.h"
#include "sip/SipAddress.h"
#include "Softphone.h"
#include "MainWindow.h"
#include "PhonePageWidget.h"
#include "SessionWindow.h"
#include "callhistory/CallHistory.h"
#include "callhistory/CallHistoryWidget.h"
#include "config/Video.h"


#include <qmainwindow.h>
#include <qmessagebox.h>

#include <cassert>
#include <iostream>
#include <phapi.h>


using namespace std;

const int AudioCall::NOT_CALLING = -1;

AudioCall::AudioCall() {
#ifdef ENABLE_VIDEO
	_enableVideo = Video::getInstance().getEnable();
#else
	_enableVideo = false;
#endif

	_sessionWindow = new SessionWindow(*this, Softphone::getInstance().getActiveWindow());
	_phonePageWidget = &_sessionWindow->getPhonePageWidget();

	_sipState = new InitSipState(*this);

	_callId = NOT_CALLING;
	_previousCallId = NOT_CALLING;
}

AudioCall::~AudioCall() {
	/*delete _phonePageWidget;
	delete _sessionWindow;*/
}

void AudioCall::setSipState(SipState * sipState) {
	SipState * oldSipState = _sipState;
	_sipState = sipState;
	delete oldSipState;
}

void AudioCall::callProgress(int callId, const CallStateInfo * info) {
	_sipState->handle(callId, info);
}

CallHistory & AudioCall::getCallHistory() const {
	return CallHistory::getInstance();
}

PhonePageWidget & AudioCall::getPhonePageWidget() const {
	assert(_phonePageWidget && "_phonePageWidget cannot be NULL");
	return *_phonePageWidget;
}

SessionWindow & AudioCall::getSessionWindow() const {
	assert(_sessionWindow && "_sessionWindow cannot be NULL");
	return *_sessionWindow;
}

bool AudioCall::isCalling() const {
	if (_sipState->getKind() == SipState::conversation_state
		|| _sipState->getKind() == SipState::dialing_state
		|| _sipState->getKind() == SipState::waiting_state) {
		return true;
	} else {
		return false;
	}
}

bool AudioCall::isWaiting() const {
	SipState::kind kind = _sipState->getKind();
	return kind == SipState::waiting_state || kind == SipState::dialing_state;
}
