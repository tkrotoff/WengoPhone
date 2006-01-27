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

#ifndef CHATHANDLER_H
#define CHATHANDLER_H

#include <model/imwrapper/IMChat.h>

#include <NonCopyable.h>
#include <Event.h>

#include <map>

class IMAccount;
class IMChatSession;
class Chat;

/**
 *
 * @ingroup model
 * @author Tanguy Krotoff
 * @author Philippe Bernery
 */
class ChatHandler : NonCopyable {
public:

	ChatHandler();

	~ChatHandler();

	/**
	 * Create a new session.
	 *
	 * @param imAccount IMAccount that will be associated with the new session
	 * @return a new IMChatSession or NULL if IMAccount does not exist
	 */
	IMChatSession * createSession(const IMAccount & imAccount);

	void connected(IMAccount & account);

	void disconnected(IMAccount & account);

private:

	typedef std::map<IMAccount *, Chat *> ChatMap;

	/**
	 * Find the Chat related to the given protocol.
	 *
	 * @param chatMap the ChatMap to search in
	 * @param protocol the protocol
	 * @return an iterator to the desired Chat or 'end' of the given ChatMap
	 */
	static ChatMap::iterator findChat(ChatMap & chatMap, IMAccount & imAccount);

	ChatMap _chatMap;

};

#endif	//CHATHANDLER_H
