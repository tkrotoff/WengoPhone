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

#ifndef PCHATHANDLER_H
#define PCHATHANDLER_H

#include <model/imwrapper/IMChat.h>
#include <presentation/Presentation.h>

#include <NonCopyable.h>
#include <Event.h>

/**
 *
 * @ingroup presentation
 * @author Tanguy Krotoff
 * @author Philippe Bernery
 */
class PChatHandler : public Presentation {
public:

	/**
	 * @see ChatHandler::createSession
	 */
	virtual int createSession(const IMAccount & imAccount) = 0;

	/**
	 * @see ChatHandler::closeSession
	 */
	virtual void closeSession(int session) = 0;

	/**
	 * @see ChatHandler::sendMessage
	 */
	virtual void sendMessage(int session, const std::string & message) = 0;

	/**
	 * @see ChatHandler::addContact
	 */
	virtual void addContact(int session, const std::string & contactId) = 0;

	/**
	 * @see ChatHandler::removeContact
	 */
	virtual void removeContact(int session, const std::string & contactId) = 0;

private:

	/**
	 * @see ChatHandler::messageReceivedEvent
	 */
	virtual void messageReceivedEventHandler(IMChat & sender, int session, const std::string & from, const std::string & message) = 0;

	/**
	 * @see ChatHandler::statusMessageEvent
	 */
	virtual void statusMessageEventHandler(IMChat & sender, int session, IMChat::StatusMessage status, const std::string & message) = 0;

};

#endif	//PCHATHANDLER_H
