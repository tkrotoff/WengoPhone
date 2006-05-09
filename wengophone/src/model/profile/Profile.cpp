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

#include "Profile.h"

Profile::Profile() {
	_sex = EnumSex::SexUnknown;
	_presenceState = EnumPresenceState::PresenceStateUnknown;
	_firstName = "";
	_lastName = "";
	_website = "";
	_company = "";
	_wengoPhoneId = "";
	_mobilePhone = "";
	_homePhone = "";
	_workPhone = "";
	_wengoPhoneNumber = "";
	_otherPhone = "";
	_fax = "";
	_personalEmail = "";
	_workEmail = "";
	_otherEmail = "";
	_notes = "";
	_alias = "";
}

Profile::Profile(const Profile & profile) {
	copy(profile);
}

void Profile::copy(const Profile & profile) {
	_sex = profile._sex;
	_presenceState = profile._presenceState;;
	_firstName = profile._firstName;
	_lastName = profile._lastName;
	_website = profile._website;
	_company = profile._company;
	_wengoPhoneId = profile._wengoPhoneId;
	_mobilePhone = profile._mobilePhone;
	_homePhone = profile._homePhone;
	_workPhone = profile._workPhone;
	_wengoPhoneNumber = profile._wengoPhoneNumber;
	_otherPhone = profile._otherPhone;
	_fax = profile._fax;
	_personalEmail = profile._personalEmail;
	_workEmail = profile._workEmail;
	_otherEmail = profile._otherEmail;
	_notes = profile._notes;
	_alias = profile._alias;
	_icon = profile._icon;
}

bool Profile::operator == (const Profile & profile) const {
	return ((_firstName == profile._firstName)
		&& (_lastName == profile._lastName)
		&& (_sex == profile._sex)
		&& (_birthdate == profile._birthdate)
		&& (_website == profile._website)
		&& (_company == profile._company)
		&& (_wengoPhoneId == profile._wengoPhoneId)
		&& (_mobilePhone == profile._mobilePhone)
		&& (_homePhone == profile._homePhone)
		&& (_workPhone == profile._workPhone)
		&& (_otherPhone == profile._otherPhone)
		&& (_fax == profile._fax)
		&& (_personalEmail == profile._personalEmail)
		&& (_workEmail == profile._workEmail)
		&& (_otherEmail == profile._otherEmail)
		&& (_streetAddress == profile._streetAddress)
		&& (_notes == profile._notes)
		&& (_presenceState == profile._presenceState));
}
