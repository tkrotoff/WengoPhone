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

#include <sipwrapper/WebcamVideoFrame.h>

#include <sipwrapper/SipWrapper.h>
#include <imwrapper/IMPresence.h>
#include <imwrapper/EnumPresenceState.h>
#include <imwrapper/IMChatSession.h>

#include <util/StringList.h>
#include <util/Logger.h>

#ifdef ENABLE_VIDEO
	#include <pixertool/pixertool.h>
#endif

#include <tinyxml.h>

using namespace std;

PhApiCallbacks::PhApiCallbacks() {
}

void PhApiCallbacks::callProgress(int callId, const phCallStateInfo_t * info) {
	int status = info->event;
	std::string from;

	PhApiWrapper * p = PhApiWrapper::PhApiWrapperHack;
	LOG_DEBUG(p->phapiCallStateToString(info->event));

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
		from = info->u.remoteUri;
		p->phoneCallStateChangedEvent(*p, callId, EnumPhoneCallState::PhoneCallStateMissed, from);
		break;

	case phCALLBUSY:
		break;

	case phCALLREDIRECTED:
		from = info->u.remoteUri;
		p->phoneCallStateChangedEvent(*p, callId, EnumPhoneCallState::PhoneCallStateRedirected, from);
		break;

	case phCALLOK:
		from = info->u.remoteUri;
		p->phoneCallStateChangedEvent(*p, callId, EnumPhoneCallState::PhoneCallStateTalking, from);
		break;

	case phCALLHELD:
		LOG_DEBUG("phCALLHELD");
		break;

	case phCALLRESUMED:
		LOG_DEBUG("phCALLRESUMED");
		break;

	case phHOLDOK:
		from = info->u.remoteUri;
		p->phoneCallStateChangedEvent(*p, callId, EnumPhoneCallState::PhoneCallStateHold, from);
		break;

	case phRESUMEOK:
		from = info->u.remoteUri;
		p->phoneCallStateChangedEvent(*p, callId, EnumPhoneCallState::PhoneCallStateResumed, from);
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

	//Transfer progress
	case phXFERPROGRESS:
		break;

	//Transfer OK
	case phXFEROK:
		break;

	//Transfer failed
	case phXFERFAIL:
		break;

	//Transfer request
	case phXFERREQ:
		break;

	case phCALLREPLACED:
		break;

	// begin ringing
	case phRINGandSTART:
		from = info->u.remoteUri;
		p->phoneCallStateChangedEvent(*p, callId, EnumPhoneCallState::PhoneCallStateRinging, from);
		break;

	// stop ringing
	case phRINGandSTOP:
		break;

	case phCALLCLOSEDandSTOPRING:
		break;

	default:
		LOG_FATAL("unknown phApi event=" + String::fromNumber(status));
	}
}

void PhApiCallbacks::videoFrameReceived(int callId, phVideoFrameReceivedEvent_t * info) {
	//LOG_DEBUG("video frame from call=" + String::fromNumber(callId));
	PhApiWrapper * p = PhApiWrapper::PhApiWrapperHack;
#ifdef ENABLE_VIDEO
	p->videoFrameReceivedEvent(*p, callId, info->frame_remote, info->frame_local);
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
		p->setRegistered(false);
		p->phoneLineStateChangedEvent(*p, lineId, EnumPhoneLineState::PhoneLineStateServerError);
		break;

	//404 Not Found
	case 404:
		p->setRegistered(false);
		p->phoneLineStateChangedEvent(*p, lineId, EnumPhoneLineState::PhoneLineStateServerError);
		break;

	//407 Proxy Authentication Required
	case 407:
		p->setRegistered(false);
		p->phoneLineStateChangedEvent(*p, lineId, EnumPhoneLineState::PhoneLineStateServerError);
		break;

	//408 Request Timeout
	case 408:
		p->setRegistered(false);
		p->phoneLineStateChangedEvent(*p, lineId, EnumPhoneLineState::PhoneLineStateTimeout);
		break;

	case -1:
		if (p->isRegistered()) {
			p->setRegistered(false);
			p->phoneLineStateChangedEvent(*p, lineId, EnumPhoneLineState::PhoneLineStateTimeout);
			//p->removeVirtualLine(p->getActiveVline(), 0);
			for (std::set<std::string>::const_iterator it = _subscribedContacts.begin();
				it != _subscribedContacts.end();
				++it) {
				p->presenceStateChangedEvent(*p, EnumPresenceState::PresenceStateUnknown, "", *it);
			}
			p->disconnectedEvent(*p, true, "No response from server");
		}
		break;

	case -2:
		p->setRegistered(false);
		p->phoneLineStateChangedEvent(*p, lineId, EnumPhoneLineState::PhoneLineStateUnknown);
		break;

	//Register ok
	case 0:
		if (!p->isRegistered()) {
			p->setRegistered(true);
			p->phoneLineStateChangedEvent(*p, lineId, EnumPhoneLineState::PhoneLineStateOk);
			p->connectedEvent(*p);
			for (std::set<std::string>::const_iterator it = _subscribedContacts.begin();
				it != _subscribedContacts.end();
				++it) {
					p->subscribeToPresenceOf(*it);
			}
		}
		break;

	//Unregister ok
	case 32768:
		p->setRegistered(false);
		p->phoneLineStateChangedEvent(*p, lineId, EnumPhoneLineState::PhoneLineStateClosed);
		for (std::set<std::string>::iterator it = _subscribedContacts.begin();
			it != _subscribedContacts.end(); ) {
			std::set<std::string>::iterator tmpIt = it++;
			p->presenceStateChangedEvent(*p, EnumPresenceState::PresenceStateUnknown, "", *tmpIt);
			_subscribedContacts.erase(tmpIt);
		}
		break;

	//500 Server Internal Error
	case 500:
		p->setRegistered(false);
		p->phoneLineStateChangedEvent(*p, lineId, EnumPhoneLineState::PhoneLineStateServerError);
		break;

	default:
		p->setRegistered(false);
		LOG_ERROR("unknown phApi event=" + String::fromNumber(status));
		p->phoneLineStateChangedEvent(*p, lineId, EnumPhoneLineState::PhoneLineStateServerError);
	}
}

void PhApiCallbacks::messageProgress(int messageId, const phMsgStateInfo_t * info) {
	PhApiWrapper * p = PhApiWrapper::PhApiWrapperHack;
	IMChatSession * imChatSession;

	if (info->event == phMsgOk) {
		//We drop status message
		return;
	}

	string content;
	if (info->content) {
		content = info->content;
	}

	LOG_DEBUG("message received from=" + string(info->from) + " content=" + content);

	//Getting maps from PhApiWrapper
	std::map<const std::string, IMChatSession *> & contactChatMap = p->getContactChatMap();

	string from;
	//<sip:robobob1535@voip.wengo.fr>;tag=b6e249633711def8c2dbe3c2d4f39996-e1d6
	if (info->event != phMsgError) {
		from = String(info->from).split(" ")[0];
	} else {
		int atPos = String(info->to).find("@");
		from = String(info->to).substr(5, atPos - 5);
	}
	//Finding associated session
	std::map<const std::string, IMChatSession *>::const_iterator sessionIt = contactChatMap.find(from);

	string ctype;
	if (info->ctype) {
		ctype = info->ctype;
	}

	string subtype;
	if (info->subtype) {
		subtype = info->subtype;
	}

	//Getting buddy icon
	if ((info->event == phMsgNew) && (ctype == "buddyicon")) {
		if (!subtype.empty()) {
			p->contactIconChangedEvent(*p, from, subtype);
		}

		return;
	}

	if (sessionIt != contactChatMap.end()) {
		imChatSession = (*sessionIt).second;
	}
	//Drop typingstate packet if there is no chat session created
	else if (ctype == "typingstate") {
		return;
	} else {
		LOG_DEBUG("creating new IMChatSession");
		imChatSession = new IMChatSession(*PhApiIMChat::PhApiIMChatHack);
		contactChatMap[from] = imChatSession;
		p->addContact(*imChatSession, from);
		p->newIMChatSessionCreatedEvent(*p, *imChatSession);
		p->sendMyIcon(from, p->_iconFilename);
		//p->contactAddedEvent(*p, *imChatSession, from);
	}

	switch(info->event) {
	case phMsgNew: {

		if (ctype == "typingstate") {
			IMChat::TypingState state;

			if (subtype == "typing") {
				state = IMChat::TypingStateTyping;
			} else if (subtype == "stoptyping") {
				state = IMChat::TypingStateStopTyping;
			} else {
				state = IMChat::TypingStateNotTyping;
			}

			p->typingStateChangedEvent(*p, *imChatSession, from, state);
		}
		else {
			p->messageReceivedEvent(*p, *imChatSession, from, content);
		}

		break;
	case phMsgError:
		LOG_DEBUG("message could not be sent");
		p->statusMessageReceivedEvent(*p, *imChatSession, PhApiIMChat::StatusMessageError, content);
		break;
	}

	default:
		LOG_FATAL("unknown message event=" + String::fromNumber(info->event));
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

	std::string buddy = computeContactId(from);
	_subscribedContacts.insert(computeContactId(buddy));

	TiXmlDocument doc;
	doc.Parse(content);

	TiXmlHandle docHandle(&doc);

	//A buddy presence
	if (tmp == "presence") {

		TiXmlText * basicText = docHandle.FirstChild("presence").FirstChild("tuple").FirstChild("status").FirstChild("basic").FirstChild().Text();
		if (basicText) {
			std::string basic = basicText->Value();

			//buddy is offline
			if (String(basic).toLowerCase() == "closed") {
				p->presenceStateChangedEvent(*p,  EnumPresenceState::PresenceStateOffline, String::null, buddy);
			}

			//buddy is online
			else if (String(basic).toLowerCase() == "open") {
				TiXmlText * noteText = docHandle.FirstChild("presence").FirstChild("tuple").FirstChild("status").FirstChild("note").FirstChild().Text();
				if (!noteText) {
					noteText = docHandle.FirstChild("presence").FirstChild("tuple").FirstChild("status").FirstChild("value").FirstChild().Text();
				}
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
		LOG_FATAL("unknown message event=" + tmp);
	}
}

std::string PhApiCallbacks::computeContactId(const std::string & contactFromPhApi) {
	std::string buddyTmp(contactFromPhApi);
	unsigned colonIndex = buddyTmp.find(':', 0);
	unsigned atIndex = buddyTmp.find('@', 0);
	return buddyTmp.substr(colonIndex + 1, atIndex - colonIndex - 1);
}
