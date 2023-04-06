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

#ifndef PRESENCECONTACTEVENT_H
#define PRESENCECONTACTEVENT_H

#include "Contact.h"

#include <observer/Event.h>

/**
 * Event: the SIP address of a Contact has been changed
 * so its presence status should be changed aswell.
 *
 * @author Tanguy Krotoff
 */
class PresenceContactEvent : public Event {
public:

	/**
	 * Constructs an PresenceContactEvent.
	 *
	 * @param contact Contact that has been changed
	 */
	PresenceContactEvent(const Contact & contact)
	: Event("PresenceContactEvent"), _contact(contact) {
	}

	virtual ~PresenceContactEvent() {
	}

	/**
	 * Gets the Contact that has been changed.
	 *
	 * @return the Contact that has been updated
	 */
	virtual const Contact & getContact() const {
		return _contact;
	}

private:

	PresenceContactEvent(const PresenceContactEvent &);
	PresenceContactEvent & operator=(const PresenceContactEvent &);

	/**
	 * The Contact that has been updated.
	 */
	const Contact & _contact;
};

#endif	//PRESENCECONTACTEVENT_H
