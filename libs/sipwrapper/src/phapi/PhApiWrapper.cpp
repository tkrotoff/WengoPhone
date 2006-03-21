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

#include "PhApiWrapper.h"

#include <imwrapper/IMChatSession.h>
#include <imwrapper/IMContact.h>

#include <Thread.h>
#include <StringList.h>
#include <Logger.h>
#include <File.h>
#include <Timer.h>

#include <AudioDevice.h>

#include <global.h>

#include <string>
using namespace std;

const std::string PhApiWrapper::PresenceStateOnline = "Online";
const std::string PhApiWrapper::PresenceStateAway = "Away";
const std::string PhApiWrapper::PresenceStateDoNotDisturb = "Do Not Disturb";


PhApiWrapper * PhApiWrapper::PhApiWrapperHack = NULL;
PhApiCallbacks * PhApiWrapper::_callbacks = NULL;

#ifdef ENABLE_VIDEO
static const int MEDIA_FLAGS = PH_STREAM_AUDIO | PH_STREAM_VIDEO_RX | PH_STREAM_VIDEO_TX;
#else
static const int MEDIA_FLAGS = PH_STREAM_AUDIO;
#endif

extern "C" {

static void callProgress(int callId, const phCallStateInfo_t * info) {
	PhApiWrapper::callProgress(callId, info);
}

static void videoFrameReceived(int callId, phVideoFrameReceivedEvent_t * info) {
	PhApiWrapper::videoFrameReceived(callId, info);
}

static void transferProgress(int /*callId*/, const phTransferStateInfo_t * /*info*/) {
}

static void conferenceProgress(int /*conferenceId*/, const phConfStateInfo_t * /*info*/) {
}

static void registerProgress(int registerId, int status) {
	PhApiWrapper::registerProgress(registerId, status);
}

static void messageProgress(int messageId, const phMsgStateInfo_t * info) {
	PhApiWrapper::messageProgress(messageId, info);
}

static void subscriptionProgress(int subscriptionId, const phSubscriptionStateInfo_t * info) {
	PhApiWrapper::subscriptionProgress(subscriptionId, info);
}

static void onNotify(const char * event, const char * from, const char * content) {
	PhApiWrapper::onNotify(event, from, content);
}

phCallbacks_t phApiCallbacks = {
	callProgress,
	transferProgress,
	conferenceProgress,
	registerProgress,
	messageProgress,
	onNotify,
	subscriptionProgress,
	videoFrameReceived
};

}	//"C"


PhApiWrapper::PhApiWrapper(PhApiCallbacks & callbacks) {
	_callbacks = &callbacks;
	_wengoVline = -1;
	_isInitialized = false;
	_tunnelNeeded = false;
	_tunnelPort = 0;
	_tunnelSSL = false;
	_natType = EnumNatType::NatTypeUnknown;
	_sipServerPort = 0;
	_sipLocalPort = 0;
	PhApiWrapperHack = this;
	_inputAudioDeviceId = 0;
	_outputAudioDeviceId = 0;

	//FIXME ugly hack for conference
	phoneCallStateChangedEvent += boost::bind(&PhApiWrapper::phoneCallStateChangedEventHandler, this, _1, _2, _3, _4);
}

PhApiWrapper::~PhApiWrapper() {
}

void PhApiWrapper::terminate() {
	if (_isInitialized) {
		phTerminate();
		_isInitialized = false;
	}
}

void PhApiWrapper::setNetworkParameter() {
	std::string natType = "auto";
	int natRefreshTime = 30;

	if (_tunnelNeeded) {
		//TODO: activate SSL for HTTP tunnel
		phTunnelConfig(_proxyServer.c_str(), _proxyPort, _tunnelServer.c_str(), _tunnelPort,
			_proxyLogin.c_str(), _proxyPassword.c_str(), 0);

		natType = "fcone";
	} else {
		switch(_natType) {
		case EnumNatType::NatTypeOpen:
			natType = "none";
			natRefreshTime = 0;
			break;

		case EnumNatType::NatTypeFullCone:
			natType = "fcone";
			break;

		case EnumNatType::NatTypeRestrictedCone:
			natType = "rcone";
			break;

		case EnumNatType::NatTypePortRestrictedCone:
			natType = "prcone";
			break;

		case EnumNatType::NatTypeSymmetric:
		case EnumNatType::NatTypeSymmetricFirewall:
		case EnumNatType::NatTypeBlocked:
		case EnumNatType::NatTypeFailure:
		case EnumNatType::NatTypeUnknown:
			natType = "sym";
			break;

		default:
			LOG_FATAL("unknown NAT type");
		}

		phcfg.use_tunnel = 0;
	}

	strncpy(phcfg.nattype, natType.c_str(), sizeof(phcfg.nattype));
	phcfg.nat_refresh_time = natRefreshTime;
}

int PhApiWrapper::addVirtualLine(const std::string & displayName,
	const std::string & username,
	const std::string & identity,
	const std::string & password,
	const std::string & realm,
	const std::string & proxyServer,
	const std::string & registerServer) {

	static const int REGISTER_TIMEOUT = 49 * 60;

	int ret = SipWrapper::VirtualLineIdError;
	if (_isInitialized) {
		phAddAuthInfo(username.c_str(), identity.c_str(), password.c_str(), String::null.c_str(), realm.c_str());
		ret = phAddVline2(displayName.c_str(), identity.c_str(), registerServer.c_str(), proxyServer.c_str(), REGISTER_TIMEOUT);
	}

	String tmp(realm);
	if (tmp.contains("wengo")) {
		_wengoVline = ret;
		_wengoSipAddress = "sip:" + identity + "@" + realm;
		_wengoRealm = realm;
	}

	return ret;
}

void PhApiWrapper::removeVirtualLine(int lineId) {
	if (_isInitialized) {
		publishOffline("");
		phDelVline(lineId);
	}
}

int PhApiWrapper::makeCall(int lineId, const std::string & sipAddress) {
	LOG_DEBUG("call=" + sipAddress);
	return phLinePlaceCall2(lineId, sipAddress.c_str(), NULL, 0, MEDIA_FLAGS);
}

void PhApiWrapper::sendRingingNotification(int callId) {
	phRingingCall(callId);
}

void PhApiWrapper::acceptCall(int callId) {
	phAcceptCall3(callId, 0, MEDIA_FLAGS);
}

void PhApiWrapper::rejectCall(int callId) {
	static const int busy = 486;

	phRejectCall(callId, busy);
}

void PhApiWrapper::closeCall(int callId) {
	phCloseCall(callId);
}

void PhApiWrapper::holdCall(int callId) {
	phHoldCall(callId);
}

void PhApiWrapper::resumeCall(int callId) {
	phResumeCall(callId);
}

void PhApiWrapper::muteCall(int callId) {
}

void PhApiWrapper::blindTransfer(int callId, const std::string & sipAddress) {
	phBlindTransferCall(callId, sipAddress.c_str());
}

void PhApiWrapper::playTone(int callId, EnumTone::Tone tone) {
	static const int INBAND_DTMF = 1;

	LOG_DEBUG("callId=" + String::fromNumber(callId) + " tone=" + String::fromNumber(tone));

	char dtmf;

	switch (tone) {
	case EnumTone::Tone0:
		dtmf = '0';
		break;

	case EnumTone::Tone1:
		dtmf = '1';
		break;

	case EnumTone::Tone2:
		dtmf = '2';
		break;

	case EnumTone::Tone3:
		dtmf = '3';
		break;

	case EnumTone::Tone4:
		dtmf = '4';
		break;

	case EnumTone::Tone5:
		dtmf = '5';
		break;

	case EnumTone::Tone6:
		dtmf = '6';
		break;

	case EnumTone::Tone7:
		dtmf = '7';
		break;

	case EnumTone::Tone8:
		dtmf = '8';
		break;

	case EnumTone::Tone9:
		dtmf = '9';
		break;

	case EnumTone::ToneStar:
		dtmf = '*';
		break;

	case EnumTone::TonePound:
		dtmf = '#';
		break;

	case EnumTone::ToneDialtone:
		break;

	case EnumTone::ToneBusy:
		break;

	case EnumTone::ToneRingback:
		break;

	case EnumTone::ToneRingtone:
		break;

	case EnumTone::ToneCallFailed:
		break;

	case EnumTone::ToneSilence:
		break;

	case EnumTone::ToneBackspace:
		break;

	case EnumTone::ToneCallWaiting:
		break;

	case EnumTone::ToneCallHeld:
		break;

	case EnumTone::ToneLoudFastBusy:
		break;

	default:
		LOG_FATAL("unknown tone");
	}

	phSendDtmf(callId, dtmf, INBAND_DTMF);
}

void PhApiWrapper::playSoundFile(int callId, const std::string & soundFile) {
	phSendSoundFile(callId, soundFile.c_str());
}

void PhApiWrapper::callProgress(int callId, const phCallStateInfo_t * info) {
	_callbacks->callProgress(callId, info);
}

void PhApiWrapper::registerProgress(int lineId, int status) {
	_callbacks->registerProgress(lineId, status);
}

void PhApiWrapper::videoFrameReceived(int callId, phVideoFrameReceivedEvent_t * info) {
	_callbacks->videoFrameReceived(callId, info);
}

bool PhApiWrapper::setCallInputAudioDevice(const std::string & deviceName) {
	_inputAudioDeviceId = AudioDevice::getWaveInDeviceId(deviceName);
	return setAudioDevices();
}

bool PhApiWrapper::setRingerOutputAudioDevice(const std::string & deviceName) {
	return false;
}

bool PhApiWrapper::setCallOutputAudioDevice(const std::string & deviceName) {
	_outputAudioDeviceId = AudioDevice::getWaveOutDeviceId(deviceName);
	return setAudioDevices();
}

bool PhApiWrapper::setAudioDevices() {
#ifndef OS_MACOSX
	//Uses PortAudio
	static const std::string INPUT_DEVICE_TAG = "pa:IN=";
	static const std::string OUTPUT_DEVICE_TAG  = "OUT=";

	std::string tmp = INPUT_DEVICE_TAG
			+ String::fromNumber(_inputAudioDeviceId)
			+ std::string(" ")
			+ OUTPUT_DEVICE_TAG
			+ String::fromNumber(_outputAudioDeviceId);

	//Takes the default Windows audio device
	//std::string tmp = "pa:";
#else
	//FIXME: needs to be changed to take the set audio device
	//(currently takes the default audio device)
	std::string tmp = "ca:";
#endif

	int ret = phChangeAudioDevices(tmp.c_str());
	if (ret == 0) {
		//Ok
		return true;
	}
	return false;
}

void PhApiWrapper::enableAEC(bool enable) {
	if (enable) {
		phcfg.noaec = 0;
		LOG_DEBUG("AEC enabled");
	} else {
		phcfg.noaec = 1;
		LOG_DEBUG("AEC disabled");
	}
}

void PhApiWrapper::enableHalfDuplex(bool enable) {
	if (enable) {
		//Half duplex (mode where speaker signal has priority over microphone signal)
		phcfg.hdxmode = PH_HDX_MODE_SPK;
		LOG_DEBUG("half-duplex enabled");
	} else {
		phcfg.hdxmode = 0;
		LOG_DEBUG("half-duplex disabled");
	}
}


/*
 * IM API implementation
 */

void PhApiWrapper::connect() {
	loginStatusEvent(*this, IMConnect::LoginStatusConnected);
}

void PhApiWrapper::disconnect() {
	removeVirtualLine(_wengoVline);
	loginStatusEvent(*this, IMConnect::LoginStatusDisconnected);
}

void PhApiWrapper::sendMessage(IMChatSession & chatSession, const std::string & message) {
	LOG_DEBUG("sending message: " + message);
	const IMContactSet & buddies = chatSession.getIMContactSet();
	IMContactSet::const_iterator it;
	for (it = buddies.begin(); it != buddies.end(); it++) {
		std::string sipAddress = "sip:" + (*it).getContactId() + "@" + _wengoRealm;
		int messageId = phLineSendMessage(_wengoVline, sipAddress.c_str(), message.c_str());
		//_messageIdChatSessionMap[messageId] = &chatSession;
	}
}

void PhApiWrapper::createSession(IMChat & imChat, IMContactSet & imContactSet) {
	IMChatSession * imChatSession = new IMChatSession(imChat);

	//FIXME: Currently, phApi supports chat with only one person
	if (imContactSet.size() > 0) {
		addContact(*imChatSession, (*imContactSet.begin()).getContactId());
		//FIXME: this map is never emptied
		//_contactChatMap[(*imContactSet.begin()).getContactId()] = imChatSession;
	}

	newIMChatSessionCreatedEvent(*this, *imChatSession);
}

void PhApiWrapper::closeSession(IMChatSession & chatSession) {
}

void PhApiWrapper::addContact(IMChatSession & chatSession, const std::string & contactId) {
	//FIXME: this map is never emptied
	//FIXME: Currently, phApi supports chat with only one person
	_contactChatMap[contactId] = &chatSession;
	contactAddedEvent(*this, chatSession, contactId);
}

void PhApiWrapper::removeContact(IMChatSession & chatSession, const std::string & contactId) {
	contactRemovedEvent(*this, chatSession, contactId);
}

void PhApiWrapper::changeMyPresence(EnumPresenceState::PresenceState state, const std::string & note) {
	switch(state) {
	case EnumPresenceState::PresenceStateOnline:
		publishOnline(PresenceStateOnline);
		break;

	case EnumPresenceState::PresenceStateAway:
		publishOnline(PresenceStateAway);
		break;

	case EnumPresenceState::PresenceStateDoNotDisturb:
		publishOnline(PresenceStateDoNotDisturb);
		break;

	case EnumPresenceState::PresenceStateUserDefined:
		publishOnline(note);
		break;

	case EnumPresenceState::PresenceStateOffline:
		publishOffline(note);
		break;

	case EnumPresenceState::PresenceStateUnknown:
		myPresenceStatusEvent(*this, EnumPresenceState::MyPresenceStatusError, note);
		break;

	default:
		LOG_FATAL("unknown presence state");
	}
}

void PhApiWrapper::publishOnline(const std::string & note) {
	if (_isInitialized) {
		std::string pidfOnline = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
		pidfOnline += "<presence entity=\"";
		pidfOnline += _wengoSipAddress;
		pidfOnline += "\">\n";
		pidfOnline += "<tuple id=\"azersdqre\">\n";
		pidfOnline += "<status><basic>open</basic>";
		pidfOnline += "<note>";
		pidfOnline += note;
		pidfOnline += "</note></status>\n";
		pidfOnline += "<contact priority=\"1\">";
		pidfOnline += _wengoSipAddress;
		pidfOnline += "</contact>\n";
		pidfOnline += "</tuple>\n";
		pidfOnline += "</presence>\n";

		publishPresence(pidfOnline, note);
	}
}

void PhApiWrapper::publishOffline(const std::string & note) {
	if (_isInitialized) {
		std::string pidfOffline = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
		pidfOffline += "<presence entity=\"";
		pidfOffline += _wengoSipAddress;
		pidfOffline += "\">\n";
		pidfOffline += "<tuple id=\"azersdqre\">\n";
		pidfOffline += "<status><basic>close</basic></status>\n";
		pidfOffline += "<contact priority=\"1\">";
		pidfOffline += _wengoSipAddress;
		pidfOffline += "</contact>\n";
		pidfOffline += "</tuple>\n";
		pidfOffline += "</presence>\n";

		publishPresence(pidfOffline, note);
	}
}

void PhApiWrapper::publishPresence(const std::string & pidf, const std::string & note) {
	static const std::string contentType = "application/pidf+xml";

	if (_isInitialized) {
		int phError = phLinePublish(_wengoVline, _wengoSipAddress.c_str(), 0, contentType.c_str(), pidf.c_str());
		if (phError != 0) {
			myPresenceStatusEvent(*this, EnumPresenceState::MyPresenceStatusError, note);
		} else {
			myPresenceStatusEvent(*this, EnumPresenceState::MyPresenceStatusOk, note);
		}
	}

	/* Timer to renew publish presence */

	//Renew presence every 9 minutes
	static const unsigned PUBLISH_TIMEOUT = 9 * 60 * 1000;

	if (_lastPidf.empty()) {
		//Creates and launches the timer only once
		static Timer timer;
		timer.timeoutEvent += boost::bind(&PhApiWrapper::renewPublishEventHandler, this);
		timer.start(PUBLISH_TIMEOUT, PUBLISH_TIMEOUT);
	}

	//Saves the pidf
	_lastPidf = pidf;
	_lastNote = note;
}

void PhApiWrapper::renewPublishEventHandler() {
	publishPresence(_lastPidf, _lastNote);
}

void PhApiWrapper::subscribeToPresenceOf(const std::string & contactId) {
	std::string sipAddress = "sip:" + contactId + "@" + _wengoRealm;

	if (phLineSubscribe(_wengoVline, sipAddress.c_str(), 0) != 0) {
		subscribeStatusEvent(*this, sipAddress, IMPresence::SubscribeStatusError);
	} else {
		subscribeStatusEvent(*this, sipAddress, IMPresence::SubscribeStatusOk);
	}
}

void PhApiWrapper::messageProgress(int messageId, const phMsgStateInfo_t * info) {
	_callbacks->messageProgress(messageId, info);
}

void PhApiWrapper::subscriptionProgress(int subscriptionId, const phSubscriptionStateInfo_t * info) {
	_callbacks->subscriptionProgress(subscriptionId, info);
}

void PhApiWrapper::onNotify(const char * event, const char * from, const char * content) {
	_callbacks->onNotify(event, from, content);
}

void PhApiWrapper::allowWatcher(const std::string & watcher) {
	static const int winfo = 1;	//Publish with event = presence.winfo
	static const std::string contentType = "application/watcherinfo+xml";

	std::string winfoAllow = "<?xml version=\"1.0\"?>\n";
	winfoAllow += "<watcherinfo>\n";
	winfoAllow += "<watcher-list>\n";
	winfoAllow += "<watcher status=\"active\">";
	winfoAllow += watcher;
	winfoAllow += "</watcher>\n";
	winfoAllow += "</watcher-list>\n";
	winfoAllow += "</watcherinfo>\n";

	phLinePublish(_wengoVline, _wengoSipAddress.c_str(), winfo, contentType.c_str(), winfoAllow.c_str());
}

void PhApiWrapper::forbidWatcher(const std::string & watcher) {
	static const int winfo = 1;	//Publish with event = presence.winfo
	static const std::string contentType = "application/watcherinfo+xml";

	std::string winfoForbid = "<?xml version=\"1.0\"?>\n";
	winfoForbid += "<watcherinfo>\n";
	winfoForbid += "<watcher-list>\n";
	winfoForbid += "<watcher status=\"pending\">";
	winfoForbid += watcher;
	winfoForbid += "</watcher>\n";
	winfoForbid += "</watcher-list>\n";
	winfoForbid += "</watcherinfo>\n";

	phLinePublish(_wengoVline, _wengoSipAddress.c_str(), winfo, contentType.c_str(), winfoForbid.c_str());
}

void PhApiWrapper::blockContact(const std::string & contactId) {
	std::string sipAddress = "sip:" + contactId + "@" + _wengoRealm;
	allowWatcher(sipAddress);
}

void PhApiWrapper::unblockContact(const std::string & contactId) {
	std::string sipAddress = "sip:" + contactId + "@" + _wengoRealm;
	allowWatcher(sipAddress);
}

void PhApiWrapper::setProxy(const std::string & address, unsigned port,
	const std::string & login, const std::string & password) {

	_proxyServer = address;
	_proxyPort = port;
	_proxyLogin = login;
	_proxyPassword = password;
}

void PhApiWrapper::setTunnel(const std::string & address, unsigned port, bool ssl) {
	_tunnelNeeded = true;
	_tunnelServer = address;
	_tunnelPort = port;
	_tunnelSSL = ssl;
}

void PhApiWrapper::setNatType(EnumNatType::NatType natType) {
	_natType = natType;
}

void PhApiWrapper::setSIP(const string & server, unsigned serverPort, unsigned localPort) {
	_sipServer = server;
	_sipServerPort = serverPort;
	_sipLocalPort = localPort;
}


/*
** Conference Algorithm **

int callId1 = placeCall();
//wait for CALLOK on callId1
phHold(callId1);
int callId2 = placeCall();
//wait for CALLOK on callId2
phConf(callId1, callId2);
phResume(callId1);
phStopConf(callId1, callId2);

*/

static int callId1 = -1;
static int callId2 = -1;
static int callId1WaitsForTalkingState = false;
static int callId2WaitsForTalkingState = false;
//               callId, state
static std::map<int, EnumPhoneCallState::PhoneCallState> callMap;


int PhApiWrapper::createConference() {
	//FIXME return phConfCreate();

	return 1;
}

void PhApiWrapper::destroyConference(int confId) {
	//FIXME phConfClose(confId);

	if (callId1 != -1 && callId2 != -1) {
		phStopConf(callId1, callId2);
	}

	callId1 = -1;
	callId1WaitsForTalkingState = false;
	callId2 = -1;
	callId2WaitsForTalkingState = false;

	LOG_DEBUG("conference call destroyed");
}

void PhApiWrapper::phoneCallStateChangedEventHandler(SipWrapper & sender, int callId,
	EnumPhoneCallState::PhoneCallState state, const std::string & from) {

	callMap[callId] = state;

	if (state == EnumPhoneCallState::PhoneCallStateTalking) {

		if (callId1 != -1 && callId == callId1 && callId1WaitsForTalkingState) {
			holdCall(callId1);
			Thread::sleep(2);
		}

		else if (callId2 != -1 && callId == callId2 && callId2WaitsForTalkingState) {
			Thread::sleep(2);
			phConf(callId1, callId2);
			Thread::sleep(2);
			resumeCall(callId1);
			LOG_DEBUG("conference call started");
		}
	}
}

void PhApiWrapper::joinConference(int confId, int callId) {
	//FIXME phConfAddMember(confId, callId);

	if (callId1 == -1) {
		callId1 = callId;

		if (callMap[callId1] == EnumPhoneCallState::PhoneCallStateTalking) {
			holdCall(callId1);
			Thread::sleep(2);
		} else {
			callId1WaitsForTalkingState = true;
		}
	}

	else if (callId2 == -1) {
		callId2 = callId;

		if (callMap[callId2] == EnumPhoneCallState::PhoneCallStateTalking
			&& callMap[callId1] == EnumPhoneCallState::PhoneCallStateHold) {

			Thread::sleep(2);
			phConf(callId1, callId2);
			Thread::sleep(2);
			resumeCall(callId1);
			LOG_DEBUG("conference call started");
		} else {
			callId2WaitsForTalkingState = true;
		}

	}


	if (callId1 != -1 && callId2 != -1) {
		/*
		phConf(callId1, callId2);
		resumeCall(callId1);
		LOG_DEBUG("conference call started");
		*/
	}
}

void PhApiWrapper::splitConference(int confId, int callId) {
	//FIXME phConfRemoveMember(confId, callId);

	if (callId1 != -1 && callId2 != -1) {
		phStopConf(callId1, callId2);
		callId1 = -1;
		callId1WaitsForTalkingState = false;
		callId2 = -1;
		callId2WaitsForTalkingState = false;
	} else {
		if (callId1 == callId) {
			closeCall(callId1);
			callId1 = -1;
			callId1WaitsForTalkingState = false;
		} else if (callId2 == callId) {
			closeCall(callId2);
			callId2 = -1;
			callId2WaitsForTalkingState = false;
		}
	}
}


void PhApiWrapper::init() {
	setNetworkParameter();

	//Plugin path
	strncpy(phcfg.plugin_path, _pluginPath.c_str(), sizeof(phcfg.plugin_path));

	//Codec list
	strncpy(phcfg.audio_codecs, "G726-64wb/16000,ILBC/8000,PCMU/8000,PCMA/8000,GSM/8000", sizeof(phcfg.audio_codecs));
	strncpy(phcfg.video_codecs, "H263,H264,MPEG4", sizeof(phcfg.video_codecs));

	strncpy(phcfg.proxy, _sipServer.c_str(), sizeof(phcfg.proxy));

	String localPort = String::fromNumber(_sipLocalPort);
	strncpy(phcfg.sipport, localPort.c_str(), sizeof(phcfg.sipport));

	//Ignored since we are in direct link mode
	static const std::string phApiServer = "127.0.0.1";

	//If asynchronous mode = false then we have to call phPoll()
	static const bool asynchronousMode = true;

	int ret = phInit(&phApiCallbacks, (char *) phApiServer.c_str(), asynchronousMode);
	if (ret == PhApiResultNoError) {
		_isInitialized = true;
		LOG_DEBUG("phApi successfully initialized");
	} else {
		_isInitialized = false;
		LOG_DEBUG("cannot initialize phApi");
	}
}

void PhApiWrapper::setPluginPath(const string & path) {
	_pluginPath = path;
}

std::string PhApiWrapper::phapiCallStateToString(enum  phCallStateEvent event) {
	std::string toReturn = "";
	switch(event) {
	case phDIALING:
		toReturn = "phDIALING";
		break;
	case phRINGING:
		toReturn = "phRINGING";
		break;
	case phNOANSWER:
		toReturn = "phNOANSWER";
		break;
	case phCALLBUSY:
		toReturn = "phCALLBUSY";
		break;
	case phCALLREDIRECTED:
		toReturn = "phCALLREDIRECTED";
		break;
	case phCALLOK:
		toReturn = "phCALLOK";
		break;
	case phCALLHELD:
		toReturn = "phCALLHELD";
		break;
	case phCALLRESUMED:
		toReturn = "phCALLRESUMED";
		break;
	case phHOLDOK:
		toReturn = "phHOLDOK";
		break;
	case phRESUMEOK: 
		toReturn = "phRESUMEOK";
		break;
	case phINCALL:
		toReturn = "phINCALL";
		break;
	case phCALLCLOSED:
		toReturn = "phCALLCLOSED";
		break; 
	case phCALLERROR:
		toReturn = "phCALLERROR";
		break;
	case phDTMF:
		toReturn = "phDTMF";
		break;
	case phXFERPROGRESS:
		toReturn = "phXFERPROGRESS";
		break;
	case phXFEROK:
		toReturn = "phXFEROK";
		break;
	case phXFERFAIL:
		toReturn = "phXFERFAIL";
		break; 
	case phXFERREQ:
		toReturn = "phXFERREQ";
		break;
	case phCALLREPLACED:
		toReturn = "phCALLREPLACED";
		break;
	case phRINGandSTART:
		toReturn = "phRINGandSTART";
		break;
	case phRINGandSTOP: 
		toReturn = "phRINGandSTOP";
		break;
	case phCALLCLOSEDandSTOPRING:
		toReturn = "phCALLCLOSEDandSTOPRING";
		break;
	default:
	 	toReturn = "Unknown phapi state: " + String::fromNumber(event);
	}
	return toReturn;
}

