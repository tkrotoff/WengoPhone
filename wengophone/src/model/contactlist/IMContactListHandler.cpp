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
	for (IMContactListMap::const_iterator it = _imContactListMap.begin() ;
		 it != _imContactListMap.end() ;
		 it++) {
		delete (*it).second;
	}
}

void IMContactListHandler::addIMContact(IMContact & imContact) {
	IMContactListMap::const_iterator it = _imContactListMap.find(&imContact.getIMAccount());

	if (it != _imContactListMap.end()) {
		(*it).second->addContact(imContact.getContactId());
	} else {
		LOG_ERROR("this IMAccount is not registered: " + imContact.getIMAccount().getLogin());		
	}
}

void IMContactListHandler::removeIMContact(IMContact & imContact) {
	IMContactListMap::const_iterator it = _imContactListMap.find(&imContact.getIMAccount());
	
	if (it != _imContactListMap.end()) {
		(*it).second->removeContact(imContact.getContactId());
	} else {
		LOG_ERROR("this IMAccount is not registered: " + imContact.getIMAccount().getLogin());		
	}	
}

void IMContactListHandler::newContactAddedEventHandler(IMContactList & sender, const std::string & contactId) {
	IMContact imContact(sender.getIMAccount(), contactId);

	_imContactSet.insert(imContact);
	
	newIMContactAddedEvent(*this, (IMContact &)*_imContactSet.find(imContact));
}

void IMContactListHandler::contactRemovedEventHandler(IMContactList & sender, const std::string & contactId) {
	IMContact imContact(sender.getIMAccount(), contactId);

	IMContactSet::iterator it = _imContactSet.find(imContact);

	imContactRemovedEvent(*this, (IMContact &)*it);

	_imContactSet.erase(it);	
}

void IMContactListHandler::newIMAccountAddedEventHandler(WengoPhone & sender, IMAccount & imAccount) {
	IMContactListMap::const_iterator it = _imContactListMap.find(&imAccount);

	if (it == _imContactListMap.end()) {
		IMContactList * imContactList = IMWrapperFactory::getFactory().createIMContactList(imAccount);
		
		imContactList->newContactAddedEvent += 
			boost::bind(&IMContactListHandler::newContactAddedEventHandler, this, _1, _2);
		imContactList->contactRemovedEvent += 
			boost::bind(&IMContactListHandler::contactRemovedEventHandler, this, _1, _2);	
		
		_imContactListMap[&imAccount] = imContactList;
	} else {
		LOG_ERROR("this IMAccount has already been added " + imAccount.getLogin());
	}
}
