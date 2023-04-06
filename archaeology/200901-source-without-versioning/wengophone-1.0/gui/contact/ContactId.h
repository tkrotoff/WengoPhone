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

#ifndef CONTACTID_H
#define CONTACTID_H

#include <qstring.h>

/**
 * Type ContactId that simply encapsulates an integer.
 *
 * Contains the current date + time + a counter in order to be unique
 * at any time.
 *
 * @author Tanguy Krotoff
 */
class ContactId {
public:

	/**
	 * This ID is reserved and no Contact corresponds to this ID.
	 *
	 * Used for example in SessionWindow and ContactWidget.
	 *
	 * @see SessionWindow
	 * @see ContactWidget
	 */
	static const unsigned int UNKNOW;

	ContactId();

	ContactId(const QString & id);

	ContactId & operator=(const ContactId & id) {
		if (&id != this) {
			_id = id._id;
		}
		return *this;
	}

	friend bool operator==(const ContactId & id1, const ContactId & id2) {
		return id1._id == id2._id;
	}

	QString toString() const;

private:

	//ContactId(const ContactId &);

	int _id;
};

#endif	//CONTACTID_H
