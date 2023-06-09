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

#ifndef PRESENCE_H
#define PRESENCE_H

#include <imwrapper/EnumPresenceState.h>
#include <imwrapper/IMPresence.h>

#include <util/NonCopyable.h>
#include <util/Event.h>
#include <util/Trackable.h>

class IMAccount;
class ContactList;
class MyPresenceState;

/**
 *
 * @ingroup model
 * @author Tanguy Krotoff
 */
class Presence : NonCopyable, public Trackable {
public:

	/**
	 * @see IMPresence::presenceStateChangedEvent
	 */
	Event<void (IMPresence & sender, EnumPresenceState::PresenceState state,
		const std::string & alias, const std::string & from)> presenceStateChangedEvent;

	/**
	 * @see IMPresence::presenceStatusEvent
	 */
	Event<void (IMPresence & sender, EnumPresenceState::MyPresenceStatus status, const std::string & note)> myPresenceStatusEvent;

	/**
	 * @see IMPresence::subscribeStatusEvent
	 */
	Event<void (IMPresence & sender, const std::string & contactId, IMPresence::SubscribeStatus status)> subscribeStatusEvent;

	/**
	 * @see IMPresence::subscribeStatusEvent
	 */
	Event<void (IMPresence & sender, const std::string & contactId, const std::string & message)> authorizationRequestEvent;

	/**
	 * @see IMPresence::contactIconChangedEvent
	 */
	Event< void (IMPresence & sender, const std::string & contactId, OWPicture icon) > contactIconChangedEvent;


	Presence(IMAccount & imAccount);

	~Presence();

	/**
	 * @see IMPresence::changeMyPresence
	 */
	void changeMyPresence(EnumPresenceState::PresenceState state, const std::string & note);

	/**
	 * @see IMPresence::changeMyAlias
	 */
	void changeMyAlias(const std::string & alias);

	/**
	 * @see IMPresence::changeMyIcon
	 */
	void changeMyIcon(const OWPicture & picture);

	/**
	 * @see IMPresence::subscribeToPresenceOf
	 */
	void subscribeToPresenceOf(const std::string & contactId);

	/**
	 * @see IMPresence::unsubscribeToPresenceOf
	 */
	void unsubscribeToPresenceOf(const std::string & contactId);

	/**
	 * @see IMPresence::blockContact
	 */
	void blockContact(const std::string & contactId);

	/**
	 * @see IMPresence::unblockContact
	 */
	void unblockContact(const std::string & contactId);

	/**
	 * @see IMPresence::authorizeContact
	 */
	void authorizeContact(const std::string & contactId, bool authorized, const std::string message);

	IMAccount & getIMAccount() const {
		return _imPresence->getIMAccount();
	}

private:

	/**
	 * Sets the state of this Presence
	 *
	 * @param the desired state
	 */
	void setState(EnumPresenceState::PresenceState state);

	/**
	 * Catches presenceStateChangedEvent for debug reason.
	 *
	 * Shows a LOG_DEBUG() message.
	 */
	void presenceStateChangedEventHandler(IMPresence & sender, EnumPresenceState::PresenceState state,
		const std::string & alias, const std::string & from);

	IMAccount & _imAccount;

	IMPresence * _imPresence;

	MyPresenceState * _state;
};

#endif	//PRESENCE_H
