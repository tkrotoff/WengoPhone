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

#include <coipmanager_base/imcontact/IMContactList.h>

bool IMContactListHelper::has(const IMContactList & imContactList, const std::string & imContactId) {
	bool result = false;

	for (IMContactList::const_iterator it = imContactList.begin();
		it != imContactList.end();
		++it) {
		if ((*it).getUUID() == imContactId) {
			result = true;
			break;
		}
	}

	return result;
}

bool IMContactListHelper::has(const IMContactList & imContactList, const IMContact & imContact) {
	bool result = false;

	for (IMContactList::const_iterator it = imContactList.begin();
		it != imContactList.end();
		++it) {
		if (((*it).getContactId() == imContact.getContactId())
			&& ((*it).getAccountType() == imContact.getAccountType())) {
			result = true;
			break;
		}
	}

	return result;
}

bool IMContactListHelper::hasAssociatedIMContact(const IMContactList & imContactList, const std::string & accountId) {
	bool result = false;

	for (IMContactList::const_iterator it = imContactList.begin();
		it != imContactList.end();
		++it) {
		if ((*it).getAccountId() == accountId) {
			result = true;
			break;
		}
	}

	return result;
}

bool IMContactListHelper::hasIMContactOfProtocol(const IMContactList & imContactList, EnumAccountType::AccountType accountType) {
	bool result = false;

	for (IMContactList::const_iterator it = imContactList.begin();
		it != imContactList.end();
		++it) {
		if ((*it).getAccountType() == accountType) {
			result = true;
			break;
		}
	}

	return result;
}

IMContact * IMContactListHelper::getCopyOfIMContact(const IMContactList & imContactList, const std::string & imContactId) {
	IMContact *result = NULL;

	for (IMContactList::const_iterator it = imContactList.begin();
		it != imContactList.end();
		++it) {
		if ((*it).getUUID() == imContactId) {
			return (*it).clone();
		}
	}

	return result;
}

IMContact * IMContactListHelper::getCopyOfIMContact(const IMContactList & imContactList, const IMContact & imContact) {
	IMContact *result = NULL;

	for (IMContactList::const_iterator it = imContactList.begin();
		it != imContactList.end();
		++it) {
		if (((*it).getContactId() == imContact.getContactId())
			&& ((*it).getAccountType() == imContact.getAccountType())) {
			return (*it).clone();
		}
	}

	return result;
}

IMContactList IMContactListHelper::getCopyOfAssociatedIMContacts(const IMContactList & imContactList, const std::string & accountId) {
	IMContactList result;

	for (IMContactList::const_iterator it = imContactList.begin();
		it != imContactList.end();
		++it) {
		if ((*it).getAccountId() == accountId) {
			result.push_back(*it);
		}
	}

	return result;
}

IMContactList IMContactListHelper::getCopyOfIMContactsOfProtocol(const IMContactList & imContactList, EnumAccountType::AccountType accountType) {
	IMContactList result;

	for (IMContactList::const_iterator it = imContactList.begin();
		it != imContactList.end();
		++it) {
		if ((*it).getAccountType() == accountType) {
			result.push_back(*it);
		}
	}

	return result;
}
