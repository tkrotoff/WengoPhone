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

#ifndef ADDCONTACTEVENT_H
#define ADDCONTACTEVENT_H

#include "Contact.h"

#include <observer/Event.h>

/**
 * Event: a Contact has been added to the ContactList.
 *
 * @see Observer
 * @see Subject
 * @author Tanguy Krotoff
 */
class AddContactEvent : public Event {
public:

	/**
	 * Constructs a AddContactEvent.
	 *
	 * @param contact Contact that has been added to the ContactList
	 */
	AddContactEvent(Contact & contact)
	: Event("AddContactEvent"), _contact(contact) {
	}

	/**
	 * Gets the Contact that has been added to the ContactList.
	 *
	 * @return Contact added to the ContactList
	 */
	virtual Contact & getContact() const {
		return _contact;
	}

	virtual ~AddContactEvent() {
	}

private:

	AddContactEvent(const AddContactEvent &);
	AddContactEvent & operator=(const AddContactEvent &);

	/**
	 * Contact that has been added to the ContactList.
	 */
	Contact & _contact;
};

#endif	//ADDCONTACTEVENT_H
