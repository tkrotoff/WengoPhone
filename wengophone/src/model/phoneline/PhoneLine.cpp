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

#include "PhoneLine.h"

#include "PhoneLineStateDefault.h"
#include "PhoneLineStateOk.h"
#include "PhoneLineStateClosed.h"
#include "PhoneLineStateTimeout.h"
#include "PhoneLineStateServerError.h"

#include <model/WengoPhone.h>
#include <model/SipCallbacks.h>
#include <model/phonecall/PhoneCall.h>
#include <model/phonecall/PhoneCallStateClosed.h>
#include <model/phonecall/PhoneCallStateError.h>
#include <model/phonecall/PhoneCallStateTalking.h>
#include <model/phonecall/PhoneCallStateIncoming.h>
#include <model/profile/UserProfile.h>
#include <model/config/ConfigManager.h>
#include <model/config/Config.h>
#include <model/history/History.h>

#include <sipwrapper/SipWrapper.h>
#include <sipwrapper/SipWrapperFactory.h>
#include <sipwrapper/EnumNatType.h>

#include <util/Logger.h>
#include <thread/Thread.h>

#include <string>
using namespace std;

#include <cstring>

PhoneLine::PhoneLine(SipAccount & sipAccount, WengoPhone & wengoPhone)
	: _sipAccount(sipAccount), _wengoPhone(wengoPhone) {

	_sipWrapper = SipWrapperFactory::getFactory().createSipWrapper();

	static SipCallbacks callbacks(*_sipWrapper, wengoPhone.getCurrentUserProfile());

	initSipWrapper();

	_lineId = SipWrapper::VirtualLineIdError;

	static PhoneLineStateDefault stateDefault;
	_phoneLineStateList += &stateDefault;
	_state = &stateDefault;

	static PhoneLineStateOk stateOk;
	_phoneLineStateList += &stateOk;

	static PhoneLineStateClosed stateClosed;
	_phoneLineStateList += &stateClosed;

	static PhoneLineStateTimeout stateTimeout;
	_phoneLineStateList += &stateTimeout;

	static PhoneLineStateServerError stateServerError;
	_phoneLineStateList += &stateServerError;

	_activePhoneCall = NULL;
}

PhoneLine::~PhoneLine() {
	disconnect();

	//Do not need to delete everything (_phoneLineStateList)
	//since states are static inside the constructor
	_state = NULL;

	_sipWrapper = NULL;
	_activePhoneCall = NULL;
}

std::string PhoneLine::getMySipAddress() const {
	return "sip:" + _sipAccount.getIdentity() + "@" + _sipAccount.getRealm();
}

int PhoneLine::makeCall(const std::string & phoneNumber) {
	if (phoneNumber.empty()) {
		LOG_ERROR("empty phone number");
		return -1;
	}

	for (PhoneCalls::iterator it = _phoneCallMap.begin(); it != _phoneCallMap.end(); ++it) {
		PhoneCall * phoneCall = (*it).second;

		EnumPhoneCallState::PhoneCallState state = phoneCall->getState().getCode();
		if (state != EnumPhoneCallState::PhoneCallStateTalking &&
			state != EnumPhoneCallState::PhoneCallStateResumed &&
			state != EnumPhoneCallState::PhoneCallStateHold) {

			LOG_ERROR("cannot place the call=" + phoneNumber + ", at least another phone call is not in talking state");
			return -1;
		}
	}

	SipAddress sipAddress = SipAddress::fromString(phoneNumber, _sipAccount.getRealm());

	//Puts all the PhoneCall in the hold state before to create a new PhoneCall
	holdCallsExcept(-1);

	PhoneCall * phoneCall = new PhoneCall(*this, sipAddress);
	int callId = _sipWrapper->makeCall(_lineId, sipAddress.getRawSipAddress());
	phoneCall->setCallId(callId);

	//Adds the PhoneCall to the list of PhoneCall
	_phoneCallMap[callId] = phoneCall;

	//Sends the event a new PhoneCall has been created
	phoneCallCreatedEvent(*this, *phoneCall);

	//History: create a HistoryMemento for this outgoing call
	HistoryMemento * memento = new HistoryMemento(
		HistoryMemento::OutgoingCall, sipAddress.getSipAddress(), callId);
	History::getInstance().addMemento(memento);

	return callId;
}

void PhoneLine::connect() {
	_lineId = _sipWrapper->addVirtualLine(
		_sipAccount.getDisplayName(),
		_sipAccount.getUsername(),
		_sipAccount.getIdentity(),
		_sipAccount.getPassword(),
		_sipAccount.getRealm(),
		_sipAccount.getSIPProxyServerHostname(),
		_sipAccount.getRegisterServerHostname());

	LOG_DEBUG("connect username=" + _sipAccount.getUsername() + " server=" + _sipAccount.getRegisterServerHostname() +
			" lineId=" + String::fromNumber(_lineId));

	if (_lineId == SipWrapper::VirtualLineIdError) {
		LOG_ERROR("SipWrapper::addVirtualLine() failed");
		_sipAccount.setConnected(false);
	} else {
		_sipAccount.setConnected(true);
	}
}

void PhoneLine::disconnect() {
	if (_lineId != SipWrapper::VirtualLineIdError) {
		_sipAccount.setConnected(false);
		_sipWrapper->removeVirtualLine(_lineId);
	}
}

void PhoneLine::checkCallId(int callId) {
	PhoneCall * phoneCall = getPhoneCall(callId);
	if (!phoneCall) {
		LOG_FATAL("unknow phone call callId=" + String::fromNumber(callId));
	}
}

void PhoneLine::acceptCall(int callId) {
	checkCallId(callId);
	_sipWrapper->acceptCall(callId);
	LOG_DEBUG("call accepted callId=" + String::fromNumber(callId));
}

void PhoneLine::rejectCall(int callId) {
	checkCallId(callId);
	_sipWrapper->rejectCall(callId);
	//History: retreive the memento and change its state to rejected
	History::getInstance().updateCallState(callId, HistoryMemento::RejectedCall);
	LOG_DEBUG("call rejected callId=" + String::fromNumber(callId));
}

void PhoneLine::closeCall(int callId) {
	checkCallId(callId);
	_sipWrapper->closeCall(callId);
	LOG_DEBUG("call closed callId=" + String::fromNumber(callId));
}

void PhoneLine::holdCall(int callId) {
	checkCallId(callId);
	_sipWrapper->holdCall(callId);
	LOG_DEBUG("call hold callId=" + String::fromNumber(callId));
}

void PhoneLine::resumeCall(int callId) {
	checkCallId(callId);
	_sipWrapper->resumeCall(callId);
	LOG_DEBUG("call resumed callId=" + String::fromNumber(callId));
}

void PhoneLine::blindTransfer(int callId, const std::string & sipAddress) {
	checkCallId(callId);
	SipAddress sipUri = SipAddress::fromString(sipAddress, getSipAccount().getRealm());
	_sipWrapper->blindTransfer(callId, sipUri.getRawSipAddress());
	LOG_DEBUG("call transfered to=" + sipAddress);
}

void PhoneLine::playTone(int callId, EnumTone::Tone tone) {
	//No check
	//checkCallId(callId);
	_sipWrapper->playTone(callId, tone);
}

void PhoneLine::playSoundFile(int callId, const std::string & soundFile) {
	//No check
	//checkCallId(callId);
	_sipWrapper->playSoundFile(callId, soundFile);
}

void PhoneLine::setPhoneCallState(int callId, EnumPhoneCallState::PhoneCallState state, const SipAddress & sipAddress) {
	LOG_DEBUG("call state changed callId=" + String::fromNumber(callId) +
		" state=" + String::fromNumber(state) +
		" from=" + sipAddress.getSipAddress());

	PhoneCall * phoneCall = getPhoneCall(callId);
	if (phoneCall) {
		if (phoneCall->getState().getCode() == state) {
			//We are already in this state
			//Prevents the state to be applied 2 times in a row
			return;
		}

		phoneCall->setState(state);
	}

	//This should not replace the state machine pattern PhoneCallState or PhoneLineState
	switch(state) {

	case EnumPhoneCallState::PhoneCallStateDefault:
		break;

	case EnumPhoneCallState::PhoneCallStateError:
		callClosed(callId);
		break;

	case EnumPhoneCallState::PhoneCallStateResumed:
		//holdCallsExcept(callId);
		_activePhoneCall = getPhoneCall(callId);
		break;

	case EnumPhoneCallState::PhoneCallStateTalking:
		_activePhoneCall = getPhoneCall(callId);
		break;

	case EnumPhoneCallState::PhoneCallStateDialing:
		break;

	case EnumPhoneCallState::PhoneCallStateRinging:
		break;

	case EnumPhoneCallState::PhoneCallStateClosed:
		callClosed(callId);
		break;

	case EnumPhoneCallState::PhoneCallStateIncoming: {
		holdCallsExcept(callId);

		//Sends SIP code 180
		//TODO automatically??
		_sipWrapper->sendRingingNotification(callId);

		PhoneCall * phoneCall = new PhoneCall(*this, sipAddress);
		phoneCall->setCallId(callId);

		//Adds the PhoneCall to the list of PhoneCall
		_phoneCallMap[callId] = phoneCall;

		//Sends the event a new PhoneCall has been created
		phoneCallCreatedEvent(*this, *phoneCall);

		phoneCall->setState(state);

		_activePhoneCall = phoneCall;

		//History: create a HistoryMemento for this incoming call
		HistoryMemento * memento = new HistoryMemento(
			HistoryMemento::IncomingCall, sipAddress.getSipAddress(), callId);
		History::getInstance().addMemento(memento);

		break;
	}

	case EnumPhoneCallState::PhoneCallStateHold:
		break;

	case EnumPhoneCallState::PhoneCallStateMissed:
		//History: retrive the memento and change its state to missed
		History::getInstance().updateCallState(callId, HistoryMemento::MissedCall);
		LOG_DEBUG("call missed callId=" + String::fromNumber(callId));
		break;

	case EnumPhoneCallState::PhoneCallStateRedirected:
		break;

	default:
		LOG_FATAL("unknown PhoneCallState=" + String::fromNumber(state));
	}
}

void PhoneLine::callClosed(int callId) {
	checkCallId(callId);
	PhoneCall * phoneCall = getPhoneCall(callId);
	if (_activePhoneCall == phoneCall) {
		_activePhoneCall = NULL;
	}

	//Deletes the PhoneCall that is closed now
	//delete phoneCall;

	//History: update the duration of the memento associated to this phonecall
	History::getInstance().updateCallDuration(callId, phoneCall->getDuration());

	//Removes it from the list of PhoneCall
	_phoneCallMap.erase(callId);

	phoneCallClosedEvent(*this, *phoneCall);
}

void PhoneLine::holdCallsExcept(int callId) {
	for (PhoneCalls::iterator it = _phoneCallMap.begin(); it != _phoneCallMap.end(); ++it) {
		PhoneCall * phoneCall = (*it).second;
		if (phoneCall) {
			if (phoneCall->getCallId() != callId /*&&
				!phoneCall->belongsToConference()*/) {
				phoneCall->hold();
			}
		}
	}
}

void PhoneLine::setState(EnumPhoneLineState::PhoneLineState state) {
	for (unsigned int i = 0; i < _phoneLineStateList.size(); i++) {
		PhoneLineState * callState = _phoneLineStateList[i];
		if (callState->getCode() == state) {
			if (_state->getCode() != callState->getCode()) {
				_state = callState;
				_state->execute(*this);
				LOG_DEBUG("line state changed lineId=" + String::fromNumber(_lineId) +
					" state=" + EnumPhoneLineState::toString(_state->getCode()));
				stateChangedEvent(*this, state);
				break;
			}
		}
	}
}

PhoneCall * PhoneLine::getPhoneCall(int callId) {
	return _phoneCallMap[callId];
}

List<PhoneCall *> PhoneLine::getPhoneCallList() const {
	List<PhoneCall *> calls;
	for (PhoneCalls::const_iterator it = _phoneCallMap.begin(); it != _phoneCallMap.end(); ++it) {
		calls += (*it).second;
	}
	return calls;
}

void PhoneLine::initSipWrapper() {
	Config & config = ConfigManager::getInstance().getCurrentConfig();

	//Setting plugin path
	string pluginPath = config.getCodecPluginPath();
	_sipWrapper->setPluginPath(pluginPath);

	//Setting proxy
	string proxyServer = config.getNetworkProxyServer();
	if (!proxyServer.empty()) {
		unsigned proxyPort = config.getNetworkProxyPort();
		string proxyLogin = config.getNetworkProxyLogin();
		string proxyPassword = config.getNetworkProxyPassword();

		_sipWrapper->setProxy(proxyServer, proxyPort, proxyLogin, proxyPassword);
	}

	//Setting HTTP tunnel
	if (_sipAccount.isHttpTunnelNeeded()) {
		_sipWrapper->setTunnel(_sipAccount.getHttpTunnelServerHostname(),
			_sipAccount.getHttpTunnelServerPort(), _sipAccount.httpTunnelHasSSL());
	}

	//Setting SIP proxy
	_sipWrapper->setSIP(_sipAccount.getSIPProxyServerHostname(), _sipAccount.getSIPProxyServerPort(), _sipAccount.getLocalSIPPort());

	//Setting NAT
	string natType = config.getNetworkNatType();
	_sipWrapper->setNatType(EnumNatType::toNatType(natType));

	//Setting audio devices
	_sipWrapper->setCallOutputAudioDevice(config.getAudioOutputDeviceName());
	_sipWrapper->setCallInputAudioDevice(config.getAudioInputDeviceName());
	_sipWrapper->setRingerOutputAudioDevice(config.getAudioRingerDeviceName());

	//AEC + half duplex
	_sipWrapper->enableAEC(config.getAudioAEC());
	_sipWrapper->enableHalfDuplex(config.getAudioHalfDuplex());

	_sipWrapper->init();
}
