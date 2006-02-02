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

#include "PhApiCallbacks.h"

#include <model/sipwrapper/SipCallbacks.h>
#include <model/sipwrapper/SipWrapper.h>
#include <model/WengoPhoneLogger.h>
#include <model/imwrapper/IMPresence.h>
#include <model/imwrapper/EnumPresenceState.h>

#include <model/sipwrapper/phapi/PhApiWrapper.h>

#ifdef ENABLE_VIDEO
	#include <pixertool.h>
#endif

#include <tinyxml.h>

#include <iostream>
using namespace std;

#ifdef ENABLE_VIDEO
class PhApiVideoFrame : public VideoFrame {
public:
	PhApiVideoFrame(piximage * image)
		: VideoFrame() {
		setWidth(image->width);
		setHeight(image->height);
		setFrame(image->data);
	}

	virtual ~PhApiVideoFrame() {
	}
};

class PhApiWebcam : public LocalWebcam {
public:
	PhApiWebcam(piximage * image)
		: LocalWebcam() {
		setWidth(image->width);
		setHeight(image->height);
		setFrame(image->data);
	}

	~PhApiWebcam() {
	}
};
#endif

PhApiCallbacks::PhApiCallbacks(SipCallbacks & callbacks)
	: _callbacks(callbacks) {
}

void PhApiCallbacks::callProgress(int callId, const phCallStateInfo_t * info) {
	int status = info->event;
	std::string from;

	switch (status) {
	case phDIALING:
		from = info->u.remoteUri;
		_callbacks.callProgress(callId, SipCallbacks::CALL_DIALING, from);
		break;

	case phRINGING:
		from = info->u.remoteUri;
		_callbacks.callProgress(callId, SipCallbacks::CALL_RINGING, from);
		break;

	case phNOANSWER:
		break;

	case phCALLBUSY:
		break;

	case phCALLREDIRECTED:
		break;

	case phCALLOK:
		from = info->u.remoteUri;
		_callbacks.callProgress(callId, SipCallbacks::CALL_TALKING, from);
		break;

	case phCALLHELD:
		break;

	case phCALLRESUMED:
		break;

	case phHOLDOK:
		from = info->u.remoteUri;
		_callbacks.callProgress(callId, SipCallbacks::CALL_HOLD_OK, from);
		break;

	case phRESUMEOK:
		break;

	case phINCALL:
		from = info->u.remoteUri;
		_callbacks.callProgress(callId, SipCallbacks::CALL_INCOMING, from);
		break;

	case phCALLCLOSED:
		_callbacks.callProgress(callId, SipCallbacks::CALL_CLOSED, from);
		break;

	case phCALLERROR:
		_callbacks.callProgress(callId, SipCallbacks::CALL_ERROR, from);
		break;

	case phDTMF:
		break;

	case phXFERPROGRESS:
		break;

	case phXFEROK:
		break;

	case phXFERFAIL:
		break;

	case phXFERREQ:
		break;

	case phCALLREPLACED:
		break;

	case phRINGandSTART:
		break;

	case phRINGandSTOP:
		break;

	case phCALLCLOSEDandSTOPRING:
		break;

	default:
		LOG_FATAL("unknown phApi event=" + String::fromNumber(status));
	}
}

void PhApiCallbacks::videoFrameReceived(int callId, phVideoFrameReceivedEvent_t * info) {
#ifdef ENABLE_VIDEO
	PhApiVideoFrame videoFrame(info->frame_remote);
	PhApiWebcam phApiWebcam(info->frame_local);

	_callbacks.videoFrameReceived(callId, videoFrame, phApiWebcam);
#endif
}

void PhApiCallbacks::transferProgress(int /*callId*/, const phTransferStateInfo_t * /*info*/) { }

void PhApiCallbacks::conferenceProgress(int /*conferenceId*/, const phConfStateInfo_t * /*info*/) { }

void PhApiCallbacks::registerProgress(int lineId, int status) {
	LOG_DEBUG("registerProgress status=" + status);

	switch(status) {
	case 401:
		_callbacks.registerProgress(lineId, SipCallbacks::LINE_PROXY_ERROR);
		break;

	case 407:
		_callbacks.registerProgress(lineId, SipCallbacks::LINE_SERVER_ERROR);
		break;

	case -1:
		_callbacks.registerProgress(lineId, SipCallbacks::LINE_TIMEOUT);
		break;

	case -2:
		_callbacks.registerProgress(lineId, SipCallbacks::LINE_DEFAULT_STATE);
		break;

	case 0:
		_callbacks.registerProgress(lineId, SipCallbacks::LINE_OK);
		break;

	case 32768:
		break;

	case 500:
		break;

	default:
		LOG_FATAL("unknown phApi event=" + String::fromNumber(status));
	}
}

#include <stdio.h>

void PhApiCallbacks::messageProgress(int messageId, const phMsgStateInfo_t * info) {
	PhApiWrapper * p = PhApiWrapper::PhApiWrapperHack;
	std::string from;
	std::string content;
	IMChatSession * imChatSession;

	std::map<int, IMChatSession *> messageIdChatSessionMap = p->getMessageIdChatSessionMap();
	std::map<int, IMChatSession *>::const_iterator it = messageIdChatSessionMap.find(messageId);
	if (it == messageIdChatSessionMap.end()) {
		imChatSession = NULL;
	} else {
		imChatSession = messageIdChatSessionMap[messageId];
	}
	
	switch(info->event) {
	case phMsgNew:
		from = info->from;
		content = info->content;
		p->messageReceivedEvent(*p, imChatSession, from, content);
		break;

	case phMsgOk:
		p->statusMessageEvent(*p, *imChatSession, IMChat::StatusMessageReceived, "");
		break;

	case phMsgError:
		p->statusMessageEvent(*p, *imChatSession, IMChat::StatusMessageError, "");
		break;

	default:
		LOG_FATAL("Unknown message event");
		break;
	}
}

void PhApiCallbacks::subscriptionProgress(int subscriptionId, const phSubscriptionStateInfo_t * info) {
	PhApiWrapper * p = PhApiWrapper::PhApiWrapperHack;
	std::string from(info->from);

	if (info->event == phSubscriptionOk) {
		p->subscribeStatusEvent(*p, from, IMPresence::SubscribeStatusOk);
	} else {
		p->subscribeStatusEvent(*p, from, IMPresence::SubscribeStatusError);
	}
}

void PhApiCallbacks::onNotify(const char * event, const char * from, const char * content) {
	std::string tmp(event);
	PhApiWrapper * p = PhApiWrapper::PhApiWrapperHack;
	std::string buddy(from);

	TiXmlDocument doc;
	doc.Parse(content);

	TiXmlHandle docHandle(&doc);

	//A buddy presence
	if (tmp == "presence") {

		TiXmlText * basicText = docHandle.FirstChild("presence").FirstChild("tuple").FirstChild("status").FirstChild("basic").FirstChild().Text();
		if (basicText) {
			std::string basic = basicText->Value();

			//buddy is offline
			if (basic == "closed") {
				p->presenceStateChangedEvent(*p,  EnumPresenceState::PresenceStateOffline, "", buddy);

			//buddy is online
			} else if (basic == "open") {
				TiXmlText * noteText = docHandle.FirstChild("presence").FirstChild("tuple").FirstChild("status").FirstChild("note").FirstChild().Text();
				if (noteText) {
					std::string note = noteText->Value();
					if (note == PhApiWrapper::PresenceStateOnline) {
						p->presenceStateChangedEvent(*p, EnumPresenceState::PresenceStateOnline, note, buddy);
					} else if (note == PhApiWrapper::PresenceStateAway) {
						p->presenceStateChangedEvent(*p, EnumPresenceState::PresenceStateAway, note, buddy);
					} else if (note == PhApiWrapper::PresenceStateDoNotDisturb) {
						p->presenceStateChangedEvent(*p, EnumPresenceState::PresenceStateDoNotDisturb, note, buddy);
					} else {
						p->presenceStateChangedEvent(*p, EnumPresenceState::PresenceStateUserDefined, note, buddy);
					}
				}
			}
		}

	//watcher list
	} else if (tmp == "presence.winfo") {

		TiXmlElement * watcherinfoElement = doc.FirstChildElement("watcherinfo");
		if (watcherinfoElement) {

			TiXmlElement * watcherElement = watcherinfoElement->FirstChildElement("watcher");
			while (watcherElement) {

				std::string watcher(watcherElement->Value());
				p->allowWatcher(watcher);

				watcherElement = watcherinfoElement->NextSiblingElement("watcher");
			}
		}

	//unknown message event
	} else {
		LOG_FATAL("Unknown message event");
	}
}
