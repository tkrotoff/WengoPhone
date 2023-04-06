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

#ifndef OWIMCONTACT_H
#define OWIMCONTACT_H

#include <coipmanager_base/imcontact/IIMContact.h>

/**
 * Represents the most little part of Contact: its id on a accountType.
 *
 * @author Philippe Bernery
 */
class COIPMANAGER_BASE_API IMContact : public IIMContact {
	friend class IMContactXMLSerializer;
public:

	IMContact();

	IMContact(EnumAccountType::AccountType accountType, const std::string & contactId);

	IMContact(const IMContact & imContact);

	explicit IMContact(const IIMContact * iIMContact);

	IMContact & operator = (const IMContact & imContact);

	virtual IMContact * clone() const;

	~IMContact();

	virtual void setAccountId(const std::string & accountId);
	virtual std::string getAccountId() const;

	virtual std::string getContactId() const;

	virtual EnumAccountType::AccountType getAccountType() const;

	virtual void setAlias(const std::string & alias);
	virtual std::string getAlias() const;

	virtual void setPresenceState(EnumPresenceState::PresenceState presenceState);
	virtual EnumPresenceState::PresenceState getPresenceState() const;

	virtual void setIcon(const OWPicture & icon);
	virtual OWPicture getIcon() const;

	IIMContact * getPrivateIMContact() const;

private:

	void copy(const IMContact & imContact);

	virtual void setContactId(const std::string & contactId);

	virtual void setPeerId(const std::string & peerId);
	virtual std::string getPeerId() const;

	virtual void setAccountType(EnumAccountType::AccountType accountType);

	IIMContact * _privateIMContact;
};

#endif	//OWIMCONTACT_H
