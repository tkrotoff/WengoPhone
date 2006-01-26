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

#ifndef PPRESENCEHANDLER_H
#define PPRESENCEHANDLER_H

#include "Presentation.h"

#include <model/imwrapper/EnumIMProtocol.h>
#include <model/imwrapper/EnumPresenceState.h>
#include <model/imwrapper/IMPresence.h>

class CPresenceHandler;

/**
 *
 * @ingroup presentation
 * @author Tanguy Krotoff
 * @author Philippe Bernery
 */
class PPresenceHandler : public Presentation {
public:
	/**
	 * @see PresenceHandler::changeMyPresence
	 */
	virtual void changeMyPresence(EnumPresenceState::PresenceState state,	
		const std::string & note, EnumIMProtocol::IMProtocol protocol = EnumIMProtocol::IMProtocolAll) = 0;

	/**
	 * @see PresenceHandler::subscribeToPresenceOf
	 */
	virtual void subscribeToPresenceOf(EnumIMProtocol::IMProtocol protocol, const std::string & contactId) = 0;

	/**
	 * @see PresenceHandler::blockContact
	 */
	virtual void blockContact(EnumIMProtocol::IMProtocol protocol, const std::string & contactId) = 0;

	/**
	 * @see PresenceHandler::unblockContact
	 */
	virtual void unblockContact(EnumIMProtocol::IMProtocol protocol, const std::string & contactId) = 0;
	
private:

	/**
	 * @see IMPresence::presenceStateChangedEvent
	 */
	virtual void presenceStateChangedEventHandler(IMPresence & sender, EnumPresenceState::PresenceState state, 
		const std::string & note, const std::string & from) = 0;

	/**
	 * @see IMPresence::presenceStatusEvent
	 */
	virtual void myPresenceStatusEventHandler(IMPresence & sender, EnumPresenceState::MyPresenceStatus status) = 0;

	/**
	 * @see IMPresence::subscribeStatusEvent
	 */
	virtual void subscribeStatusEventHandler(IMPresence & sender, const std::string & contactId, IMPresence::SubscribeStatus status) = 0;

};

#endif //PPRESENCEHANDLER_H
