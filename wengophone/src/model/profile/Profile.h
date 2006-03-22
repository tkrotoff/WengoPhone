/*
 * WengoPhone, a voice over Internet phone
 * Copyright (C) 2004-2006 Wengo
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

#ifndef PROFILE_H
#define PROFILE_H

#include "EnumSex.h"
#include "StreetAddress.h"

#include <imwrapper/EnumPresenceState.h>

#include <util/Interface.h>
#include <util/Date.h>
#include <util/Event.h>

/**
 * Handle common data between a Contact a UserProfile.
 *
 * @author Philippe BERNERY
 */
class Profile : Interface {
public:

	/**
	 * Emitted when a properties has been changed.
	 *
	 * @param sender this class
	 */
	Event< void(Profile & sender) > profileChangedEvent;

	virtual ~Profile() {}

	void setFirstName(const std::string & firstName) { _firstName = firstName; profileChangedEvent(*this); }
	const std::string & getFirstName() const { return _firstName; }

	void setLastName(const std::string & lastName) { _lastName = lastName; profileChangedEvent(*this); }
	const std::string & getLastName() const { return _lastName; }

	void setSex(EnumSex::Sex sex) { _sex = sex; profileChangedEvent(*this); }
	EnumSex::Sex getSex() const { return _sex; }

	void setBirthdate(const Date & birthdate) { _birthdate = birthdate; profileChangedEvent(*this); }
	Date getBirthdate() const { return _birthdate; }

	void setPicture(const std::string & picture) { _picture = picture; profileChangedEvent(*this); }
	const std::string & getPicture() const { return _picture; }

	void setWebsite(const std::string & website) { _website = website; profileChangedEvent(*this); }
	const std::string & getWebsite() const { return _website; }

	void setCompany(const std::string & company) { _company = company; profileChangedEvent(*this); }
	const std::string & getCompany() const { return _company; }

	void setWengoPhoneId(const std::string & wengoPhoneId) { _wengoPhoneId = wengoPhoneId; profileChangedEvent(*this); }
	const std::string & getWengoPhoneId() const { return _wengoPhoneId; }

	void setMobilePhone(const std::string & mobilePhone) { _mobilePhone = mobilePhone; profileChangedEvent(*this); }
	const std::string & getMobilePhone() const { return _mobilePhone; }

	void setHomePhone(const std::string & homePhone) { _homePhone = homePhone; profileChangedEvent(*this); }
	const std::string & getHomePhone() const { return _homePhone; }

	void setWorkPhone(const std::string & workPhone) { _workPhone = workPhone; profileChangedEvent(*this); }
	const std::string & getWorkPhone() const { return _workPhone; }

	void setOtherPhone(const std::string & otherPhone) { _otherPhone = otherPhone; profileChangedEvent(*this); }
	const std::string & getOtherPhone() const { return _otherPhone; }

	void setWengoPhoneNumber(const std::string & wengoPhoneNumber) { _wengoPhoneNumber = wengoPhoneNumber; profileChangedEvent(*this); }
	const std::string & getWengoPhoneNumber() const { return _wengoPhoneNumber; }

	void setFax(const std::string & fax) { _fax = fax; profileChangedEvent(*this); }
	const std::string & getFax() const { return _fax; }

	void setPersonalEmail(const std::string & personalEmail) { _personalEmail = personalEmail; profileChangedEvent(*this); }
	const std::string & getPersonalEmail() const { return _personalEmail; }

	void setWorkEmail(const std::string & workEmail) { _workEmail = workEmail; profileChangedEvent(*this); }
	const std::string & getWorkEmail() const { return _workEmail; }

	void setOtherEmail(const std::string & otherEmail) { _otherEmail = otherEmail; profileChangedEvent(*this); }
	const std::string & getOtherEmail() const { return _otherEmail; }

	void setStreetAddress(const StreetAddress & streetAddress) { _streetAddress = streetAddress; profileChangedEvent(*this); }
	const StreetAddress & getStreetAddress() const { return _streetAddress; }

	void setNotes(const std::string & notes) { _notes = notes; profileChangedEvent(*this); }
	const std::string & getNotes() const { return _notes; }

	/**
	 * Return the PresenceState of this Profile.
	 *
	 * @return the PresenceState
	 */
	virtual EnumPresenceState::PresenceState getPresenceState() const {
		return _presenceState;
	}

	/**
	 * Set the PresenceState of the profile.
	 *
	 * @return the PresenceState
	 */
	virtual void setPresenceState(EnumPresenceState::PresenceState presenceState) {
		_presenceState = presenceState;
	}

protected:

	std::string _firstName;

	std::string _lastName;

	EnumSex::Sex _sex;

	Date _birthdate;

	std::string _picture;

	std::string _website;

	std::string _company;

	std::string _wengoPhoneId;

	std::string _mobilePhone;

	std::string _homePhone;

	std::string _workPhone;

	std::string _wengoPhoneNumber;

	std::string _otherPhone;

	std::string _fax;

	std::string _personalEmail;

	std::string _workEmail;

	std::string _otherEmail;

	StreetAddress _streetAddress;

	std::string _notes;

	EnumPresenceState::PresenceState _presenceState;

};

#endif //PROFILE_H
