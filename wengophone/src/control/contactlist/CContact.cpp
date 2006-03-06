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

#include "CContact.h"

#include <presentation/PFactory.h>
#include <presentation/PContact.h>

#include <control/CWengoPhone.h>

#include <model/contactlist/Contact.h>
#include <model/contactlist/ContactGroup.h>

#include <imwrapper/IMContactSet.h>

#include "CContactGroup.h"

using namespace std;

CContact::CContact(Contact & contact, CContactGroup & cContactGroup, CWengoPhone & cWengoPhone)
	: _contact(contact),
	_cContactGroup(cContactGroup),
	_cWengoPhone(cWengoPhone) {

	_pContact = PFactory::getFactory().createPresentationContact(*this);
}

void CContact::call() {
	/*if (this.softphone.isConnected) {
		this.softphone.decrocher(this.model.address);
	}*/
}

string CContact::getDisplayName() const {
	string result;

	if (!_contact.getFirstName().empty() || !_contact.getLastName().empty()) {
		result = _contact.getFirstName() + " " + _contact.getLastName();
	} else if (!_contact.getWengoPhoneId().empty()) {
		result = _contact.getWengoPhoneId();
	} else {
		// Take the contact id of the first IMContact
		IMContactSet::const_iterator it = _contact.getIMContactSet().begin();
		if (it != _contact.getIMContactSet().end()) {
			result = (*it).getContactId();
		}
	}

	return result;
}

string CContact::getId() const {
	//FIXME: this id is not unique
	return _contact.getFirstName() + " " + _contact.getLastName();
}

bool CContact::haveIM() const {
	return _contact.haveIM();
}

bool CContact::haveCall() const {
	return _contact.haveCall();
}

bool CContact::haveVideo() const {
	return _contact.haveVideo();
}

void CContact::placeCall() {
	_contact.placeCall();
}

void CContact::placeVideoCall(){
	_contact.placeVideoCall();
}

void CContact::startIM() {
	_contact.startIM();
}
