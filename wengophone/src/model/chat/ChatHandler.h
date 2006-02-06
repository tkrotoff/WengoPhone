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
#include <model/imwrapper/IMChatMap.h>

#include <NonCopyable.h>
#include <Event.h>

#include <map>

class IMAccount;
class IMChatSession;
class ConnectHandler;
class WengoPhone;

/**
 *
 * @ingroup model
 * @author Tanguy Krotoff
 * @author Philippe Bernery
 */
class ChatHandler : NonCopyable {
public:

	ChatHandler(WengoPhone & wengoPhone);

	~ChatHandler();

	/**
	 * Emitted when a new IMChatSession has been created.
	 * 
	 * @param sender this class
	 * @param imChatSession the new IMChatSession
	 */
	Event<void (ChatHandler & sender, IMChatSession & imChatSession)> newIMChatSessionCreatedEvent;

	/**
	 * Create a new IMChatSession.
	 * The newChatSessionCreatedEvent is emitted when an IMChatSession is created
	 * 
	 * @param imAccount the IMAccount for which we want to create the new IMChatSession
	 */
	void createSession(const IMAccount & imAccount);

private:

	typedef std::vector<IMChatSession *> IMChatSessionList;

	void connectedEventHandler(ConnectHandler & sender, IMAccount & account);

	void disconnectedEventHandler(ConnectHandler & sender, IMAccount & account);

	void newIMChatSessionCreatedEventHandler(IMChat & sender, IMChatSession & imChatSession);

	IMChatMap _imChatMap;

	IMChatSessionList _imChatSessionList;

	WengoPhone & _wengoPhone;

};

#endif	//CHATHANDLER_H
