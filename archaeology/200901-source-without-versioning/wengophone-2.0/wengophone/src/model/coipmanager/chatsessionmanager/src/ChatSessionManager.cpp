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

#include <chatsessionmanager/ChatSessionManager.h>

#include <chatsessionmanager/ChatSession.h>
#include <chatsessionmanager/IChatSessionManager.h>

ChatSessionManager::ChatSessionManager(UserProfile & userProfile) 
	: _userProfile(userProfile) {
}

ChatSessionManager::~ChatSessionManager() {
	for (std::vector<IChatSessionManager *>::const_iterator it = _chatSessionManagerVector.begin();
		it != _chatSessionManagerVector.end();
		++it) {
		delete (*it);
	}
}

ChatSession * ChatSessionManager::createChatSession() {
	return new ChatSession(*this, _userProfile);
}

void ChatSessionManager::newIChatSessionCreatedEventHandler(IChatSessionManager & sender, 
	IChatSession * iChatSession) {
	ChatSession * newChatSession = new ChatSession(*this, _userProfile, iChatSession);
	newChatSessionCreatedEvent(*this, newChatSession);
}

IChatSession * ChatSessionManager::createIChatSessionForAccount(const Account & account) {
	return NULL;
}
