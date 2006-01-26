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

//FIXME include to remove
#include <model/WengoPhone.h>

#include <model/account/SipAccount.h>

#ifdef ENABLE_NETLIB
#include <netlib.h>
#endif

#include <Thread.h>
#include <StringList.h>
#include <Logger.h>
#include <File.h>

#include <string>
using namespace std;

const std::string PhApiWrapper::PresenceStateOnline = "Online";
const std::string PhApiWrapper::PresenceStateAway = "Away";
const std::string PhApiWrapper::PresenceStateDoNotDisturb = "Do Not Disturb";


PhApiWrapper * PhApiWrapper::PhApiWrapperHack = NULL;
PhApiCallbacks * PhApiWrapper::_callbacks = NULL;

static const int MEDIA_FLAGS = PH_STREAM_AUDIO | PH_STREAM_VIDEO_RX | PH_STREAM_VIDEO_TX;

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

	//NAT type + HTTP tunnel
	detectNetworkConfig();

	//Ignored since we are in direct link mode
	static const std::string phApiServer = "127.0.0.1";

	//If asynchronous mode = false then we have to call phPoll()
	static const bool asynchronousMode = true;

	//FIXME ugly hack for xpcom component
	std::string pluginPath = File::convertPathSeparators(WengoPhone::getConfigFilesPath() + "../extensions/{debaffee-a972-4d8a-b426-8029170f2a89}/libraries/");
	strncpy(phcfg.plugin_path, pluginPath.c_str(), 256);

	//Codec list
	strncpy(phcfg.audio_codecs, "PCMU,PCMA,GSM,ILBC,SPEEX,SPEEX,AMR,AMR-WB", 128);
	strncpy(phcfg.video_codecs, "H263,H264,MPEG4", 128);

	int ret = phInit(&phApiCallbacks, (char *) phApiServer.c_str(), asynchronousMode);
	if (ret == PhApiResultNoError) {
		_isInitialized = true;
	} else {
		_isInitialized = false;
		LOG_DEBUG("cannot initialize phApi");
	}

	//Stub
	setCallInputAudioDevice("");

	PhApiWrapperHack = this;
}

PhApiWrapper::~PhApiWrapper() {
}

void PhApiWrapper::terminate() {
	if (_isInitialized) {
		phTerminate();
	}
}

void PhApiWrapper::detectNetworkConfig() {
	const string stunAddress = "stun.wengo.fr";

	//is_udp_port_open only tests if the stun server is accessible by any port
	if (is_udp_port_open(stunAddress.c_str(), SIP_PORT) != NETLIB_TRUE) {
		LOG_DEBUG("phApi initialization with http tunnel");

		//Connection via Http or Https Tunnel maybe through a proxy
		char * localProxyUrl = get_local_http_proxy_address();
		int localProxyPort = get_local_http_proxy_port();
		//TODO: test if proxy needs an authentication
		string proxyLogin = "";
		string proxyPassword = "";

		if (localProxyUrl) {
			LOG_DEBUG("local proxy URL: " + string(localProxyUrl));
			LOG_DEBUG("local proxy port: " + String::fromNumber(localProxyPort));
		}

		int port = 0;
		if (is_tcp_port_open(stunAddress.c_str(), HTTP_PORT) == NETLIB_TRUE) {
			port = HTTP_PORT;
		} else if (is_tcp_port_open(stunAddress.c_str(), HTTPS_PORT) == NETLIB_TRUE) {
			port = HTTPS_PORT;
		} else {
			LOG_DEBUG("cannot open a connection to stun server");
		}

		phTunnelConfig(localProxyUrl, localProxyPort, "", port,
			proxyLogin.c_str(), proxyPassword.c_str(), 0);
	} else {
		//Direct Connection
		LOG_DEBUG("phApi initialization without HTTP tunnel");

		//Configure the NAT type
		//If fcone then it sends a SIP option message in order to traverse NAT
		std::string tmp = "fcone";
		strncpy(phcfg.nattype, tmp.c_str(), 16);

		phcfg.nat_refresh_time = 30;
		phcfg.use_tunnel = 0;
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

	int ret = SipWrapper::VirtualLineIdError;
	if (_isInitialized) {
		phAddAuthInfo(username.c_str(), identity.c_str(), password.c_str(), String::null.c_str(), realm.c_str());
		ret = phAddVline2(displayName.c_str(), identity.c_str(), registerServer.c_str(), proxyServer.c_str(), REGISTER_TIMEOUT);
	}

	String tmp(realm);
	if (tmp.contains("wengo")) {
		_wengoVline = ret;
		_wengoSipAddress = "sip:" + identity + "@" + realm;
		_realm = realm;
	}

	return ret;
}

void PhApiWrapper::removeVirtualLine(int lineId) {
	if (_isInitialized) {
		publishOffline("");
		phDelVline(lineId);
	}
}

int PhApiWrapper::makeCall(int lineId, const std::string & phoneNumber) {
	LOG_DEBUG("call " + phoneNumber);
	return phLinePlaceCall2(lineId, phoneNumber.c_str(), NULL, 0, MEDIA_FLAGS);
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
	//Stub
	static const std::string INPUT_DEVICE_TAG = "pa:IN=";
	static const std::string OUTPUT_DEVICE_TAG  = "OUT=";

	/*std::string tmp = INPUT_DEVICE_TAG
			+ std::string("0")
			+ std::string(" ")
			+ OUTPUT_DEVICE_TAG
			+ std::string("0");*/

	//Takes the default Windows sound device
	std::string tmp = "pa:";

	int ret = phChangeAudioDevices(tmp.c_str());
	if (ret == 0) {
		//Ok
		return true;
	}
	return false;
}

bool PhApiWrapper::setRingerOutputAudioDevice(const std::string & deviceName) {
	//Stub
	return setCallInputAudioDevice("");
}

bool PhApiWrapper::setCallOutputAudioDevice(const std::string & deviceName) {
	//Stub
	return setCallInputAudioDevice("");
}

bool PhApiWrapper::enableAEC(bool enable) {
	return true;
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

int PhApiWrapper::sendMessage(const std::string & contactId, const std::string & message) {
	std::string sipAddress = "sip:" + contactId + "@" + _realm;

	return phLineSendMessage(_wengoVline, sipAddress.c_str(), message.c_str());
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
		myPresenceStatusEvent(*this, EnumPresenceState::MyPresenceStatusError);
		break;

	default:
		LOG_FATAL("unknown presence state");
		break;
	}
}

void PhApiWrapper::publishOnline(const std::string & note) {
	static const std::string contentType = "application/pidf+xml";

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

		int phError = phLinePublish(_wengoVline, _wengoSipAddress.c_str(), 0, contentType.c_str(), pidfOnline.c_str());

		if (phError != 0) {
			myPresenceStatusEvent(*this, EnumPresenceState::MyPresenceStatusError);
		} else {
			myPresenceStatusEvent(*this, EnumPresenceState::MyPresenceStatusOk);
		}
	}
}

void PhApiWrapper::publishOffline(const std::string & note) {
	static const std::string contentType = "application/pidf+xml";

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

		int phError = phLinePublish(_wengoVline, _wengoSipAddress.c_str(), 0, contentType.c_str(), pidfOffline.c_str());
		if (phError != 0) {
			myPresenceStatusEvent(*this, EnumPresenceState::MyPresenceStatusError);
		} else {
			myPresenceStatusEvent(*this, EnumPresenceState::MyPresenceStatusOk);
		}
	}
}

void PhApiWrapper::subscribeToPresenceOf(const std::string & contactId) {
	std::string sipAddress = "sip:" + contactId + "@" + _realm;

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

void PhApiWrapper::blockContact(const std::string & /*contactId*/) {
}

void PhApiWrapper::unblockContact(const std::string & /*contactId*/) {
}
