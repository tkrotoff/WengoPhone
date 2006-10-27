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

#include <util/Event.h>
#include <util/NonCopyable.h>
#include <util/Trackable.h>

#include <list>
#include <map>

class Connect;
class ContactList;
class IMAccount;
class IMContact;
class Presence;
class Thread;
class UserProfile;

/**
 *
 * @ingroup model
 * @author Tanguy Krotoff
 * @author Philippe Bernery
 */
class PresenceHandler : NonCopyable, public Trackable {
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
	 * @see IMPresence::authorizationRequestEvent
	 */
	Event<void (PresenceHandler & sender, const IMContact & imContact,
		const std::string & message)> authorizationRequestEvent;

	/**
	 * @see IMPresence::contactIconChangedEvent
	 */
	Event< void (PresenceHandler & sender, const IMContact & imContact,
		OWPicture icon) > contactIconChangedEvent;


	PresenceHandler(UserProfile & userProfile);

	~PresenceHandler();

	/**
	 * Changes my presence on desired protocol.
	 *
	 * @param state my state
	 * @param note used by PresenceStateUserDefined
	 * @param imAccount the IMAccount that we want the presence changed. If NULL
	 * presence state of all accounts is changed.
	 */
	void changeMyPresenceState(EnumPresenceState::PresenceState state,
		const std::string & note, IMAccount * imAccount = NULL);

	/**
	 * Changes my alias.
	 *
	 * @param alias the desired alias
	 * @param imAccount the IMAccount that we want the alias changed. If NULL
	 * alias of all accounts is changed.
	 */
	void changeMyAlias(const std::string & alias, IMAccount * imAccount = NULL);

	/**
	 * Changes my icon.
	 *
	 * @param picture the desired icon
	 * @param imAccount the IMAccount that we want the icon changed. If NULL
	 * icon of all accounts is changed.
	 */
	void changeMyIcon(const OWPicture & picture, IMAccount * imAccount = NULL);

	/**
	 * Gets the icon of the given IMContact.
	 *
	 * If no icon has been set, an empty OWPicture is returned.
	 *
	 * @param imContact the IMContact to get the icon of
	 * @return the icon
	 */
	OWPicture getContactIcon(const IMContact & imContact);

	/**
	 * Subscribes to the presence of an IMContact.
	 *
	 * @param imContact the IMContact
	 */
	void subscribeToPresenceOf(const IMContact & imContact);

	/**
	 * Unsubscribes to the presence of an IMContact.
	 *
	 * @param imContact the IMContact
	 */
	void unsubscribeToPresenceOf(const IMContact & imContact);

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
		const std::string & message);

private:

	typedef std::map<IMAccount *, Presence *> PresenceMap;

	typedef std::multimap<IMAccount *, const IMContact *> IMContactMultiMap;

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
	 * @see presenceStateChangedEventHandler
	 */
	void presenceStateChangedEventHandlerThreadSafe(IMAccount * imAccount, EnumPresenceState::PresenceState state,
		std::string note, std::string from);

	/**
	 * @see IMPresence::myPresenceStatusEvent
	 */
	void myPresenceStatusEventHandler(IMPresence & sender, EnumPresenceState::MyPresenceStatus status);

	/**
	 * @see myPresenceStatusEventHandler
	 */
	void myPresenceStatusEventHandlerThreadSafe(IMAccount * imAccount, EnumPresenceState::MyPresenceStatus status);

	/**
	 * @see IMPresence::subscribeStatusEvent
	 */
	void subscribeStatusEventHandler(IMPresence & sender, const std::string & contactId, IMPresence::SubscribeStatus status);

	/**
	 * @see subscribeStatusEventHandler
	 */
	void subscribeStatusEventHandlerThreadSafe(IMAccount * imAccount, std::string contactId, IMPresence::SubscribeStatus status);

	/**
	 * @see IMPresence::authorizationRequestEvent
	 */
	void authorizationRequestEventHandler(IMPresence & sender, const std::string & contactId, const std::string & message);

	/**
	 * @see authorizationRequestEventHandler
	 */
	void authorizationRequestEventHandlerThreadSafe(IMAccount * imAccount, std::string contactId, std::string message);

	/**
	 * @see UserProfile::newIMAccountAddedEvent
	 */
	void newIMAccountAddedEventHandler(UserProfile & sender, IMAccount & imAccount);

	/**
	 * @see IMAccount::imAccountDeadEvent
	 */
	void imAccountDeadEventHandler(IMAccount & sender);

	/**
	 * @see IMPresence::contactIconChangedEvent
	 */
	void contactIconChangedEventHandler(IMPresence & sender, const std::string & contactId, OWPicture icon);

	/**
	 * Finds the Presence related to the given protocol.
	 *
	 * @param presenceMap the PresenceMap to search in
	 * @param protocol the protocol
	 * @return an iterator to the desired Presence or 'end' of the given PresenceMap
	 */
	static PresenceMap::iterator findPresence(PresenceMap & presenceMap, IMAccount * imAccount);

	PresenceMap _presenceMap;

	IMContactMultiMap _pendingSubscriptions;

	/** List of Contact for which Presence State has been subsribed. */
	std::list<const IMContact *> _subscribedContacts;

	UserProfile & _userProfile;
};

#endif	//PRESENCEHANDLER_H
