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

#ifndef IMCHAT_H
#define IMCHAT_H

#include <Interface.h>
#include <Event.h>

#include <string>

class IMAccount;

/**
 * Wrapper for Instant Messaging chat.
 *
 * @ingroup model
 * @author Tanguy Krotoff
 */
class IMChat : Interface {
public:

	/**
	 * Chat message received callback.
	 *
	 * @param sender this class
	 * @param from message sender
	 * @param message message received
	 */
	Event<void (IMChat & sender, const std::string & from, const std::string & message)> messageReceivedEvent;

	enum MessageStatus {
		/** Chat message has been received. */
		MessageStatusReceived,

		/** Chat message sending error. */
		MessageStatusError
	};

	/**
	 * Status changed.
	 *
	 * @param sender this class
	 * @param status new status
	 */
	Event<void (IMChat & sender, MessageStatus status, int messageId)> messageStatusEvent;

	virtual ~IMChat() { }

	/**
	 * Sends a chat message.
	 *
	 * @param contactId message receiver, something like "toto@hotmail.com"
	 * @param message message to send
	 * @return message id
	 */
	virtual int sendMessage(const std::string & contactId, const std::string & message) = 0;

protected:

	IMChat(IMAccount & account) : _account(account) {}

	IMAccount & _account;
};

#endif	//IMCHAT_H
