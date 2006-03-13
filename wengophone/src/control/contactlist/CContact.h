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

#ifndef CCONTACT_H
#define CCONTACT_H

#include <imwrapper/EnumPresenceState.h>

#include <Event.h>

#include <string>

class Contact;
class CContactGroup;
class CWengoPhone;
class PContact;

/**
 *
 * @ingroup control
 * @author Tanguy Krotoff
 */
class CContact {
public:

	/**
	 * @see Contact::contactModifiedEvent
	 */
	Event<void (Contact & sender)> contactModifiedEvent;

	CContact(Contact & contact, CContactGroup & cContactGroup, CWengoPhone & cWengoPhone);

	void call();

	PContact * getPresentation() const {
		return _pContact;
	}

	Contact & getContact() const {
		return _contact;
	}

	std::string getDisplayName() const;

	std::string getId() const;

	bool hasIM() const;

	bool hasCall() const;

	bool hasVideo() const;

	EnumPresenceState::PresenceState getPresenceState() const;

private:

	Contact & _contact;

	CContactGroup & _cContactGroup;

	CWengoPhone & _cWengoPhone;

	PContact * _pContact;
};

#endif	//CCONTACT_H
