/*
 * WengoPhone, a voice over Internet phone
 * Copyright (C) 2004-2007  Wengo
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

#ifndef OWIMCONTACTLIST_H
#define OWIMCONTACTLIST_H

#include <coipmanager_base/imcontact/IMContact.h>

#include <list>

typedef std::list<IMContact> IMContactList;

class COIPMANAGER_BASE_API IMContactListHelper {
public:

	/**
	 * @return true there is an IMContact with the given id.
	 */
	static bool has(const IMContactList & imContactList, const std::string & imContactId);

	/**
	 * @return true there is an equivalent IMContact.
	 */
	static bool has(const IMContactList & imContactList, const IMContact & imContact);

	/**
	 * @return true if the IMContactList contains an IMContact associated with the given accountId.
	 */
	static bool hasAssociatedIMContact(const IMContactList & imContactList, const std::string & accountId);

	/**
	 * @return true if the IMContactList contains an IMContact of the given accountType.
	 */
	static bool hasIMContactOfProtocol(const IMContactList & imContactList, EnumAccountType::AccountType accountType);

	/**
	 * Gets a copy of the IMContact of id imContactId.
	 */
	static IMContact * getCopyOfIMContact(const IMContactList & imContactList, const std::string & imContactId);

	/**
	 * Gets a copy of the IMContact equivalent to the given one (same contactId/accountType).
	 */
	static IMContact * getCopyOfIMContact(const IMContactList & imContactList, const IMContact & imContact);

	/**
	 * Gets a copy of IMContacts associated with the given accountId.
	 */
	static IMContactList getCopyOfAssociatedIMContacts(const IMContactList & imContactList, const std::string & accountId);

	/**
	 * Gets a copy of IMContacts of the given accountType.
	 */
	static IMContactList getCopyOfIMContactsOfProtocol(const IMContactList & imContactList, EnumAccountType::AccountType accountType);

};

#endif //OWIMCONTACTLIST_H
