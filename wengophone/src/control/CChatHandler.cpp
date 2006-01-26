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

#include "CChatHandler.h"

#include <model/ChatHandler.h>
#include <presentation/PFactory.h>

CChatHandler::CChatHandler(ChatHandler & chatHandler)
	: _chatHandler(chatHandler) {

	_pChatHandler = PFactory::getFactory().createPresentationChatHandler(*this);

	_chatHandler.messageReceivedEvent += messageReceivedEvent;
	_chatHandler.statusMessageEvent += statusMessageEvent;

}

CChatHandler::~CChatHandler() {

}

int CChatHandler::createSession(EnumIMProtocol::IMProtocol protocol, const std::string & login) {
	return _chatHandler.createSession(protocol, login);
}

void CChatHandler::closeSession(int session) {
	_chatHandler.closeSession(session);
}

void CChatHandler::sendMessage(int session, const std::string & message) {
	_chatHandler.sendMessage(session, message);
}

void CChatHandler::addContact(int session, const std::string & contactId) {
	_chatHandler.addContact(session, contactId);
}

void CChatHandler::removeContact(int session, const std::string & contactId) {
	_chatHandler.removeContact(session, contactId);
}
