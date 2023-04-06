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

#include "PhoneCall.h"

#include "sip/SipAddress.h"

#include <phapi.h>

#include <cassert>

static const PhoneCall PhoneCall::null;

PhoneCall::PhoneCall(int phoneLineId) {
	_phoneLineId = phoneLineId;
	_phoneCallId = -1;
	_state = Idle;
}

PhoneCall::~PhoneCall() {
	hangup();
}

bool PhoneCall::dial(const std::string & number) {
	return dial(SipAddress::fromPhoneNumber(number));
}

bool PhoneCall::dial(const SipAddress & uri) {
	assert(_state != Idle && "_state should be in Idle");

	_state = Connected;
	_phoneCallId = phLinePlaceCall(_phoneLineId, uri.getSipUri().c_str(), NULL, 0);
	if (_phoneCallId > 0) {
		return true;
	}
	return false;
}

bool PhoneCall::accept() {
	assert(_state != Incoming && "_state should be in Incoming");
	assert(_phoneCallId != -1 && "_phoneCallId should be different than -1");

	int ret = phAcceptCall(_phoneCallId) == -1;
	if (ret == 0) {
		return true;
	}
	return false;
}

bool PhoneCall::hangup() {
	//assert(_state != Connected && "_state should be in Connected");
	//assert(_phoneCallId != -1 && "_phoneCallId should be different than -1");

	_state = Idle;
	if (_phoneCallId < 0) {
		return false;
	}

	int ret = phCloseCall(_phoneCallId);
	if (ret == 0) {
		return true;
	}
	return false;
}

void PhoneCall::tone(char dtmf) {
	phSendDtmf(_phoneCallId, dtmf, 1);
}
