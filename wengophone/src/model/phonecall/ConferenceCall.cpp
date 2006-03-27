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

#include "ConferenceCallParticipant.h"
#include "PhoneCall.h"

#include <model/phoneline/IPhoneLine.h>
#include <model/account/SipAccount.h>

#include <sipwrapper/SipWrapper.h>

#include <util/Logger.h>

ConferenceCall::ConferenceCall(IPhoneLine & phoneLine)
	: _phoneLine(phoneLine) {

	//No conference started yet
	_confId = -1;

	//_phoneCallList.clear();
}

ConferenceCall::~ConferenceCall() {
	stop();
	/*for (unsigned i = 0; i < _phoneCallList.size(); i++) {
		//PhoneCall * phoneCall = _phoneCallList[i];
		//delete phoneCall;
	}
	_phoneCallList.clear();*/
}

void ConferenceCall::addPhoneCall(PhoneCall & phoneCall) {
	if (_phoneCallMap.size() == 2) {
		ConferenceCallParticipant * participant = new ConferenceCallParticipant(*this, phoneCall, true);
	} else {
		ConferenceCallParticipant * participant = new ConferenceCallParticipant(*this, phoneCall, false);
	}
}

void ConferenceCall::removePhoneCall(PhoneCall & phoneCall) {
	int callId = phoneCall.getCallId();

	if (isStarted()) {
		_phoneLine.getSipWrapper().splitConference(_confId, callId);
	}

	//_phoneCallList -= &phoneCall;
	//delete &phoneCall;
}

void ConferenceCall::addPhoneNumber(const std::string & phoneNumber) {
	int callId = _phoneLine.makeCall(phoneNumber);
	if (callId == -1) {
		_phoneCallMap[phoneNumber] = NULL;
	} else {
		PhoneCall * phoneCall = _phoneLine.getPhoneCall(callId);
		_phoneCallMap[phoneNumber] = phoneCall;
		phoneCall->stateChangedEvent += boost::bind(&ConferenceCall::phoneCallStateChangedEventHandler, this, _1, _2);
		addPhoneCall(*phoneCall);
		LOG_DEBUG("phone number added=" + phoneNumber);
	}
}

void ConferenceCall::addPhoneCall(int callId) {
	join(callId);
}

void ConferenceCall::phoneCallStateChangedEventHandler(PhoneCall & sender, EnumPhoneCallState::PhoneCallState state) {
	switch(state) {

	case EnumPhoneCallState::PhoneCallStateDefault:
		break;

	case EnumPhoneCallState::PhoneCallStateError:
		break;

	case EnumPhoneCallState::PhoneCallStateResumed:
		break;

	case EnumPhoneCallState::PhoneCallStateTalking: {
		//Takes randomly a phoneNumber that has no PhoneCall associated
		//and creates a PhoneCall
		PhoneCalls::iterator it;
		for (it = _phoneCallMap.begin(); it != _phoneCallMap.end(); it++) {
			if (!(*it).second) {
				addPhoneNumber((*it).first);
				break;
			}
		}

		break;
	}

	case EnumPhoneCallState::PhoneCallStateDialing:
		break;

	case EnumPhoneCallState::PhoneCallStateRinging:
		break;

	case EnumPhoneCallState::PhoneCallStateClosed:
		break;

	case EnumPhoneCallState::PhoneCallStateIncoming:
		break;

	case EnumPhoneCallState::PhoneCallStateHold:
		break;

	case EnumPhoneCallState::PhoneCallStateMissed:
		break;

	case EnumPhoneCallState::PhoneCallStateRedirected:
		break;

	default:
		LOG_FATAL("unknown PhoneCallState=" + String::fromNumber(state));
	}
}

PhoneCall * ConferenceCall::getPhoneCall(const std::string & phoneNumber) const {
	/*PhoneCall * phoneCall = NULL;

	List<PhoneCall *> calls = _phoneLine.getPhoneCallList();

	SipAddress sipAddress = SipAddress::fromString(phoneNumber, _phoneLine.getSipAccount().getRealm());
	for (unsigned i = 0; i < calls.size(); i++) {
		SipAddress tmp = calls[i]->getPeerSipAddress();
		if (tmp.toString() == sipAddress.toString()) {
			return calls[i];
		}
	}

	return phoneCall;*/
	return NULL;
}

void ConferenceCall::removePhoneNumber(const std::string & phoneNumber) {
	/*SipAddress sipAddress = SipAddress::fromString(phoneNumber, _phoneLine.getSipAccount().getRealm());
	for (unsigned i = 0; i < _phoneCallList.size(); i++) {
		SipAddress tmp = _phoneCallList[i]->getPeerSipAddress();
		if (tmp.toString() == sipAddress.toString()) {
			PhoneCall * phoneCall = _phoneCallList[i];
			removePhoneCall(*phoneCall);
		}
	}*/
}

void ConferenceCall::start() {
	if (!isStarted()) {
		_confId = _phoneLine.getSipWrapper().createConference();
		stateChangedEvent(*this, EnumConferenceCallState::ConferenceCallStateStarted);
		/*for (unsigned i = 0; i < _phoneCallList.size(); i++) {
			PhoneCall * phoneCall = _phoneCallList[i];
			_phoneLine.getSipWrapper().joinConference(_confId, phoneCall->getCallId());
		}*/
	}
}

void ConferenceCall::stop() {
	if (isStarted()) {
		_phoneLine.getSipWrapper().destroyConference(_confId);
		stateChangedEvent(*this, EnumConferenceCallState::ConferenceCallStateStopped);
	}
	_confId = -1;
}

void ConferenceCall::join(int callId) {
	//if (isStarted()) {
		_phoneLine.getSipWrapper().joinConference(_confId, callId);
	//}
}
