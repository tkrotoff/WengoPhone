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

#include <model/profile/UserProfile.h>
#include <imwrapper/IMContactList.h>
#include <imwrapper/IMWrapperFactory.h>

#include <util/Logger.h>

IMContactListHandler::IMContactListHandler(UserProfile & userProfile) {
	userProfile.newIMAccountAddedEvent +=
		boost::bind(&IMContactListHandler::newIMAccountAddedEventHandler, this, _1, _2);
}

IMContactListHandler::~IMContactListHandler() {
}

void IMContactListHandler::addIMContact(const std::string & groupName, const IMContact & imContact) {
	IMContactListMap::const_iterator it = _imContactListMap.find((IMAccount *)imContact.getIMAccount());

	if (it != _imContactListMap.end()) {
		(*it).second->addContact(groupName, imContact.getContactId());
	} else {
		LOG_DEBUG("this IMAccount is not registered: "
			+ (imContact.getIMAccount() ? imContact.getIMAccount()->getLogin() : "(null)"));
	}
}

void IMContactListHandler::removeIMContact(const std::string & groupName, const IMContact & imContact) {
	IMContactListMap::const_iterator it = _imContactListMap.find((IMAccount *)imContact.getIMAccount());

	if (it != _imContactListMap.end()) {
		(*it).second->removeContact(groupName, imContact.getContactId());
	} else {
		LOG_DEBUG("this IMAccount is not registered: "
			+ (imContact.getIMAccount() ? imContact.getIMAccount()->getLogin() : "(null)"));
	}
}

void IMContactListHandler::newContactAddedEventHandler(IMContactList & sender,
	const std::string & groupName, const std::string & contactId) {
	IMContact imContact(sender.getIMAccount(), contactId);

	std::pair<IMContactSet::iterator, bool> result = _imContactSet.insert(imContact);

	newIMContactAddedEvent(*this, groupName, (IMContact &)*result.first);
}

void IMContactListHandler::contactRemovedEventHandler(IMContactList & sender,
	const std::string & groupName, const std::string & contactId) {
	IMContact imContact(sender.getIMAccount(), contactId);

	IMContactSet::iterator it = _imContactSet.find(imContact);

	imContactRemovedEvent(*this, groupName, (IMContact &)*it);

	_imContactSet.erase(it);
}

void IMContactListHandler::newIMAccountAddedEventHandler(UserProfile & sender, IMAccount & imAccount) {
	IMContactListMap::const_iterator it = _imContactListMap.find(&imAccount);

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

			_imContactListMap[&imAccount] = imContactList;

			imAccount.imAccountDeadEvent +=
				boost::bind(&IMContactListHandler::imAccountDeadEventHandler, this, _1);
		} else {
			LOG_DEBUG("cannot create an IMContactList");
		}
	} else {
		LOG_ERROR("this IMAccount has already been added " + imAccount.getLogin());
	}
}

void IMContactListHandler::imAccountDeadEventHandler(IMAccount & sender) {
	IMContactListMap::iterator it = _imContactListMap.find(&sender);

	if (it != _imContactListMap.end()) {
		delete (*it).second;
		_imContactListMap.erase(it);
	} else {
		LOG_ERROR("this IMAccount has not been added " + sender.getLogin());
	}
}

void IMContactListHandler::contactMovedEventHandler(IMContactList & sender,
	const std::string & groupName, const std::string & contactId) {

	IMContact imContact(sender.getIMAccount(), contactId);
	IMContactSet::const_iterator it = _imContactSet.find(imContact);

	if (it != _imContactSet.end()) {
		LOG_DEBUG("an IMContact has moved:" + contactId);
		imContactMovedEvent(*this, groupName, imContact);
	} else {
		LOG_DEBUG("this IMContact(" + contactId + ") from group " + groupName
			+ " has not been added yet. Adding it as new IMContact.");
		newContactAddedEventHandler(sender, groupName, contactId);
	}
}

void IMContactListHandler::addGroup(const std::string & groupName) {
	for (IMContactListMap::const_iterator it = _imContactListMap.begin();
		it != _imContactListMap.end(); it++) {
		(*it).second->addGroup(groupName);
	}
}

void IMContactListHandler::removeGroup(const std::string & groupName) {
	for (IMContactListMap::const_iterator it = _imContactListMap.begin();
		it != _imContactListMap.end(); it++) {
		(*it).second->removeGroup(groupName);
	}
}

void IMContactListHandler::moveContactToGroup(const std::string & newGroupName,
	const std::string & oldGroupName, const IMContact & imContact) {

	IMContactListMap::const_iterator it = _imContactListMap.find((IMAccount *)imContact.getIMAccount());

	if (it != _imContactListMap.end()) {
		(*it).second->moveContactToGroup(newGroupName, oldGroupName, imContact.getContactId());
	} else {
		LOG_DEBUG("this IMAccount is not registered: "
			+ (imContact.getIMAccount() ? imContact.getIMAccount()->getLogin() : "(null)"));
	}
}

void IMContactListHandler::changeGroupName(const std::string & oldGroupName, const std::string & newGroupName) {
	for (IMContactListMap::const_iterator it = _imContactListMap.begin();
		it != _imContactListMap.end(); it++) {
		(*it).second->changeGroupName(oldGroupName, newGroupName);
	}
}
