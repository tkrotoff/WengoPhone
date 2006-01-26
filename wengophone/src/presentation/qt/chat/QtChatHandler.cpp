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

#include "QtChatHandler.h"

#include <control/chat/CChatHandler.h>

#include <Logger.h>

QtChatHandler::QtChatHandler(CChatHandler & cChatHandler) 
	: _cChatHandler(cChatHandler) {

	_cChatHandler.messageReceivedEvent +=
		boost::bind(&QtChatHandler::messageReceivedEventHandler, this, _1, _2, _3, _4);
	_cChatHandler.statusMessageEvent +=
		boost::bind(&QtChatHandler::statusMessageEventHandler, this, _1, _2, _3, _4);
}

QtChatHandler::~QtChatHandler() {

}

int QtChatHandler::createSession(const IMAccount & imAccount) {
	return _cChatHandler.createSession(imAccount);
}

void QtChatHandler::closeSession(int session) {
	_cChatHandler.closeSession(session);
}

void QtChatHandler::sendMessage(int session, const std::string & message) {
	_cChatHandler.sendMessage(session, message);
}

void QtChatHandler::addContact(int session, const std::string & contactId) {
	_cChatHandler.addContact(session, contactId);
}

void QtChatHandler::removeContact(int session, const std::string & contactId) {
	_cChatHandler.removeContact(session, contactId);
}

void QtChatHandler::messageReceivedEventHandler(IMChat & sender, int session, const std::string & from, const std::string & message) {
	LOG_DEBUG("message received from " + from + ": " + message);
}

void QtChatHandler::statusMessageEventHandler(IMChat & sender, int session, IMChat::StatusMessage status, const std::string & message) {
	LOG_DEBUG("status message #" + String::fromNumber(status) + " received: " + message);
}
