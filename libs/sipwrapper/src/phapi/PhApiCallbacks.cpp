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

#include "PhApiWrapper.h"
#include "PhApiIMChat.h"

#include <sipwrapper/SipWrapper.h>
#include <imwrapper/IMPresence.h>
#include <imwrapper/EnumPresenceState.h>

#include <Logger.h>

#ifdef ENABLE_VIDEO
	#include <pixertool.h>
#endif

#include <tinyxml.h>

#include <iostream>
using namespace std;

#ifdef ENABLE_VIDEO
class PhApiVideoFrame : public WebcamVideoFrame {
public:

	PhApiVideoFrame(piximage * image)
		: WebcamVideoFrame() {
		setWidth(image->width);
		setHeight(image->height);
		setFrame(image->data);
	}

	virtual ~PhApiVideoFrame() {
	}
};

class PhApiWebcam : public WebcamVideoFrame {
public:

	PhApiWebcam(piximage * image)
		: WebcamVideoFrame() {
		setWidth(image->width);
		setHeight(image->height);
		setFrame(image->data);
	}

	~PhApiWebcam() {
	}
};
#endif

PhApiCallbacks::PhApiCallbacks() {
}

void PhApiCallbacks::callProgress(int callId, const phCallStateInfo_t * info) {
	int status = info->event;
	std::string from;

	PhApiWrapper * p = PhApiWrapper::PhApiWrapperHack;

	switch (status) {
	case phDIALING:
		from = info->u.remoteUri;
		p->phoneCallStateChangedEvent(*p, callId, EnumPhoneCallState::PhoneCallStateDialing, from);
		break;

	case phRINGING:
		from = info->u.remoteUri;
		p->phoneCallStateChangedEvent(*p, callId, EnumPhoneCallState::PhoneCallStateRinging, from);
		break;

	case phNOANSWER:
		break;

	case phCALLBUSY:
		break;

	case phCALLREDIRECTED:
		break;

	case phCALLOK:
		from = info->u.remoteUri;
		p->phoneCallStateChangedEvent(*p, callId, EnumPhoneCallState::PhoneCallStateTalking, from);
		break;

	case phCALLHELD:
		break;

	case phCALLRESUMED:
		break;

	case phHOLDOK:
		from = info->u.remoteUri;
		p->phoneCallStateChangedEvent(*p, callId, EnumPhoneCallState::PhoneCallStateHoldOk, from);
		break;

	case phRESUMEOK:
		break;

	case phINCALL:
		from = info->u.remoteUri;
		p->phoneCallStateChangedEvent(*p, callId, EnumPhoneCallState::PhoneCallStateIncoming, from);
		break;

	case phCALLCLOSED:
		p->phoneCallStateChangedEvent(*p, callId, EnumPhoneCallState::PhoneCallStateClosed, from);
		break;

	case phCALLERROR:
		p->phoneCallStateChangedEvent(*p, callId, EnumPhoneCallState::PhoneCallStateError, from);
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
	PhApiWrapper * p = PhApiWrapper::PhApiWrapperHack;
#ifdef ENABLE_VIDEO
	PhApiVideoFrame videoFrame(info->frame_remote);
	PhApiWebcam phApiWebcam(info->frame_local);

	p->videoFrameReceivedEvent(*p, callId, videoFrame, phApiWebcam);
#endif
}

void PhApiCallbacks::transferProgress(int /*callId*/, const phTransferStateInfo_t * /*info*/) { }

void PhApiCallbacks::conferenceProgress(int /*conferenceId*/, const phConfStateInfo_t * /*info*/) { }

void PhApiCallbacks::registerProgress(int lineId, int status) {
	LOG_DEBUG("registerProgress status=" + String::fromNumber(status));

	PhApiWrapper * p = PhApiWrapper::PhApiWrapperHack;

	switch(status) {

	//401 Unauthorized
	case 401:
		p->phoneLineStateChangedEvent(*p, lineId, EnumPhoneLineState::PhoneLineStateServerError);
		break;

	//404 Not Found
	case 404:
		p->phoneLineStateChangedEvent(*p, lineId, EnumPhoneLineState::PhoneLineStateServerError);
		break;

	//407 Proxy Authentication Required
	case 407:
		p->phoneLineStateChangedEvent(*p, lineId, EnumPhoneLineState::PhoneLineStateServerError);
		break;

	//408 Request Timeout
	case 408:
		p->phoneLineStateChangedEvent(*p, lineId, EnumPhoneLineState::PhoneLineStateTimeout);
		break;

	case -1:
		p->phoneLineStateChangedEvent(*p, lineId, EnumPhoneLineState::PhoneLineStateTimeout);
		break;

	case -2:
		p->phoneLineStateChangedEvent(*p, lineId, EnumPhoneLineState::PhoneLineStateDefault);
		break;

	//Register ok
	case 0:
		p->phoneLineStateChangedEvent(*p, lineId, EnumPhoneLineState::PhoneLineStateOk);
		break;

	//Unregister ok
	case 32768:
		p->phoneLineStateChangedEvent(*p, lineId, EnumPhoneLineState::PhoneLineStateClosed);
		break;

	//500 Server Internal Error
	case 500:
		p->phoneLineStateChangedEvent(*p, lineId, EnumPhoneLineState::PhoneLineStateServerError);
		break;

	default:
		LOG_ERROR("unknown phApi event=" + String::fromNumber(status));
		p->phoneLineStateChangedEvent(*p, lineId, EnumPhoneLineState::PhoneLineStateServerError);
	}
}

#include <stdio.h>

void PhApiCallbacks::messageProgress(int messageId, const phMsgStateInfo_t * info) {
	PhApiWrapper * p = PhApiWrapper::PhApiWrapperHack;
	IMChatSession * imChatSession;

	std::map<int, IMChatSession *> messageIdChatSessionMap = p->getMessageIdChatSessionMap();
	std::map<int, IMChatSession *>::const_iterator it = messageIdChatSessionMap.find(messageId);
	if (it == messageIdChatSessionMap.end()) {
		//imChatSession = new IMChatSession(PhApiIMChat::PhApiIMChatHack);
		//p->newIMChatSessionCreatedEvent(*p, *imChatSession);
	} else {
		imChatSession = messageIdChatSessionMap[messageId];
	}

	switch(info->event) {
	case phMsgNew: {
		std::string from = info->from;
		std::string content = info->content;
		p->contactAddedEvent(*p, *imChatSession, from);
		p->messageReceivedEvent(*p, *imChatSession, from, content);
		break;
	}

	case phMsgOk:
		p->statusMessageReceivedEvent(*p, *imChatSession, PhApiIMChat::StatusMessageReceived, String::null);
		break;

	case phMsgError:
		p->statusMessageReceivedEvent(*p, *imChatSession, PhApiIMChat::StatusMessageError, String::null);
		break;

	default:
		LOG_FATAL("unknown message event");
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
				p->presenceStateChangedEvent(*p,  EnumPresenceState::PresenceStateOffline, String::null, buddy);
			}

			//buddy is online
			else if (basic == "open") {
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
