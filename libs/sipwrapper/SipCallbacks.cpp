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

#include "SipCallbacks.h"

#include "SipWrapper.h"

#include <model/WengoPhone.h>
#include <model/phoneline/IPhoneLine.h>
#include <model/phonecall/PhoneCall.h>

#include <StringList.h>
#include <Logger.h>

SipCallbacks::SipCallbacks(SipWrapper & sipWrapper, WengoPhone & wengoPhone)
	: _sipWrapper(sipWrapper),
	_wengoPhone(wengoPhone) {

	_sipWrapper.phoneCallStateChangedEvent +=
		boost::bind(&SipCallbacks::phoneCallStateChangedEventHandler, this, _1, _2, _3, _4);
	_sipWrapper.phoneLineStateChangedEvent +=
		boost::bind(&SipCallbacks::phoneLineStateChangedEventHandler, this, _1, _2, _3);
	_sipWrapper.videoFrameReceivedEvent +=
		boost::bind(&SipCallbacks::videoFrameReceivedEventHandler, this, _1, _2, _3, _4);
}

void SipCallbacks::phoneCallStateChangedEventHandler(SipWrapper & sender, int callId,
	EnumPhoneCallState::PhoneCallState state, const std::string & from) {

	LOG_DEBUG("callProgress: callId=" + String::fromNumber(callId) +
		" state=" + String::fromNumber(state) +
		" from=" + from);

	const WengoPhone::PhoneLines & lines = _wengoPhone.getPhoneLineList();
	for (unsigned i = 0; i < lines.size(); i++) {
		IPhoneLine * line = lines[i];
		line->setPhoneCallState(callId, state, SipAddress(from));
	}
}

void SipCallbacks::phoneLineStateChangedEventHandler(SipWrapper & sender, int lineId,
	EnumPhoneLineState::PhoneLineState state) {

	LOG_DEBUG("registerProgress: lineId=" + String::fromNumber(lineId) +
		" state=" + String::fromNumber(state));

	const WengoPhone::PhoneLines & lines = _wengoPhone.getPhoneLineList();
	for (unsigned i = 0; i < lines.size(); i++) {
		IPhoneLine * line = lines[i];
		LOG_DEBUG("lineId=" + String::fromNumber(line->getLineId()));
		if (line->getLineId() == lineId) {
			line->setState(state);
			break;
		}
	}
}

void SipCallbacks::videoFrameReceivedEventHandler(SipWrapper & sender, int callId,
	const WebcamVideoFrame & remoteVideoFrame, const WebcamVideoFrame & localVideoFrame) {

	//LOG_DEBUG("videoFrameReceived: callId=" + String::fromNumber(callId));

	const WengoPhone::PhoneLines & lines = _wengoPhone.getPhoneLineList();
	for (unsigned i = 0; i < lines.size(); i++) {
		IPhoneLine * line = lines[i];

		PhoneCall * call = line->getPhoneCall(callId);
		if (call) {
			//The correct PhoneCall has been found given its callId
			call->videoFrameReceived(remoteVideoFrame, localVideoFrame);
			break;
		}
	}
}
