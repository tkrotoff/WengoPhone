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

#include "IMChatSession.h"

#include "IMContact.h"
#include "IMChatMap.h"

#include <Logger.h>

using namespace std;

IMChatSession::IMChatSession(IMChatMap & imChatMap) 
	: _imChatMap(imChatMap) {	
}

IMChatSession::~IMChatSession() {

}

void IMChatSession::addIMContact(const IMContact & imContact) {
	const IMAccount & account = imContact.getIMAccount();
	IMChatMap::const_iterator it = _imChatMap.find(&(IMAccount &)account);

	if (it != _imChatMap.end()) {
		(*it).second->messageReceivedEvent += 
			boost::bind(&IMChatSession::messageReceivedEventHandler, this, _1, _2, _3, _4);
		(*it).second->statusMessageEvent += 
			boost::bind(&IMChatSession::statusMessageEventHandler, this, _1, _2, _3, _4);
		(*it).second->createSession(*this);

		_imContactList.insert(&imContact);
		(*it).second->addContact(*this, imContact.getContactId());

		updateIMChatList();

	} else {
		LOG_ERROR("this IMContact is owned by an IMAccount that is not connected yet");
	}
}

void IMChatSession::removeIMContact(const IMContact & imContact) {
	const IMAccount & account = imContact.getIMAccount();
	IMChatMap::const_iterator it = _imChatMap.find(&(IMAccount &)account);

	if (it != _imChatMap.end()) {
		_imContactList.erase(_imContactList.find(&imContact));
		(*it).second->removeContact(*this, imContact.getContactId());

		updateIMChatList();

	} else {
		LOG_ERROR("this IMContact is not in this IMChatSession");	
	}
}

void IMChatSession::sendMessage(const std::string & message) {
	for (IMChatSet::const_iterator it = _imChatSet.begin() ; it != _imChatSet.end() ; it++) {
		(*it)->sendMessage(*this, message);
	}
}

const IMChatSession::IMContactList & IMChatSession::getIMContactList() const {
	return _imContactList;
}

void IMChatSession::messageReceivedEventHandler(IMChat & sender, IMChatSession * imChatSession, const std::string & from, const std::string & message) {
	LOG_DEBUG("message received: " + message);

	if (*imChatSession == *this) {
		const IMContact * imContact = getIMContact(sender.getIMAccount(), from);
		if (imContact) {
			messageReceivedEvent(*this, *imContact, message);
		} else {
			LOG_ERROR("this session does not know " + from);
		}
	}
}

void IMChatSession::statusMessageEventHandler(IMChat & sender, IMChatSession & imChatSession, IMChat::StatusMessage status, const std::string & message) {
	LOG_DEBUG("status message received: " + message);

	if (imChatSession == *this) {
		statusMessageEvent(*this, status, message);
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
	return (int)this;
}

void IMChatSession::updateIMChatList() {
	_imChatSet.clear();

	for (IMContactList::const_iterator it = _imContactList.begin() ; it != _imContactList.end() ; it++) {
		const IMAccount & account = (*it)->getIMAccount();
		IMChatMap::const_iterator imChatIt = _imChatMap.find(&(IMAccount &)account);

		_imChatSet.insert((*imChatIt).second);
	}
}
