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

#include <chatsessionmanager/ChatSession.h>

#include <model/contactlist/Contact.h>
#include <model/profile/UserProfile.h>

#include <util/Logger.h>
#include <util/Macro.h>

ChatSession::ChatSession(ChatSessionManager & chatSessionManager, UserProfile & userProfile)
	: Session(userProfile), _chatSessionManager(chatSessionManager) {

	_currentIChatSession = NULL;
}

ChatSession::ChatSession(ChatSessionManager & chatSessionManager, UserProfile & userProfile, 
	IChatSession * iChatSessionImp)
	: Session(userProfile), _chatSessionManager(chatSessionManager) {

	_currentIChatSession = iChatSessionImp;
}

ChatSession::~ChatSession() {
	SAFE_DELETE(_currentIChatSession);
}

void ChatSession::start() {
	// If a IChatSession is running, do nothing
	if (_currentIChatSession) {
		return;
	}

	/*
	Account * account = findCommonAccount();
	_currentIChatSession = _chatSessionManager.createIChatSessionForAccount(*account);
	if (_currentIChatSession) {
		IMContactSet imContactSet = getCompatibleIMContacts(imAccount);
		_currentIChatSession->setContactList(imContactSet);
		_currentIChatSession->start();
	}
	*/
}

void ChatSession::stop() {
	if (_currentIChatSession) {
		_currentIChatSession->stop();
	}
}

void ChatSession::pause() {
	if (_currentIChatSession) {
		_currentIChatSession->pause();
	}
}

void ChatSession::resume() {
	if (_currentIChatSession) {
		_currentIChatSession->resume();
	}
}

void ChatSession::addContact(const std::string & contactId) {
	StringList contactIdList = _contactIdList;
	contactIdList += contactId;

	/*
	Account * account = findCommonAccount(contactIdList);
	if (account) {
		Session::addContact(contactId);
	
		if (_currentIChatSession) {
			// Check if the found IMAccount is the same as the one in the 
			// current IChatSession
			if ((*account) == _currentIChatSession->getAccount()) {
				_userProfile.getContactList().lock();
				Contact * contact = _userProfile.getContactList().getContact(contactId);
				if (contact) {
					const IMContact * imContact = contact->getFirstValidIMContact(_currentIChatSession->getAccount());
					if (imContact) {
						_currentIChatSession->addContact(*imContact);
					}
				}
				_userProfile.getContactList().unlock();
			}
		} else {
			SAFE_DELETE(_currentIChatSession);
			start();
		}
	} else {
		LOG_ERROR("No common IMAccount found. Add of Contact refused");
	}
	*/
}

void ChatSession::removeContact(const std::string & contactId) {
	/*
	if (_currentIChatSession) {
		_userProfile.getContactList().lock();
		Contact * contact = _userProfile.getContactList().getContact(contactId);
		if (contact) {
			const IMContact * imContact = contact->getFirstValidIMContact(_currentIChatSession->getAccount());
			if (imContact) {
				_currentIChatSession->removeContact(*imContact);
			}
		}
		_userProfile.getContactList().unlock();
	}
	*/

	Session::removeContact(contactId);
}

void ChatSession::setContactList(const StringList & contactList) {
	/*
	Account * account = findCommonAccount(contactIdList);
	if (account) {
		Session::setContactList(contactList);

		if (_currentIChatSession) {
			SAFE_DELETE(_currentIChatSession);
			start();
		}
	} else {
		LOG_ERROR("No common IMAccount found. Set of ContactList refused");
	}
	*/
}

void ChatSession::sendMessage(const std::string & message) {
	if (_currentIChatSession) {
		_currentIChatSession->sendMessage(message);
	} else {
		LOG_ERROR("No IChatSession running");
	}
}

void ChatSession::setTypingState(IChatSession::TypingState state) {
	if (_currentIChatSession) {
		_currentIChatSession->setTypingState(state);
	} else {
		LOG_ERROR("No IChatSession running");
	}
}
