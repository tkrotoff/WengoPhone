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

#include <imwrapper/EnumIMProtocol.h>
#include <imwrapper/IMContact.h>
#include <imwrapper/IMAccount.h>

#include <util/Logger.h>

using namespace std;

IMChatSession::IMChatMessage::IMChatMessage(const IMContact & imContact, const string & message)
	: _imContact(imContact),
	_message(message) {
}

IMChatSession::IMChatMessage::~IMChatMessage() {
}

IMChatSession::IMChatSession(IMChat & imChat, bool userCreated)
	: _imChat(imChat),
	_userCreated(userCreated) {

	_imChat.messageReceivedEvent +=
		boost::bind(&IMChatSession::messageReceivedEventHandler, this, _1, _2, _3, _4);
	_imChat.statusMessageReceivedEvent +=
		boost::bind(&IMChatSession::statusMessageReceivedEventHandler, this, _1, _2, _3, _4);
	_imChat.typingStateChangedEvent +=
		boost::bind(&IMChatSession::typingStateChangedEventHandler, this, _1, _2, _3, _4);
	_imChat.contactAddedEvent +=
		boost::bind(&IMChatSession::contactAddedEventHandler, this, _1, _2, _3);
	_imChat.contactRemovedEvent +=
		boost::bind(&IMChatSession::contactRemovedEventHandler, this, _1, _2, _3);
}

IMChatSession::~IMChatSession() {
	for (IMChatMessageList::const_iterator it = _receivedIMChatMessageList.begin();
		it != _receivedIMChatMessageList.end(); ++it) {
		delete (*it);
	}
}

void IMChatSession::close() {
	LOG_DEBUG("closing IMChatSession");
	imChatSessionWillDieEvent(*this);
	_imChat.closeSession(*this);
	delete this;
}

void IMChatSession::addIMContact(const IMContact & imContact) {
	Mutex::ScopedLock scopedLock(_mutex);

	//The IMContact must be of the same protocol of the IMAccount
	if (_imChat.getIMAccount() == *imContact.getIMAccount()) {
		LOG_DEBUG("adding a new IMContact=" + imContact.getContactId());
		_imChat.addContact(*this, imContact.getContactId());
	}
}

void IMChatSession::removeIMContact(const IMContact & imContact) {
	Mutex::ScopedLock scopedLock(_mutex);

	LOG_DEBUG("removing an IMContact=" + imContact.getContactId());
	_imChat.removeContact(*this, imContact.getContactId());
}

void IMChatSession::removeAllIMContact() {
	Mutex::ScopedLock scopedLock(_mutex);

	for (IMContactSet::const_iterator it = _imContactSet.begin(); it != _imContactSet.end(); ++it) {
		_imChat.removeContact(*this, (*it).getContactId());
	}
}

void IMChatSession::sendMessage(const std::string & message) {
	LOG_DEBUG("sending raw message=" + message);
	_imChat.sendMessage(*this, message);
	messageSentEvent(*this, message);
}

void IMChatSession::changeTypingState(IMChat::TypingState state) {
	std::string message;

	switch (state) {
		case IMChat::TypingStateTyping:
			message = "typing";
			break;

		case IMChat::TypingStateStopTyping:
			message = "stop typing";
			break;

		default:
			message = "not typing";
			break;
	}

	LOG_DEBUG("changing my typing state=" + message);
	_imChat.changeTypingState(*this, state);
}

void IMChatSession::messageReceivedEventHandler(IMChat & sender, IMChatSession & imChatSession, const std::string & contactId, const std::string & message) {
	LOG_DEBUG("message received=" + message);

	if (imChatSession == *this) {
		IMContact imContact(_imChat.getIMAccount(), contactId);
		if (_imContactSet.find(imContact) != _imContactSet.end()) {
			const IMContact & foundIMContact = *_imContactSet.find(imContact);
			_receivedIMChatMessageList.push_back(new IMChatMessage(foundIMContact, message));
			messageReceivedEvent(*this);
		} else {
			LOG_ERROR("this session does not know contact=" + contactId);
		}
	}
}

void IMChatSession::statusMessageReceivedEventHandler(IMChat & sender, IMChatSession & imChatSession, IMChat::StatusMessage status, const std::string & message) {
	LOG_DEBUG("status message received=" + message);

	if (imChatSession == *this) {
		statusMessageReceivedEvent(*this, status, message);
	}
}

void IMChatSession::typingStateChangedEventHandler(IMChat & sender, IMChatSession & imChatSession, const std::string & contactId, IMChat::TypingState state) {
	LOG_DEBUG("typing state changed=" + contactId);

	if (imChatSession == *this) {
		IMContact imContact(_imChat.getIMAccount(), contactId);
		if (_imContactSet.find(imContact) != _imContactSet.end()) {
			const IMContact & foundIMContact = *_imContactSet.find(imContact);
			typingStateChangedEvent(*this, foundIMContact, state);
		} else {
			LOG_ERROR("this session does not know contact=" + contactId);
		}
	}
}

bool IMChatSession::operator == (const IMChatSession & imChatSession) const {
	return (getId() == imChatSession.getId());
}

int IMChatSession::getId() const {
	return (long)this;
}

void IMChatSession::contactAddedEventHandler(IMChat & sender, IMChatSession & imChatSession, const IMContact & imContact) {
	Mutex::ScopedLock scopedLock(_mutex);

	if (imChatSession == *this) {
		contactAddedEvent(*this, *_imContactSet.find(imContact));
	}
}

void IMChatSession::contactRemovedEventHandler(IMChat & sender, IMChatSession & imChatSession, const std::string & contactId) {
	Mutex::ScopedLock scopedLock(_mutex);

	if (imChatSession == *this) {
		IMContact imContact(_imChat.getIMAccount(), contactId);
		if (_imContactSet.find(imContact) == _imContactSet.end()) {
			LOG_ERROR("IMContact for=" + contactId + " not in IMContactList");
		} else {
			contactRemovedEvent(*this, *_imContactSet.find(imContact));
			_imContactSet.erase(_imContactSet.find(imContact));
			LOG_DEBUG("IMContact=" + contactId + " removed from IMContactList");
		}
	}
}

bool IMChatSession::canDoMultiChat() const {
	EnumIMProtocol::IMProtocol proto = _imChat.getIMAccount().getProtocol();

	switch (proto) {
		case EnumIMProtocol::IMProtocolMSN:
		case EnumIMProtocol::IMProtocolYahoo:
		case EnumIMProtocol::IMProtocolAIMICQ:
			return true;

		default:
			return false;
	}
}

const IMChatSession::IMChatMessageList IMChatSession::getReceivedMessage(unsigned int fromIndex) {
	IMChatSession::IMChatMessageList result;

	if (_receivedIMChatMessageList.size() > 0) {
		if (fromIndex >= 0 && fromIndex < _receivedIMChatMessageList.size()) {
			for(unsigned int i = fromIndex; i < _receivedIMChatMessageList.size(); i++) {
				result.push_back(_receivedIMChatMessageList.at(i));
			}
		}
	}

	return result;
}

bool IMChatSession::isContactInSession(const IMContact & imContact) const {
	IMContactSet::iterator it;

	for (it = _imContactSet.begin(); it != _imContactSet.end(); it++) {
		
		if (imContact == (*it)) {
			return true;
		}
	}
	return false;
}
