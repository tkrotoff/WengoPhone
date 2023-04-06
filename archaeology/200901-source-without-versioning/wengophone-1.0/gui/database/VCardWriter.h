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

#ifndef DATABASE_VCARDWRITER_H
#define DATABASE_VCARDWRITER_H

#include <VCard.h>

class Contact;
class StreetAddress;
class QDate;
class QString;

namespace database {

/**
 * Writes a Contact in the VCard format.
 *
 * @author Tanguy Krotoff
 */
class VCardWriter {
public:

	VCardWriter(const Contact & contact);

	~VCardWriter() {
	}

	QString getData() const {
		return _data;
	}

private:

	VCardWriter();
	VCardWriter(const VCardWriter &);
	VCardWriter & operator=(const VCardWriter &);

	static void addTextValue(VCARD::VCard & vcard, VCARD::EntityType type, const QString & txt);

	static void addDateValue(VCARD::VCard & vcard, VCARD::EntityType type, const QDate & date);

	static void addNValue(VCARD::VCard & vcard, const Contact & contact);

	static void addTelephoneValue(VCARD::VCard & vcard, const Contact & contact);

	static void addAddressValue(VCARD::VCard & vcard, const StreetAddress & address);

	static void addCustomValue(VCARD::VCard & vcard, const QString & txt);

	QString _data;
};

}

#endif	//DATABASE_VCARDWRITER_H
