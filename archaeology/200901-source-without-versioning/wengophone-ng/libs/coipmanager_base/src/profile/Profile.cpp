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

#include <coipmanager_base/profile/Profile.h>

#include <util/String.h>

Profile::Profile()
	: Identifiable() {
	_sex = EnumSex::SexUnknown;
	_firstName = String::null;
	_lastName = String::null;
	_website = String::null;
	_company = String::null;
	_mobilePhone = String::null;
	_homePhone = String::null;
	_workPhone = String::null;
	_wengoPhoneNumber = String::null;
	_otherPhone = String::null;
	_fax = String::null;
	_personalEmail = String::null;
	_workEmail = String::null;
	_otherEmail = String::null;
	_notes = String::null;
	_alias = String::null;
	_wengoPhoneId = String::null;
	_smsSignature = String::null;
}

Profile::Profile(const Profile & profile)
	: Identifiable(profile) {
	copy(profile);
}

void Profile::copy(const Profile & profile) {
	Identifiable::copy(profile);

	_firstName = profile._firstName;
	_lastName = profile._lastName;
	_sex = profile._sex;
	_birthdate = profile._birthdate;
	_website = profile._website;
	_company = profile._company;
	_mobilePhone = profile._mobilePhone;
	_homePhone = profile._homePhone;
	_workPhone = profile._workPhone;
	_wengoPhoneNumber = profile._wengoPhoneNumber;
	_otherPhone = profile._otherPhone;
	_fax = profile._fax;
	_personalEmail = profile._personalEmail;
	_workEmail = profile._workEmail;
	_otherEmail = profile._otherEmail;
	_streetAddress = profile._streetAddress;
	_notes = profile._notes;
	_wengoPhoneId = profile._wengoPhoneId;
	_alias = profile._alias;
	_icon = profile._icon;
	_smsSignature = profile._smsSignature;
}

Profile::~Profile() {
}

void Profile::setFirstName(const std::string & firstName) {
	_firstName = firstName;
}

const std::string & Profile::getFirstName() const {
	return _firstName;
}

void Profile::setLastName(const std::string & lastName) {
	_lastName = lastName;
}

const std::string & Profile::getLastName() const {
	return _lastName;
}

std::string Profile::getCompleteName() const {
	return _firstName + " " + _lastName;
}

void Profile::setSex(EnumSex::Sex sex) {
	_sex = sex;
}

EnumSex::Sex Profile::getSex() const {
	return _sex;
}

void Profile::setBirthdate(const Date & birthdate) {
	_birthdate = birthdate;
}

const Date & Profile::getBirthdate() const {
	return _birthdate;
}

void Profile::setWebsite(const std::string & website) {
	_website = website;
}

const std::string & Profile::getWebsite() const {
	return _website;
}

void Profile::setCompany(const std::string & company) {
	_company = company;
}

const std::string & Profile::getCompany() const {
	return _company;
}

void Profile::setMobilePhone(const std::string & mobilePhone) {
	_mobilePhone = mobilePhone;
}

const std::string & Profile::getMobilePhone() const {
	return _mobilePhone;
}

void Profile::setHomePhone(const std::string & homePhone) {
	_homePhone = homePhone;
}

const std::string & Profile::getHomePhone() const {
	return _homePhone;
}

void Profile::setWorkPhone(const std::string & workPhone) {
	_workPhone = workPhone;
}

const std::string & Profile::getWorkPhone() const {
	return _workPhone;
}

void Profile::setOtherPhone(const std::string & otherPhone) {
	_otherPhone = otherPhone;
}

const std::string & Profile::getOtherPhone() const {
	return _otherPhone;
}

void Profile::setWengoPhoneNumber(const std::string & wengoPhoneNumber) {
	_wengoPhoneNumber = wengoPhoneNumber;
}

const std::string & Profile::getWengoPhoneNumber() const {
	return _wengoPhoneNumber;
}

void Profile::setFax(const std::string & fax) {
	_fax = fax;
}

const std::string & Profile::getFax() const {
	return _fax;
}

void Profile::setPersonalEmail(const std::string & personalEmail) {
	_personalEmail = personalEmail;
}

const std::string & Profile::getPersonalEmail() const {
	return _personalEmail;
}

void Profile::setWorkEmail(const std::string & workEmail)  {
	_workEmail = workEmail;
}

const std::string & Profile::getWorkEmail() const {
	return _workEmail;
}

void Profile::setOtherEmail(const std::string & otherEmail) {
	_otherEmail = otherEmail;
}

const std::string & Profile::getOtherEmail() const {
	return _otherEmail;
}

void Profile::setStreetAddress(const StreetAddress & streetAddress) {
	_streetAddress = streetAddress;
}

StreetAddress Profile::getStreetAddress() const {
	return _streetAddress;
}

void Profile::setNotes(const std::string & notes) {
	_notes = notes;
}

const std::string & Profile::getNotes() const {
	return _notes;
}

void Profile::setSmsSignature(const std::string & signature) {
	_smsSignature = signature;
}

const std::string & Profile::getSmsSignature() const {
	return _smsSignature;
}

bool Profile::hasPhoneNumber() const {
	return (!_workPhone.empty() || !_homePhone.empty() || !_mobilePhone.empty() || !_otherPhone.empty());
}

void Profile::setAlias(const std::string & alias) {
	_alias = alias;
}

const std::string & Profile::getAlias() const {
	return _alias;
}

void Profile::setWengoPhoneId(const std::string & wengoPhoneId) {
	_wengoPhoneId = wengoPhoneId;
}

const std::string & Profile::getWengoPhoneId() const {
	return _wengoPhoneId;
}

void Profile::setIcon(const OWPicture & icon) {
	_icon = icon;
}

const OWPicture & Profile::getIcon() const {
	return _icon;
}

void Profile::setPresenceState(EnumPresenceState::PresenceState state) {
	_presenceState = state;
}

EnumPresenceState::PresenceState Profile::getPresenceState() const {
	return _presenceState;
}
