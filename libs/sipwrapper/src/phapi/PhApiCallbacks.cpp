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

extern "C" {

	static int phApiEventsHandler(OWPL_EVENT_CATEGORY category, void* pInfo, void* pUserData) {
		switch(category) {
			case EVENT_CATEGORY_CALLSTATE :
				PhApiCallbacks::getInstance().callProgress((OWPL_CALLSTATE_INFO *)pInfo);
				break;

			case EVENT_CATEGORY_LINESTATE : 
				PhApiCallbacks::getInstance().registerProgress((OWPL_LINESTATE_INFO *)pInfo);
				break;

			case EVENT_CATEGORY_MESSAGE : 
				PhApiCallbacks::getInstance().messageProgress((OWPL_MESSAGE_INFO *)pInfo);
				break;

			case EVENT_CATEGORY_SUB_STATUS : 
				PhApiCallbacks::getInstance().subscriptionProgress((OWPL_SUBSTATUS_INFO *)pInfo);
				break;

			case EVENT_CATEGORY_NOTIFY : 
				PhApiCallbacks::getInstance().onNotify((OWPL_NOTIFICATION_INFO *)pInfo);
				break;

			default :
				break;
		}

		return 0;
	}

}

PhApiCallbacks::PhApiCallbacks() {}

PhApiCallbacks::~PhApiCallbacks() {}

void PhApiCallbacks::startListeningPhApiEvents() {
	owplEventListenerAdd(phApiEventsHandler, NULL);
}

void PhApiCallbacks::callProgress(OWPL_CALLSTATE_INFO * info) {
	if(info != NULL) {
		PhApiWrapper * p = PhApiWrapper::PhApiWrapperHack;
		std::string from;
		phVideoFrameReceivedEvent_t * video_data = NULL;
		switch(info->event) {
			case CALLSTATE_UNKNOWN :			
				break;

			case CALLSTATE_NEWCALL :
				switch(info->cause) {
					case CALLSTATE_NEW_CALL_NORMAL :
						break;

					case CALLSTATE_NEW_CALL_TRANSFERRED :
						break;

					case CALLSTATE_NEW_CALL_TRANSFER :
						break;

					default :
						break;
				}
				break;

			case CALLSTATE_REMOTE_OFFERING :
				switch(info->cause) {
					case CALLSTATE_REMOTE_OFFERING_NORMAL :
						from = info->szRemoteIdentity;
						p->phoneCallStateChangedEvent(*p, info->hCall, EnumPhoneCallState::PhoneCallStateDialing, from);
						break;

					default :
						break;
				}
				break;

			case CALLSTATE_REMOTE_ALERTING :
				switch(info->cause) {
					case CALLSTATE_REMOTE_ALERTING_NORMAL :
						from = info->szRemoteIdentity;
						p->phoneCallStateChangedEvent(*p, info->hCall, EnumPhoneCallState::PhoneCallStateRinging, from);
						break;

					case CALLSTATE_REMOTE_ALERTING_MEDIA :
						break;

					default :
						break;
				}
				break;

			case CALLSTATE_CONNECTED :
				switch(info->cause) {
					case CALLSTATE_CONNECTED_ACTIVE :
						from = info->szRemoteIdentity;
						p->phoneCallStateChangedEvent(*p, info->hCall, EnumPhoneCallState::PhoneCallStateTalking, from);
						break;

					case CALLSTATE_CONNECTED_ACTIVE_HELD :
						break;

					case CALLSTATE_CONNECTED_INACTIVE :
						break;

					default :
						break;
				}
				break;

			case CALLSTATE_DISCONNECTED :
				switch(info->cause) {
					case CALLSTATE_DISCONNECTED_BADADDRESS :
						break;

					case CALLSTATE_DISCONNECTED_BUSY :
						// TODO nothing?
						LOG_DEBUG("CALLSTATE_DISCONNECTED_BUSY");
						break;

					case CALLSTATE_DISCONNECTED_NORMAL :
						p->phoneCallStateChangedEvent(*p, info->hCall, EnumPhoneCallState::PhoneCallStateClosed, from);
						break;

					case CALLSTATE_DISCONNECTED_RESOURCES :
						break;

					case CALLSTATE_DISCONNECTED_NETWORK :
						p->phoneCallStateChangedEvent(*p, info->hCall, EnumPhoneCallState::PhoneCallStateError, from);
						break;

					case CALLSTATE_DISCONNECTED_REDIRECTED :
						break;

					case CALLSTATE_DISCONNECTED_NO_RESPONSE :
						from = info->szRemoteIdentity;
						p->phoneCallStateChangedEvent(*p, info->hCall, EnumPhoneCallState::PhoneCallStateMissed, from);
						break;

					case CALLSTATE_DISCONNECTED_AUTH :
						break;

					case CALLSTATE_DISCONNECTED_UNKNOWN :
						p->phoneCallStateChangedEvent(*p, info->hCall, EnumPhoneCallState::PhoneCallStateError, from);
						break;

					default :
						break;
				}
				break;

			case CALLSTATE_OFFERING :
				switch(info->cause) {
					case CALLSTATE_OFFERING_ACTIVE :
						from = info->szRemoteIdentity;
						p->phoneCallStateChangedEvent(*p, info->hCall, EnumPhoneCallState::PhoneCallStateIncoming, from);
						break;

					default :
						break;
				}
				break;
				
			case CALLSTATE_ALERTING :
				switch(info->cause) {
					case CALLSTATE_ALERTING_NORMAL :
						break;

					default :
						break;
				}
				break;
				
			case CALLSTATE_DESTROYED :
				switch(info->cause) {
					case CALLSTATE_DESTROYED_NORMAL :
						break;

					default :
						break;
				}
				break;
				
			case CALLSTATE_AUDIO_EVENT :
				switch(info->cause) {
					case CALLSTATE_AUDIO_START :
						break;

					case CALLSTATE_AUDIO_STOP :
						break;

					case CALLSTATE_AUDIO_DTMF :
						break;

					default :
						break;
				}
				break;
				
			case CALLSTATE_VIDEO_EVENT :
				switch(info->cause) {
					case CALLSTATE_VIDEO_START :
						break;

					case CALLSTATE_VIDEO_STOP :
						break;

					case CALLSTATE_VIDEO_FRAME_RCV :
						if(info->pData != NULL) {
							video_data = (phVideoFrameReceivedEvent_t *) info->pData;
#ifdef ENABLE_VIDEO
							if(video_data != NULL) {
								p->videoFrameReceivedEvent(*p, info->hCall, video_data->frame_remote, video_data->frame_local);
							}
#endif
						}
						break;

					default :
						break;
				}
				break;
				
			case CALLSTATE_TRANSFER :
				switch(info->cause) {
					case CALLSTATE_TRANSFER_INITIATED :					
						break;

					case CALLSTATE_TRANSFER_ACCEPTED :
						break;

					case CALLSTATE_TRANSFER_TRYING :
						break;

					case CALLSTATE_TRANSFER_RINGING :
						break;

					case CALLSTATE_TRANSFER_SUCCESS :
						break;

					case CALLSTATE_TRANSFER_FAILURE :
						break;

					default :
						break;
				}
				break;
				
			case CALLSTATE_REDIRECTED :
				switch(info->cause) {
					case CALLSTATE_REDIRECTED_NORMAL :
						from = info->szRemoteIdentity;
						p->phoneCallStateChangedEvent(*p, info->hCall, EnumPhoneCallState::PhoneCallStateRedirected, from);
						break;

					default :
						break;
				}
				break;
				
			case CALLSTATE_HOLD :
				switch(info->cause) {
					case CALLSTATE_HOLD_STARTED :
						from = info->szRemoteIdentity;
						p->phoneCallStateChangedEvent(*p, info->hCall, EnumPhoneCallState::PhoneCallStateHold, from);
						break;

					case CALLSTATE_HOLD_RESUMED :
						from = info->szRemoteIdentity;
						p->phoneCallStateChangedEvent(*p, info->hCall, EnumPhoneCallState::PhoneCallStateResumed, from);
						break;

					default :
						break;
				}
				break;
				
			case CALLSTATE_SECURITY_EVENT :
				switch(info->cause) {
					case CALLSTATE_SECURITY_SELF_SIGNED_CERT :
						break;

					case CALLSTATE_SECURITY_SESSION_NOT_SECURED :
						break;

					case CALLSTATE_SECURITY_REMOTE_SMIME_UNSUPPORTED :
						break;

					default :
						break;
				}
				break;
				
			case CALLSTATE_IDENTITY_CHANGE :
				switch(info->cause) {
					case CALLSTATE_IDENTITY_CHANGE_UNKNOWN :
						break;

					default :
						break;
				}
				break;

			default :
				break;
		}
	}
}

void PhApiCallbacks::registerProgress(OWPL_LINESTATE_INFO * info) {
	PhApiWrapper * p = PhApiWrapper::PhApiWrapperHack;
	switch(info->event) {
		case LINESTATE_UNKNOWN :
			p->setRegistered(false);
			p->phoneLineStateChangedEvent(*p, info->hLine, EnumPhoneLineState::PhoneLineStateUnknown);
			break;

		case LINESTATE_REGISTERING :
			switch(info->cause) {
				case LINESTATE_REGISTERING_NORMAL :
					break;

				default :
					break;
			}
			break;

		case LINESTATE_REGISTERED :
			switch(info->cause) {
				case LINESTATE_REGISTERED_NORMAL :
				if(!p->isRegistered()) {
					p->setRegistered(true);
					p->phoneLineStateChangedEvent(*p, info->hLine, EnumPhoneLineState::PhoneLineStateOk);
					p->connectedEvent(*p);
					for (std::set<std::string>::const_iterator it = _subscribedContacts.begin();
						it != _subscribedContacts.end();
						++it) {
							p->subscribeToPresenceOf(*it);
					}
				}
				break;

				default :
				break;
			}
			break;

		case LINESTATE_UNREGISTERING :
			switch(info->cause) {
				case LINESTATE_UNREGISTERING_NORMAL :
					break;

				default :
					break;
			}
			break;

		case LINESTATE_UNREGISTERED :
			switch(info->cause) {
				case LINESTATE_UNREGISTERED_NORMAL :
					p->setRegistered(false);
					p->phoneLineStateChangedEvent(*p, info->hLine, EnumPhoneLineState::PhoneLineStateClosed);
					for (std::set<std::string>::const_iterator it = _subscribedContacts.begin();
						it != _subscribedContacts.end();
						++it) {
							p->presenceStateChangedEvent(*p, EnumPresenceState::PresenceStateUnknown, "", *it);
					}
					break;

				default :
					break;
			}
			break;

		case LINESTATE_REGISTER_FAILED :
			switch(info->cause) {
				case LINESTATE_REGISTER_FAILED_COULD_NOT_CONNECT :
					if (p->isRegistered()) {
						p->setRegistered(false);
						p->phoneLineStateChangedEvent(*p, info->hLine, EnumPhoneLineState::PhoneLineStateTimeout);
						//p->removeVirtualLine(p->getActiveVline(), 0);
						for (std::set<std::string>::const_iterator it = _subscribedContacts.begin();
							it != _subscribedContacts.end();
							++it) {
								p->presenceStateChangedEvent(*p, EnumPresenceState::PresenceStateUnknown, "", *it);
						}
						p->disconnectedEvent(*p, true, "No response from server");
					}
					break;

				case LINESTATE_REGISTER_FAILED_NOT_AUTHORIZED :
					p->setRegistered(false);
					p->phoneLineStateChangedEvent(*p, info->hLine, EnumPhoneLineState::PhoneLineStateServerError);
					break;

				case LINESTATE_REGISTER_FAILED_TIMEOUT :
					p->setRegistered(false);
					p->phoneLineStateChangedEvent(*p, info->hLine, EnumPhoneLineState::PhoneLineStateTimeout);
					break;

				case LINESTATE_REGISTER_FAILED_NOT_FOUND :
					p->setRegistered(false);
					p->phoneLineStateChangedEvent(*p, info->hLine, EnumPhoneLineState::PhoneLineStateServerError);
					break;

				default :
					p->setRegistered(false);
					LOG_ERROR("unknown phApi event");
					p->phoneLineStateChangedEvent(*p, info->hLine, EnumPhoneLineState::PhoneLineStateServerError);
					break;
			}
			break;

		case LINESTATE_UNREGISTER_FAILED :
			switch(info->cause) {
				case LINESTATE_UNREGISTER_FAILED_COULD_NOT_CONNECT :
					break;

				case LINESTATE_UNREGISTER_FAILED_NOT_AUTHORIZED :
					break;

				case LINESTATE_UNREGISTER_FAILED_TIMEOUT :
					break;

				default :
					break;
			}
			break;

		case LINESTATE_PROVISIONED :
			switch(info->cause) {
				case LINESTATE_PROVISIONED_NORMAL :
					break;

				default :
					break;
			}
			break;

		default :
			p->setRegistered(false);
			LOG_ERROR("unknown phApi event");
			p->phoneLineStateChangedEvent(*p, info->hLine, EnumPhoneLineState::PhoneLineStateServerError);
			break;
	}
}

void PhApiCallbacks::messageProgress(OWPL_MESSAGE_INFO * info) {
	PhApiWrapper * p = PhApiWrapper::PhApiWrapperHack;
	IMChatSession * imChatSession;

	if(info->event == MESSAGE_SUCCESS && info->cause == MESSAGE_SUCCESS_NORMAL ) {
		//We drop status message
		return;
	}

	std::string content;
	if(info->szContent) {
		content = info->szContent;
	}

	LOG_DEBUG("message received from=" + std::string(info->szRemoteIdentity) + " content=" + content);

	//Getting maps from PhApiWrapper
	std::map<const std::string, IMChatSession *> & contactChatMap = p->getContactChatMap();

	std::string from;
	//<sip:robobob1535@voip.wengo.fr>;tag=b6e249633711def8c2dbe3c2d4f39996-e1d6
	if (info->event != MESSAGE_FAILURE) {
		from = String(info->szRemoteIdentity).split(" ")[0];
	} else {
		int atPos = String(info->szLocalIdentity).find("@");
		from = String(info->szLocalIdentity).substr(5, atPos - 5);
	}
	//Finding associated session
	std::map<const std::string, IMChatSession *>::const_iterator sessionIt = contactChatMap.find(from);

	std::string ctype;
	if (info->szContentType) {
		ctype = info->szContentType;
	}

	std::string subtype;
	if (info->szSubContentType) {
		subtype = info->szSubContentType;
	}

	//Getting buddy icon
	if ((info->event == MESSAGE_NEW) && (ctype == "buddyicon")) {
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
	case MESSAGE_NEW : {

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
	case MESSAGE_FAILURE :
		LOG_DEBUG("message could not be sent");
		p->statusMessageReceivedEvent(*p, *imChatSession, PhApiIMChat::StatusMessageError, content);
		break;
	}

	default:
		LOG_FATAL("unknown message event=" + String::fromNumber(info->event));
	}
}

/*
TODO for future use ?
void PhApiCallbacks::messageProgress(OWPL_MESSAGE_INFO * info) {
	PhApiWrapper * p = PhApiWrapper::PhApiWrapperHack;
	IMChatSession * imChatSession;
	string from;
	int atPos = 0;
	std::string buddyIcon;
	std::map<const std::string, IMChatSession *>::const_iterator sessionIt;
	switch(info->event) {
		case MESSAGE_UNKNOWN :
			break;
		
		case MESSAGE_NEW :
			switch(info->cause) {
				case MESSAGE_NEW_NORMAL :
					LOG_DEBUG("message received from=" + std::string(info->szRemoteIdentity) + " content=" + std::string(info->szContent));
					from = String(info->szRemoteIdentity).split(" ")[0];
					sessionIt = contactChatMap.find(from);
					if (sessionIt != contactChatMap.end()) {
						imChatSession = (*sessionIt).second;						
					} else {
						LOG_DEBUG("creating new IMChatSession");
						imChatSession = new IMChatSession(*PhApiIMChat::PhApiIMChatHack);
						contactChatMap[from] = imChatSession;
						p->addContact(*imChatSession, from);
						p->newIMChatSessionCreatedEvent(*p, *imChatSession);
						p->sendMyIcon(from, p->_iconFilename);
					}
					p->messageReceivedEvent(*p, *imChatSession, from, std::string(info->szContent));
					break;

				case MESSAGE_NEW_BUDDY_ICON :
					LOG_DEBUG("message received from=" + std::string(info->szRemoteIdentity) + " content=" + std::string(info->szContent));
					from = String(info->szRemoteIdentity).split(" ")[0];
					buddyIcon = info->szSubContentType;
					if(!buddyIcon.empty()) {
						p->contactIconChangedEvent(*p, from, buddyIcon);
					}
					break;

				case MESSAGE_NEW_TYPING :
					LOG_DEBUG("message received from=" + std::string(info->szRemoteIdentity) + " content=" + std::string(info->szContent));
					from = String(info->szRemoteIdentity).split(" ")[0];
					sessionIt = contactChatMap.find(from);
					if (sessionIt != contactChatMap.end()) {
						imChatSession = (*sessionIt).second;
						p->typingStateChangedEvent(*p, *imChatSession, from, IMChat::TypingStateTyping);
					}
					break;

				case MESSAGE_NEW_STOP_TYPING :
					LOG_DEBUG("message received from=" + std::string(info->szRemoteIdentity) + " content=" + std::string(info->szContent));
					from = String(info->szRemoteIdentity).split(" ")[0];
					sessionIt = contactChatMap.find(from);
					if (sessionIt != contactChatMap.end()) {
						imChatSession = (*sessionIt).second;
						p->typingStateChangedEvent(*p, *imChatSession, from, IMChat::TypingStateStopTyping);
					}
					break;

				case MESSAGE_NEW_NOT_TYPING :
					LOG_DEBUG("message received from=" + std::string(info->szRemoteIdentity) + " content=" + std::string(info->szContent));
					from = String(info->szRemoteIdentity).split(" ")[0];
					sessionIt = contactChatMap.find(from);
					if (sessionIt != contactChatMap.end()) {
						imChatSession = (*sessionIt).second;
						p->typingStateChangedEvent(*p, *imChatSession, from, IMChat::TypingStateNotTyping);
					}
					break;

				default :
					break;
			}
			break;
		
		case MESSAGE_SUCCESS :
			switch(info->cause) {
				case MESSAGE_SUCCESS_NORMAL :
					// do nothing : drop the status message
					break;

				default :
					break;
			}
			break;
		
		case MESSAGE_FAILURE :
			switch(info->cause) {
				case MESSAGE_FAILURE_UNKNOWN :
					LOG_DEBUG("message received from=" + std::string(info->szRemoteIdentity) + " content=" + std::string(info->szContent));
					atPos = String(info->szLocalIdentity).find("@");
					from = String(info->szLocalIdentity).substr(5, atPos - 5);
					sessionIt = contactChatMap.find(from);
					if (sessionIt != contactChatMap.end()) {
						imChatSession = (*sessionIt).second;						
					} else {
						LOG_DEBUG("creating new IMChatSession");
						imChatSession = new IMChatSession(*PhApiIMChat::PhApiIMChatHack);
						contactChatMap[from] = imChatSession;
						p->addContact(*imChatSession, from);
						p->newIMChatSessionCreatedEvent(*p, *imChatSession);
						p->sendMyIcon(from, p->_iconFilename);
					}
					LOG_DEBUG("message could not be sent");
					p->statusMessageReceivedEvent(*p, *imChatSession, PhApiIMChat::StatusMessageError, std::string(info->szContent));
					break;

				case MESSAGE_FAILURE_COULD_NOT_SEND :
					LOG_DEBUG("message received from=" + std::string(info->szRemoteIdentity) + " content=" + std::string(info->szContent));
					atPos = String(info->szLocalIdentity).find("@");
					from = String(info->szLocalIdentity).substr(5, atPos - 5);
					sessionIt = contactChatMap.find(from);
					if (sessionIt != contactChatMap.end()) {
						imChatSession = (*sessionIt).second;						
					} else {
						LOG_DEBUG("creating new IMChatSession");
						imChatSession = new IMChatSession(*PhApiIMChat::PhApiIMChatHack);
						contactChatMap[from] = imChatSession;
						p->addContact(*imChatSession, from);
						p->newIMChatSessionCreatedEvent(*p, *imChatSession);
						p->sendMyIcon(from, p->_iconFilename);
					}
					LOG_DEBUG("message could not be sent");
					p->statusMessageReceivedEvent(*p, *imChatSession, PhApiIMChat::StatusMessageError, std::string(info->szContent));
					break;

				default :
					LOG_DEBUG("message received from=" + std::string(info->szRemoteIdentity) + " content=" + std::string(info->szContent));
					LOG_DEBUG("message could not be sent : unknown failure cause=" + String::fromNumber(info->cause));
					break;
			}
			break;

		default :
			LOG_DEBUG("message received from=" + std::string(info->szRemoteIdentity) + " content=" + std::string(info->szContent));
			LOG_FATAL("unknown message event=" + String::fromNumber(info->event));
			break;
	}
}
*/

void PhApiCallbacks::subscriptionProgress(OWPL_SUBSTATUS_INFO * info) {
	PhApiWrapper * p = PhApiWrapper::PhApiWrapperHack;
	std::string from(info->szRemoteIdentity);

	switch(info->state) {

		case OWPL_SUBSCRIPTION_PENDING :
			switch(info->cause) {
				case SUBSCRIPTION_CAUSE_NORMAL :
					break;

				default:
					break;
			}
			break;

		case OWPL_SUBSCRIPTION_ACTIVE :
			switch(info->cause) {
				case SUBSCRIPTION_CAUSE_NORMAL :
					p->subscribeStatusEvent(*p, from, IMPresence::SubscribeStatusOk);
					break;

				default:
					break;
			}
			break;

		case OWPL_SUBSCRIPTION_FAILED :
			switch(info->cause) {
				case SUBSCRIPTION_CAUSE_UNKNOWN :
					p->subscribeStatusEvent(*p, from, IMPresence::SubscribeStatusError);
					break;

				default:
					break;
			}
			break;

		case OWPL_SUBSCRIPTION_EXPIRED :
			switch(info->cause) {
				case SUBSCRIPTION_CAUSE_UNKNOWN :
					break;

				case SUBSCRIPTION_CAUSE_NORMAL :
					break;

				default:
					break;
			}
			break;

		default :
			break;
	}
}

void PhApiCallbacks::onNotify(OWPL_NOTIFICATION_INFO * info) {
	PhApiWrapper * p = PhApiWrapper::PhApiWrapperHack;
	std::string buddy = computeContactId(info->szRemoteIdentity);
	_subscribedContacts.insert(computeContactId(buddy));
	TiXmlDocument doc;
	doc.Parse(info->szXmlContent);
	TiXmlHandle docHandle(&doc);
	TiXmlText * basicText = NULL;
	TiXmlText * noteText = NULL;
	TiXmlElement * watcherinfoElement = NULL;
	TiXmlElement * watcherElement = NULL;

	switch(info->event) {
		//A buddy presence
		case NOTIFICATION_PRESENCE :
			LOG_DEBUG("buddy=" + buddy + " notification=presence content=" + std::string(info->szXmlContent));

			basicText = docHandle.FirstChild("presence").FirstChild("tuple").FirstChild("status").FirstChild("basic").FirstChild().Text();
			if (basicText) {
				std::string basic = basicText->Value();
				LOG_DEBUG("basic=" + basic);

				//buddy is offline
				if (String(basic).toLowerCase() == "closed") {
					p->presenceStateChangedEvent(*p,  EnumPresenceState::PresenceStateOffline, String::null, buddy);
				}

				//buddy is online
				else if (String(basic).toLowerCase() == "open") {
					noteText = docHandle.FirstChild("presence").FirstChild("tuple").FirstChild("status").FirstChild("note").FirstChild().Text();
					if (!noteText) {
						noteText = docHandle.FirstChild("presence").FirstChild("tuple").FirstChild("status").FirstChild("value").FirstChild().Text();
					}
					if (noteText) {
						std::string note = noteText->Value();
						LOG_DEBUG("note=" + note);
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
			break;

		//watcher list
		case NOTIFICATION_WATCHER :
			LOG_DEBUG("buddy=" + buddy + " notification=presence.winfo content=" + std::string(info->szXmlContent));
			watcherinfoElement = doc.FirstChildElement("watcherinfo");
			if (watcherinfoElement) {

				watcherElement = watcherinfoElement->FirstChildElement("watcher");
				while (watcherElement) {

					std::string watcher(watcherElement->Value());
					p->allowWatcher(watcher);

					watcherElement = watcherinfoElement->NextSiblingElement("watcher");
				}
			}
			break;

		case NOTIFICATION_UNKNOWN :
			LOG_FATAL("unknown message event from="+ std::string(info->szRemoteIdentity) +" content=" + std::string(info->szXmlContent));
			break;

		default :
			LOG_FATAL("unknown message event from="+ std::string(info->szRemoteIdentity) +" content=" + std::string(info->szXmlContent));
			break;
	}
}

std::string PhApiCallbacks::computeContactId(const std::string & contactFromPhApi) {
	std::string buddyTmp(contactFromPhApi);
	unsigned colonIndex = buddyTmp.find(':', 0);
	unsigned atIndex = buddyTmp.find('@', 0);
	return buddyTmp.substr(colonIndex + 1, atIndex - colonIndex - 1);
}
