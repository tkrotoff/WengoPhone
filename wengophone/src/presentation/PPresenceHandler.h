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

#include <imwrapper/EnumIMProtocol.h>
#include <imwrapper/EnumPresenceState.h>
#include <imwrapper/IMPresence.h>

class PresenceHandler;
class IMContact;

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
		const std::string & note, IMAccount * imAccount = NULL) = 0;

	/**
	 * @see PresenceHandler::subscribeToPresenceOf
	 */
	virtual void subscribeToPresenceOf(const IMContact & imContact) = 0;

	/**
	 * @see PresenceHandler::blockContact
	 */
	virtual void blockContact(const IMContact & imContact) = 0;

	/**
	 * @see PresenceHandler::unblockContact
	 */
	virtual void unblockContact(const IMContact & imContact) = 0;
	
private:

	/**
	 * @see IMPresence::presenceStateChangedEvent
	 */
	virtual void presenceStateChangedEventHandler(PresenceHandler & sender, EnumPresenceState::PresenceState state, 
		const std::string & note, const IMAccount & imAccount, const std::string & from) = 0;

	/**
	 * @see IMPresence::presenceStatusEvent
	 */
	virtual void myPresenceStatusEventHandler(PresenceHandler & sender, const IMAccount & imAccount, 
		EnumPresenceState::MyPresenceStatus status) = 0;

	/**
	 * @see IMPresence::subscribeStatusEvent
	 */
	virtual void subscribeStatusEventHandler(PresenceHandler & sender, const IMAccount & imAccount, 
		const std::string & contactId, IMPresence::SubscribeStatus status) = 0;

};

#endif //PPRESENCEHANDLER_H
