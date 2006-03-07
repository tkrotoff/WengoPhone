/*
 * WengoPhone, a voice over Internet phone
 * Copyright (C) 2004-2006  Wengo
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

#include "IMContactListHandler.h"

#include <model/WengoPhone.h>
#include <imwrapper/IMContactList.h>
#include <imwrapper/IMWrapperFactory.h>

#include <Logger.h>

IMContactListHandler::IMContactListHandler(WengoPhone & wengoPhone) {
	wengoPhone.newIMAccountAddedEvent +=
		boost::bind(&IMContactListHandler::newIMAccountAddedEventHandler, this, _1, _2);
}

IMContactListHandler::~IMContactListHandler() {
	for (IMContactListMap::const_iterator it = _imContactListMap.begin();
		it != _imContactListMap.end(); it++) {
		delete (*it).second;
	}
}

void IMContactListHandler::addIMContact(const std::string & groupName, IMContact & imContact) {
	IMContactListMap::const_iterator it = _imContactListMap.find(imContact.getIMAccount());

	if (it != _imContactListMap.end()) {
		(*it).second->addContact(groupName, imContact.getContactId());
	} else {
		LOG_ERROR("this IMAccount is not registered: "
			+ imContact.getIMAccount().getLogin());
	}
}

void IMContactListHandler::removeIMContact(const std::string & groupName, IMContact & imContact) {
	IMContactListMap::const_iterator it = _imContactListMap.find(imContact.getIMAccount());

	if (it != _imContactListMap.end()) {
		(*it).second->removeContact(groupName, imContact.getContactId());
	} else {
		LOG_ERROR("this IMAccount is not registered: "
			+ imContact.getIMAccount().getLogin());
	}
}

void IMContactListHandler::newContactAddedEventHandler(IMContactList & sender,
	const std::string & groupName, const std::string & contactId) {
	IMContact imContact(sender.getIMAccount(), contactId);

	_imContactSet.insert(imContact);

	newIMContactAddedEvent(*this, groupName, (IMContact &)*_imContactSet.find(imContact));
}

void IMContactListHandler::contactRemovedEventHandler(IMContactList & sender,
	const std::string & groupName, const std::string & contactId) {
	IMContact imContact(sender.getIMAccount(), contactId);

	IMContactSet::iterator it = _imContactSet.find(imContact);

	imContactRemovedEvent(*this, groupName, (IMContact &)*it);

	_imContactSet.erase(it);
}

void IMContactListHandler::newIMAccountAddedEventHandler(WengoPhone & sender, IMAccount & imAccount) {
	IMContactListMap::const_iterator it = _imContactListMap.find(imAccount);

	if (it == _imContactListMap.end()) {
		IMContactList * imContactList = IMWrapperFactory::getFactory().createIMContactList(imAccount);

		if (imContactList) {
			imContactList->newContactAddedEvent +=
				boost::bind(&IMContactListHandler::newContactAddedEventHandler, this, _1, _2, _3);
			imContactList->contactRemovedEvent +=
				boost::bind(&IMContactListHandler::contactRemovedEventHandler, this, _1, _2, _3);
			imContactList->newContactGroupAddedEvent += newContactGroupAddedEvent;
			imContactList->contactGroupRemovedEvent += contactGroupRemovedEvent;
			imContactList->contactMovedEvent += 
				boost::bind(&IMContactListHandler::contactMovedEventHandler, this, _1, _2, _3);

			_imContactListMap[imAccount] = imContactList;
		} else {
			LOG_DEBUG("cannot create an IMContactList");
		}
	} else {
		LOG_ERROR("this IMAccount has already been added " + imAccount.getLogin());
	}
}

void IMContactListHandler::contactMovedEventHandler(IMContactList & sender,
	const std::string & groupName, const std::string & contactId) {

	IMContact imContact(sender.getIMAccount(), contactId);
	IMContactSet::const_iterator it = _imContactSet.find(imContact);

	if (it != _imContactSet.end()) {
		imContactMovedEvent(*this, groupName, imContact);
	} else {
		LOG_ERROR("this IMContact has not been added yet: " + contactId);
	}
}
