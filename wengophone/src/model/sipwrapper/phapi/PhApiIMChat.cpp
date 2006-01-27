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

#include "PhApiIMChat.h"

#include <model/imwrapper/IMChatSession.h>

#include "PhApiWrapper.h"

PhApiIMChat::PhApiIMChat(IMAccount & account, PhApiWrapper & phApiWrapper)
	: IMChat(account),
	_phApiWrapper(phApiWrapper) {

	_phApiWrapper.messageReceivedEvent += boost::bind(&PhApiIMChat::messageReceivedEventHandler, this, _1, _2, _3, _4);
	_phApiWrapper.statusMessageEvent += boost::bind(&PhApiIMChat::messageStatusEventHandler, this, _1, _2, _3, _4);
}

void PhApiIMChat::messageReceivedEventHandler(PhApiWrapper & sender, IMChatSession & chatSession, const std::string & from, const std::string & message) {
	//messageReceivedEvent(*this, from, message);
}

void PhApiIMChat::messageStatusEventHandler(PhApiWrapper & sender, IMChatSession & chatSession, StatusMessage status, const std::string & message) {
	//messageStatusEvent(*this, status, messageId);
}

void PhApiIMChat::sendMessage(IMChatSession & chatSession, const std::string & message) {
	_phApiWrapper.sendMessage(chatSession, message);
}

void PhApiIMChat::createSession(IMChatSession & chatSession) {
	_phApiWrapper.createSession(chatSession);
}

void PhApiIMChat::closeSession(IMChatSession & chatSession) {
	_phApiWrapper.closeSession(chatSession);
}

void PhApiIMChat::addContact(IMChatSession & chatSession, const std::string & contactId) {
	_phApiWrapper.addContact(chatSession, contactId);
}

void PhApiIMChat::removeContact(IMChatSession & chatSession, const std::string & contactId) {
	_phApiWrapper.removeContact(chatSession, contactId);
}
