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

#ifndef UPDATECONTACTEVENT_H
#define UPDATECONTACTEVENT_H

#include "Contact.h"

#include <observer/Event.h>

/**
 * Event: a Contact has been updated (its name has changed for example).
 *
 * @author Tanguy Krotoff
 */
class UpdateContactEvent : public Event {
public:

	/**
	 * Constructs an UpdateContactEvent.
	 *
	 * @param contact Contact that has been updated
	 */
	UpdateContactEvent(const Contact & contact)
	: Event("UpdateContactEvent"), _contact(contact) {
	}

	virtual ~UpdateContactEvent() {
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

	UpdateContactEvent(const UpdateContactEvent &);
	UpdateContactEvent & operator=(const UpdateContactEvent &);

	/**
	 * The Contact that has been updated.
	 */
	const Contact & _contact;
};

#endif	//UPDATECONTACTEVENT_H
