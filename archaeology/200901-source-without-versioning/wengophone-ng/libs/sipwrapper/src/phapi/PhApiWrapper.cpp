/*
 * WengoPhone, a voice over Internet phone
 * Copyright (C) 2004-2007  Wengo
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

#include "PhApiChatWrapper.h"
#include "PhApiContactPresenceWrapper.h"
#include "PhApiCodecList.h"
#include "PhApiSFPWrapper.h"

#include <networkdiscovery/NetworkDiscovery.h>

#include <cutil/global.h>

#include <util/File.h>
#include <util/Logger.h>
#include <util/SafeConnect.h>
#include <util/String.h>
#include <util/StringList.h>

#include <string>

#include <cstdio>

PhApiWrapper * PhApiWrapper::PhApiWrapperHack = NULL;

#ifdef ENABLE_VIDEO
static const int VIDEO_FLAGS = OWPL_STREAM_AUDIO | OWPL_STREAM_VIDEO_RX | OWPL_STREAM_VIDEO_TX;
static const int VIDEO_RX_ONLY_FLAGS = OWPL_STREAM_AUDIO | OWPL_STREAM_VIDEO_RX;
#else
static const int VIDEO_FLAGS = OWPL_STREAM_AUDIO;
static const int VIDEO_RX_ONLY_FLAGS = OWPL_STREAM_AUDIO;
#endif

PhApiWrapper * PhApiWrapper::getInstance() {
	static PhApiWrapper instance(PhApiCallbacks::getInstance());
	return &instance;
}

PhApiWrapper::PhApiWrapper(PhApiCallbacks & callbacks)
	: _phApiSFPWrapper(PhApiSFPWrapper::getInstance()) {

	_callbacks = &callbacks;
	_isInitialized = false;
	_tunnelNeeded = false;
	_tunnelPort = 0;
	_tunnelSSL = false;
	_natType = EnumNatType::NatTypeUnknown;
	PhApiWrapperHack = this;
	_registered = false;

	_sipChatWrapper = new PhApiChatWrapper();
	_sipContactPresenceWrapper = new PhApiContactPresenceWrapper();

	//FIXME ugly hack for conference
	SAFE_CONNECT(this,
		SIGNAL(phoneCallStateChangedSignal(int, EnumPhoneCallState::PhoneCallState, std::string)),
		SLOT(phoneCallStateChangedSlot(int, EnumPhoneCallState::PhoneCallState, std::string)));
}

PhApiWrapper::~PhApiWrapper() {
	terminate();
	OWSAFE_DELETE(_sipContactPresenceWrapper);
	OWSAFE_DELETE(_sipChatWrapper);
}

void PhApiWrapper::terminate() {
	if (_isInitialized) {
		LOG_DEBUG("terminating phapi");
		owplShutdown();
		_isInitialized = false;
	}
}

void PhApiWrapper::setNetworkParameter() {
	//int natRefreshTime = 25;

	if (_tunnelNeeded) {
		if (owplConfigSetLocalHttpProxy(_proxyServer.c_str(), _proxyPort,
			_proxyLogin.c_str(), _proxyPassword.c_str()) != OWPL_RESULT_SUCCESS) {
			// TODO what? throw an exception? exit?
		}

		// TODO: Add Tunnel through SSL support
		owplConfigSetHttpTunnel(_tunnelServer.c_str(), _tunnelPort, 5);
		owplConfigEnableHttpTunnel(1, _tunnelSSL);

	} else {
		// in case of change after first initialization
		owplConfigEnableHttpTunnel(0, 0);
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
	OWPL_LINE hLine = -1;

	if (_isInitialized) {
		if (owplLineAdd(displayName.c_str(), identity.c_str(), registerServer.c_str(),
			proxyServer.c_str(), OWPL_TRANSPORT_UDP, REGISTER_TIMEOUT, &hLine) != OWPL_RESULT_SUCCESS) {
			return SipWrapper::VirtualLineIdError;
		}

		if (owplLineAddCredential(hLine, identity.c_str(), password.c_str(), realm.c_str()) != OWPL_RESULT_SUCCESS) {
			return -1; // TODO ?
		}

		if (owplLineSetAutoKeepAlive(hLine, 1, 30)) {
			return SipWrapper::VirtualLineIdError;
		}

	}

	return hLine;
}

bool PhApiWrapper::registerVirtualLine(int lineId) {
	phoneLineStateChangedSignal(lineId, EnumPhoneLineState::PhoneLineStateProgress);
	int ret = owplLineRegister(lineId, 1);
	if (ret == 0) {
		//Success
		return true;
	}
	return false;
}

void PhApiWrapper::removeVirtualLine(int lineId, bool now) {
	if (_isInitialized) {
		publishPresence(lineId, false, String::null);
		owplLineDelete(lineId, now);
		if (now) {
			setRegistered(false);
			phoneLineStateChangedSignal(lineId, EnumPhoneLineState::PhoneLineStateClosed);
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

	//sendMyIcon(sipAddress, _iconFilename);

	if (owplCallCreate(lineId, &hCall) != OWPL_RESULT_SUCCESS) {
		return -1;
	}
	if (owplCallConnect(hCall, sipAddress.c_str(), mediaFlags) != OWPL_RESULT_SUCCESS) {
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

void PhApiWrapper::playDtmf(int callId, char dtmf) {
	phSendDtmf(callId, dtmf, 3);
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
		LOG_WARN("unknown codec=" + tmp);
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
		LOG_WARN("unknown codec=" + tmp);
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
#if defined(OWSOUND_PORTAUDIO_SUPPORT)
	static const std::string INPUT_DEVICE_TAG = "pa:IN=";
	devices = INPUT_DEVICE_TAG
		+ _inputAudioDevice.getData()[1]
		+ std::string(" ") + OUTPUT_DEVICE_TAG
		+ _outputAudioDevice.getData()[1];
#else
	static const std::string INPUT_DEVICE_TAG = "alsa:IN=";
	devices = INPUT_DEVICE_TAG
		+ _inputAudioDevice.getData()[1]
		+ std::string(" ") + OUTPUT_DEVICE_TAG
		+ _outputAudioDevice.getData()[1];
#endif
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

void PhApiWrapper::enablePIM(bool enable) {
	owplConfigEnablePIM(enable);
}

/*void PhApiWrapper::changeTypingState(IMChatSession & chatSession, IMChat::TypingState state) {
	const IMContactSet & buddies = chatSession.getIMContactSet();
	IMContactSet::const_iterator it;
	int messageId = -1;

	for (it = buddies.begin(); it != buddies.end(); it++) {
		std::string sipAddress = makeSipAddress((*it).getContactId());

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
*/

/*void PhApiWrapper::sendMyIcon(const std::string & contactId, const std::string & iconFilename) {
	int messageId = -1;

	if (iconFilename.empty()) {
		return;
	}

	std::string sipAddress = makeSipAddress(contactId);
	owplMessageSendIcon(_wengoVline,
		sipAddress.c_str(),
		iconFilename.c_str(),
		&messageId);
}
*/

void PhApiWrapper::publishPresence(int lineId, bool online, const std::string & note) {
	if (_isInitialized) {
		owplPresencePublish(lineId, (online) ? 1 : 0, note.c_str(), NULL);
	}
}

/*
void PhApiWrapper::subscribeToPresenceOf(const std::string & contactId) {
	OWPL_SUB hSub;
	std::string sipAddress = makeSipAddress(contactId);

	if(owplPresenceSubscribe(_wengoVline, sipAddress.c_str(), 0, &hSub) != OWPL_RESULT_SUCCESS) {
		subscribeStatusEvent(*this, sipAddress, IMPresence::SubscribeStatusError);
	} else {
		subscribeStatusEvent(*this, sipAddress, IMPresence::SubscribeStatusOk);
	}
}

void PhApiWrapper::unsubscribeToPresenceOf(const std::string & contactId) {
	std::string sipAddress = makeSipAddress(contactId);

	owplPresenceUnsubscribe(sipAddress.c_str());
}

void PhApiWrapper::allowWatcher(const std::string & watcher) {*/
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
/*}

void PhApiWrapper::forbidWatcher(const std::string & watcher) {*/
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
/*}

void PhApiWrapper::blockContact(const std::string & contactId) {
	std::string sipAddress = makeSipAddress(contactId);
	allowWatcher(sipAddress);
}

void PhApiWrapper::unblockContact(const std::string & contactId) {
	std::string sipAddress = makeSipAddress(contactId);
	allowWatcher(sipAddress);
}
*/
void PhApiWrapper::setHTTPProxy(const std::string & address, unsigned port,
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

void PhApiWrapper::phoneCallStateChangedSlot(int callId,
	EnumPhoneCallState::PhoneCallState state, std::string from) {

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

	LOG_DEBUG("join conference call callId=" + String::fromNumber(callId));

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

void PhApiWrapper::setAudioCodecList(const StringList & audioCodecList) {
	owplConfigSetAudioCodecs(audioCodecList.toString(",").c_str());
}

void PhApiWrapper::init() {
	setNetworkParameter();

	//Plugin path
	strncpy(phcfg.plugin_path, _pluginPath.c_str(), sizeof(phcfg.plugin_path));

	/* Codec list *
	owplConfigAddAudioCodecByName(PhApiCodecList::AUDIO_CODEC_SPEEXWB.c_str());
	owplConfigAddAudioCodecByName(PhApiCodecList::AUDIO_CODEC_ILBC.c_str());
	owplConfigAddAudioCodecByName(PhApiCodecList::AUDIO_CODEC_AMRWB.c_str());
	owplConfigAddAudioCodecByName(PhApiCodecList::AUDIO_CODEC_PCMU.c_str());
	owplConfigAddAudioCodecByName(PhApiCodecList::AUDIO_CODEC_PCMA.c_str());
	owplConfigAddAudioCodecByName(PhApiCodecList::AUDIO_CODEC_AMRNB.c_str());
	owplConfigAddAudioCodecByName(PhApiCodecList::AUDIO_CODEC_GSM.c_str());
	owplConfigAddVideoCodecByName(PhApiCodecList::VIDEO_CODEC_H263.c_str());
	*/

#ifdef ENABLE_VIDEO
    // default webcam capture size must be initialized to avoid a 0x0 size
	phVideoControlSetWebcamCaptureResolution(320, 240);
#endif

	PhApiSFPWrapper::getInstance().initialize();

	_callbacks->startListeningPhApiEvents();

	if (owplInit(1, 0, -1, -1, NULL, 0) == OWPL_RESULT_SUCCESS) {
		_isInitialized = true;
		LOG_DEBUG("phApi successfully initialized");
	} else {
		_isInitialized = false;
		LOG_DEBUG("cannot initialize phApi");
	}
}

void PhApiWrapper::setPluginPath(const std::string & path) {
	_pluginPath = path;
}

std::string PhApiWrapper::getPluginPath() {
	return _pluginPath;
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

bool PhApiWrapper::isRegistered() const {
	return _registered;
}

void PhApiWrapper::setRegistered(bool registered) {
	_registered = registered;
}

void PhApiWrapper::setCallsEncryption(bool enable) {
	LOG_INFO("set call encryption to: " + String::fromBoolean(enable));
	owplConfigSetEncryptionMode((int)enable);
}

bool PhApiWrapper::isCallEncrypted(int callId) {
	int result;
	owplCallGetEncryptionMode(callId, &result);
	LOG_INFO("call with callId " + String::fromNumber(callId)
		+ " has encryption mode " + String::fromNumber(result));

	return (bool)result;
}
