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

#include "PhoneLineStateUnknown.h"
#include "PhoneLineStateProgress.h"
#include "PhoneLineStateOk.h"
#include "PhoneLineStateClosed.h"
#include "PhoneLineStateTimeout.h"
#include "PhoneLineStateServerError.h"

#include <model/SipCallbacks.h>
#include <model/config/ConfigManager.h>
#include <model/config/Config.h>
#include <model/history/History.h>
#include <model/phonecall/PhoneCall.h>
#include <model/profile/UserProfile.h>

#include <sipwrapper/SipWrapper.h>
#include <sipwrapper/SipWrapperFactory.h>
#include <sipwrapper/EnumNatType.h>

#include <util/Logger.h>

#include <string>
using namespace std;

#include <cstring>

PhoneLine::PhoneLine(SipAccount & sipAccount, UserProfile & userProfile)
	: _sipAccount(sipAccount),
	_userProfile(userProfile) {

	_sipWrapper = SipWrapperFactory::getFactory().createSipWrapper();

	_sipCallbacks = new SipCallbacks(*_sipWrapper, _userProfile);

	initSipWrapper();

	_lineId = SipWrapper::VirtualLineIdError;

	static PhoneLineStateUnknown stateUnknown;
	_phoneLineStateList += &stateUnknown;
	_state = &stateUnknown;

	static PhoneLineStateProgress stateProgress;
	_phoneLineStateList += &stateProgress;

	static PhoneLineStateOk stateOk;
	_phoneLineStateList += &stateOk;

	static PhoneLineStateClosed stateClosed;
	_phoneLineStateList += &stateClosed;

	static PhoneLineStateTimeout stateTimeout;
	_phoneLineStateList += &stateTimeout;

	static PhoneLineStateServerError stateServerError;
	_phoneLineStateList += &stateServerError;

	_activePhoneCall = NULL;

	Config & config = ConfigManager::getInstance().getCurrentConfig();
	config.valueChangedEvent += boost::bind(&PhoneLine::configureSipWrapper, this);
}

PhoneLine::~PhoneLine() {
	disconnect();

	//Do not need to delete everything (_phoneLineStateList)
	//since states are static inside the constructor
	_state = NULL;

	if (_sipCallbacks) {
		delete _sipCallbacks;
		_sipCallbacks = NULL;
	}

	_sipWrapper = NULL;

	if (_activePhoneCall) {
		delete _activePhoneCall;
		_activePhoneCall = NULL;
	}
}

std::string PhoneLine::getMySipAddress() const {
	return "sip:" + _sipAccount.getIdentity() + "@" + _sipAccount.getRealm();
}

int PhoneLine::makeCall(const std::string & phoneNumber, bool enableVideo) {
	if (!_sipAccount.isConnected()) {
		LOG_ERROR("SipAccount not connected");
		return SipWrapper::CallIdError;
	}

	if (phoneNumber.empty()) {
		LOG_ERROR("empty phone number");
		return SipWrapper::CallIdError;
	}

	for (PhoneCalls::iterator it = _phoneCallMap.begin(); it != _phoneCallMap.end(); ++it) {
		PhoneCall * phoneCall = (*it).second;

		//FIXME this should be a LOG_FATAL()
		if (phoneCall) {
			EnumPhoneCallState::PhoneCallState state = phoneCall->getState();
			if (state != EnumPhoneCallState::PhoneCallStateHold) {

				LOG_ERROR("cannot place the call=" + phoneNumber + ", at least another phone call is not in hold state");
				return SipWrapper::CallIdError;
			}
		}
	}

	SipAddress sipAddress = SipAddress::fromString(phoneNumber, _sipAccount.getRealm());

	//Puts all the PhoneCall in the hold state before to create a new PhoneCall
	//holdAllCalls();

	if (enableVideo) {
		//Sets the video device
		Config & config = ConfigManager::getInstance().getCurrentConfig();
		_sipWrapper->setVideoDevice(config.getVideoWebcamDevice());
	}

	PhoneCall * phoneCall = new PhoneCall(*this, sipAddress);
	int callId = _sipWrapper->makeCall(_lineId, sipAddress.getRawSipAddress(), enableVideo);
	if (callId < 0) {
		//FIXME should be replaced by a LOG_FATAL()
		LOG_ERROR("couldn't place the call, SipWrapper returned an error=" + String::fromNumber(callId));
		return callId;
	}

	phoneCall->setCallId(callId);

	//Adds the PhoneCall to the list of PhoneCall
	_phoneCallMap[callId] = phoneCall;

	phoneCall->setState(EnumPhoneCallState::PhoneCallStateDialing);

	//Sends the event a new PhoneCall has been created
	phoneCallCreatedEvent(*this, *phoneCall);

	//History: create a HistoryMemento for this outgoing call
	HistoryMemento * memento = new HistoryMemento(
		HistoryMemento::OutgoingCall, sipAddress.getSipAddress(), callId);
	_userProfile.getHistory().addMemento(memento);

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
}

void PhoneLine::disconnect(bool now) {
	if (_lineId != SipWrapper::VirtualLineIdError) {
		_sipAccount.setConnected(false);
		_sipWrapper->removeVirtualLine(_lineId, now ? 0 : -1);
	}
}

void PhoneLine::checkCallId(int callId) {
	PhoneCall * phoneCall = getPhoneCall(callId);
	if (!phoneCall) {
		//If it crashes inside checkCallId() this is probably due
		//to a duplicated phCALLCLOSED message
		LOG_FATAL("unknown phone call callId=" + String::fromNumber(callId));
	}
}

void PhoneLine::acceptCall(int callId, bool enableVideo) {
	checkCallId(callId);
	holdCallsExcept(callId);
	_sipWrapper->acceptCall(callId, enableVideo);
	LOG_DEBUG("call accepted callId=" + String::fromNumber(callId));
}

void PhoneLine::rejectCall(int callId) {
	checkCallId(callId);
	_sipWrapper->rejectCall(callId);
	//History: retrieve the memento and change its state to rejected
	_userProfile.getHistory().updateCallState(callId, HistoryMemento::RejectedCall);
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

CodecList::AudioCodec PhoneLine::getAudioCodecUsed(int callId) {
	return _sipWrapper->getAudioCodecUsed(callId);
}

CodecList::VideoCodec PhoneLine::getVideoCodecUsed(int callId) {
	return _sipWrapper->getVideoCodecUsed(callId);
}

void PhoneLine::setPhoneCallState(int callId, EnumPhoneCallState::PhoneCallState state, const SipAddress & sipAddress) {
	LOG_DEBUG("call state changed callId=" + String::fromNumber(callId) +
		" state=" + EnumPhoneCallState::toString(state) +
		" from=" + sipAddress.getSipAddress());

	//save the last state
	EnumPhoneCallState::PhoneCallState lastState = EnumPhoneCallState::PhoneCallStateUnknown;
	PhoneCall * phoneCall = getPhoneCall(callId);
	if (phoneCall) {

		lastState = phoneCall->getState();

		if (phoneCall->getState() == state) {
			//We are already in this state
			//Prevents the state to be applied 2 times in a row
			return;
		}

		phoneCall->setState(state);
	}

	//This should not replace the state machine pattern PhoneCallState or PhoneLineState
	switch (state) {

	case EnumPhoneCallState::PhoneCallStateUnknown:
		break;

	case EnumPhoneCallState::PhoneCallStateError:
		callClosed(callId);
		break;

	case EnumPhoneCallState::PhoneCallStateResumed:
		holdCallsExcept(callId);
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
		if (lastState == EnumPhoneCallState::PhoneCallStateIncoming) {
			//History: retrieve the memento and change its state to missed
			_userProfile.getHistory().updateCallState(callId, HistoryMemento::MissedCall);
			LOG_DEBUG("call missed callId=" + String::fromNumber(callId));
		}
		callClosed(callId);
		break;

	case EnumPhoneCallState::PhoneCallStateIncoming: {
		//Sends SIP code 180
		//TODO automatically??
		_sipWrapper->sendRingingNotification(callId);

		PhoneCall * phoneCall = new PhoneCall(*this, sipAddress);
		phoneCall->setCallId(callId);

		//Adds the PhoneCall to the list of PhoneCall
		_phoneCallMap[callId] = phoneCall;

		phoneCall->setState(state);

		_activePhoneCall = phoneCall;

		//Sends the event a new PhoneCall has been created
		phoneCallCreatedEvent(*this, *phoneCall);

		//History: create a HistoryMemento for this incoming call
		HistoryMemento * memento = new HistoryMemento(
				HistoryMemento::IncomingCall, sipAddress.getSipAddress(), callId);
		_userProfile.getHistory().addMemento(memento);

		break;
	}

	case EnumPhoneCallState::PhoneCallStateHold:
		break;

	case EnumPhoneCallState::PhoneCallStateMissed:
		//History: retrieve the memento and change its state to missed
		_userProfile.getHistory().updateCallState(callId, HistoryMemento::MissedCall);
		LOG_DEBUG("call missed callId=" + String::fromNumber(callId));
		break;

	case EnumPhoneCallState::PhoneCallStateRedirected:
		break;

	default:
		LOG_FATAL("unknown PhoneCallState=" + EnumPhoneCallState::toString(state));
	}
}

void PhoneLine::callClosed(int callId) {
	//If it crashes inside checkCallId() this is probably due
	//to a duplicated phCALLCLOSED message
	checkCallId(callId);

	PhoneCall * phoneCall = getPhoneCall(callId);
	if (_activePhoneCall == phoneCall) {
		_activePhoneCall = NULL;
	}

	//Deletes the PhoneCall that is closed now
	//delete phoneCall;

	//History: update the duration of the memento associated to this phonecall
	_userProfile.getHistory().updateCallDuration(callId, phoneCall->getDuration());

	//Removes it from the list of PhoneCall
	_phoneCallMap.erase(callId);

	phoneCallClosedEvent(*this, *phoneCall);
}

void PhoneLine::holdCallsExcept(int callId) {
	for (PhoneCalls::iterator it = _phoneCallMap.begin(); it != _phoneCallMap.end(); ++it) {
		PhoneCall * phoneCall = (*it).second;
		if (phoneCall) {
			if (phoneCall->getCallId() != callId &&
				!phoneCall->getConferenceCall()) {

				phoneCall->hold();
			}
		}
	}
}

void PhoneLine::holdAllCalls() {
	holdCallsExcept(SipWrapper::CallIdError);
}

void PhoneLine::setState(EnumPhoneLineState::PhoneLineState state) {
	LOG_DEBUG("PhoneLineState=" + String::fromNumber(state));

	for (unsigned i = 0; i < _phoneLineStateList.size(); i++) {
		PhoneLineState * callState = _phoneLineStateList[i];
		if (callState->getCode() == state) {
			if (_state->getCode() != callState->getCode()) {
				_state = callState;
				_state->execute(*this);
				LOG_DEBUG("line state changed lineId=" + String::fromNumber(_lineId) +
					" state=" + EnumPhoneLineState::toString(_state->getCode()));
				stateChangedEvent(*this, state);
				return;
			}
		}
	}

	//LOG_FATAL("unknown PhoneLineState=" + String::fromNumber(state));
}

PhoneCall * PhoneLine::getPhoneCall(int callId) {
	return _phoneCallMap[callId];
}

IPhoneLine::PhoneCallList PhoneLine::getPhoneCallList() const {
	PhoneCallList phoneCallList;

	for (PhoneCalls::const_iterator it = _phoneCallMap.begin(); it != _phoneCallMap.end(); ++it) {
		PhoneCall * phoneCall = (*it).second;
		if (phoneCall) {
			phoneCallList += phoneCall;
		}
	}
	return phoneCallList;
}

void PhoneLine::initSipWrapper() {
	configureSipWrapper();
	_sipWrapper->init();
}

void PhoneLine::configureSipWrapper() {
	//TODO check if Settings keys have changed
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
	_sipWrapper->setSIP(_sipAccount.getSIPProxyServerHostname(),
				_sipAccount.getSIPProxyServerPort(),
				_sipAccount.getLocalSIPPort());

	//Setting NAT
	string natType = config.getNetworkNatType();
	_sipWrapper->setNatType(EnumNatType::toNatType(natType));

	//Setting video quality
	_sipWrapper->setVideoQuality((EnumVideoQuality::VideoQuality) config.getVideoQuality());

	//Setting audio devices
	_sipWrapper->setCallOutputAudioDevice(config.getAudioOutputDeviceName());
	_sipWrapper->setCallInputAudioDevice(config.getAudioInputDeviceName());
	_sipWrapper->setRingerOutputAudioDevice(config.getAudioRingerDeviceName());

	//AEC + half duplex
	_sipWrapper->enableAEC(config.getAudioAEC());
	_sipWrapper->enableHalfDuplex(config.getAudioHalfDuplex());
}
