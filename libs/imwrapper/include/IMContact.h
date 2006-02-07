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

#include "IMAccount.h"

#include <string>

class PresenceHandler;
class IMAccount;

/**
 * An Instant Message Contact.
 *
 * @author Philippe Bernery
 */
class IMContact {
public:
	/**
	 * Construct a new IMContact.
	 * 
	 * @param imAccount the imAccount that the IMContact is associated with
	 * @param contactId id of the IMContact to create
	 * @param presenceHandler the PresenceHandler that will receive presence message
	 */
	IMContact(const IMAccount & imAccount, const std::string & contactId);

	IMContact(const IMContact & imContact);

	~IMContact();

	/**
	 * Check if imContact is equal to this IMContact.
	 *
	 * @param imContact the IMContact to test
	 */
	bool operator == (const IMContact & imContact) const;

	std::string serialize();

	const IMAccount & getIMAccount() const {
		return _imAccount;
	}

	const std::string & getContactId() const {
		return _contactId;
	}

private:

	const IMAccount & _imAccount;

	std::string _contactId;

};

#endif //IMCONTACT_H
