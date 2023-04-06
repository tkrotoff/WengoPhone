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

#include <coipmanager_threaded/chatsessionmanager/TChatSessionManager.h>

#include <coipmanager_threaded/TCoIpManager.h>
#include <coipmanager_threaded/chatsessionmanager/TChatSession.h>

#include <coipmanager/CoIpManager.h>
#include <coipmanager/chatsessionmanager/ChatSession.h>
#include <coipmanager/chatsessionmanager/ChatSessionManager.h>

#include <util/SafeDelete.h>
#include <util/SafeConnect.h>

//#include <functional>

TChatSessionManager::TChatSessionManager(TCoIpManager & tCoIpManager)
	: ITCoIpSessionManager(tCoIpManager) {

	SAFE_CONNECT(getChatSessionManager(), SIGNAL(newChatSessionCreatedSignal(ChatSession *)),
		SLOT(newChatSessionCreatedSlot(ChatSession *)));
}

TChatSessionManager::~TChatSessionManager() {
}

ChatSessionManager * TChatSessionManager::getChatSessionManager() const {
	return &_tCoIpManager.getCoIpManager().getChatSessionManager();
}

ICoIpSessionManager * TChatSessionManager::getICoIpSessionManager() const {
	return getChatSessionManager();
}

TChatSession * TChatSessionManager::createTChatSession() {
	return new TChatSession(_tCoIpManager,
		getChatSessionManager()->createChatSession());
}

void TChatSessionManager::newChatSessionCreatedSlot(ChatSession * chatSession) {
	newChatSessionCreatedSignal(new TChatSession(_tCoIpManager, chatSession));
}
