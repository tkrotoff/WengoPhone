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

#include <coipmanager_threaded/chatsessionmanager/TChatSession.h>

#include <coipmanager_threaded/TCoIpManager.h>
#include <coipmanager_threaded/chatsessionmanager/TChatSessionManager.h>

#include <coipmanager/CoIpManager.h>
#include <coipmanager/chatsessionmanager/ChatSession.h>
#include <coipmanager/chatsessionmanager/ChatSessionManager.h>

#include <util/PostEvent.h>
#include <util/SafeConnect.h>

TChatSession::TChatSession(TCoIpManager & tCoIpManager, ChatSession * chatSession)
	: TSession(tCoIpManager, chatSession) {

	SAFE_CONNECT(chatSession,
		SIGNAL(messageAddedSignal()),
		SIGNAL(messageAddedSignal()));
	SAFE_CONNECT(chatSession,
		SIGNAL(statusMessageReceivedSignal(EnumChatStatusMessage::ChatStatusMessage, std::string)),
		SIGNAL(statusMessageReceivedSignal(EnumChatStatusMessage::ChatStatusMessage, std::string)));
	SAFE_CONNECT(chatSession,
		SIGNAL(typingStateChangedSignal(Contact, EnumChatTypingState::ChatTypingState)),
		SIGNAL(typingStateChangedSignal(Contact, EnumChatTypingState::ChatTypingState)));
}

TChatSession::~TChatSession() {
}

ChatSession * TChatSession::getChatSession() const {
	return static_cast<ChatSession *>(_module);
}

ChatMessageList TChatSession::getMessageHistory() const {
	return getChatSession()->getMessageHistory();
}

void TChatSession::sendMessage(const std::string & message) {
	PostEvent::invokeMethod(getChatSession(), "sendMessage",
		Q_ARG(std::string, message));
}

void TChatSession::setTypingState(EnumChatTypingState::ChatTypingState state) {
	PostEvent::invokeMethod(getChatSession(), "setTypingState",
		Q_ARG(EnumChatTypingState::ChatTypingState, state));
}

EnumChatTypingState::ChatTypingState TChatSession::getTypingState(const Contact & contact) const {
	return getChatSession()->getTypingState(contact);
}
