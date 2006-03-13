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

#include "PhoneLine.h"

#include "PhoneLineStateDefault.h"
#include "PhoneLineStateOk.h"
#include "PhoneLineStateClosed.h"
#include "PhoneLineStateTimeout.h"
#include "PhoneLineStateServerError.h"

#include <model/SipCallbacks.h>
#include <model/phonecall/PhoneCall.h>
#include <model/phonecall/PhoneCallStateClosed.h>
#include <model/phonecall/PhoneCallStateError.h>
#include <model/phonecall/PhoneCallStateTalking.h>
#include <model/phonecall/PhoneCallStateIncoming.h>
#include <model/config/ConfigManager.h>
#include <model/config/Config.h>

#include <sipwrapper/SipWrapper.h>
#include <sipwrapper/SipWrapperFactory.h>
#include <sipwrapper/EnumNatType.h>

#include <Logger.h>

#include <cstring>

#include <string>

using namespace std;

PhoneLine::PhoneLine(SipAccount & sipAccount, WengoPhone & wengoPhone)
	: _sipAccount(sipAccount), _wengoPhone(wengoPhone) {

	_sipWrapper = SipWrapperFactory::getFactory().createSipWrapper();

	static SipCallbacks callbacks(*_sipWrapper, wengoPhone);

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

	//PhoneCall in waiting state for incoming calls
	//createWaitingPhoneCall();
	_waitingPhoneCall = NULL;
	_activePhoneCall = NULL;
}

PhoneLine::~PhoneLine() {
	disconnect();

	//Do not need to delete everything (_phoneLineStateList)
	//since states are static inside the constructor
	_state = NULL;

	_sipWrapper = NULL;
	_activePhoneCall = NULL;
	_waitingPhoneCall = NULL;
}

std::string PhoneLine::getMySipAddress() const {
	return "sip:" + _sipAccount.getIdentity() + "@" + _sipAccount.getRealm();
}

int PhoneLine::makeCall(const std::string & phoneNumber) {
	if (phoneNumber.empty()) {
		return -1;
	}

	//Puts all the PhoneCall in the hold state
	/*for (PhoneCalls::iterator it = _phoneCallHash.begin(); it != _phoneCallHash.end(); ++it) {
		(* it).second->hold();
	}*/

	String sipUri(phoneNumber);
	size_t length = strspn(sipUri.c_str(), " .,;:()[]{}-_/#+0123456789");
	if (length == sipUri.length()) {
		//sipUri is a real phone number not a SIP URI or a pseudo

		sipUri.remove(" ");
		sipUri.remove(".");
		sipUri.remove(",");
		sipUri.remove(";");
		sipUri.remove(":");
		sipUri.remove("(");
		sipUri.remove(")");
		sipUri.remove("[");
		sipUri.remove("]");
		sipUri.remove("{");
		sipUri.remove("}");
		sipUri.remove("-");
		sipUri.remove("_");
		sipUri.remove("/");
		sipUri.remove("#");

		//Replaces + by 00
		sipUri.replace("+", "00");
	}

	if (!sipUri.contains("@")) {
		//Not a SIP URI
		sipUri = "sip:" + sipUri + "@" + getSipAccount().getRealm();
	}

	SipAddress sipAddress(sipUri);

	//PhoneCall in waiting state
	//PhoneCall * phoneCall = createWaitingPhoneCall();
	PhoneCall * phoneCall = new PhoneCall(*this, sipAddress);
	_activePhoneCall = phoneCall;

	int callId = _sipWrapper->makeCall(_lineId, sipUri);
	phoneCall->setCallId(callId);

	//Adds the PhoneCall to the list of PhoneCall
	_phoneCallHash[callId] = phoneCall;

	//Sends the event a new PhoneCall has been created
	phoneCallCreatedEvent(*this, *phoneCall);

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
		_sipWrapper->removeVirtualLine(_lineId);
	}
}

void PhoneLine::acceptCall(int callId) {
	_sipWrapper->acceptCall(callId);
}

void PhoneLine::rejectCall(int callId) {
	_sipWrapper->rejectCall(callId);
}

void PhoneLine::closeCall(int callId) {
	PhoneCall * phoneCall = _phoneCallHash[callId];
	if (!phoneCall) {
		LOG_FATAL("rejecting an unknow phone call");
	}

	if (_activePhoneCall == phoneCall) {
		_activePhoneCall = NULL;
	}

	//Deletes the PhoneCall that is closed now
	//Removes it from the list of PhoneCall
	//Cannot do that here
	/*delete _phoneCallHash[callId];
	_phoneCallHash[callId] = NULL;*/

	_sipWrapper->closeCall(callId);
	LOG_DEBUG("call closed callId=" + String::fromNumber(callId));
}

void PhoneLine::holdCall(int callId) {
	_sipWrapper->holdCall(callId);
}

void PhoneLine::resumeCall(int callId) {
	_sipWrapper->resumeCall(callId);
}

void PhoneLine::playTone(int callId, EnumTone::Tone tone) {
	_sipWrapper->playTone(callId, tone);
}

void PhoneLine::playSoundFile(int callId, const std::string & soundFile) {
	_sipWrapper->playSoundFile(callId, soundFile);
}

void PhoneLine::setPhoneCallState(int callId, int status, const SipAddress & sipAddress) {
	LOG_DEBUG("call state changed callId=" + String::fromNumber(callId) +
		" status=" + String::fromNumber(status) +
		" from=" + sipAddress.getSipAddress());

	//FIXME
	//No double call for the moment: one active call at a time
	if (status == PhoneCallStateTalking::CODE || status == PhoneCallStateIncoming::CODE) {
		for (PhoneCalls::iterator it = _phoneCallHash.begin(); it != _phoneCallHash.end(); ++it) {
			PhoneCall * phoneCall = it->second;
			if (phoneCall) {
				const PhoneCallState & state = phoneCall->getState();
				if (phoneCall->getCallId() != callId) {
					if (state.getCode() == PhoneCallStateTalking::CODE ||
						state.getCode() == PhoneCallStateIncoming::CODE) {

						rejectCall(callId);
						return;
					}
				}
			}
		}
	}


	if (!_phoneCallHash[callId]) {
		//No PhoneCall for this callId

		if (!_waitingPhoneCall) {
			//Incoming call
			//Sends SIP code 180
			//TODO automatically??
			_sipWrapper->sendRingingNotification(callId);

			PhoneCall * phoneCall = new PhoneCall(*this, sipAddress);
			phoneCall->setCallId(callId);

			//Adds the PhoneCall to the list of PhoneCall
			_phoneCallHash[callId] = phoneCall;

			//Sends the event a new PhoneCall has been created
			phoneCallCreatedEvent(*this, *_phoneCallHash[callId]);

			phoneCall->setState(status);

			if (!_activePhoneCall) {
				_activePhoneCall = phoneCall;
				//this.status = Status.RECEIVING;
			}

			//Adds the PhoneCall to the history
			//phoneCall.type = "Appel entrant";
			//this._softphone.history.addMemento(phoneCall);
		}

		else {
			//Outgoing call, 1st callback
			_waitingPhoneCall->setCallId(callId);
			_waitingPhoneCall->setState(status);

			//Adds the PhoneCall to the list of PhoneCall
			_phoneCallHash[callId] = _waitingPhoneCall;

			_activePhoneCall = _waitingPhoneCall;

			_waitingPhoneCall = NULL;
			//_activePhoneCall->setAsMakeCall();
			//this.status = Status.CALLING;

			//We can make new call
			//isAvailableForNewCall_t = true;
		}
	}

	else {
		//No callId? so PhoneCall already closed

		_phoneCallHash[callId]->setState(status);

		if (status == PhoneCallStateClosed::CODE || status == PhoneCallStateError::CODE) {
			//this.ClosePhoneCall(callid);
			if (_waitingPhoneCall && _waitingPhoneCall->getCallId() == callId) {
				_waitingPhoneCall = NULL;
			}
			if (_activePhoneCall && _activePhoneCall->getCallId() == callId) {
				_activePhoneCall = NULL;
			}
		} else {
			_waitingPhoneCall = NULL;
		}
		//this.StatusChangeSendEvent(this.status);
	}
}

PhoneCall * PhoneLine::createWaitingPhoneCall() {
	/*if (_waitingPhoneCall) {
		LOG_ERROR("can't have more than one waiting PhoneCall");
		return _waitingPhoneCall;
	}

	//this.status = Status.CALLING;
	//this.StatusChangeSendEvent(this.status);
	//isAvailableForNewCall_t = false;
	_waitingPhoneCall = new PhoneCall(this);

	return _waitingPhoneCall;*/
	return NULL;
}

void PhoneLine::setState(int status) {
	for (unsigned int i = 0; i < _phoneLineStateList.size(); i++) {
		PhoneLineState * state = _phoneLineStateList[i];
		if (state->getCode() == status) {
			if (_state->getCode() != state->getCode()) {
				_state = state;
				_state->execute(*this);
				LOG_DEBUG("line state changed lineId=" + String::fromNumber(_lineId) + " state=" + _state->toString());
				stateChangedEvent(*this);
				break;
			}
		}
	}
}

PhoneCall * PhoneLine::getPhoneCall(int callId) {
	return _phoneCallHash[callId];
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
	string nat = config.getNetworkNatType();
	EnumNatType::NatType natType;
	if (nat == "NatTypeOpen") {
		natType = EnumNatType::NatTypeOpen;
	} else if (nat == "NatTypeFullCone") {
		natType = EnumNatType::NatTypeFullCone;
	} else if (nat == "NatTypeRestrictedCone") {
		natType = EnumNatType::NatTypeRestrictedCone;
	} else if (nat == "NatTypePortRestrictedCone") {
		natType = EnumNatType::NatTypePortRestrictedCone;
	} else if (nat == "NatTypeSymmetric") {
		natType = EnumNatType::NatTypeSymmetric;
	} else if (nat == "NatTypeSymmetricFirewall") {
		natType = EnumNatType::NatTypeSymmetricFirewall;
	} else if (nat == "NatTypeBlocked") {
		natType = EnumNatType::NatTypeBlocked;
	} else if (nat == "NatTypeFailure") {
		natType = EnumNatType::NatTypeFailure;
	} else if (nat == "NatTypeUnknown") {
		natType = EnumNatType::NatTypeUnknown;
	} else {
		LOG_FATAL("unknown NAT type");
	}
	_sipWrapper->setNatType(natType);

	//Setting audio devices
	_sipWrapper->setCallOutputAudioDevice(config.getAudioOutputDeviceName());
	_sipWrapper->setCallInputAudioDevice(config.getAudioInputDeviceName());
	_sipWrapper->setRingerOutputAudioDevice(config.getAudioRingerDeviceName());

	//AEC + half duplex
	_sipWrapper->enableAEC(config.getAudioAEC());
	_sipWrapper->enableHalfDuplex(config.getAudioHalfDuplex());

	_sipWrapper->init();
}
