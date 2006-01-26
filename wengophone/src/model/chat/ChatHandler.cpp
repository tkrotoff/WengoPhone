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

#include "ChatHandler.h"

#include <model/chat/Chat.h>
#include <model/imwrapper/IMAccount.h>

#include <Logger.h>

using namespace std;

ChatHandler::ChatHandler() {

}

ChatHandler::~ChatHandler() {
	for (ChatMap::iterator i = _chatMap.begin() ; i != _chatMap.end() ; i++) {
		delete (*i).second;
	}
}

int ChatHandler::createSession(const IMAccount & imAccount) {
	ChatMap::iterator it = findChat(_chatMap, (IMAccount &)imAccount);

	if (it != _chatMap.end()) {
		int newSession = (*it).second->createSession();
		LOG_DEBUG("new session created: #" + String::fromNumber(newSession) 
			+ " for protocol: " + String::fromNumber(imAccount.getProtocol()) 
			+ " and login: " + imAccount.getLogin());
		_sessionChatMap[newSession] = (*it).second;
		return newSession;
	} else {
		return -1;
	}
}

void ChatHandler::closeSession(int session) {
	Chat * chat = _sessionChatMap[session];

	if (chat) {
		LOG_DEBUG("closing session #" + String::fromNumber(session));
		chat->closeSession(session);
	}
}

void ChatHandler::sendMessage(int session, const std::string & message) {
	Chat * chat = _sessionChatMap[session];

	if (chat) {
		LOG_DEBUG("sending message: session #" + String::fromNumber(session)
			+ ", message: " + message);
		chat->sendMessage(session, message);
	}
}

void ChatHandler::addContact(int session, const std::string & contactId) {
	Chat * chat = _sessionChatMap[session];

	if (chat) {
		LOG_DEBUG("adding a contact to session #" + String::fromNumber(session));
		chat->addContact(session, contactId);
	}
}

void ChatHandler::removeContact(int session, const std::string & contactId) {
	Chat * chat = _sessionChatMap[session];

	if (chat) {
		LOG_DEBUG("removing a contact to session #" + String::fromNumber(session));
		chat->addContact(session, contactId);
	}
}

void ChatHandler::connected(IMAccount & account) {
	ChatMap::iterator i = _chatMap.find(&account);
	
	LOG_DEBUG("an account is connected: login: " + account.getLogin() 
		+ "protocol: " + String::fromNumber(account.getProtocol()));
	//Chat for this IMAccount has not been created yet
	if (i == _chatMap.end()) {
		Chat * chat = new Chat(account);
		_chatMap[&account] = chat;

		chat->messageReceivedEvent += messageReceivedEvent;
		chat->statusMessageEvent += statusMessageEvent;
	}
}

void ChatHandler::disconnected(IMAccount & account) {

}

ChatHandler::ChatMap::iterator ChatHandler::findChat(ChatMap & chatMap, IMAccount & imAccount) {
	ChatMap::iterator i;
	for (i = chatMap.begin() ; i != chatMap.end() ; i++) {
		if ((*((*i).first)) == imAccount) {
			break;
		} 
	}
	return i;
}
