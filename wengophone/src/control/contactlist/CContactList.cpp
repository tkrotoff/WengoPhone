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

#include "CContactList.h"

#include <presentation/PFactory.h>
#include <presentation/PContactList.h>

#include <control/CWengoPhone.h>

#include <model/contactlist/ContactList.h>
#include <model/contactlist/Contact.h>
#include <model/contactlist/ContactGroup.h>

#include "CContactGroup.h"

#include <Logger.h>

CContactList::CContactList(ContactList & contactList, CWengoPhone & cWengoPhone)
	: _contactList(contactList),
	_cWengoPhone(cWengoPhone) {

	_pContactList = PFactory::getFactory().createPresentationContactList(*this);

	_contactList.contactGroupAddedEvent += boost::bind(&CContactList::contactGroupAddedEventHandler, this, _1, _2);
	_contactList.contactGroupRemovedEvent += boost::bind(&CContactList::contactGroupRemovedEventHandler, this, _1, _2);
	_contactList.contactAddedEvent += boost::bind(&CContactList::contactAddedEventHandler, this, _1, _2);
	_contactList.contactRemovedEvent += boost::bind(&CContactList::contactRemovedEventHandler, this, _1, _2);
}

void CContactList::contactAddedEventHandler(ContactList & sender, Contact & contact) {
	
	//_pContactList->addContact(
}

void CContactList::contactRemovedEventHandler(ContactList & sender, Contact & contact) {
	
}

void CContactList::contactGroupAddedEventHandler(ContactList & sender, ContactGroup & contactGroup) {
	CContactGroup * cContactGroup = new CContactGroup(contactGroup, *this, _cWengoPhone);

	LOG_DEBUG("contact group added: " + contactGroup.getName());
	_pContactList->addContactGroup(cContactGroup->getPresentation());
	_pContactList->updatePresentation();
}

void CContactList::contactGroupRemovedEventHandler(ContactList & sender, ContactGroup & contactGroup) {
//	LOG_DEBUG("contact group removed: " + contactGroup.getName());
//	_pContactList->removeContactGroup(contactGroup->getPresentation());
//	_pContactList->updatePresentation();
}

StringList CContactList::getContactGroupStringList() const {
	StringList result;

	ContactList::ContactGroupSet contactGroupSet = _contactList.getContactGroupSet();
	for (ContactList::ContactGroupSet::const_iterator it = contactGroupSet.begin();
		it != contactGroupSet.end();
		++it) {
		result.add((*it).getName());
	}

	return result;
}
