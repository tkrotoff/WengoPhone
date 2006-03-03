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

#include "CContactGroup.h"

#include "model/contactlist/Contact.h"
#include "model/contactlist/ContactGroup.h"
#include "presentation/PFactory.h"
#include "control/CWengoPhone.h"
#include "presentation/PContactGroup.h"
#include "control/contactlist/CContactList.h"
#include "control/contactlist/CContact.h"
#include "CContactList.h"

CContactGroup::CContactGroup(ContactGroup & contactGroup, CContactList & cContactList, CWengoPhone & cWengoPhone)
	: _contactGroup(contactGroup),
	_cContactList(cContactList),
	_cWengoPhone(cWengoPhone) {

	_pContactGroup = PFactory::getFactory().createPresentationContactGroup(*this);

	_contactGroup.contactAddedEvent += boost::bind(&CContactGroup::contactAddedEventHandler, this, _1, _2);
	_contactGroup.contactRemovedEvent += boost::bind(&CContactGroup::contactRemovedEventHandler, this, _1, _2);
	_contactGroup.contactGroupModifiedEvent += boost::bind(&CContactGroup::contactGroupModifiedEventHandler, this, _1);
}

std::string CContactGroup::getContactGroupName() const {
	return _contactGroup.toString();
}

void CContactGroup::contactAddedEventHandler(ContactGroup & sender, Contact & contact) {
	CContact * cContact = new CContact(contact, *this, _cWengoPhone);

	_pContactGroup->addContact(cContact->getPresentation());
	_pContactGroup->updatePresentation();
}

void CContactGroup::contactRemovedEventHandler(ContactGroup & sender, Contact & contact) {
}

void CContactGroup::contactGroupModifiedEventHandler(ContactGroup & sender) {
}
