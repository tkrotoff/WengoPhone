/*
 * WengoPhone, a voice over Internet phone
 * Copyright (C) 2004-2005  Wengo
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

#ifndef CCONTACTGROUP_H
#define CCONTACTGROUP_H

#include <string>
#include <vector>

class ContactGroup;
class CContactList;
class CWengoPhone;
class PContactGroup;
class Contact;
class CContact;

/**
 *
 * @ingroup control
 * @author Tanguy Krotoff
 */
class CContactGroup {
public:
	CContactGroup(ContactGroup & contactGroup, CContactList & cContactList, CWengoPhone & cWengoPhone);

	PContactGroup * getPresentation() const {
		return _pContactGroup;
	}

	ContactGroup & getContactGroup() const {
		return _contactGroup;
	}

	std::string getContactGroupName() const;

	CWengoPhone & getCWengoPhone() const {
		return _cWengoPhone;
	}

private:

	void contactAddedEventHandler(ContactGroup & sender, Contact & contact);

	void contactRemovedEventHandler(ContactGroup & sender, Contact & contact);

	void contactGroupModifiedEventHandler(ContactGroup & sender);

	ContactGroup & _contactGroup;

	CContactList & _cContactList;

	CWengoPhone & _cWengoPhone;

	PContactGroup * _pContactGroup;

	typedef std::vector<CContact *> CContactVector;
	CContactVector _cContactVector;
};

#endif	//CCONTACTGROUP_H
