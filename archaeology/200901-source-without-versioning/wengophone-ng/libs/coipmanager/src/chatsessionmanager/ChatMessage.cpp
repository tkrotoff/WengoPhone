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

#include <coipmanager/chatsessionmanager/ChatMessage.h>

ChatMessage::ChatMessage(const Peer & peer, const std::string & message)
	: _peer(peer),
	_message(message) {
}

ChatMessage::ChatMessage(const ChatMessage & message)
	: _peer(message._peer),
	_message(message._message),
	_date(message._date),
	_time(message._time) {
}

ChatMessage::~ChatMessage() {
}

const Date & ChatMessage::getDate() const {
	return _date;
}

const Time & ChatMessage::getTime() const {
	return _time;
}

const Peer & ChatMessage::getPeer() const {
	return _peer;
}

const std::string & ChatMessage::getMessage() const {
	return _message;
}
