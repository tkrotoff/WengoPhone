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

#ifndef OWSIPCHATWRAPPER_H
#define OWSIPCHATWRAPPER_H

#include <sipwrapper/sipwrapperdll.h>

#include <coipmanager_base/EnumChatTypingState.h>

#include <QtCore/QObject>

#include <string>

/**
 * Chat part of SipWrapper.
 *
 * @author Philippe Bernery
 */
class SIPWRAPPER_API SipChatWrapper : public QObject {
	Q_OBJECT
public:

	/**
	 * Sends a message to contactId.
	 *
	 * @param lineId virtual line to use to send the message.
	 * @param contactId contact to send the message to (e.g: "sip:contact@voip.wengo.fr")
	 * @param msg message to send
	 */
	virtual void sendMessage(int lineId, const std::string &contactId, const std::string &msg) = 0;

	/**
	 * Sends typing state.
	 *
	 * @param lineId virtual line to use to send the message.
	 * @param contactId contact to send the typing state to (e.g: "sip:contact@voip.wengo.fr")
	 * @param typingState the typing state
	 */
	virtual void sendTypingState(int lineId, const std::string &contactId, EnumChatTypingState::ChatTypingState typingState) = 0;

	/**
	 * Emits the chatMessageReceivedSignal signal.
	 */
	void emitChatMessageReceivedSignal(std::string contactIdReceiver, std::string contactIdSender,
		std::string message);

	/**
	 * Emits the chatTypingStateChangedSignal signal.
	 */
	void emitChatTypingStateChangedSignal(std::string contactIdReceiver, std::string contactIdSender,
		EnumChatTypingState::ChatTypingState state);

Q_SIGNALS:

	/**
	 * Chat message received.
	 *
	 * @param contactIdReceiver id of the recipient (e.g: "sip:identity@realm")
	 * @param contactIdSender Id of the contact that send the message (e.g: "sip:identity@realm")
	 * @param message sent message
	 */
	void chatMessageReceivedSignal(std::string contactIdReceiver, std::string contactIdSender,
		std::string message);

	/**
	 * Chat typing state changed.
	 *
	 * @param contactIdReceiver id of the recipient (e.g: "sip:identity@realm")
	 * @param contactIdSender Id of the contact that send the message (e.g: "sip:identity@realm")
	 * @param state TypingState
	 */
	void chatTypingStateChangedSignal(std::string contactIdReceiver, std::string contactIdSender,
		EnumChatTypingState::ChatTypingState state);

};

#endif //OWSIPCHATWRAPPER_H
