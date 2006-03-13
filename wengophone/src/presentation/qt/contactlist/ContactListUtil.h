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
#ifndef CONTACTLISTUTIL_H
#define CONTACTLISTUTIL_H

#include <QVariant>
#include "MetaContact.h"
#include "MetaContactGroup.h"
#include "IQtContact.h"

class Contact;
class ContactGroup;

class ContactListUtil {
public:
	/**
	 * Return the IQtContact contained by a Qvariant
	 *
	 * @param v the QVariant to get the Contact from
	 * @return the Contact or NULL if not found
	 */
	static IQtContact * getIQtContact(const QVariant &v) {
		if (qVariantCanConvert<MetaContact>(v)) {
			MetaContact metaContact = qVariantValue<MetaContact>(v);
			return metaContact.getIQtContact();
		} else {
			return NULL;
		}
	}
	
	/**
	 * Return the ContactGroup contained by a Qvariant
	 *
	 * @param v the QVariant to get the ContactGroup from
	 * @return the ContactGroup or NULL if not found
	 */
	static ContactGroup * getContactGroup(const QVariant &v) {
		if (qVariantCanConvert<MetaContactGroup>(v)) {
			MetaContactGroup metaContactGroup = qVariantValue<MetaContactGroup>(v);
			return metaContactGroup.getContactGroup();
		} else {
			return NULL;
		}
	}
};

#endif //CONTACTLISTUTIL_H
