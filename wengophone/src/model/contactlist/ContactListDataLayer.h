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

#ifndef CONTACTLISTDATALAYER_H
#define CONTACTLISTDATALAYER_H

#include "model/DataLayer.h"

class ContactList;

/**
 *
 * @ingroup model
 * @author Tanguy Krotoff
 */
class ContactListDataLayer : public DataLayer {
public:

	ContactListDataLayer(ContactList & contactList)
		: _contactList(contactList) { }

	virtual ~ContactListDataLayer() { }

	virtual bool load() = 0;

	virtual bool save() = 0;

protected:

	ContactList & _contactList;
};

#endif	//CONTACTLISTDATALAYER_H
