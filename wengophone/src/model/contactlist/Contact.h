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

#ifndef CONTACT_H
#define CONTACT_H

#include <model/profile/Profile.h>

#include <imwrapper/EnumPresenceState.h>
#include <imwrapper/IMAccount.h>
#include <imwrapper/IMContact.h>
#include <imwrapper/IMContactSet.h>

#include <util/Event.h>
#include <util/Date.h>

#include <string>
#include <set>

class UserProfile;
class ContactList;

/**
 * Contact inside an address book.
 *
 * @ingroup model
 * @author Tanguy Krotoff
 * @author Philippe Bernery
 */
class Contact : public Profile {
	friend class ContactList;
	friend class ContactXMLSerializer;
public:

	/**
	 * A Contact has been modified/updated.
	 *
	 * @param sender the Contact modified/updated
	 */
	Event<void (Contact & sender)> contactChangedEvent;

	Contact(UserProfile & userProfile);

	Contact(const Contact & contact);

	Contact & operator = (const Contact & contact);

	~Contact();

	bool operator == (const Contact & contact) const;

	/**
	 * Set the preferred phone number (can also be a wengo id or a sip address).
	 *
	 * @param number the preferred phone number
	 */
	void setPreferredPhoneNumber(const std::string & number) { _preferredNumber = number; contactChangedEvent(*this); }

	/**
	 * Get the preferred phone number.
	 *
	 * @return the preferred phone number. If no preferred phone number has been set
	 * the first set phone number is returned (the test is made in this order:
	 * wengo id (if online), mobile, home, work and other phone number). If no phone number has
	 * been set, a null string is returned.
	 */
	std::string getPreferredNumber() const;

	/**
	 * Set the preferred IMContact to use.
	 *
	 * @param the imContact to set. The given reference must stay valid during
	 * the execution (this must be a reference to an IMContact of this Contact).
	 */
	void setPreferredIMContact(const IMContact & imContact) { _preferredIMContact = (IMContact *)&imContact; contactChangedEvent(*this); }

	/**
	 * Get the preferred IMContact.
	 *
	 * @return the preferred IMContact. If no IMContact has been set or no
	 * IMContact is online, return NULL.
	 */
	IMContact * getPreferredIMContact() const;

	/**
	 * Add the Contact to the given ContactGroup.
	 *
	 * The process is the same as in addIMContact
	 *
	 * @param groupName the group name to add to
	 */
	void addToContactGroup(const std::string & groupName);

	/**
	 * Remove the Contact from a ContactGroup.
	 *
	 * The process is the same as in addIMContact
	 *
	 * @param groupName the group name to remove from
	 */
	void removeFromContactGroup(const std::string & groupName);

	/**
	 * Test if the Contact is in a group.
	 *
	 * @param groupName thr group
	 * @return true if in the group
	 */
	bool isInContactGroup(const std::string & groupName);

	/**
	 * Add an IMContact to the Contact.
	 *
	 * This method will send a request to ContactList that will send a request
	 * to the appropriated IMContactList. Then the ContactList will call
	 * _addIMContact that will actually add the IMContact.
	 *
	 * @param imContact IMContact to add
	 */
	void addIMContact(const IMContact & imContact);

	/**
	 * Remove an IMContact from the Contact.
	 *
	 * The process is the same as in addIMContact
	 *
	 * @param imContact IMContact to remove
	 */
	void removeIMContact(const IMContact & imContact);

	/**
	 * @param imContact the maybe associated IMContact
	 * @return true if this Contact is associated with the given IMContact.
	 */
	bool hasIMContact(const IMContact & imContact) const;

	/**
	 * Return the IMContact equals to given IMContact.
	 *
	 * @param imContact the desired IMContact
	 * @return the IMContact
	 */
	IMContact & getIMContact(const IMContact & imContact) const;

	/**
	 * @return Set of IMContact
	 */
	const IMContactSet & getIMContactSet() const {
		return _imContactSet;
	}

	/**
	 * @return true if chat is available with this Contact.
	 */
	bool hasIM() const;

	/**
	 * @return true if we can place a call with this Contact.
	 */
	bool hasCall() const;

	/**
	 * @return true if we can place a video call with this Contact.
	 */
	bool hasVideo() const;

	/**
	 * @return a display name computed from available Contact information.
	 */
	std::string getDisplayName() const;

	/**
	 * Move the Contact from a group to another one.
	 *
	 * @param to the new group
	 * @param from the old group
	 */
	void moveToGroup(const std::string & to, const std::string & from);

	EnumPresenceState::PresenceState getPresenceState() const;

	/**
	 * Avoid this contact to see my presence.
	 */
	void block();

	/**
	 * Authorize this contact to see my presence.
	 */
	void unblock();

	/**
	 * @return true if the Contact is blocked.
	 */
	bool isBlocked() { return _blocked; }

	std::string toString() const {
		std::string display;
		if (!getLastName().empty()) {
			display += getLastName();
		}
		if (!getFirstName().empty()) {
			if (!getLastName().empty()) {
				display += ", " + getFirstName();
			} else {
				display += getFirstName();
			}
		}

		return display;
	}

	UserProfile & getUserProfile() const {
			return _userProfile;
	}

	// Inherited from Profile
	void setWengoPhoneId(const std::string & wengoId);

	Picture getIcon() const;
	////

private:

	/**
	 * @see Profile::profileChangedEvent
	 */
	void profileChangedEventHandler(Profile & profile);

	/**
	 * @see IMContact::imContactAddedToGroupEvent
	 */
	void imContactAddedToGroupEventHandler(IMContact & sender, const std::string & groupName);

	/**
	 * @see IMContact::imContactRemovedFromGroupEvent
	 */
	void imContactRemovedFromGroupEventHandler(IMContact & sender, const std::string & groupName);

	/**
	 * @see IMContact::imContactChangedEvent
	 */
	void imContactChangedEventHandler(IMContact & sender);

	/**
	 * Actually add the Contact to a ContactGroup.
	 *
	 * This method must be called only by ContactList
	 *
	 * @param groupName the group name
	 */
	void _addToContactGroup(const std::string & groupName);

	/**
	 * Actually remove the Contact from a ContactGroup.
	 *
	 * This method must be called only by ContactList
	 *
	 * @param groupName the group name to remove from
	 */
	void _removeFromContactGroup(const std::string & groupName);

	/**
	 * Actually add an IMContact to this Contact.
	 *
	 * This method must be called only by ContactList
	 *
	 * The IMContact is copied internally
	 *
	 * @param the IMContact to add
	 */
	void _addIMContact(const IMContact & imContact);

	/**
	 * Actually remove an IMContact to this Contact.
	 *
 	 * This method must be called only by ContactList
 	 *
	 * @param the IMContact to remove
	 */
	void _removeIMContact(const IMContact & imContact);

	/** Factorizes code between contructor and copy contructor. */
	void initialize(const Contact & contact);

	/**
	 * @return true when a wengo id has been declared and this
	 * id is online.
	 */
	bool wengoIsAvailable() const;

	/** Can't be used on a Contact.*/
	void setIcon(const Picture & icon);

	std::string _preferredNumber;

	IMContact * _preferredIMContact;

	bool _blocked;

	IMContactSet _imContactSet;

	typedef std::set<std::string> ContactGroupSet;

	ContactGroupSet _contactGroupSet;

	UserProfile & _userProfile;

	ContactList & _contactList;
};

#endif	//CONTACT_H
