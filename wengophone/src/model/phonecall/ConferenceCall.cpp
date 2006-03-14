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

ConferenceCall::ConferenceCall(const PhoneLine & phoneLine)
	: _phoneLine(phoneLine) {
}

ConferenceCall::~ConferenceCall() {
}

void ConferenceCall::addPhoneCall(const PhoneCall & phoneCall) {
	_phoneCallList += &phoneCall;
}

void ConferenceCall::removePhoneCall(const PhoneCall & phoneCall) {
	_phoneCallList -= &phoneCall;
}

void ConferenceCall::addPhoneNumber(const std::string & phoneNumber) {
	SipAddress sipAddress = SipAddress::fromString(phoneNumber, _phoneLine.getSipAccount().getRealm());
	PhoneCall * phoneCall = new PhoneCall(_phoneLine, sipAddress);
	_phoneCallList += phoneCall;
}

void ConferenceCall::removePhoneNumber(const std::string & phoneNumber) {
	SipAddress sipAddress = SipAddress::fromString(phoneNumber, _phoneLine.getSipAccount().getRealm());
	for (unsigned i = 0; i < _phoneCallList.size(); i++) {
		SipAddress tmp = _phoneCallList[i].getPeerSipAddress();
		if (tmp.toString() == sipAddress.toString()) {
			PhoneCall * phoneCall = _phoneCallList[i];
			_phoneCallList -= phoneCall;
			delete phoneCall;
		}
	}
}

void ConferenceCall::start() {
	_phoneCallList[];
}

void ConferenceCall::stop() {
}
