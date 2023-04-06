/*
 * WengoPhone, a voice over Internet phone
 * Copyright (C) 2004-2007  Wengo
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

#include <coipmanager/chatsessionmanager/ChatSession.h>

#include <coipmanager/CoIpManager.h>
#include <coipmanager/chatsessionmanager/ChatSessionManager.h>
#include <coipmanager/chatsessionmanager/IChatSessionManagerPlugin.h>
#include <coipmanager/chatsessionmanager/IChatSessionPlugin.h>
#include <coipmanager/datamanager/UserProfileManager.h>

#include <util/Logger.h>
#include <util/SafeConnect.h>
#include <util/SafeDelete.h>

ChatSession::ChatSession(CoIpManager & coIpManager)
	: Session(coIpManager) {

	_currentIChatSession = NULL;
}

ChatSession::ChatSession(CoIpManager & coIpManager,
	IChatSessionPlugin * iChatSessionPlugin)
	: Session(coIpManager) {

	_currentIChatSession = iChatSessionPlugin;

	SAFE_CONNECT(_currentIChatSession, SIGNAL(messageReceivedSignal(IMContact, std::string)),
			SLOT(messageReceivedSlot(IMContact, std::string)));

	SAFE_CONNECT(_currentIChatSession, SIGNAL(typingStateChangedSignal(IMContact, EnumChatTypingState::ChatTypingState)),
			SLOT(typingStateChangedSlot(IMContact, EnumChatTypingState::ChatTypingState)));
}

ChatSession::~ChatSession() {
	OWSAFE_DELETE(_currentIChatSession);
}

void ChatSession::start() {
	QMutexLocker lock(_mutex);

	// If a IChatSession is running, do nothing
	if (_currentIChatSession) {
		LOG_ERROR("ChatSession already running");
		return;
	}

	AccountList accountList = _coIpManager.getUserProfileManager().getCopyOfUserProfile().getAccountList();
	Account * account = AccountListHelper::getCopyOfAccount(accountList, _accountToUse);
	if (account) {
		IChatSessionManagerPlugin *iChatSessionManager =
			_coIpManager.getChatSessionManager().getICoIpManagerPlugin(*account, _contactList);
		if (iChatSessionManager) {
			_currentIChatSession = iChatSessionManager->createIChatSessionPlugin();
			IMContactList imContactList = iChatSessionManager->getValidIMContacts(*account, _contactList);
			_currentIChatSession->setIMContactList(imContactList);
			_currentIChatSession->setAccount(*account);

			SAFE_CONNECT(_currentIChatSession,
				SIGNAL(messageReceivedSignal(IMContact, std::string)),
				SLOT(messageReceivedSlot(IMContact, std::string)));

			SAFE_CONNECT(_currentIChatSession,
				SIGNAL(statusMessageReceivedSignal(EnumChatStatusMessage::ChatStatusMessage, std::string)),
				SIGNAL(statusMessageReceivedSignal(EnumChatStatusMessage::ChatStatusMessage, std::string)));

			SAFE_CONNECT(_currentIChatSession,
				SIGNAL(typingStateChangedSignal(IMContact, EnumChatTypingState::ChatTypingState)),
				SLOT(typingStateChangedSlot(IMContact, EnumChatTypingState::ChatTypingState)));

			_currentIChatSession->start();
		}
		OWSAFE_DELETE(account);
	} else {
		LOG_ERROR("couldn't find a common Account");
	}
}

void ChatSession::stop() {
	QMutexLocker lock(_mutex);

	if (_currentIChatSession) {
		_currentIChatSession->stop();
	} else {
		LOG_ERROR("session not started");
	}
}

void ChatSession::pause() {
	QMutexLocker lock(_mutex);

	if (_currentIChatSession) {
		_currentIChatSession->pause();
	} else {
		LOG_ERROR("session not started");
	}
}

void ChatSession::resume() {
	QMutexLocker lock(_mutex);

	if (_currentIChatSession) {
		_currentIChatSession->resume();
	} else {
		LOG_ERROR("session not started");
	}
}

bool ChatSession::canCreateISession(const Account & account, const ContactList & contactList) const {
	return (_coIpManager.getChatSessionManager().getICoIpManagerPlugin(account, contactList) != NULL);
}

ChatMessageList ChatSession::getMessageHistory() const {
	QMutexLocker lock(_mutex);

	ChatMessageList result = _chatMessageList;

	return result;
}

void ChatSession::sendMessage(const std::string & message) {
	QMutexLocker lock(_mutex);

	if (_currentIChatSession) {
		_currentIChatSession->sendMessage(message);

		ChatMessage chatMessage(*_currentIChatSession->getAccount().getPrivateAccount(), message);
		_chatMessageList.push_back(chatMessage);

		messageAddedSignal();
	} else {
		LOG_ERROR("ChatSession not started");
	}
}

void ChatSession::setTypingState(EnumChatTypingState::ChatTypingState state) {
	QMutexLocker lock(_mutex);

	if (_currentIChatSession) {
		_currentIChatSession->setTypingState(state);
	} else {
		LOG_ERROR("ChatSession not started");
	}
}

EnumChatTypingState::ChatTypingState ChatSession::getTypingState(const Contact & contact) const {
	//TODO
	return EnumChatTypingState::ChatTypingStateNotTyping;
}

void ChatSession::messageReceivedSlot(IMContact imContact, std::string message) {
	QMutexLocker lock(_mutex);

	_chatMessageList.push_back(ChatMessage(imContact, message));

	messageAddedSignal();
}

void ChatSession::typingStateChangedSlot(IMContact imContact, EnumChatTypingState::ChatTypingState state) {
	LOG_DEBUG("typing state changed");
	typingStateChangedSignal(getContactThatOwns(imContact), state);
}
