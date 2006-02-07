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

#include <IMChatSession.h>

#include <IMContact.h>
#include <IMContactMap.h>

#include <Logger.h>

using namespace std;

IMChatSession::IMChatSession(IMChat & imChat)
	: _imChat(imChat) {

	_imChat.messageReceivedEvent +=
		boost::bind(&IMChatSession::messageReceivedEventHandler, this, _1, _2, _3, _4);
	_imChat.statusMessageReceivedEvent +=
		boost::bind(&IMChatSession::statusMessageReceivedEventHandler, this, _1, _2, _3, _4);
	_imChat.contactAddedEvent +=
		boost::bind(&IMChatSession::contactAddedEventHandler, this, _1, _2, _3);
	_imChat.contactRemovedEvent +=
		boost::bind(&IMChatSession::contactRemovedEventHandler, this, _1, _2, _3);

}

IMChatSession::~IMChatSession() {
	_imChat.closeSession(*this);
}

void IMChatSession::addIMContact(const IMContact & imContact) {
	//The IMContact must be of the same protocol of the IMAccount
	if (_imChat.getIMAccount() == imContact.getIMAccount()) {
		LOG_DEBUG("adding a new IMContact: " + imContact.getContactId());
		_imChat.addContact(*this, imContact.getContactId());
	}
}

void IMChatSession::removeIMContact(const IMContact & imContact) {
	LOG_DEBUG("removing an IMContact" + imContact.getContactId());
	_imChat.removeContact(*this, imContact.getContactId());
}

void IMChatSession::sendMessage(const std::string & message) {
	LOG_DEBUG("sending message: " + message);
	_imChat.sendMessage(*this, message);
}

const IMChatSession::IMContactList & IMChatSession::getIMContactList() const {
	return _imContactList;
}

void IMChatSession::messageReceivedEventHandler(IMChat & sender, IMChatSession & imChatSession, const std::string & from, const std::string & message) {
	LOG_DEBUG("message received: " + message);

	if (imChatSession == *this) {
		const IMContact * imContact = getIMContact(sender.getIMAccount(), from);
		if (imContact) {
			messageReceivedEvent(*this, *imContact, message);
		} else {
			LOG_ERROR("this session does not know " + from);
		}
	}
}

void IMChatSession::statusMessageReceivedEventHandler(IMChat & sender, IMChatSession & imChatSession, IMChat::StatusMessage status, const std::string & message) {
	LOG_DEBUG("status message received: " + message);

	if (imChatSession == *this) {
		statusMessageReceivedEvent(*this, status, message);
	}
}

const IMContact * IMChatSession::getIMContact(const IMAccount & imAccount, const std::string & contactId) const {
	for (IMContactList::const_iterator it = _imContactList.begin() ; it != _imContactList.end() ; it++) {
		const IMContact * imContact = (*it);
		if ((imContact->getIMAccount() == imAccount)
			&& (imContact->getContactId() == contactId)) {
			return imContact;
		}
	}

	return NULL;
}

bool IMChatSession::operator == (const IMChatSession & imChatSession) const {
	return (getId() == imChatSession.getId());
}

int IMChatSession::getId() const {
	return (long)this;
}

void IMChatSession::contactAddedEventHandler(IMChat & sender, IMChatSession & imChatSession, const std::string & contactId) {
	if (imChatSession == *this) {
		IMContact * imContact = _imChat.getIMContactMap().findIMContact(_imChat.getIMAccount(), contactId);
		if (imContact) {
			_imContactList.insert(imContact);
			contactAddedEvent(*this, *imContact);
		} else {
			LOG_ERROR("IMContact for " + contactId + " not in IMContactMap");
		}
	}
}

void IMChatSession::contactRemovedEventHandler(IMChat & sender, IMChatSession & imChatSession, const std::string & contactId) {
	if (imChatSession == *this) {
		IMContact * imContact = _imChat.getIMContactMap().findIMContact(_imChat.getIMAccount(), contactId);
		if (imContact) {
			_imContactList.erase(_imContactList.find(imContact));
			contactRemovedEvent(*this, *imContact);
		} else {
			LOG_ERROR("IMContact for " + contactId + " not in IMContactList");
		}
	}
}
