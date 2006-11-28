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

#include "PhApiCodecList.h"

#include "PhApiSFPWrapper.h"

#include <imwrapper/IMChatSession.h>
#include <imwrapper/IMContact.h>

#include <cutil/global.h>
#include <thread/Thread.h>
#include <thread/Timer.h>
#include <util/File.h>
#include <util/Logger.h>
#include <util/StringList.h>

#include <string>

#include <cstdio>

using namespace std;


const std::string PhApiWrapper::PresenceStateOnline = "Online";
const std::string PhApiWrapper::PresenceStateAway = "Away";
const std::string PhApiWrapper::PresenceStateDoNotDisturb = "Do Not Disturb";


PhApiWrapper * PhApiWrapper::PhApiWrapperHack = NULL;


#ifdef ENABLE_VIDEO
static const int VIDEO_FLAGS = OWPL_STREAM_AUDIO | OWPL_STREAM_VIDEO_RX | OWPL_STREAM_VIDEO_TX;
static const int VIDEO_RX_ONLY_FLAGS = OWPL_STREAM_AUDIO | OWPL_STREAM_VIDEO_RX;
#else
static const int VIDEO_FLAGS = OWPL_STREAM_AUDIO;
static const int VIDEO_RX_ONLY_FLAGS = OWPL_STREAM_AUDIO;
#endif

PhApiWrapper::PhApiWrapper(PhApiCallbacks & callbacks)
	: _phApiSFPWrapper(PhApiSFPWrapper::getInstance()) {
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
	_registered = false;

	//FIXME ugly hack for conference
	phoneCallStateChangedEvent +=
		boost::bind(&PhApiWrapper::phoneCallStateChangedEventHandler, this, _1, _2, _3, _4);
}

PhApiWrapper::~PhApiWrapper() {
	terminate();
}

void PhApiWrapper::terminate() {
	if (_isInitialized) {
		LOG_DEBUG("terminating phapi");
		phTerminate();
		_isInitialized = false;
	}
}

void PhApiWrapper::setNetworkParameter() {
	int natRefreshTime = 25;

	if (_tunnelNeeded) {
		if(owplConfigSetLocalHttpProxy(_proxyServer.c_str(), _proxyPort, _proxyLogin.c_str(), _proxyPassword.c_str()) != OWPL_RESULT_SUCCESS) {
			// TODO what? throw an exception? exit?
		}
		// activate SSL for HTTP tunnel
		int mask = OWPL_TUNNEL_USE;
		if(_tunnelSSL) {
			mask = mask | OWPL_TUNNEL_SSL;
		}
		if(owplConfigSetTunnel(_tunnelServer.c_str(), _tunnelPort, mask) != OWPL_RESULT_SUCCESS) {
			// TODO what? throw an exception? exit?
		}

		owplConfigSetNat(OWPL_NAT_TYPE_FCONE, natRefreshTime);

	} else {
		switch(_natType) {
		case EnumNatType::NatTypeOpen:
			owplConfigSetNat(OWPL_NAT_TYPE_NONE, 0);
			break;

		case EnumNatType::NatTypeFullCone:
			owplConfigSetNat(OWPL_NAT_TYPE_FCONE, natRefreshTime);
			break;

		case EnumNatType::NatTypeRestrictedCone:
			owplConfigSetNat(OWPL_NAT_TYPE_RCONE, natRefreshTime);
			break;

		case EnumNatType::NatTypePortRestrictedCone:
			owplConfigSetNat(OWPL_NAT_TYPE_PRCONE, natRefreshTime);
			break;

		case EnumNatType::NatTypeSymmetric:
		case EnumNatType::NatTypeSymmetricFirewall:
		case EnumNatType::NatTypeBlocked:
		case EnumNatType::NatTypeFailure:
		case EnumNatType::NatTypeUnknown:
			owplConfigSetNat(OWPL_NAT_TYPE_SYMETRIC, natRefreshTime);
			break;

		default:
			owplConfigSetNat(OWPL_NAT_TYPE_AUTO, natRefreshTime);
			LOG_FATAL("unknown NAT type");
		}

		// in case of change after first initialization
		owplConfigSetTunnel(NULL, 0, OWPL_TUNNEL_NOT_USED);
	}

}

int PhApiWrapper::addVirtualLine(const std::string & displayName,
	const std::string & username,
	const std::string & identity,
	const std::string & password,
	const std::string & realm,
	const std::string & proxyServer,
	const std::string & registerServer) {

	static const int REGISTER_TIMEOUT = 49 * 60;
	OWPL_LINE gVline = -1;

	if (_isInitialized) {
		phAddAuthInfo(username.c_str(), identity.c_str(), password.c_str(), String::null, realm.c_str());

		std::string tmp = proxyServer;
		tmp += ":" + String::fromNumber(_sipServerPort);
		
		if(owplLineAdd(displayName.c_str(), identity.c_str(), registerServer.c_str(), tmp.c_str(), REGISTER_TIMEOUT, &gVline) != OWPL_RESULT_SUCCESS) {
			return SipWrapper::VirtualLineIdError;
		}
		if(owplLineRegister(gVline, 1) != OWPL_RESULT_SUCCESS) {
			return SipWrapper::VirtualLineIdError;
		}

		phoneLineStateChangedEvent(*this, gVline, EnumPhoneLineState::PhoneLineStateProgress);
	}

	_wengoVline = gVline;
	_wengoSipAddress = "sip:" + identity + "@" + realm;
	_wengoRealm = realm;

	return gVline;
}

void PhApiWrapper::removeVirtualLine(int lineId, int regTimeout) {
	if (_isInitialized) {
		publishOffline(String::null);
		phDelVline(lineId, regTimeout);
		if (regTimeout == 0) {
			setRegistered(false);
			phoneLineStateChangedEvent(*this, lineId, EnumPhoneLineState::PhoneLineStateClosed);
		}
	}
}

int PhApiWrapper::makeCall(int lineId, const std::string & sipAddress, bool enableVideo) {
	OWPL_CALL hCall;

	LOG_DEBUG("call=" + sipAddress);
	int mediaFlags = VIDEO_RX_ONLY_FLAGS;
	if (enableVideo) {
		mediaFlags = VIDEO_FLAGS;
	}

	if(owplCallCreate(lineId, &hCall) != OWPL_RESULT_SUCCESS) {
		return -1;
	}
	if(owplCallConnect(hCall, sipAddress.c_str(), mediaFlags) != OWPL_RESULT_SUCCESS) {
		return -1;
	}

	return hCall;
}

void PhApiWrapper::sendRingingNotification(int callId, bool enableVideo) {
	int mediaFlags = VIDEO_RX_ONLY_FLAGS;
	if (enableVideo) {
		mediaFlags = VIDEO_FLAGS;
	}

	owplCallAccept(callId, mediaFlags);
}

void PhApiWrapper::acceptCall(int callId, bool enableVideo) {
	int mediaFlags = VIDEO_RX_ONLY_FLAGS;
	if (enableVideo) {
		mediaFlags = VIDEO_FLAGS;
	}

	owplCallAnswer(callId, mediaFlags);
}

void PhApiWrapper::rejectCall(int callId) {
	owplCallReject(callId, 486, NULL);
}

void PhApiWrapper::closeCall(int callId) {
	owplCallDisconnect(callId);
}

void PhApiWrapper::holdCall(int callId) {
	//Thread::sleep(3);
	
	owplCallHold(callId);

	//Thread::sleep(3);
}

void PhApiWrapper::resumeCall(int callId) {
	owplCallUnhold(callId);
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

CodecList::AudioCodec PhApiWrapper::getAudioCodecUsed(int callId) {
	static const int LENGTH = 255;
	static char audioCodec[LENGTH];
	static char videoCodec[LENGTH];

	phCallGetCodecs(callId, audioCodec, LENGTH, videoCodec, LENGTH);
	String tmp(audioCodec);
	if (tmp.contains(PhApiCodecList::AUDIO_CODEC_PCMU)) {
		return CodecList::AudioCodecPCMU;
	}
	else if (tmp.contains(PhApiCodecList::AUDIO_CODEC_PCMA)) {
		return CodecList::AudioCodecPCMA;
	}
	else if (tmp.contains(PhApiCodecList::AUDIO_CODEC_ILBC)) {
		return CodecList::AudioCodecILBC;
	}
	else if (tmp.contains(PhApiCodecList::AUDIO_CODEC_GSM)) {
		return CodecList::AudioCodecGSM;
	}
	else if (tmp.contains(PhApiCodecList::AUDIO_CODEC_AMRNB)) {
		return CodecList::AudioCodecAMRNB;
	}
	else if (tmp.contains(PhApiCodecList::AUDIO_CODEC_AMRWB)) {
		return CodecList::AudioCodecAMRWB;
	}
	else if (tmp.contains(PhApiCodecList::AUDIO_CODEC_SPEEXWB)) {
		return CodecList::AudioCodecSPEEXWB;
	}
	else if (tmp.empty()) {
		return CodecList::AudioCodecError;
	}
	else {
		LOG_FATAL("unknown codec=" + tmp);
		return CodecList::AudioCodecError;
	}
}

CodecList::VideoCodec PhApiWrapper::getVideoCodecUsed(int callId) {
	static const int LENGTH = 255;
	static char audioCodec[LENGTH];
	static char videoCodec[LENGTH];

	phCallGetCodecs(callId, audioCodec, LENGTH, videoCodec, LENGTH);
	String tmp(videoCodec);
	if (tmp.contains(PhApiCodecList::VIDEO_CODEC_H263)) {
		return CodecList::VideoCodecH263;
	}
	else if (tmp.contains(PhApiCodecList::VIDEO_CODEC_H264)) {
		return CodecList::VideoCodecH264;
	}
	else if (tmp.contains(PhApiCodecList::VIDEO_CODEC_MPEG4)) {
		return CodecList::VideoCodecMPEG4;
	}
	else if (tmp.empty()) {
		return CodecList::VideoCodecError;
	}
	else {
		LOG_FATAL("unknown codec=" + tmp);
		return CodecList::VideoCodecError;
	}
}

bool PhApiWrapper::setCallInputAudioDevice(const AudioDevice & device) {
	_inputAudioDevice = device;
	return setAudioDevices();
}

bool PhApiWrapper::setRingerOutputAudioDevice(const AudioDevice & device) {
	return false;
}

bool PhApiWrapper::setCallOutputAudioDevice(const AudioDevice & device) {
	_outputAudioDevice = device;
	return setAudioDevices();
}

bool PhApiWrapper::setAudioDevices() {
	static const std::string OUTPUT_DEVICE_TAG  = "OUT=";
	std::string devices;

#ifdef OS_MACOSX
	static const std::string INPUT_DEVICE_TAG = "ca:IN=";
	devices = INPUT_DEVICE_TAG
		+ _inputAudioDevice.getData().toString(":")
		+ std::string(" ") + OUTPUT_DEVICE_TAG
		+ _outputAudioDevice.getData().toString(":");
#else
	static const std::string INPUT_DEVICE_TAG = "pa:IN=";
	devices = INPUT_DEVICE_TAG
		+ _inputAudioDevice.getData()[1]
		+ std::string(" ") + OUTPUT_DEVICE_TAG
		+ _outputAudioDevice.getData()[1];
#endif

	int ret = phChangeAudioDevices(devices.c_str());
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
	if (isRegistered())
		connectedEvent(*this);
}

void PhApiWrapper::disconnect() {
	removeVirtualLine(_wengoVline);
	disconnectedEvent(*this, false, String::null);
}

void PhApiWrapper::sendMessage(IMChatSession & chatSession, const std::string & message) {
	LOG_DEBUG("sending message: " + message);
	const IMContactSet & buddies = chatSession.getIMContactSet();
	IMContactSet::const_iterator it;
	int messageId = -1;

	for (it = buddies.begin(); it != buddies.end(); it++) {
		std::string sipAddress = "sip:" + (*it).getContactId() + "@" + _wengoRealm;
		owplMessageSendPlainText(_wengoVline,
					sipAddress.c_str(),
					message.c_str(),
					&messageId);
	}
}

void PhApiWrapper::changeTypingState(IMChatSession & chatSession, IMChat::TypingState state) {
	const IMContactSet & buddies = chatSession.getIMContactSet();
	IMContactSet::const_iterator it;
	int messageId = -1;

	for (it = buddies.begin(); it != buddies.end(); it++) {
		std::string sipAddress = "sip:" + (*it).getContactId() + "@" + _wengoRealm;

		switch (state) {
			case IMChat::TypingStateTyping :
				owplMessageSendTypingState(_wengoVline,
					sipAddress.c_str(),
					OWPL_TYPING_STATE_TYPING,
					&messageId);
				break;

			case IMChat::TypingStateStopTyping :
				owplMessageSendTypingState(_wengoVline,
					sipAddress.c_str(),
					OWPL_TYPING_STATE_STOP_TYPING,
					&messageId);
				break;

			default :
				owplMessageSendTypingState(_wengoVline,
					sipAddress.c_str(),
					OWPL_TYPING_STATE_NOT_TYPING,
					&messageId);
				break;
		}
	}
}

void PhApiWrapper::createSession(IMChat & imChat, IMContactSet & imContactSet) {
	Mutex::ScopedLock lock(_mutex);

	IMChatSession * imChatSession;

	//FIXME: Currently, phApi supports chat with only one person
	if (imContactSet.size() > 0) {
		//Check if a session already exists with the contact (phApi can manage only
		// one contact) given in parameter
		const IMContact & imContact = *imContactSet.begin();
		if (_contactChatMap.find(imContact.getContactId()) != _contactChatMap.end()) {
			//A Session with this contact already exists
			imChatSession = _contactChatMap[imContact.getContactId()];
		} else {
			imChatSession = new IMChatSession(imChat, true);
			addContact(*imChatSession, (*imContactSet.begin()).getContactId());
		}
	}

	newIMChatSessionCreatedEvent(*this, *imChatSession);
	sendMyIcon((*imContactSet.begin()).getContactId(), _iconFilename);
}

void PhApiWrapper::closeSession(IMChatSession & sender) {
	Mutex::ScopedLock lock(_mutex);

	std::map<const std::string, IMChatSession *>::iterator it =
		_contactChatMap.find((*sender.getIMContactSet().begin()).getContactId());

	if (it != _contactChatMap.end()) {
		_contactChatMap.erase(it);
	} else {
		LOG_ERROR("Session not registered");
	}
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

	case EnumPresenceState::PresenceStateInvisible:
	case EnumPresenceState::PresenceStateOffline:
		publishOffline(note);
		break;

	case EnumPresenceState::PresenceStateUnknown:
		myPresenceStatusEvent(*this, EnumPresenceState::MyPresenceStatusError, note);
		break;

	default:
		LOG_FATAL("unknown presence state=" + String::fromNumber(state));
	}
}

void PhApiWrapper::sendMyIcon(const std::string & contactId, const std::string & iconFilename) {
	int messageId = -1;

	if (iconFilename.empty()) {
		return;
	}

	std::string sipAddress = "sip:" + contactId + "@" + _wengoRealm;
	owplMessageSendIcon(_wengoVline,
		sipAddress.c_str(),
		iconFilename.c_str(),
		&messageId);
}

void PhApiWrapper::publishOnline(const std::string & note) {
	if (_isInitialized) {
		publishPresence(true, note);
	}
}

void PhApiWrapper::publishOffline(const std::string & note) {
	if (_isInitialized) {
		publishPresence(false, note);
	}
}

void PhApiWrapper::publishPresence(bool online, const std::string & note) {
	static const std::string contentType = "application/pidf+xml";

	if (_isInitialized) {
		if(owplPresencePublish(_wengoVline, (online)?1:0, note.c_str(), NULL) != OWPL_RESULT_SUCCESS) {
			myPresenceStatusEvent(*this, EnumPresenceState::MyPresenceStatusError, note);
		} else {
			myPresenceStatusEvent(*this, EnumPresenceState::MyPresenceStatusOk, note);
		}
	}
}

void PhApiWrapper::subscribeToPresenceOf(const std::string & contactId) {
	std::string sipAddress = "sip:" + contactId + "@" + _wengoRealm;

	OWPL_SUB hSub;
	if(owplPresenceSubscribe(_wengoVline, sipAddress.c_str(), 0, &hSub) != OWPL_RESULT_SUCCESS) {
		subscribeStatusEvent(*this, sipAddress, IMPresence::SubscribeStatusError);
	} else {
		subscribeStatusEvent(*this, sipAddress, IMPresence::SubscribeStatusOk);
	}
}

void PhApiWrapper::unsubscribeToPresenceOf(const std::string & contactId) {
	std::string sipAddress = "sip:" + contactId + "@" + _wengoRealm;

	owplPresenceUnsubscribe(sipAddress.c_str());
}

void PhApiWrapper::allowWatcher(const std::string & watcher) {
	/*
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
	*/
	// TODO REFACTOR
	// owplPresencePublishAllowed(...);
}

void PhApiWrapper::forbidWatcher(const std::string & watcher) {
	/*
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
	*/
	// TODO REFACTOR
	// owplPresencePublishAllowed(...);
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

void PhApiWrapper::setVideoQuality(EnumVideoQuality::VideoQuality videoQuality) {
	phVideoControlSetWebcamCaptureResolution(320, 240);

	switch(videoQuality) {
	case EnumVideoQuality::VideoQualityNormal:
		phcfg.video_config.video_line_configuration = PHAPI_VIDEO_LINE_128KBPS;
		break;

	case EnumVideoQuality::VideoQualityGood:
		phcfg.video_config.video_line_configuration = PHAPI_VIDEO_LINE_256KBPS;
		break;

	case EnumVideoQuality::VideoQualityVeryGood:
		phcfg.video_config.video_line_configuration = PHAPI_VIDEO_LINE_512KBPS;
		break;

	case EnumVideoQuality::VideoQualityExcellent:
		phcfg.video_config.video_line_configuration = PHAPI_VIDEO_LINE_1024KBPS;
		break;

	default:
		LOG_FATAL("unknown video quality=" + String::fromNumber(videoQuality));
	}
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
phHold(callId2);
phConf(callId1, callId2);
phResume(callId1);
phResume(callId2);
phStopConf(callId1, callId2);

*/

static int callId1 = SipWrapper::CallIdError;
static int callId2 = SipWrapper::CallIdError;

int PhApiWrapper::createConference() {
	//FIXME return phConfCreate();

	return 1;
}

void PhApiWrapper::phoneCallStateChangedEventHandler(SipWrapper & sender, int callId,
	EnumPhoneCallState::PhoneCallState state, const std::string & from) {

	if (callId == callId1 &&
		(state == EnumPhoneCallState::PhoneCallStateClosed ||
		state == EnumPhoneCallState::PhoneCallStateError)) {

		callId1 = SipWrapper::CallIdError;
	}

	else if (callId == callId2 &&
		(state == EnumPhoneCallState::PhoneCallStateClosed ||
		state == EnumPhoneCallState::PhoneCallStateError)) {

		callId2 = SipWrapper::CallIdError;
	}
}

void PhApiWrapper::joinConference(int confId, int callId) {
	//FIXME phConfAddMember(confId, callId);

	if (callId1 == SipWrapper::CallIdError) {
		callId1 = callId;
	}

	else if (callId2 == SipWrapper::CallIdError) {
		callId2 = callId;
	}

	if (callId1 != SipWrapper::CallIdError && callId2 != SipWrapper::CallIdError) {
		//Thread::sleep(5);
		phConf(callId1, callId2);
		LOG_DEBUG("conference call started");
		resumeCall(callId1);
		resumeCall(callId2);
	}
}

void PhApiWrapper::splitConference(int confId, int callId) {
	//FIXME phConfRemoveMember(confId, callId);

	//FIXME phConfClose(confId);
	if (callId1 != SipWrapper::CallIdError && callId2 != SipWrapper::CallIdError) {
		phStopConf(callId1, callId2);
		callId1 = SipWrapper::CallIdError;
		callId2 = SipWrapper::CallIdError;
		LOG_DEBUG("conference call destroyed");
	}
}


void PhApiWrapper::init() {
	setNetworkParameter();

	//Plugin path
	strncpy(phcfg.plugin_path, _pluginPath.c_str(), sizeof(phcfg.plugin_path));

	//Codec list
	owplConfigAddAudioCodecByName(PhApiCodecList::AUDIO_CODEC_SPEEXWB.c_str());
	owplConfigAddAudioCodecByName(PhApiCodecList::AUDIO_CODEC_ILBC.c_str());
	owplConfigAddAudioCodecByName(PhApiCodecList::AUDIO_CODEC_AMRWB.c_str());
	owplConfigAddAudioCodecByName(PhApiCodecList::AUDIO_CODEC_PCMU.c_str());
	owplConfigAddAudioCodecByName(PhApiCodecList::AUDIO_CODEC_PCMA.c_str());
	owplConfigAddAudioCodecByName(PhApiCodecList::AUDIO_CODEC_AMRNB.c_str());
	owplConfigAddAudioCodecByName(PhApiCodecList::AUDIO_CODEC_GSM.c_str());
	owplConfigAddVideoCodecByName(PhApiCodecList::VIDEO_CODEC_H263.c_str());

#ifdef ENABLE_VIDEO
    // default webcam capture size must be initialized to avoid a 0x0 size
	phVideoControlSetWebcamCaptureResolution(320, 240);
#endif

	//If asynchronous mode = 0 then we have to call phPoll()
	owplConfigSetAsynchronous(1);

	_callbacks->startListeningPhApiEvents();

	std::string tmp = _sipServer;
	tmp += ":" + String::fromNumber(_sipServerPort);
	owplConfigSetOutboundProxy(tmp.c_str());

	if(owplInit(5060, 0, 0, NULL, 0) == OWPL_RESULT_SUCCESS) {
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

std::string PhApiWrapper::phapiCallStateToString(enum phCallStateEvent event) {
	std::string toReturn;
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
		LOG_FATAL("unknown phapi state=" + String::fromNumber(event));
	}
	return toReturn;
}

void PhApiWrapper::setVideoDevice(const std::string & deviceName) {
	strncpy(phcfg.video_config.video_device, deviceName.c_str(), sizeof(phcfg.video_config.video_device));
}

void PhApiWrapper::flipVideoImage(bool flip) {
	phVideoControlSetCameraFlip((int)flip);
}
