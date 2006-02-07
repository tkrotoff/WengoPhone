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

#include <imwrapper/IMChatSession.h>

#include <imwrapper/IMContact.h>

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

void IMChatSession::messageReceivedEventHandler(IMChat & sender, IMChatSession & imChatSession, const std::string & contactId, const std::string & message) {
	LOG_DEBUG("message received: " + message);

	if (imChatSession == *this) {
		IMContact imContact(_imChat.getIMAccount(), contactId);
		if (_imContactList.find(imContact) != _imContactList.end()) {
			messageReceivedEvent(*this, *_imContactList.find(imContact), message);
		} else {
			LOG_ERROR("this session does not know " + contactId);
		}
	}
}

void IMChatSession::statusMessageReceivedEventHandler(IMChat & sender, IMChatSession & imChatSession, IMChat::StatusMessage status, const std::string & message) {
	LOG_DEBUG("status message received: " + message);

	if (imChatSession == *this) {
		statusMessageReceivedEvent(*this, status, message);
	}
}

bool IMChatSession::operator==(const IMChatSession & imChatSession) const {
	return (getId() == imChatSession.getId());
}

int IMChatSession::getId() const {
	return (long)this;
}

void IMChatSession::contactAddedEventHandler(IMChat & sender, IMChatSession & imChatSession, const std::string & contactId) {
	if (imChatSession == *this) {
		IMContact imContact(_imChat.getIMAccount(), contactId);
		if (_imContactList.find(imContact) != _imContactList.end()) {
			LOG_ERROR("IMContact for " + contactId + " already in IMContactList");
		} else {
			_imContactList.insert(imContact);
			LOG_DEBUG("IMContact " + contactId + " added to IMContactList");

			//Takes the one from _imContactList rather than imContact directly
			contactAddedEvent(*this, *_imContactList.find(imContact));
		}
	}
}

void IMChatSession::contactRemovedEventHandler(IMChat & sender, IMChatSession & imChatSession, const std::string & contactId) {
	if (imChatSession == *this) {
		IMContact imContact(_imChat.getIMAccount(), contactId);
		if (_imContactList.find(imContact) == _imContactList.end()) {
			LOG_ERROR("IMContact for " + contactId + " not in IMContactList");
		} else {
			contactRemovedEvent(*this, *_imContactList.find(imContact));
			_imContactList.erase(_imContactList.find(imContact));
			LOG_DEBUG("IMContact " + contactId + " removed from IMContactList");
		}
	}
}
