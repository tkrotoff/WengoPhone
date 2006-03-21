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
#include <model/phonecall/PhoneCallStateHold.h>
#include <model/phonecall/PhoneCallStateIncoming.h>
#include <model/phonecall/PhoneCallStateTalking.h>
#include <model/phonecall/PhoneCallStateResumed.h>
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

void CPhoneCall::stateChangedEventHandler(PhoneCall & sender, EnumPhoneCallState::PhoneCallState state) {
	_pPhoneCall->stateChangedEvent(state);
}

void CPhoneCall::videoFrameReceivedEventHandler(PhoneCall & sender, const WebcamVideoFrame & remoteVideoFrame,
	const WebcamVideoFrame & localVideoFrame) {

	_pPhoneCall->videoFrameReceivedEvent(remoteVideoFrame, localVideoFrame);
}

void CPhoneCall::hangUp() {
	_phoneCall.close();
}

void CPhoneCall::pickUp() {
	_phoneCall.accept();
}

void CPhoneCall::hold() {
	_phoneCall.hold();
}

void CPhoneCall::resume() {
	_phoneCall.resume();
}

void CPhoneCall::blindTransfer(const std::string & phoneNumber) {
	_phoneCall.blindTransfer(phoneNumber);
}
