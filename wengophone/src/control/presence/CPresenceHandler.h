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

#include <EnumIMProtocol.h>
#include <EnumPresenceState.h>
#include <IMPresence.h>
#include <NonCopyable.h>
#include <Event.h>

class PresenceHandler;
class PPresenceHandler;
class IMContact;

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
	 * @param imAccount the IMAccount that posses
	 */
	Event<void (PresenceHandler & sender, EnumPresenceState::PresenceState state, 
		const std::string & note, const IMAccount & imAccount, const std::string & from)> presenceStateChangedEvent;

	/**
	 * @see IMPresence::presenceStatusEvent
	 */
	Event<void (PresenceHandler & sender, const IMAccount & imAccount, 
		EnumPresenceState::MyPresenceStatus status)> myPresenceStatusEvent;

	/**
	 * @see IMPresence::subscribeStatusEvent
	 * @param protocol protocol of the contact
	 */
	Event<void (PresenceHandler & sender, const IMAccount & imAccount, 
		const std::string & contactId, IMPresence::SubscribeStatus status)> subscribeStatusEvent;

	/**
	 * Change my presence on desired protocol.
	 *
	 * @param state my state
	 * @param note used by PresenceStateUserDefined
	 * @param imAccount the IMAccount that we want the presence changed
	 */
	void changeMyPresence(EnumPresenceState::PresenceState state,	
		const std::string & note, IMAccount * imAccount = NULL);

	/**
	 * Subscribe to presence of a IMContact.
	 *
	 * @param imContact the IMContact
	 */
	void subscribeToPresenceOf(const IMContact & imContact);

	/**
	 * @see IMPresence::blockContact
	 */
	void blockContact(const IMContact & imContact);

	/**
	 * @see IMPresence::unblockContact
	 */
	void unblockContact(const IMContact & imContact);

private:

	PresenceHandler & _presenceHandler;

	PPresenceHandler * _pPresenceHandler;

};

#endif	//CPRESENCEHANDLER_H
