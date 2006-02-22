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

#ifndef IMCONTACTLISTHANDLER_H
#define IMCONTACTLISTHANDLER_H

#include <imwrapper/IMContact.h>

#include <Event.h>

#include <map>
#include <set>
#include <string>

class WengoPhone;
class IMAccount;
class IMContactList;

class IMContactListHandler {
public:

	/**
	 * Emitted when an IMContact has been added remotely.
	 *
	 * @param sender this class
	 * @param newIMContact the new IMContact
	 */
	Event< void (IMContactListHandler & sender, IMContact & newIMContact) > newIMContactAddedEvent;

	/**
	 * Emitted when an IMContact has been removed remotely.
	 *
	 * @param sender this class
	 * @param imContact the removed IMContact
	 */
	Event< void (IMContactListHandler & sender, IMContact & imContact) > imContactRemovedEvent;

	IMContactListHandler(WengoPhone & wengoPhone);

	~IMContactListHandler();

	/**
	 * Remotely adds an IMContact.
	 *
	 * @param imContact the IMContact to add
	 */
	void addIMContact(IMContact & imContact);

	/**
	 * Remotely deletes an IMContact.
	 *
	 * @param imContact the IMContact to remove
	 */
	void removeIMContact(IMContact & imContact);

private:

	void newContactAddedEventHandler(IMContactList & sender, const std::string & contactId);

	void contactRemovedEventHandler(IMContactList & sender, const std::string & contactId);

	void newIMAccountAddedEventHandler(WengoPhone & sender, IMAccount & imAccount);

	typedef std::map<const IMAccount *, IMContactList *> IMContactListMap;

	typedef std::set<IMContact> IMContactSet;

	IMContactListMap _imContactListMap;

	IMContactSet _imContactSet;

};

#endif //IMCONTACTLISTHANDLER_H
