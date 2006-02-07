/*
 * WengoPhone, a voice over Internet phone
 * Copyright (C) 2004-2006  Wengo
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

#include "PhoneCall.h"

#include "PhoneCallStateDefault.h"
#include "PhoneCallStateClosed.h"
#include "PhoneCallStateDialing.h"
#include "PhoneCallStateError.h"
#include "PhoneCallStateHoldOk.h"
#include "PhoneCallStateIncoming.h"
#include "PhoneCallStateTalking.h"
#include "PhoneCallStateResumeOk.h"
#include "PhoneCallStateRinging.h"


#include <model/phoneline/IPhoneLine.h>
#include <model/wenbox/WenboxPlugin.h>
#include <model/SipCallbacks.h>
#include <model/WengoPhone.h>

#include <StringList.h>
#include <Logger.h>

PhoneCall::PhoneCall(IPhoneLine & phoneLine, const SipAddress & sipAddress)
	: _phoneLine(phoneLine) {

	_sipAddress = sipAddress;

	static PhoneCallStateDefault stateDefault;

	//Default state (PhoneCallStateDefault)
	_state = &stateDefault;

	_phoneCallStateList += &stateDefault;

	static PhoneCallStateClosed stateClosed;
	_phoneCallStateList += &stateClosed;

	static PhoneCallStateDialing stateDialing;
	_phoneCallStateList += &stateDialing;

	static PhoneCallStateError stateError;
	_phoneCallStateList += &stateError;

	static PhoneCallStateHoldOk stateHoldOk;
	_phoneCallStateList += &stateHoldOk;

	static PhoneCallStateIncoming stateIncoming;
	_phoneCallStateList += &stateIncoming;

	static PhoneCallStateTalking stateTalking;
	_phoneCallStateList += &stateTalking;

	static PhoneCallStateResumeOk stateResumeOk;
	_phoneCallStateList += &stateResumeOk;

	static PhoneCallStateRinging stateRinging;
	_phoneCallStateList += &stateRinging;
}

PhoneCall::~PhoneCall() {
	//Do not need to delete everything (_phoneCallStateList)
	//since states are static inside the constructor
	_state = NULL;
}

void PhoneCall::hold() {
	_phoneLine.holdCall(_callId);
	//stateChangedEvent(this, _state->getCode());
	LOG_DEBUG("call hold");
}

void PhoneCall::accept() {
	_phoneLine.acceptCall(_callId);
	if (_state->getCode() == PhoneCallStateIncoming::CODE) {
		_phoneLine.setPhoneCallState(_callId, PhoneCallStateTalking::CODE, String::null);
	}
	LOG_DEBUG("call accepted");
}

void PhoneCall::resume() {
	_phoneLine.resumeCall(_callId);
	//stateChangedEvent(this, _state->getCode());
	LOG_DEBUG("call resumed");
}

void PhoneCall::setState(int status) {
	for (unsigned i = 0; i < _phoneCallStateList.size(); i++) {
		PhoneCallState * state = _phoneCallStateList[i];
		if (state->getCode() == status) {
			if (_state->getCode() != state->getCode()) {
				_state = state;
				_state->execute(*this);
				LOG_DEBUG("call state changed callId=" + String::fromNumber(_callId) + " state=" + _state->toString());
				stateChangedEvent(*this, status);
				break;
			}
		}
	}
}

void PhoneCall::close() {
	if (_state->getCode() != PhoneCallStateClosed::CODE) {
		if (_state->getCode() == PhoneCallStateIncoming::CODE /* && this.hasTakedDown == false*/) {
			_phoneLine.rejectCall(_callId);
		} else {
			_phoneLine.closeCall(_callId);
		}
		setState(PhoneCallStateClosed::CODE);

		//FIXME Already done?
		//int code = PhoneCallStateClosed::CODE;
		//stateChangedEvent(this, code);

		LOG_DEBUG("call closed");
	}
}

WenboxPlugin & PhoneCall::getWenboxPlugin() const {
	WengoPhone & wengoPhone = getPhoneLine().getWengoPhone();
	return wengoPhone.getWenboxPlugin();
}

void PhoneCall::videoFrameReceived(const WebcamVideoFrame & remoteVideoFrame, const WebcamVideoFrame & localVideoFrame) {
	videoFrameReceivedEvent(*this, remoteVideoFrame, localVideoFrame);
}
