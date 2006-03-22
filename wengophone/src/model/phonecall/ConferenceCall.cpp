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

#include "ConferenceCall.h"

#include <model/phoneline/IPhoneLine.h>
#include <model/phonecall/PhoneCall.h>
#include <model/account/SipAccount.h>

#include <sipwrapper/SipWrapper.h>

#include <util/Logger.h>

Mutex ConferenceCall::_mutex;

ConferenceCall::ConferenceCall(IPhoneLine & phoneLine)
	: _phoneLine(phoneLine) {

	//No conference started yet
	_confId = -1;

	_phoneCallList.clear();
}

ConferenceCall::~ConferenceCall() {
	stop();
	for (unsigned i = 0; i < _phoneCallList.size(); i++) {
		//PhoneCall * phoneCall = _phoneCallList[i];
		//delete phoneCall;
	}
	_phoneCallList.clear();
}

void ConferenceCall::addPhoneCall(PhoneCall & phoneCall) {
	int callId = phoneCall.getCallId();

	if (_confId != -1) {
		_phoneLine.getSipWrapper().joinConference(_confId, callId);
	}

	_phoneCallList.push_back(&phoneCall);
}

void ConferenceCall::removePhoneCall(PhoneCall & phoneCall) {
	int callId = phoneCall.getCallId();

	if (_confId != -1) {
		_phoneLine.getSipWrapper().splitConference(_confId, callId);
	}

	//_phoneCallList -= &phoneCall;
	//delete &phoneCall;
}

void ConferenceCall::addPhoneNumber(const std::string & phoneNumber) {
	Mutex::ScopedLock scopedLock(_mutex);

	LOG_DEBUG("phone number added=" + phoneNumber);

	PhoneCall * phoneCall = getPhoneCall(phoneNumber);
	if (phoneCall == NULL) {
		int callId = _phoneLine.makeCall(phoneNumber);
		phoneCall = _phoneLine.getPhoneCall(callId);
	}

	addPhoneCall(*phoneCall);
}

PhoneCall * ConferenceCall::getPhoneCall(const std::string & phoneNumber) const {
	PhoneCall * phoneCall = NULL;

	List<PhoneCall *> calls = _phoneLine.getPhoneCallList();

	SipAddress sipAddress = SipAddress::fromString(phoneNumber, _phoneLine.getSipAccount().getRealm());
	for (unsigned i = 0; i < calls.size(); i++) {
		SipAddress tmp = calls[i]->getPeerSipAddress();
		if (tmp.toString() == sipAddress.toString()) {
			return calls[i];
		}
	}

	return phoneCall;
}

void ConferenceCall::removePhoneNumber(const std::string & phoneNumber) {
	SipAddress sipAddress = SipAddress::fromString(phoneNumber, _phoneLine.getSipAccount().getRealm());
	for (unsigned i = 0; i < _phoneCallList.size(); i++) {
		SipAddress tmp = _phoneCallList[i]->getPeerSipAddress();
		if (tmp.toString() == sipAddress.toString()) {
			PhoneCall * phoneCall = _phoneCallList[i];
			removePhoneCall(*phoneCall);
		}
	}
}

void ConferenceCall::start() {
	if (_confId == -1) {
		_confId = _phoneLine.getSipWrapper().createConference();
		for (unsigned i = 0; i < _phoneCallList.size(); i++) {
			PhoneCall * phoneCall = _phoneCallList[i];
			_phoneLine.getSipWrapper().joinConference(_confId, phoneCall->getCallId());
		}
	}
}

void ConferenceCall::stop() {
	if (_confId != -1) {
		_phoneLine.getSipWrapper().destroyConference(_confId);
	}
	_confId = -1;
}
