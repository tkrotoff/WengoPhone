/*
 * WengoPhone, a voice over Internet phone
 * Copyright (C) 2004-2007  Wengo
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

#ifndef OWCONTACTLISTFILESTORAGE_H
#define OWCONTACTLISTFILESTORAGE_H

#include <coipmanager_base/coipmanagerbasedll.h>

#include <coipmanager_base/contact/ContactList.h>

#include <string>

/**
 * @ingroup coipmanager
 * @author Tanguy Krotoff
 * @author Philippe Bernery
 */
class COIPMANAGER_BASE_API ContactListFileStorage {
public:

	ContactListFileStorage();

	~ContactListFileStorage();

	bool load(const std::string & url, ContactList & contactList);

	bool save(const std::string & url, const ContactList & contactList);
};

#endif	//OWCONTACTLISTFILESTORAGE_H
