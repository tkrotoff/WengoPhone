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

#include <model/chat/ChatHandler.h>

#include <imwrapper/IMConnect.h>
#include <imwrapper/EnumIMProtocol.h>
#include <imwrapper/EnumPresenceState.h>
#include <imwrapper/IMPresence.h>

#include <util/NonCopyable.h>
#include <util/Event.h>

#include <map>

class UserProfile;
class ContactList;
class Connect;
class Presence;
class IMAccount;
class IMContact;

/**
 *
 * @ingroup model
 * @author Tanguy Krotoff
 * @author Philippe Bernery
 */
class PresenceHandler : NonCopyable {
public:

	/**
	 * @see IMPresence::presenceStateChangedEvent
	 * @param imContact the IMContact for wich the state has changed
	 */
	Event<void (PresenceHandler & sender, EnumPresenceState::PresenceState state, 
		const std::string & alias, const IMContact & imContact)> presenceStateChangedEvent;

	/**
	 * @see IMPresence::presenceStatusEvent
	 */
	Event<void (PresenceHandler & sender, const IMAccount & imAccount, 
		EnumPresenceState::MyPresenceStatus status)> myPresenceStatusEvent;

	/**
	 * @see IMPresence::subscribeStatusEvent
	 */
	Event<void (PresenceHandler & sender, const IMContact & imContact,
		IMPresence::SubscribeStatus status)> subscribeStatusEvent;

	/**
	 * @see IMPresence::subscribeStatusEvent
	 */
	Event<void (PresenceHandler & sender, const IMContact & imContact,
		const std::string & message)> authorizationRequestEvent;


	PresenceHandler(UserProfile & userProfile);

	~PresenceHandler();

	/**
	 * Change my presence on desired protocol.
	 *
	 * @param state my state
	 * @param note used by PresenceStateUserDefined
	 * @param imAccount the IMAccount that we want the presence changed. If NULL
	 * presence state of all accounts is changed.
	 */
	void changeMyPresenceState(EnumPresenceState::PresenceState state,	
		const std::string & note, IMAccount * imAccount = NULL);

	/**
	 * Change my alias.
	 *
	 * @param alias the desired alias
	 * @param imAccount the IMAccount that we want the alias changed. If NULL
	 * alias of all accounts is changed.
	 */
	void changeMyAlias(const std::string & alias, IMAccount * imAccount = NULL);

	/**
	 * Change my icon.
	 *
	 * @param picture the desired icon
	 * @param imAccount the IMAccount that we want the icon changed. If NULL
	 * icon of all accounts is changed.
	 */
	void changeMyIcon(const Picture & picture, IMAccount * imAccount = NULL);

	/**
	 * Gets the icon of the given IMContact.
	 *
	 * If no icon has been set, an empty Picture is returned.
	 *
	 * @param imContact the IMContact to get the icon of
	 * @return the icon
	 */
	Picture getContactIcon(const IMContact & imContact);

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

	/**
	 * @see IMPresence::authorizeContact
	 */
	void authorizeContact(const IMContact & imContact, bool authorized, 
		const std::string message);

private:

	typedef std::map<IMAccount *, Presence *> PresenceMap;

	typedef	std::multimap<IMAccount *, const IMContact *> IMContactMultiMap;

	/**
	 * @see ConnectHandler::connectedEvent
	 */
	void connectedEventHandler(ConnectHandler & sender, IMAccount & account);

	/**
	 * @see ConnectHandler::disconnectedEvent
	 */
	void disconnectedEventHandler(ConnectHandler & sender, IMAccount & account);

	/**
	 * @see IMPresence::presenceStateChangedEvent
	 */
	void presenceStateChangedEventHandler(IMPresence & sender, EnumPresenceState::PresenceState state,
		const std::string & note, const std::string & from);

	/**
	 * @see IMPresence::myPresenceStatusEvent
	 */
	void myPresenceStatusEventHandler(IMPresence & sender, EnumPresenceState::MyPresenceStatus status);

	/**
	 * @see IMPresence::subscribeStatusEvent
	 */
	void subscribeStatusEventHandler(IMPresence & sender, const std::string & contactId, IMPresence::SubscribeStatus status);
	
	/**
	 * @see IMPresence::authorizationRequestEvent
	 */
	void authorizationRequestEventHandler(IMPresence & sender, const std::string & contactId, const std::string & message);

	/**
	 * @see UserProfile::newIMAccountAddedEvent
	 */
	void newIMAccountAddedEventHandler(UserProfile & sender, IMAccount & imAccount);

	/**
	 * @see UserProfile::imAccountRemovedEvent
	 */
	void imAccountRemovedEventHandler(UserProfile & sender, IMAccount & imAccount);

	/**
	 * Find the Presence related to the given protocol.
	 *
	 * @param presenceMap the PresenceMap to search in
	 * @param protocol the protocol
	 * @return an iterator to the desired Presence or 'end' of the given PresenceMap
	 */
	static PresenceMap::iterator findPresence(PresenceMap & presenceMap, IMAccount * imAccount);

	PresenceMap _presenceMap;

	IMContactMultiMap _pendingSubscriptions;

};

#endif	//PRESENCEHANDLER_H
