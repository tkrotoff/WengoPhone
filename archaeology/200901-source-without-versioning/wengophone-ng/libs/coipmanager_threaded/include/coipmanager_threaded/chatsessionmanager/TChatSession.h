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

#ifndef OWTCHATSESSION_H
#define OWTCHATSESSION_H

#include <coipmanager_threaded/session/TSession.h>

#include <coipmanager/chatsessionmanager/ChatSession.h>
#include <coipmanager/chatsessionmanager/EnumChatStatusMessage.h>

#include <coipmanager_base/EnumChatTypingState.h>

class TCoIpManager;

/**
 * Threaded ChatSession.
 *
 * @see ChatSession
 * @ingroup TCoIpManager
 * @author Philippe Bernery
 * @author Tanguy Krotoff
 */
class COIPMANAGER_THREADED_API TChatSession : public TSession {
	Q_OBJECT
	friend class TChatSessionManager;
public:

	~TChatSession();

	/**
	 * @see ChatSession::getMessageHistory()
	 */
	ChatMessageList getMessageHistory() const;

	/**
	 * @see ChatSession::sendMessage()
	 */
	void sendMessage(const std::string & message);

	/**
	 * @see ChatSession::setTypingState()
	 */
	void setTypingState(EnumChatTypingState::ChatTypingState state);

	/**
	 * @see ChatSession::getTypingState()
	 */
	EnumChatTypingState::ChatTypingState getTypingState(const Contact & contact) const;

Q_SIGNALS:

	/**
	 * @see ChatSession::messageAddedSignal()
	 */
	void messageAddedSignal();

	/**
	 * @see ChatSession::statusMessageReceivedSignal()
	 */
	void statusMessageReceivedSignal(EnumChatStatusMessage::ChatStatusMessage status, std::string message);

	/**
	 * @see ChatSession::typingStateChangedSignal()
	 */
	void typingStateChangedSignal(Contact contact, EnumChatTypingState::ChatTypingState state);

private:

	/**
	 * Constructor for TChatSession created from outside.
	 */
	TChatSession(TCoIpManager & tCoIpManager, ChatSession * chatSession);

	/**
	 * Code factorization.
	 */
	ChatSession * getChatSession() const;
};

#endif	//OWTCHATSESSION_H
