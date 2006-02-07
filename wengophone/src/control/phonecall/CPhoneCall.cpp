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

#include "CPhoneCall.h"

#include <presentation/PPhoneCall.h>
#include <presentation/PFactory.h>

#include <control/CWengoPhone.h>

#include <model/WengoPhone.h>
#include <model/SipCallbacks.h>
#include <model/phonecall/PhoneCall.h>
#include <model/phoneline/PhoneLine.h>
#include <model/phonecall/PhoneCallStateDefault.h>
#include <model/phonecall/PhoneCallStateClosed.h>
#include <model/phonecall/PhoneCallStateDialing.h>
#include <model/phonecall/PhoneCallStateError.h>
#include <model/phonecall/PhoneCallStateHoldOk.h>
#include <model/phonecall/PhoneCallStateIncoming.h>
#include <model/phonecall/PhoneCallStateTalking.h>
#include <model/phonecall/PhoneCallStateResumeOk.h>
#include <model/phonecall/PhoneCallStateRinging.h>

#include <Logger.h>

CPhoneCall::CPhoneCall(PhoneCall & phoneCall, CWengoPhone & cWengoPhone)
	: _phoneCall(phoneCall),
	_cWengoPhone(cWengoPhone) {

	_pPhoneCall = PFactory::getFactory().createPresentationPhoneCall(*this);

	_phoneCall.stateChangedEvent += boost::bind(&CPhoneCall::stateChangedEventHandler, this, _1, _2);
	_phoneCall.videoFrameReceivedEvent += boost::bind(&CPhoneCall::videoFrameReceivedEventHandler, this, _1, _2, _3);
}

std::string CPhoneCall::getPeerSipAddress() const {
	return _phoneCall.getPeerSipAddress().getSipAddress();
}

std::string CPhoneCall::getPeerUserName() const {
	return _phoneCall.getPeerSipAddress().getUserName();
}

std::string CPhoneCall::getPeerDisplayName() const {
	return _phoneCall.getPeerSipAddress().getDisplayName();
}

void CPhoneCall::stateChangedEventHandler(PhoneCall & sender, int status) {
	if (status == PhoneCallStateClosed::CODE) {
		_pPhoneCall->close();
	}
	_pPhoneCall->updatePresentation();

	std::string sipAddress = sender.getPeerSipAddress().getSipAddress();
	std::string userName = sender.getPeerSipAddress().getUserName();
	std::string displayName = sender.getPeerSipAddress().getDisplayName();
	int lineId = sender.getPhoneLine().getLineId();
	int callId = sender.getCallId();

	switch (sender.getState().getCode()) {
	case PhoneCallStateDefault::CODE:
		break;

	case PhoneCallStateTalking::CODE:
		_pPhoneCall->phoneCallStateChangedEvent(PPhoneCall::CallTalking, lineId, callId, sipAddress, userName, displayName);
		break;

	case PhoneCallStateClosed::CODE:
		_pPhoneCall->phoneCallStateChangedEvent(PPhoneCall::CallClosed, lineId, callId, sipAddress, userName, displayName);
		break;

	case PhoneCallStateDialing::CODE:
		_pPhoneCall->phoneCallStateChangedEvent(PPhoneCall::CallDialing, lineId, callId, sipAddress, userName, displayName);
		break;

	case PhoneCallStateError::CODE:
		_pPhoneCall->phoneCallStateChangedEvent(PPhoneCall::CallError, lineId, callId, sipAddress, userName, displayName);
		break;

	case PhoneCallStateHoldOk::CODE:
		_pPhoneCall->phoneCallStateChangedEvent(PPhoneCall::CallHoldOk, lineId, callId, sipAddress, userName, displayName);
		break;

	case PhoneCallStateIncoming::CODE:
		_pPhoneCall->phoneCallStateChangedEvent(PPhoneCall::CallIncoming, lineId, callId, sipAddress, userName, displayName);
		break;

	case PhoneCallStateResumeOk::CODE:
		_pPhoneCall->phoneCallStateChangedEvent(PPhoneCall::CallResumeOk, lineId, callId, sipAddress, userName, displayName);
		break;

	case PhoneCallStateRinging::CODE:
		_pPhoneCall->phoneCallStateChangedEvent(PPhoneCall::CallRinging, lineId, callId, sipAddress, userName, displayName);
		break;

	default:
		LOG_FATAL("Unknown PhoneLine state");
	};
}

void CPhoneCall::videoFrameReceivedEventHandler(PhoneCall & sender, const WebcamVideoFrame & remoteVideoFrame,
	const WebcamVideoFrame & localVideoFrame) {

	_pPhoneCall->videoFrameReceived(remoteVideoFrame, localVideoFrame);
}

void CPhoneCall::hangUp() {
	_phoneCall.close();
}

bool CPhoneCall::canHangUp() const {
	int code = _phoneCall.getState().getCode();

	if (code == PhoneCallStateTalking::CODE ||
		code == PhoneCallStateIncoming::CODE ||
		code == PhoneCallStateRinging::CODE ||
		code == PhoneCallStateHoldOk::CODE ||
		code == PhoneCallStateResumeOk::CODE ||
		code == PhoneCallStateDialing::CODE) {

		return true;
	} else {
		return false;
	}
}

void CPhoneCall::pickUp() {
	_phoneCall.accept();
	_pPhoneCall->updatePresentation();
}

bool CPhoneCall::canPickUp() const {
	if (_phoneCall.getState().getCode() == PhoneCallStateIncoming::CODE/* && this.model.hasTakedDown == false*/) {
		return true;
	} else {
		return false;
	}
}
