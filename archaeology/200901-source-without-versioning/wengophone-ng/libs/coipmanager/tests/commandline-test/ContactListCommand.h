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

#ifndef OWCONTACTLISTCOMMAND_H
#define OWCONTACTLISTCOMMAND_H

#include "Command.h"

/**
 * Command for Contact list management.
 *
 * @author Philippe Bernery
 */
class ContactListCommand : public Command {
public:

	ContactListCommand(CoIpManager & coIpManager);

	virtual ~ContactListCommand();

private:

	/**
	 * @name Menu entry
	 * @{
	 */

	/** Add a Contact. */
	void addContact();

	/** Remove a Contact. */
	void removeContact();

	/** Modify a Contact. */
	void modifyContact();

	/** Lists Contacts. */
	void listContact();

	/** @} */

};

#endif //OWCONTACTLISTCOMMAND_H
