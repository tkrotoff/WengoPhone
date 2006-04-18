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

	_connection = (_contact.contactChangedEvent += 
		boost::bind(&CContact::contactChangedEventHandler, this, _1));
}

CContact::~CContact() {
	_connection.disconnect();
	delete _pContact;
}

bool CContact::operator == (const CContact & cContact) {
	QMutexLocker locker(&_mutex);
	return (_pContact == cContact._pContact);
}

void CContact::call() {
	/*if (this.softphone.isConnected) {
		this.softphone.decrocher(this.model.address);
	}*/
}

string CContact::getDisplayName() const {
	return _contact.getDisplayName();
}

bool CContact::hasIM() const {
	return _contact.hasIM();
}

bool CContact::hasCall() const {
	return _contact.hasCall();
}

bool CContact::hasVideo() const {
	return _contact.hasVideo();
}

EnumPresenceState::PresenceState CContact::getPresenceState() const {
	return _contact.getPresenceState();
}

void CContact::contactChangedEventHandler(Contact & contact) {
	_pContact->updatePresentation();
}

Picture CContact::getIcon() const {
	return _contact.getIcon();
}