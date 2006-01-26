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

#ifndef CPRESENCEHANDLER_H
#define CPRESENCEHANDLER_H

#include <model/imwrapper/EnumIMProtocol.h>
#include <model/imwrapper/EnumPresenceState.h>
#include <model/imwrapper/IMPresence.h>

#include <NonCopyable.h>
#include <Event.h>

class PresenceHandler;
class PPresenceHandler;

/**
 *
 * @ingroup control
 * @author Tanguy Krotoff
 * @author Philippe Bernery
 */
class CPresenceHandler : NonCopyable {
public:
	CPresenceHandler(PresenceHandler & presenceHandler);

	/**
	 * @see IMPresence::presenceStateChangedEvent
	 */
	Event<void (IMPresence & sender, EnumPresenceState::PresenceState state, 
		const std::string & note, const std::string & from)> presenceStateChangedEvent;

	/**
	 * @see IMPresence::presenceStatusEvent
	 */
	Event<void (IMPresence & sender, EnumPresenceState::MyPresenceStatus status)> myPresenceStatusEvent;

	/**
	 * @see IMPresence::subscribeStatusEvent
	 */
	Event<void (IMPresence & sender, const std::string & contactId, IMPresence::SubscribeStatus status)> subscribeStatusEvent;

	/**
	 * @see PresenceHandler::changeMyPresence
	 */
	void changeMyPresence(EnumPresenceState::PresenceState state,	
		const std::string & note, EnumIMProtocol::IMProtocol protocol = EnumIMProtocol::IMProtocolAll);

	/**
	 * @see PresenceHandler::subscribeToPresenceOf
	 */
	void subscribeToPresenceOf(EnumIMProtocol::IMProtocol protocol, const std::string & contactId);

	/**
	 * @see PresenceHandler::blockContact
	 */
	void blockContact(EnumIMProtocol::IMProtocol protocol, const std::string & contactId);

	/**
	 * @see PresenceHandler::unblockContact
	 */
	void unblockContact(EnumIMProtocol::IMProtocol protocol, const std::string & contactId);

private:

	PresenceHandler & _presenceHandler;

	PPresenceHandler * _pPresenceHandler;

};

#endif	//CPRESENCEHANDLER_H
