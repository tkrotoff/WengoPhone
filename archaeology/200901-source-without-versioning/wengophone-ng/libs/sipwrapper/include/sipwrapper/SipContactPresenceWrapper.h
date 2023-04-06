/*
 * WengoPhone, a voice over Internet phone
 * Copyright (C) 2004-2007  Wengo
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

#ifndef OWSIPCONTACTPRESENCEWRAPPER_H
#define OWSIPCONTACTPRESENCEWRAPPER_H

#include <coipmanager_base/EnumPresenceState.h>

#include <sipwrapper/sipwrapperdll.h>

#include <QtCore/QObject>

/**
 * Presence part of SipWrapper.
 *
 * @author Philippe Bernery
 */
class SIPWRAPPER_API SipContactPresenceWrapper : public QObject {
	Q_OBJECT
public:

	/**
	 * Avoids a contact to see presence.
	 *
	 * @param lineId virtual line associated with the account of wich we want
	 * to block the presence
	 * @param contactId contact to be blocked (e.g: "sip:test@voip.wengo.fr")
	 */
	virtual void blockContact(int lineId, const std::string & contactId) = 0;

	/**
	 * Unblocks a contact.
	 *
	 * @param lineId virtual line associated with the account of wich we want
	 * to block the presence
	 * @param contactId contact to be allowed
	 */
	virtual void unblockContact(int lineId, const std::string & contactId) = 0;

	/**
	 * Authorizes a contact to add me in his contact list.
	 *
	 * @param contactId contact id
 	 * @param authorized TRUE if we agree
	 */
	virtual void authorizeContact(int lineId, const std::string & contactId,
		bool auhorized, const std::string message) = 0;

	/**
	 * Subscribes to presence of a contact.
	 *
	 * @param contactId id of the contact (e.g: "sip:text@voip.wengo.fr")
	 */
	virtual void subscribeToPresenceOf(int lineId,
		const std::string & contactId) = 0;

	/**
	 * Unsubscribes to presence of a contact.
	 *
	 * @param contactId id of the contact (e.g: "sip:text@voip.wengo.fr")
	 */
	virtual void unsubscribeToPresenceOf(int lineId,
		const std::string & contactId) = 0;

	/**
	 * Emits the associated signal.
	 */
	void emitPresenceStateChangedSignal(EnumPresenceState::PresenceState state,
		const std::string & alias, const std::string & from);

Q_SIGNALS:

	/**
	 * Presence status of a buddy has changed.
	 *
	 * @param state new buddy presence status
	 * @param alias buddy alias (Joe @ the beach)
	 * @param from buddy who changed its presence status
	 */
	void presenceStateChangedSignal(EnumPresenceState::PresenceState state,
		const std::string & alias, const std::string & from);

};

#endif //OWSIPCONTACTPRESENCEWRAPPER_H
