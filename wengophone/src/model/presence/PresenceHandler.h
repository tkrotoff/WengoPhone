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

#ifndef PRESENCEHANDLER_H
#define PRESENCEHANDLER_H

#include <model/ChatHandler.h>
#include <model/imwrapper/IMConnect.h>
#include <model/imwrapper/EnumIMProtocol.h>
#include <model/imwrapper/EnumPresenceState.h>
#include <model/imwrapper/IMPresence.h>

#include <NonCopyable.h>
#include <Event.h>

#include <map>

class WengoPhone;
class ContactList;
class Connect;
class Presence;
class IMAccount;

/**
 *
 * @ingroup model
 * @author Tanguy Krotoff
 * @author Philippe Bernery
 */
class PresenceHandler : NonCopyable {
public:

	static PresenceHandler & getInstance() {
		static PresenceHandler presenceHandler;
		return presenceHandler;
	}

	/**
	 * @see IMPresence::presenceStateChangedEvent
	 * @param protocol protocol of the contact
	 */
	Event<void (IMPresence & sender, EnumPresenceState::PresenceState state, 
		const std::string & note, const std::string & from)> presenceStateChangedEvent;

	/**
	 * @see IMPresence::presenceStatusEvent
	 */
	Event<void (IMPresence & sender, EnumPresenceState::MyPresenceStatus status)> myPresenceStatusEvent;

	/**
	 * @see IMPresence::subscribeStatusEvent
	 * @param protocol protocol of the contact
	 */
	Event<void (IMPresence & sender, const std::string & contactId, IMPresence::SubscribeStatus status)> subscribeStatusEvent;

	/**
	 * Change my presence on desired protocol.
	 *
	 * @param state my state
	 * @param note used by PresenceStateUserDefined
	 * @param protocol desired protocol. All by default
	 */
	void changeMyPresence(EnumPresenceState::PresenceState state,	
		const std::string & note, EnumIMProtocol::IMProtocol protocol = EnumIMProtocol::IMProtocolAll);

	/**
	 * Subscribe to presence of a IMContact.
	 *
	 * @param imContact the IMContact
	 */
	void subscribeToPresenceOf(EnumIMProtocol::IMProtocol protocol, const std::string & contactId);

	/**
	 * @see IMPresence::blockContact
	 */
	void blockContact(EnumIMProtocol::IMProtocol protocol, const std::string & contactId);

	/**
	 * @see IMPresence::unblockContact
	 */
	void unblockContact(EnumIMProtocol::IMProtocol protocol, const std::string & contactId);

	void connected(IMAccount & account);

	void disconnected(IMAccount & account);

private:

	typedef std::map<IMAccount *, Presence *> PresenceMap;

	typedef	std::multimap<EnumIMProtocol::IMProtocol, const std::string> ContactIDMultiMap;

	PresenceHandler();

	~PresenceHandler();

	/**
	 * Find the Presence related to the given protocol.
	 *
	 * @param presenceMap the PresenceMap to search in
	 * @param protocol the protocol
	 * @return an iterator to the desired Presence or 'end' of the given PresenceMap
	 */
	static PresenceMap::iterator findPresence(PresenceMap & presenceMap, EnumIMProtocol::IMProtocol protocol);

	PresenceMap _presenceMap;

	ContactIDMultiMap _pendingSubscriptions;

};

#endif	//PRESENCEHANDLER_H
