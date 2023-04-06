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

#include "PhApiCallbacks.h"

#include "AudioCallManager.h"
#include "Softphone.h"
#include "StatusBar.h"
#include "connect/Register.h"
#include "presence/Presence.h"
#include "sip/SipAddress.h"
#include "MainWindow.h"

#include <josua/jmainconfig.h>

#include <qmessagebox.h>
#include <qstring.h>

#include <cassert>

#include <iostream>


using namespace std;

extern "C" {

static void callProgress(int callId, const phCallStateInfo_t * info) {
	PhApiCallbacks::callProgress(callId, info);
}

static void onFrameReady(int cid, phVideoFrameReceivedEvent_t *ev) {
	PhApiCallbacks::onFrameReady(cid, ev);
}

static void transferProgress(int, const phTransferStateInfo_t *) { 
}

static void conferenceProgress(int, const phConfStateInfo_t *) { 
}

static void registerProgress(int registerId, int status) {
	Register::registerProgress(registerId, status);
}

static void messageProgress(int messageId, const phMsgStateInfo_t * info) {
	PhApiCallbacks::messageProgress(messageId, info);
}

static void subscriptionProgress(int subscriptionId, const phSubscriptionStateInfo_t *info) {
	PhApiCallbacks::subscriptionProgress(subscriptionId, info);
}

static void onNotify(const char * event, const char * from, const char * content) {
	PhApiCallbacks::onNotify(event, from, content);
}

static void errorNotify(enum phErrors error) {
	PhApiCallbacks::errorNotify(error);
}

phCallbacks_t callbacks = {
	callProgress,
	transferProgress,
	conferenceProgress,
	registerProgress,
	messageProgress,
	onNotify,
	subscriptionProgress,
	onFrameReady,
	errorNotify,
};

}	//"C"

AudioCallManager * PhApiCallbacks::_audioCallManager = NULL;

Presence * PhApiCallbacks::_presence = NULL;

int PhApiCallbacks::initPhApi() {
	//Ignored since we are in direct link mode
	static char * phApiServer = "127.0.0.1";

	//Asynchronous mode = false
	//We have to call phPoll()
	static const int asynchronousMode = 0;

	return phInit(&callbacks, phApiServer, asynchronousMode);
}

void PhApiCallbacks::setAudioCallManager(AudioCallManager & audioCallManager) {
	_audioCallManager = &audioCallManager;
}

void PhApiCallbacks::callProgress(int callId, const phCallStateInfo_t * info) {
	assert(_audioCallManager && "AudioCallManager is NULL, call setAudioCallManager() first");
	_audioCallManager->callProgress(callId, info);
}

void PhApiCallbacks::transferProgress(int /*callId*/, const phCallStateInfo_t * /*info*/) {
	assert(_audioCallManager && "AudioCallManager is NULL, call setAudioCallManager() first");
	//_audioCall->transferProgress(callId, info);
}

void PhApiCallbacks::conferenceProgress(int /*conferenceId*/, const phCallStateInfo_t * /*info*/) {
	assert(_audioCallManager && "AudioCallManager is NULL, call setAudioCallManager() first");
	//_audioCall->conferenceProgress(conferenceId, info);
}

void PhApiCallbacks::messageProgress(int messageId,  const phMsgStateInfo_t * info) {
	assert(_audioCallManager && "AudioCallManager is NULL, call setAudioCallManager() first");
	_audioCallManager->messageProgress(messageId, info);
}

void PhApiCallbacks::onNotify(const char * event, const char * from, const char * content) {
	Presence::onNotify(event, SipAddress(from), content);
}

void PhApiCallbacks::subscriptionProgress(int subscriptionId, const phSubscriptionStateInfo_t * info) {
	bool success = false;

	//SUBSCRIBE request succeeded
	if (info->event == phSubscriptionOk) {
		cerr << "SUBSCRIBE request sent to " << info->to << " succeeded" << endl;
		success = true;
	}

	//SUBSCRIBE request sent to an unexisted user
	else if (info->event == phSubscriptionErrNotFound) {
		cerr << "SUBSCRIBE request failure: " << info->to << " doesn't exist" << endl;
		success = false;
	}

	//Failure with unknown error
	else {
		cerr << "SUBSCRIBE request sent to " << info->to << " failed" << endl;
		success = false;
	}	

	Presence::subscriptionProgress(subscriptionId, SipAddress(info->to), success);
}

void PhApiCallbacks::onFrameReady(int cid, phVideoFrameReceivedEvent_t *ev) {
	_audioCallManager->onFrameReady(cid, ev);
}

void PhApiCallbacks::errorNotify(enum phErrors error) {
	switch(error) {
		case PH_NOMEDIA:
			QMessageBox::critical(Softphone::getInstance().getActiveWindow(),
				QObject::tr("Couldn't open audio"),
				QObject::tr("\tPlease check that:\n" 
					"Your soundcard is configured properly.\n"
					"No other program is blocking the soundcard."));
			break;
		default: break;
	}
}
