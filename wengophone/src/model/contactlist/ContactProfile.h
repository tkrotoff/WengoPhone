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

#ifndef CONTACTPROFILE_H
#define CONTACTPROFILE_H

#include <model/profile/Profile.h>

#include <imwrapper/EnumPresenceState.h>
#include <imwrapper/IMContactSet.h>

#include <util/Event.h>

#include <string>
#include <set>

class UserProfile;
class ContactList;
class IMChatSession;

/**
 * Contact Profile.
 *
 * This class is a simple container of information needed for a Contact.
 *
 * @ingroup model
 * @author Philippe Bernery
 */
class ContactProfile : public Profile {
	friend class Contact;
public:

	//typedef std::set<std::string> ContactGroupSet;

	ContactProfile();

	ContactProfile(const ContactProfile & contactProfile);

	ContactProfile & operator = (const ContactProfile & contactProfile);

	~ContactProfile();

	bool operator == (const ContactProfile & contactProfile) const;

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
	 * Get the preferred IMContact.
	 *
	 * @return the preferred IMContact. If no IMContact has been set or no
	 * IMContact is online, return NULL.
	 */
	IMContact * getPreferredIMContact() const;

	/**
	 * Get an available IMContact.
	 *
	 * An available IMContact is a connected IMContact of protocol used in
	 * the imChatSession.
	 *
	 * @return an available IMContact. If no IMContact has been found,  return NULL.
	 */
	IMContact * getAvailableIMContact(IMChatSession & imChatSession) const;

	/**
	 * Add an IMContact to the ContactProfile.
	 *
	 * @param imContact IMContact to add
	 */
	virtual void addIMContact(const IMContact & imContact);

	/**
	 * Remove an IMContact from the ContactProfile.
	 *
	 * @param imContact IMContact to remove
	 */
	virtual void removeIMContact(const IMContact & imContact);

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
	const IMContactSet & getIMContactSet() {
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
	 * @return true if we can place a free call with this Contact
	 * A free call can be made if the Contact has a Wengo ID or
	 * a SIP address.
	 */
	bool hasFreeCall() const;

	/**
	 * @return free phone number (Wengo ID, SIP address).
	 * an empty string if no phone number found
	 */
	std::string getFreePhoneNumber() const;

	/**
	 * @return true if we can place a video call with this Contact.
	 */
	bool hasVideo() const;

	/**
	 * @return a display name computed from available Contact information.
	 */
	std::string getDisplayName() const;

	/**
	 * Gets the PresenceState of this Contact.
	 *
	 * @return the PresenceState of this Contact
	 */
	EnumPresenceState::PresenceState getPresenceState() const;

	/**
	 * Gets the UUID of this Contact.
	 *
	 * @return the UUID of this Contact.
	 */
	std::string getUUID() const {
		return _uuid;
	}

	/**
	 * @return true if the Contact is blocked.
	 */
	bool isBlocked() { return _blocked; }

	// Inherited from Profile
	Picture getIcon() const;
	////

	/**
	 * Actually sets the groupId.
	 */
	void setGroupId(const std::string & groupId);

	/**
	 * Gets the group id of this Contact.
	 * Empty string if no group set.
	 */
	std::string getGroupId() const {
		return _groupId;
	}

protected:

	// Inherited from Profile

	/**
	 * Sets the group of this ContactProfile.
	 *
	 * @param groupName the group name
	 */
	virtual void setGroup(const std::string & groupName) {}

	/**
	 * Gets the group of the ContactProfile.
	 *
	 * @return the group name
	 */
	virtual std::string getGroup() const { return ""; }

	////

	/** Copy a ContactProfile. */
	virtual void copy(const ContactProfile & contactProfile);

	/**
	 * @return true when a wengo id has been declared and this
	 * id is online.
	 */
	bool wengoIsAvailable() const;

	/**
	 * Gets an available SIP address than can be called
	 * taken from IMContacts of protocol SIP/SIMPLE.
	 *
	 * This method does not check Wengo ID
	 *
	 * @return the found SIP address or an empty string
	 * if no SIP address found
	 */
	std::string getAvailableSIPNumber() const;

	/**
	 * Check if a SIP number is available.
	 *
	 * This method does not check Wengo ID.
	 *
	 * @return true if the Contact has an available SIP Address.
	 */
	bool hasAvailableSIPNumber() const;

	/** The preferrred number for this Contact. */
	std::string _preferredNumber;

	/** The preferred IMContact to use for this Contact. */
	IMContact * _preferredIMContact;

	/** Is the contact blocked? */
	bool _blocked;

	/** List of IMContact of this Contact. */
	IMContactSet _imContactSet;

	/** Name of the group of this Contact. */
	std::string _groupId;

	/**
	 * UUID of this Contact.
	 *
	 * Each Contact is identified by a UUID.
	 * A Contact copy has the same UUID.
	 */
	std::string _uuid;
};

#endif	//CONTACTPROFILE_H