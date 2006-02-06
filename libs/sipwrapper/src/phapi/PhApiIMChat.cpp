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

PhApiIMChat * PhApiIMChat::PhApiIMChatHack = NULL;

PhApiIMChat::PhApiIMChat(IMAccount & account, const IMContactMap & imContactMap, PhApiWrapper & phApiWrapper)
	: IMChat(account, imContactMap),
	_phApiWrapper(phApiWrapper) {

	PhApiIMChatHack = this;

	_phApiWrapper.messageReceivedEvent += boost::bind(&PhApiIMChat::messageReceivedEventHandler, this, _1, _2, _3, _4);
	_phApiWrapper.statusMessageReceivedEvent += boost::bind(&PhApiIMChat::statusMessageReceivedEventHandler, this, _1, _2, _3, _4);
	_phApiWrapper.contactAddedEvent += boost::bind(&PhApiIMChat::contactAddedEventHandler, this, _1, _2, _3);
	_phApiWrapper.contactRemovedEvent += boost::bind(&PhApiIMChat::contactRemovedEventHandler, this, _1, _2, _3);
	_phApiWrapper.newIMChatSessionCreatedEvent += boost::bind(&PhApiIMChat::newIMChatSessionCreatedEventHandler, this, _1, _2);
}

void PhApiIMChat::messageReceivedEventHandler(PhApiWrapper & sender, IMChatSession & chatSession, const std::string & from, const std::string & message) {
	messageReceivedEvent(*this, chatSession, from, message);
}

void PhApiIMChat::statusMessageReceivedEventHandler(PhApiWrapper & sender, IMChatSession & chatSession, StatusMessage status, const std::string & message) {
	statusMessageReceivedEvent(*this, chatSession, status, message);
}

void PhApiIMChat::newIMChatSessionCreatedEventHandler(PhApiWrapper & sender, IMChatSession & imChatSession) {
	newIMChatSessionCreatedEvent(*this, imChatSession);
}

void PhApiIMChat::contactAddedEventHandler(PhApiWrapper & sender, IMChatSession & imChatSession, const std::string & contactId) {
	contactAddedEvent(*this, imChatSession, contactId);
}

void PhApiIMChat::contactRemovedEventHandler(PhApiWrapper & sender, IMChatSession & imChatSession, const std::string & contactId) {
	contactRemovedEvent(*this, imChatSession, contactId);
}

void PhApiIMChat::sendMessage(IMChatSession & chatSession, const std::string & message) {
	_phApiWrapper.sendMessage(chatSession, message);
}

void PhApiIMChat::createSession() {
	_phApiWrapper.createSession(*this);
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
