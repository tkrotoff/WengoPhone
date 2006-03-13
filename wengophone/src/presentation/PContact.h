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

#ifndef PCONTACT_H
#define PCONTACT_H

#include <imwrapper/EnumPresenceState.h>

#include "Presentation.h"

#include <string>

class Contact;

/**
 *
 * @ingroup presentation
 * @author Tanguy Krotoff
 */
class PContact : public Presentation {
public:
	PContact(Contact & contact) : _contact(contact) {}

	virtual std::string getDisplayName() const = 0;
	virtual std::string getId() const = 0;
	virtual bool hasIM() const = 0;
	virtual bool hasCall() const = 0;
	virtual bool hasVideo() const = 0;
	virtual EnumPresenceState::PresenceState getPresenceState() const = 0;

	Contact & getContact() const {
		return _contact;
	}

private:

	Contact & _contact;

};

#endif	//PCONTACT_H
