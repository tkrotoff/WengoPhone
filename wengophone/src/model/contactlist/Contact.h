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

#include "StreetAddress.h"
#include "IMContact.h"

#include <model/imwrapper/EnumPresenceState.h>
#include <model/imwrapper/IMAccount.h>

#include <Event.h>
#include <Serializable.h>
#include <Date.h>

#include <string>

/**
 * Contact inside an address book.
 *
 * @ingroup model
 * @author Tanguy Krotoff
 * @author Philippe Bernery
 */
class Contact : public Serializable {
public:

	enum Sex {
		SexUnknown,
		SexMale,
		SexFemale
	};

	/**
	 * A Contact has been modified/updated.
	 *
	 * @param sender the Contact modified/updated
	 */
	Event<void (Contact & sender)> contactModifiedEvent;

	Contact() {
		_sex = SexUnknown;
		_state = EnumPresenceState::PresenceStateOffline;
		_blocked = false;
	}

	Contact(const Contact & contact) {
		initialize(contact);
	}

	~Contact();

	Contact & operator=(const Contact & contact) {
		if (&contact != this) {
			initialize(contact);
		}
		return *this;
	}

	void setFirstName(const std::string & firstName) { _firstName = firstName; }
	const std::string & getFirstName() const { return _firstName; }

	void setLastName(const std::string & lastName) { _lastName = lastName; }
	const std::string & getLastName() const { return _lastName; }

	void setSex(Sex sex) { _sex = sex; }
	Sex getSex() const { return _sex; }

	void setBirthdate(const Date & birthdate) { _birthdate = birthdate; }
	Date getBirthdate() const { return _birthdate; }

	void setPicture(const std::string & picture) { _picture = picture; }
	const std::string & getPicture() const { return _picture; }

	void setWebsite(const std::string & website) { _website = website; }
	const std::string & getWebsite() const { return _website; }

	void setCompany(const std::string & company) { _company = company; }
	const std::string & getCompany() const { return _company; }

	void setWengoPhone(const std::string & wengoPhone) { _wengoPhone = wengoPhone; }
	const std::string & getWengoPhone() const { return _wengoPhone; }

	void setMobilePhone(const std::string & mobilePhone) { _mobilePhone = mobilePhone; }
	const std::string & getMobilePhone() const { return _mobilePhone; }

	void setHomePhone(const std::string & homePhone) { _homePhone = homePhone; }
	const std::string & getHomePhone() const { return _homePhone; }

	void setWorkPhone(const std::string & workPhone) { _workPhone = workPhone; }
	const std::string & getWorkPhone() const { return _workPhone; }

	void setOtherPhone(const std::string & otherPhone) { _otherPhone = otherPhone; }
	const std::string & getOtherPhone() const { return _otherPhone; }

	void setFax(const std::string & fax) { _fax = fax; }
	const std::string & getFax() const { return _fax; }

	void setPersonalEmail(const std::string & personalEmail) { _personalEmail = personalEmail; }
	const std::string & getPersonalEmail() const { return _personalEmail; }

	void setWorkEmail(const std::string & workEmail) { _workEmail = workEmail; }
	const std::string & getWorkEmail() const { return _workEmail; }

	void setOtherEmail(const std::string & otherEmail) { _otherEmail = otherEmail; }
	const std::string & getOtherEmail() const { return _otherEmail; }

	void setStreetAddress(const StreetAddress & streetAddress) { _streetAddress = streetAddress; }
	const StreetAddress & getStreetAddress() const { return _streetAddress; }

	void setNotes(const std::string & notes) { _notes = notes; }
	const std::string & getNotes() const { return _notes; }

	void setPresenceState(EnumPresenceState::PresenceState state) { _state = state; }
	EnumPresenceState::PresenceState getPresenceState() const { return _state; }

	/**
	 * Add an IMContact to the Contact.
	 * addIMContactId checks if the IMContact already exists.
	 * The imContact is copied so the one provided does not need to stay alive.
	 *
	 * @param imContact IMContact to add
	 */
	void addIMContact(const IMContact & imContact);

	/**
	 * Remove an IMContact to the Contact.
	 *
	 * @param imContact IMContact to remove
	 */
	void removeIMContact(const IMContact & imContact);

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

	std::string serialize();

	bool unserialize(const std::string & data);

private:

	typedef std::vector<IMContact *> IMContactList;
	
	/** Find an IMContact in a IMContactList. */
	static IMContactList::iterator findIMContact(IMContactList & imContactList, const IMContact & imContact);
	
	/** Factorizes code between contructor and copy contructor. */
	void initialize(const Contact & contact);

	/** Return all IMContact in a a vCard string format. */
	std::string imContactsToString();

	std::string _firstName;
	std::string _lastName;
	Sex _sex;
	Date _birthdate;
	std::string _picture;
	std::string _website;
	std::string _company;
	std::string _wengoPhone;
	std::string _mobilePhone;
	std::string _homePhone;
	std::string _workPhone;
	std::string _otherPhone;
	std::string _fax;
	std::string _personalEmail;
	std::string _workEmail;
	std::string _otherEmail;
	StreetAddress _streetAddress;
	std::string _notes;
	EnumPresenceState::PresenceState _state;
	bool _blocked;
	IMContactList _imContactList;

};

#endif	//CONTACT_H
