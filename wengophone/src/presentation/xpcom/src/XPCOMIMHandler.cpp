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

#include "XPCOMIMHandler.h"

#include "ListenerList.h"

#include <IMWrapperFactory.h>
#include <EnumIMProtocol.h>
#include <IMAccount.h>
#include <IMConnect.h>
#include <IMChat.h>
#include <IMPresence.h>

#include <model/phonecall/SipAddress.h>

#include <Logger.h>

void XPCOMIMHandler::connected() {
	if (!_account) {
		_account = new IMAccount(_wengoLogin, _wengoPassword, EnumIMProtocol::IMProtocolSIPSIMPLE);
	}

	if (!_connect) {
		_connect = IMWrapperFactory::getFactory().createIMConnect(*_account);
	}

	if (!_chat) {
		/*_chat = IMWrapperFactory::getFactory().createIMChat(*_account);
		_chat->messageReceivedEvent +=
				boost::bind(&XPCOMIMHandler::messageReceivedEventHandler, this, _1, _2, _3);*/
	}

	if (!_presence) {
		_presence = IMWrapperFactory::getFactory().createIMPresence(*_account);
		_presence->presenceStateChangedEvent +=
				boost::bind(&XPCOMIMHandler::presenceStateChangedEventHandler, this, _1, _2, _3, _4);
	}
}

void XPCOMIMHandler::messageReceivedEventHandler(IMChat & sender, const std::string & from, const std::string & message) {
	SipAddress sipUri(from);
	std::string sipAddress = sipUri.getSipAddress();
	std::string userName = sipUri.getUserName();
	std::string displayName = sipUri.getDisplayName();

	ListenerList & listenerList = ListenerList::getInstance();

	for (unsigned i = 0; i < listenerList.size(); i++) {
		Listener * listener = listenerList[i];
		listener->chatMessageReceivedEvent(message, sipAddress, userName, displayName, NULL);
	}
}

void XPCOMIMHandler::presenceStateChangedEventHandler(IMPresence & sender, EnumPresenceState::PresenceState state,
		const std::string & note, const std::string & from) {

	SipAddress sipUri(from);
	std::string sipAddress = sipUri.getSipAddress();
	std::string userName = sipUri.getUserName();
	std::string displayName = sipUri.getDisplayName();

	ListenerList & listenerList = ListenerList::getInstance();

	for (unsigned i = 0; i < listenerList.size(); i++) {
		Listener * listener = listenerList[i];

		switch(state) {
		case EnumPresenceState::PresenceStateOnline:
			listener->presenceStateChangedEvent(Listener::PresenceOnline, note, sipAddress, userName, displayName, NULL);
			break;

		case EnumPresenceState::PresenceStateOffline:
			listener->presenceStateChangedEvent(Listener::PresenceOffline, note, sipAddress, userName, displayName, NULL);
			break;

		case EnumPresenceState::PresenceStateAway:
			listener->presenceStateChangedEvent(Listener::PresenceAway, note, sipAddress, userName, displayName, NULL);
			break;

		case EnumPresenceState::PresenceStateDoNotDisturb:
			listener->presenceStateChangedEvent(Listener::PresenceDoNotDisturb, note, sipAddress, userName, displayName, NULL);
			break;

		case EnumPresenceState::PresenceStateUserDefined:
			listener->presenceStateChangedEvent(Listener::PresenceUserDefined, note, sipAddress, userName, displayName, NULL);
			break;

		case EnumPresenceState::PresenceStateUnknown:
			listener->presenceStateChangedEvent(Listener::PresenceUnknown, note, sipAddress, userName, displayName, NULL);
			break;

		default:
			LOG_FATAL("unknown presence state");
		}
	}
}

void XPCOMIMHandler::subscribeToPresenceOf(const std::string & sipAddress) {
	if (!_presence) {
		LOG_DEBUG("cannot call subscribeToPresenceOf(): _presence is NULL");
		return;
	}

	_presence->subscribeToPresenceOf(sipAddress);
}

void XPCOMIMHandler::publishMyPresence(Listener::PresenceState state, const std::string & note) {
	if (!_presence) {
		LOG_DEBUG("cannot call publishMyPresence(): _presence is NULL");
		return;
	}

	switch(state) {
	case Listener::PresenceOnline:
		_presence->changeMyPresence(EnumPresenceState::PresenceStateOnline);
		break;

	case Listener::PresenceOffline:
		_presence->changeMyPresence(EnumPresenceState::PresenceStateOffline);
		break;

	case Listener::PresenceAway:
		_presence->changeMyPresence(EnumPresenceState::PresenceStateAway);
		break;

	case Listener::PresenceDoNotDisturb:
		_presence->changeMyPresence(EnumPresenceState::PresenceStateDoNotDisturb);
		break;

	case Listener::PresenceUserDefined:
		_presence->changeMyPresence(EnumPresenceState::PresenceStateUserDefined, note);
		break;

	case Listener::PresenceUnknown:
		_presence->changeMyPresence(EnumPresenceState::PresenceStateUnknown, note);
		break;

	default:
		LOG_FATAL("unknown presence state");
	}
}

int XPCOMIMHandler::sendChatMessage(const std::string & sipAddress, const std::string & message) {
	if (!_chat) {
		LOG_DEBUG("cannot call sendChatMessage(): _chat is NULL");
		return 0;
	}

	//return _chat->sendMessage(sipAddress, message);
	return 0;
}
