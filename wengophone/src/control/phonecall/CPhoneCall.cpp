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

#include "CPhoneCall.h"

#include <presentation/PPhoneCall.h>
#include <presentation/PFactory.h>

#include <control/CWengoPhone.h>

#include <model/WengoPhone.h>
#include <model/SipCallbacks.h>
#include <model/phonecall/PhoneCall.h>
#include <model/phoneline/PhoneLine.h>
#include <model/config/ConfigManager.h>
#include <model/config/Config.h>

#include <util/Logger.h>

CPhoneCall::CPhoneCall(PhoneCall & phoneCall, CWengoPhone & cWengoPhone)
	: _phoneCall(phoneCall),
	_cWengoPhone(cWengoPhone) {

	_pPhoneCall = PFactory::getFactory().createPresentationPhoneCall(*this);

	_phoneCall.stateChangedEvent += boost::bind(&CPhoneCall::stateChangedEventHandler, this, _1, _2);
	_phoneCall.videoFrameReceivedEvent += boost::bind(&CPhoneCall::videoFrameReceivedEventHandler, this, _1, _2, _3);
}

CPhoneCall::~CPhoneCall() {
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

int CPhoneCall::getDuration() const {
	return _phoneCall.getDuration();
}

void CPhoneCall::stateChangedEventHandler(PhoneCall & sender, EnumPhoneCallState::PhoneCallState state) {
	if (state == EnumPhoneCallState::PhoneCallStateClosed) {
		_pPhoneCall->close();
		_pPhoneCall = NULL;
	} else if (_pPhoneCall) {
		_pPhoneCall->stateChangedEvent(state);
	}
}

void CPhoneCall::videoFrameReceivedEventHandler(PhoneCall & sender, piximage * remoteVideoFrame, piximage * localVideoFrame) {
	if (_pPhoneCall) {
		_pPhoneCall->videoFrameReceivedEvent(remoteVideoFrame, localVideoFrame);
	}
}

void CPhoneCall::hangUp() {
	_phoneCall.close();
}

void CPhoneCall::accept() {
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

CodecList::AudioCodec CPhoneCall::getAudioCodecUsed() {
	return _phoneCall.getAudioCodecUsed();
}

CodecList::VideoCodec CPhoneCall::getVideoCodecUsed() {
	return _phoneCall.getVideoCodecUsed();
}

EnumPhoneCallState::PhoneCallState CPhoneCall::getState() const {
	return _phoneCall.getState();
}
