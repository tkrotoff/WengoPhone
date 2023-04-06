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

#include "SipPrimitives.h"
#include "AudioCallManager.h"
#include "AudioCall.h"
#include "SessionWindow.h"

#include "config/Authentication.h"
#include "config/Video.h"
#include "config/Audio.h"
#include "connect/Connect.h"
#include "connect/Register.h"

#include <phapi/phapi.h>
#include <phapi/phcall.h>

#include <qstring.h>
#include <qstringlist.h>

#include <cassert>
#include <iostream>
using namespace std;

const int FLAG_VIDEO = PH_STREAM_AUDIO | PH_STREAM_VIDEO_RX | PH_STREAM_VIDEO_TX;
const int FLAG_VIDEO_RX_ONLY = PH_STREAM_AUDIO | PH_STREAM_VIDEO_RX;

int SipPrimitives::placeVideoCall(const SipAddress & sipAddress) {
#ifdef ENABLE_VIDEO
	//QString res = Video::getInstance().getCaptureResolution();

	/*if (res == "160x120") {
		phVideoControlSetWebcamCaptureResolution(160, 120);
	} else if (res == "176x144") {
		phVideoControlSetWebcamCaptureResolution(176, 144);
	} else if (res == "320x240") {
		phVideoControlSetWebcamCaptureResolution(320, 240);
	} else if (res == "352x288") {
		phVideoControlSetWebcamCaptureResolution(352, 288);
	} else if (res == "640x480") {
		phVideoControlSetWebcamCaptureResolution(640, 480);
	}*/
	phVideoControlSetWebcamCaptureResolution(320, 240);

	QString cType = Video::getInstance().getConnectionType();
	phConfig_t *cfg = phGetConfig();
	int minrate, maxrate;

	if (cType == "128") {
		cfg->video_config.video_line_configuration = PHAPI_VIDEO_LINE_128KBPS;
	} else if (cType == "256") {
		cfg->video_config.video_line_configuration = PHAPI_VIDEO_LINE_256KBPS;
	} else if (cType == "512") {
		cfg->video_config.video_line_configuration = PHAPI_VIDEO_LINE_512KBPS;
	} else if (cType == "1024") {
		cfg->video_config.video_line_configuration = PHAPI_VIDEO_LINE_1024KBPS;
	} else if (cType == "Automatic") {
//		cfg->video_config.video_line_configuration = PHAPI_VIDEO_LINE_AUTOMATIC;
	}

	//Update sound devices
	Audio::getInstance().updateDevicesId();

	cout << "SipPrimitives::placeCall with video" << endl;
	int ret = phLinePlaceCall2(Connect::getInstance().getPhoneLineId(),
				sipAddress.getSipUri().c_str(),
				NULL, 0, FLAG_VIDEO);
#else
	cout << "SipPrimitives::placeCall without video (video disabled at compile time)" << endl;

	int ret = phLinePlaceCall2(Connect::getInstance().getPhoneLineId(),
				sipAddress.getSipUri().c_str(),
				NULL, 0, PH_STREAM_AUDIO);
#endif
	return ret;
}

int SipPrimitives::placeAudioCall(const SipAddress & sipAddress) {
	//Update sound devices
	Audio::getInstance().updateDevicesId();

	cout << "SipPrimitives::placeCall without video" << endl;
#ifdef ENABLE_VIDEO
	int ret = phLinePlaceCall2(Connect::getInstance().getPhoneLineId(),
				sipAddress.getSipUri().c_str(),
				NULL, 0, FLAG_VIDEO_RX_ONLY);
#else
	int ret = phLinePlaceCall2(Connect::getInstance().getPhoneLineId(),
				sipAddress.getSipUri().c_str(),
				NULL, 0, PH_STREAM_AUDIO);
#endif

	return ret;
}

void SipPrimitives::acceptVideoCall(int callId) {
	//Update sound devices
	Audio::getInstance().updateDevicesId();

#ifdef ENABLE_VIDEO
	if (phAcceptCall3(callId, 0, FLAG_VIDEO) == -1) {
#else
	if (phAcceptCall3(callId, 0, PH_STREAM_AUDIO) == -1) {
#endif
		assert(NULL && "phAcceptCall() returned an incorrect statement");
	}
}

void SipPrimitives::acceptAudioCall(int callId) {
	//Update sound devices
	Audio::getInstance().updateDevicesId();

#ifdef ENABLE_VIDEO
	if (phAcceptCall3(callId, 0, FLAG_VIDEO_RX_ONLY) == -1) {
#else
	if (phAcceptCall3(callId, 0, PH_STREAM_AUDIO) == -1) {
#endif
		//assert(NULL && "phAcceptCall() returned an incorrect statement");
	}
}

void SipPrimitives::rejectCall(int callId, int reason) {
	if (phRejectCall(callId, reason) == -1) {
		//assert(NULL && "phRejectCall() returned an incorrect statement");
	}
}

void SipPrimitives::closeCall(int callId) {
	if (phCloseCall(callId) == -1) {
		//assert(NULL && "phCloseCall() returned an incorrect statement");
	}
}

void SipPrimitives::ringingCall(int callId) {
	if (phRingingCall(callId) == -1) {
		//assert(NULL && "phRingingCall() returned an incorrect statement");
	}
}

void SipPrimitives::holdCall(int callId) {
	if (phHoldCall(callId) == -1) {
		//assert(NULL && "phHoldCall() returned an incorrect statement.");
	}
}

void SipPrimitives::resumeCall(int callId) {
	if (phResumeCall(callId) == -1) {
		//assert(NULL && "phResumeCall() returned an incorrect statement.");
	}
}

void SipPrimitives::playDTMF(int callId, char dtmf) {
	phSendDtmf(callId, dtmf, 1);
}

bool SipPrimitives::sendMessage(const SipAddress & sipAddress, const char * message) {
	int ret = phSendMessage(phGetConfig()->identity,
				sipAddress.getSipUri().c_str(),
				message);
	if (ret == 0) {
		//OK
		return true;
	}
	return false;
}

bool SipPrimitives::askForWatcherList() {
	static const int winfo = 1;	//Subscribe with event = presence.winfo

	Connect & connect = Connect::getInstance();
	if (connect.isConnected()) {
		//Sends a subscribe to myself
		//in order to know the watchers (people watching for my presence).
		SipAddress mySipAddress(phGetConfig()->identity);
		int ret = phSubscribe(mySipAddress.getMinimalSipUri().c_str(),
					mySipAddress.getMinimalSipUri().c_str(),
					winfo);
		if (ret == 0) {
			//OK
			return true;
		}
	}
	return false;
}

bool SipPrimitives::subscribeToPresence(const SipAddress & sipAddress) {
	static const int winfo = 0;	//Subscribe with event = presence

	Connect & connect = Connect::getInstance();
	if (connect.isConnected()) {
		//I subscribe to the presence of somebody
		SipAddress mySipAddress(phGetConfig()->identity);
		int ret = phSubscribe(mySipAddress.getMinimalSipUri().c_str(),
					sipAddress.getSipUri().c_str(),
					winfo);
		if (ret == 0) {
			//OK
			return true;
		}
	}
	return false;
}

bool SipPrimitives::allowWatcher(const SipAddress & sipAddress) {
	static const int winfo = 1;	//Publish with event = presence.winfo
	static const QString contentType = "application/watcherinfo+xml";

	Connect & connect = Connect::getInstance();
	if (connect.isConnected()) {
		QString winfoAllow = "<?xml version=\"1.0\"?>\n";
		winfoAllow += "<watcherinfo>\n";
		winfoAllow += "<watcher-list>\n";
		winfoAllow += "<watcher status=\"active\">";
		winfoAllow += sipAddress.getMinimalSipUri();
		winfoAllow += "</watcher>\n";
		winfoAllow += "</watcher-list>\n";
		winfoAllow += "</watcherinfo>\n";

		SipAddress mySipAddress(phGetConfig()->identity);
		int ret = phPublish(mySipAddress.getMinimalSipUri().c_str(),
					mySipAddress.getMinimalSipUri().c_str(),
					winfo, contentType, winfoAllow);
		if (ret == 0) {
			//OK
			return true;
		}
	}
	return false;
}

bool SipPrimitives::forbidWatcher(const SipAddress & sipAddress) {
	static const int winfo = 1;	//Publish with event = presence.winfo
	static const QString contentType = "application/watcherinfo+xml";

	Connect & connect = Connect::getInstance();
	if (connect.isConnected()) {
		QString winfoForbid = "<?xml version=\"1.0\"?>\n";
		winfoForbid += "<watcherinfo>\n";
		winfoForbid += "<watcher-list>\n";
		winfoForbid += "<watcher status=\"pending\">";
		winfoForbid += sipAddress.getMinimalSipUri();
		winfoForbid += "</watcher>\n";
		winfoForbid += "</watcher-list>\n";
		winfoForbid += "</watcherinfo>\n";

		SipAddress mySipAddress(phGetConfig()->identity);
		int ret = phPublish(mySipAddress.getMinimalSipUri().c_str(),
					mySipAddress.getMinimalSipUri().c_str(),
					winfo, contentType, winfoForbid);
		if (ret == 0) {
			//OK
			return true;
		}
	}
	return false;
}

bool SipPrimitives::publishOnline(const QString & note) {
	static const int winfo = 0;	//Publish with event = presence
	static const QString contentType = "application/pidf+xml";

	Connect & connect = Connect::getInstance();
	if (connect.isConnected()) {
		QString pidfOnline = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
		pidfOnline += "<presence entity=\"";
		SipAddress mySipAddress(phGetConfig()->identity);
		pidfOnline += mySipAddress.getMinimalSipUri();
		pidfOnline += "\">\n";
		pidfOnline += "<tuple id=\"azersdqre\">\n";
		pidfOnline += "<status><basic>open</basic>";
		pidfOnline += "<note>";
		pidfOnline += note;
		pidfOnline += "</note></status>\n";
		pidfOnline += "<contact priority=\"1\">";
		pidfOnline += mySipAddress.getMinimalSipUri();
		pidfOnline += "</contact>\n";
		pidfOnline += "</tuple>\n";
		pidfOnline += "</presence>\n";

		cerr << "SipPrimitives::publishOnline" << endl;

		int ret = phPublish(mySipAddress.getMinimalSipUri().c_str(),
					mySipAddress.getMinimalSipUri().c_str(),
					winfo, contentType, pidfOnline);
		if (ret == 0) {
			//OK
			return true;
		}
	}
	return false;
}

bool SipPrimitives::publishOffline() {
	static const int winfo = 0;	//Publish with event = presence
	static const QString contentType = "application/pidf+xml";

	Connect & connect = Connect::getInstance();
	if (connect.isConnected()) {
		QString pidfOffline = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
		pidfOffline += "<presence entity=\"";
		SipAddress mySipAddress(phGetConfig()->identity);
		pidfOffline += mySipAddress.getMinimalSipUri();
		pidfOffline += "\">\n";
		pidfOffline += "<tuple id=\"azersdqre\">\n";
		pidfOffline += "<status><basic>close</basic></status>\n";
		pidfOffline += "<contact priority=\"1\">";
		pidfOffline += mySipAddress.getMinimalSipUri();
		pidfOffline += "</contact>\n";
		pidfOffline += "</tuple>\n";
		pidfOffline += "</presence>\n";

		cerr << "SipPrimitives::publishOffline" << endl;

		int ret = phPublish(mySipAddress.getMinimalSipUri().c_str(),
					mySipAddress.getMinimalSipUri().c_str(),
					winfo, contentType, pidfOffline);
		if (ret == 0) {
			//OK
			return true;
		}
	}
	return false;
}
