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

#ifndef REMOVECONTACTEVENT_H
#define REMOVECONTACTEVENT_H

#include "Contact.h"

#include <observer/Event.h>

/**
 * Event: a Contact has been removed from the ContactList.
 *
 * The Contact will be removed just after this Event is created.
 *
 * @author Tanguy Krotoff
 */
class RemoveContactEvent : public Event {
public:

	/**
	 * Constructs a RemoveContactEvent.
	 *
	 * @param contact Contact removed from the list
	 */
	RemoveContactEvent(Contact & contact)
	: Event("RemoveContactEvent"), _contact(contact) {
	}

	/**
	 * Gets the Contact that will be removed from the ContactList.
	 *
	 * @return the Contact removed
	 */
	virtual Contact & getContact() const {
		return _contact;
	}

	virtual ~RemoveContactEvent() {
	}

private:

	RemoveContactEvent(const RemoveContactEvent &);
	RemoveContactEvent & operator=(const RemoveContactEvent &);

	/**
	 * The Contact to be removed from the ContactList.
	 */
	Contact & _contact;
};

#endif	//REMOVECONTACTEVENT_H
