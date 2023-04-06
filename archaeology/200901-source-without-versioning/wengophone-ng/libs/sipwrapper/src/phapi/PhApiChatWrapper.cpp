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

#include "PhApiChatWrapper.h"

#include <phapi.h>

PhApiChatWrapper::PhApiChatWrapper()
	: SipChatWrapper() {
}

PhApiChatWrapper::~PhApiChatWrapper() {
}

void PhApiChatWrapper::sendMessage(int lineId, const std::string &contactId, const std::string &msg) {
	int messageId = -1;
	owplMessageSendPlainText(lineId, contactId.c_str(), msg.c_str(), &messageId);
}

void PhApiChatWrapper::sendTypingState(int lineId, const std::string &contactId, EnumChatTypingState::ChatTypingState typingState) {
	int messageId = -1;
	OWPL_TYPING_STATE tState = OWPL_TYPING_STATE_NOT_TYPING;

	switch (typingState) {
	case EnumChatTypingState::ChatTypingStateNotTyping:
		tState = OWPL_TYPING_STATE_NOT_TYPING;
		break;
	case EnumChatTypingState::ChatTypingStateTyping:
		tState = OWPL_TYPING_STATE_TYPING;
		break;
	case EnumChatTypingState::ChatTypingStateStopTyping:
		tState = OWPL_TYPING_STATE_STOP_TYPING;
		break;
	}
	owplMessageSendTypingState(lineId, contactId.c_str(), tState, &messageId);
}
