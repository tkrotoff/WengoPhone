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

#ifndef OWCONTACT_H
#define OWCONTACT_H

#include <coipmanager_base/profile/Profile.h>

#include <coipmanager_base/EnumUpdateSection.h>
#include <coipmanager_base/account/Account.h>
#include <coipmanager_base/imcontact/IMContactList.h>

#include <util/StringList.h>

/**
 * A CoIp contact.
 *
 * @author Tanguy Krotoff
 * @author Philippe Bernery
 */
class COIPMANAGER_BASE_API Contact : public Profile {
public:

	Contact();

	Contact(const Contact & contact);

	Contact & operator = (const Contact & contact);

	virtual Contact * clone() const;

	~Contact();

	/**
	 * Add an IMContact to the Contact.
	 *
	 * @param imContact IMContact to add
	 * @return true if actually added (was not present)
	 */
	bool addIMContact(const IMContact & imContact);

	/**
	 * Remove an IMContact from the Contact.
	 *
	 * @param imContactId the id of the IMContact to remove
	 * @return true if actually removed (was present)
	 */
	bool removeIMContact(const std::string & imContactId);

	/**
	 * Updates an IMContact.
	 *
	 * @param section tells updateIMContact which section should be updated.
	 * If section == UpdateSectionUnknown, all IMContact is updated.
	 * @return true if actually updated (was present)
	 */
	bool updateIMContact(const IMContact & imContact,
		EnumUpdateSection::UpdateSection section = EnumUpdateSection::UpdateSectionUnknown);

	/**
	 * Gets the IMContactList.
	 */
	const IMContactList & getIMContactList() const;

	/**
	 * Adds the Contact to a group/tag.
	 *
	 * @return true if the Contac was not in the group
	 */
	bool addToGroup(const std::string & groupId);

	/**
	 * Removes the Contact from a group/tag.
	 *
	 * @return true if Contact was actually in the group
	 */
	bool removeFromGroup(const std::string & groupId);

	/**
	 * Gets the group/tag ids list to whom this Contact belongs to.
	 */
	StringList getGroupList() const;

	/**
	 * Sets the group/tag list of this Contact.
	 */
	void setGroupList(const StringList & stringList);

	/**
	 * @return an alias computed from all available IMContact.
	 */
	std::string getAlias() const;

	/**
	 * @return the presence state computed from all available IMContact.
	 */
	EnumPresenceState::PresenceState getPresenceState() const;

	/**
	 * @return a display name computed from available Contact information.
	 */
	std::string getDisplayName() const;

	/**
	 * @return a display name in a short way.
	 */
	std::string getShortDisplayName() const;

private:

	void copy(const Contact & contact);

	IMContactList::iterator findByComparingIdProtocol(const IMContact & imContact);

	/**
	 * Will browse all IMContact to see if groups declared in IMContacts are
	 * also declared in this Contact _groupList.
	 */
	void synchronizeGroupList();

	IMContactList _imContactList;

	StringList _groupList;
};

#endif	//OWCONTACT_H
