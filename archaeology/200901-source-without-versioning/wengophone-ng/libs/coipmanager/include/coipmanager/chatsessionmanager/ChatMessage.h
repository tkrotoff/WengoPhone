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

#ifndef OWCHATMESSAGE_H
#define OWCHATMESSAGE_H

#include <coipmanager/coipmanagerdll.h>

#include <coipmanager_base/peer/Peer.h>

#include <util/Date.h>
#include <util/Time.h>

#include <list>

/**
 * Represents a chat message.
 *
 * @author Philippe Bernery
 * @author Tanguy Krotoff
 */
class ChatMessage {
public:

	COIPMANAGER_API ChatMessage(const Peer & peer, const std::string & message);

	COIPMANAGER_API ChatMessage(const ChatMessage & chatMessage);

	COIPMANAGER_API ~ChatMessage();

	COIPMANAGER_API const Date & getDate() const;

	COIPMANAGER_API const Time & getTime() const;

	COIPMANAGER_API const Peer & getPeer() const;

	COIPMANAGER_API const std::string & getMessage() const;

private:

	/** Author of the message. */
	Peer _peer;

	/** Message text. */
	std::string _message;

	/** Date of message creation. */
	Date _date;

	/** Time of message creation. */
	Time _time;
};

typedef std::list<ChatMessage> ChatMessageList;

#endif	//OWCHATMESSAGE_H
