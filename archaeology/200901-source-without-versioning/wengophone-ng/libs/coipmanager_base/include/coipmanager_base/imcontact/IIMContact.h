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

#ifndef OWIIMCONTACT_H
#define OWIIMCONTACT_H

#include <coipmanager_base/peer/Peer.h>

#include <util/StringList.h>

/**
 * Interface for IMContact.
 * Represents the most little part of Contact: its id on a accountType.
 *
 * @author Philippe Bernery
 */
class COIPMANAGER_BASE_API IIMContact : public Peer {
public:

	IIMContact();

	IIMContact(EnumAccountType::AccountType accountType, const std::string & contactId);

	IIMContact(const IIMContact & imContact);

	IIMContact & operator=(const IIMContact & imContact);

	virtual IIMContact * clone() const;

	virtual ~IIMContact();

	virtual void setPeerId(const std::string & peerId);
	virtual std::string getPeerId() const;

	virtual void setAccountId(const std::string & accountId);
	virtual std::string getAccountId() const;

	virtual void setContactId(const std::string & contactId);
	virtual std::string getContactId() const;

	virtual void setPresenceState(EnumPresenceState::PresenceState presenceState);

	virtual void setAccountType(EnumAccountType::AccountType accountType);

	/**
	 * Adds the Contact to a group/tag.
	 */
	void addToGroup(const std::string & group);

	/**
	 * Removes the Contact from a group/tag.
	 */
	void removeFromGroup(const std::string & group);

	/**
	 * Gets the group/tag id list to whom this Contact belongs to.
	 */
	StringList getGroupList() const;

protected:

	void copy(const IIMContact & imContact);

	std::string _accountId;

	StringList _groupList;
};

#endif	//OWIIMCONTACT_H
