/*
 * WengoPhone, a voice over Internet phone
 * Copyright (C) 2004-2007 Wengo
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

#ifndef OWPROFILE_H
#define OWPROFILE_H

#include <coipmanager_base/profile/EnumSex.h>
#include <coipmanager_base/profile/StreetAddress.h>

#include <coipmanager_base/EnumPresenceState.h>

#include <util/Date.h>
#include <util/Identifiable.h>
#include <util/Interface.h>
#include <util/OWPicture.h>
#include <util/List.h>

/**
 * Handle common data between a Contact and a UserProfile.
 *
 * @author Philippe Bernery
 * @author Mathieu Stute
 */
class COIPMANAGER_BASE_API Profile : public Identifiable {
public:

	Profile();

	Profile(const Profile & profile);

	~Profile();

	void setFirstName(const std::string & firstName);
	const std::string & getFirstName() const;

	void setLastName(const std::string & lastName);
	const std::string & getLastName() const;

	std::string getCompleteName() const;

	void setSex(EnumSex::Sex sex);
	EnumSex::Sex getSex() const;

	void setBirthdate(const Date & birthdate);
	const Date & getBirthdate() const;

	void setWebsite(const std::string & website);
	const std::string & getWebsite() const;

	void setCompany(const std::string & company);
	const std::string & getCompany() const;

	void setMobilePhone(const std::string & mobilePhone);
	const std::string & getMobilePhone() const;

	void setHomePhone(const std::string & homePhone);
	const std::string & getHomePhone() const;

	void setWorkPhone(const std::string & workPhone);
	const std::string & getWorkPhone() const;

	void setOtherPhone(const std::string & otherPhone);
	const std::string & getOtherPhone() const;

	void setWengoPhoneNumber(const std::string & wengoPhoneNumber);
	const std::string & getWengoPhoneNumber() const;

	void setFax(const std::string & fax);
	const std::string & getFax() const;

	void setPersonalEmail(const std::string & personalEmail);
	const std::string & getPersonalEmail() const;

	void setWorkEmail(const std::string & workEmail);
	const std::string & getWorkEmail() const;

	void setOtherEmail(const std::string & otherEmail);
	const std::string & getOtherEmail() const;

	void setStreetAddress(const StreetAddress & streetAddress);
	StreetAddress getStreetAddress() const;

	void setNotes(const std::string & notes);
	const std::string & getNotes() const;

	void setSmsSignature(const std::string & signature);
	const std::string & getSmsSignature() const;

	bool hasPhoneNumber() const;

	void setAlias(const std::string & alias);
	const std::string & getAlias() const;

	void setWengoPhoneId(const std::string & wengoPhoneId);
	const std::string & getWengoPhoneId() const;

	void setIcon(const OWPicture & icon);
	const OWPicture & getIcon() const;

	void setPresenceState(EnumPresenceState::PresenceState state);
	EnumPresenceState::PresenceState getPresenceState() const;

protected:

	void copy(const Profile & profile);

	std::string _firstName;

	std::string _lastName;

	EnumSex::Sex _sex;

	Date _birthdate;

	std::string _website;

	std::string _company;

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

	std::string _alias;

	std::string _wengoPhoneId;

	OWPicture _icon;

	std::string _smsSignature;

	EnumPresenceState::PresenceState _presenceState;
};

#endif	//OWPROFILE_H
