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

#include <model/imwrapper/IMAccount.h>
#include <model/imwrapper/EnumIMProtocol.h>

#include <string>

class PresenceHandler;

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
	 * @param protocol protocol of the contact id
	 * @param contactId the contact id
	 * @param presenceHandler the PresenceHandler that will receive presence message
	 */
	IMContact(EnumIMProtocol::IMProtocol protocol, const std::string & contactId, PresenceHandler & presenceHandler);

	IMContact(const IMContact & imContact);

	~IMContact();

	/**
	 * Check if imContact is equal to this IMContact.
	 *
	 * @param imContact the IMContact to test
	 */
	bool operator == (const IMContact & imContact) const;

	/**
	 * Subscribe to Presence of this IMContact.
	 */
	void subscribeToPresence();

	/**
	 * Avoid this IMContact to see MyPresenceState.
	 */
	void block();

	/**
	 * Authorize this IMContact to see MyPresenceState.
	 */
	void unblock();

	std::string serialize();

	const EnumIMProtocol::IMProtocol & getIMProtocol() const {
		return _protocol;
	}

	const std::string & getContactId() const {
		return _contactId;
	}

private:

	PresenceHandler & _presenceHandler;
	
	EnumIMProtocol::IMProtocol _protocol;

	std::string _contactId;

};

#endif //IMCONTACT_H
