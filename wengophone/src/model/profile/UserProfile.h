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

#ifndef USERPROFILE_H
#define USERPROFILE_H

#include "Profile.h"

#include <imwrapper/EnumPresenceState.h>

class SipAccount;
class IMAccount;
class WengoPhone;

/**
 * Handle the profile of a User.
 *
 * @author Philippe BERNERY
 */
class UserProfile : public Profile {
public:

	UserProfile(WengoPhone & wengoPhone);

	virtual ~UserProfile();

	void addSipAccount(const SipAccount & sipAccount);

	void addIMAccount(const IMAccount & imAccount);

	virtual EnumPresenceState::PresenceState getPresenceState() const;

	virtual void setPresenceState(EnumPresenceState::PresenceState presenceState);

	/**
	 * Change alias of this user.
	 *
	 * The alias is seen in every IM protocols.
	 *
	 * @param alias the alias to set.
	 */
	void setAlias(const std::string & alias);

	/**
	 * Get the alias of this user.
	 *
	 * @return the alias
	 */
	std::string getAlias() const;

private:

	WengoPhone & _wengoPhone;

};

#endif //USERPROFILE_H
