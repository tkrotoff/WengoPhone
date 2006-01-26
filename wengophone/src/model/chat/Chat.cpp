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

#include "Chat.h"

#include <model/imwrapper/IMWrapperFactory.h>

Chat::Chat(IMAccount & account)
	: _imAccount(account) {
	
	_imChat = IMWrapperFactory::getFactory().createIMChat(account);
	
	_imChat->messageReceivedEvent += messageReceivedEvent;
	_imChat->statusMessageEvent += statusMessageEvent;
}

int Chat::createSession() {
	return _imChat->createSession();
}

void Chat::closeSession(int session) {
	_imChat->closeSession(session);
}

void Chat::sendMessage(int session, const std::string & message) {
	_imChat->sendMessage(session, message);
}

void Chat::addContact(int session, const std::string & contactId) {
	_imChat->addContact(session, contactId);
}

void Chat::removeContact(int session, const std::string & contactId) {
	_imChat->removeContact(session, contactId);
}
