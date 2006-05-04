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

#ifndef IMCONTACT_H
#define IMCONTACT_H

#include <imwrapper/IMAccount.h>
#include <imwrapper/EnumPresenceState.h>

#include <util/Event.h>
#include <util/Picture.h>

#include <string>
#include <set>

class PresenceHandler;
class IMAccount;

/**
 * An Instant Message Contact.
 *
 * Associates a contact identifier (bob@hotmail.com) with a IMAccount.
 * This is needed since several accounts using the same protocol can used in parallel.
 *
 * @author Philippe Bernery
 */
class IMContact {
	friend class IMContactXMLSerializer;
public:

	/**
	 * Emitted when this IMContact has been added to a group.
	 *
	 * @param sender this IMContact
	 * @param groupName the group where the IMContact has been added to
	 */
	Event< void (IMContact & sender, const std::string & groupName)> imContactAddedToGroupEvent;

	/**
	 * Emitted when this IMContact has been removed from a group.
	 *
	 * @param sender this IMContact
	 * @param groupName the group where the IMContact has been removed from
	 */
	Event< void (IMContact & sender, const std::string & groupName)> imContactRemovedFromGroupEvent;

	/**
	 * Emitted when this IMContact has changed (e.g. its presence state
	 * could have changed or its alias).
	 *
	 * @param sender this class
	 */
	Event< void (IMContact & sender) > imContactChangedEvent;

	/**
	 * Constructs a new IMContact.
	 *
	 * @param imAccount the imAccount that the IMContact is associated with.
	 * @param contactId id of the IMContact to create
	 * @param presenceHandler the PresenceHandler that will receive presence message
	 */
	IMContact(const IMAccount & imAccount, const std::string & contactId);

	/**
	 * Constructs a new IMContact.
	 *
	 * @param protocol the protocol of the IMContact
	 * @param contactId id of the IMContact to create
	 * @param presenceHandler the PresenceHandler that will receive presence message
	 */
	IMContact(EnumIMProtocol::IMProtocol protocol, const std::string & contactId);

	IMContact(const IMContact & imContact);

	~IMContact();

	/**
	 * Checks if imContact is equal to this IMContact.
	 *
	 * @param imContact the IMContact to test
	 */
	bool operator==(const IMContact & imContact) const;

	bool operator<(const IMContact & imContact) const;

	const IMAccount * getIMAccount() const {
		return _imAccount;
	}

	void setIMAccount(const IMAccount * imAccount);

	const std::string & getContactId() const {
		return _contactId;
	}

	EnumIMProtocol::IMProtocol getProtocol() const;

	/**
	 * Add this IMContact to a group.
	 *
	 * @param groupName the group to add this IMContact to
	 */
	void addToGroup(const std::string & groupName);

	/**
	 * Remove this IMContact to a group.
	 *
	 * @param groupName the group to remove this IMContact from
	 */
	void removeFromGroup(const std::string & groupName);

	/**
	 * Remove from all group.
	 */
	void removeFromAllGroup();

	EnumPresenceState::PresenceState getPresenceState() const {
		return _presenceState;
	}

	void setPresenceState(EnumPresenceState::PresenceState presenceState) {
		_presenceState = presenceState;
		imContactChangedEvent(*this);
	}

	const std::string & getAlias() const {
		return _alias;
	}

	void setAlias(const std::string & alias) {
		_alias = alias;
		imContactChangedEvent(*this);
	}

	void setIcon(const Picture & icon) {
		_icon = icon;
		imContactChangedEvent(*this);
	}

	const Picture & getIcon() const {
		return _icon;
	}

	void setBlocked(bool blocked) {
		_blocked = blocked;
	}

	bool isBlocked() const {
		return _blocked;
	}

private:

	/**
	 * Used to get a cleaned contact id.
	 *
	 * When chatting with a Jabber contact, its id changed to
	 * our_contact_id/something wich made some strange behaviour
	 * in our model. So we strip everything after the '/'.
	 *
	 * @return the clean contact id
	 */
	std::string cleanContactId() const;

	/**
	 * Pointer to the associated IMAccount.
	 * If NULL, no IMAccount is currently associated with this IMAccount
	 */
	const IMAccount  * _imAccount;

	std::string _contactId;

	EnumIMProtocol::IMProtocol _protocol;

	std::string _alias;

	Picture _icon;

	/** True if this IMContact is blocked. */
	bool _blocked;

	EnumPresenceState::PresenceState _presenceState;

	typedef std::set<std::string> GroupSet;
	GroupSet _groupSet;
};

#endif	//IMCONTACT_H
