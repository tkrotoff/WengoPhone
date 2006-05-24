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

#include "CUserProfile.h"

#include <model/account/wengo/WengoAccount.h>
#include <model/config/ConfigManager.h>
#include <model/config/Config.h>
#include <model/contactlist/Contact.h>
#include <model/profile/UserProfile.h>

CUserProfile::CUserProfile(UserProfile & userProfile, Thread & modelThread) 
: _userProfile(userProfile), 
_cContactList(userProfile.getContactList(), modelThread),
_modelThread(modelThread) { 
}

void CUserProfile::disconnect() {
	typedef ThreadEvent0<void ()> MyThreadEvent;
	MyThreadEvent * event = 
		new MyThreadEvent(boost::bind(&CUserProfile::disconnectThreadSafe, this));

	_modelThread.postEvent(event);
}

void CUserProfile::disconnectThreadSafe() {
	_userProfile.disconnect();
}

void CUserProfile::makeContactCall(const std::string & contactId) {
	typedef ThreadEvent1<void (std::string contactId), std::string> MyThreadEvent;
	MyThreadEvent * event = 
		new MyThreadEvent(boost::bind(&CUserProfile::makeContactCallThreadSafe, this, _1), contactId);

	_modelThread.postEvent(event);
}

void CUserProfile::makeContactCallThreadSafe(std::string contactId) {
	Contact * contact = _cContactList.getContact(contactId);
	if (contact) {
		Config & config = ConfigManager::getInstance().getCurrentConfig();
		_userProfile.makeCall(*contact, config.getVideoEnable());
	}
}

void CUserProfile::makeCall(const std::string & phoneNumber) {
	typedef ThreadEvent1<void (std::string phoneNumber), std::string> MyThreadEvent;
	MyThreadEvent * event = 
		new MyThreadEvent(boost::bind(&CUserProfile::makeCallThreadSafe, this, _1), phoneNumber);

	_modelThread.postEvent(event);
}

void CUserProfile::makeCallThreadSafe(std::string phoneNumber) {
	Config & config = ConfigManager::getInstance().getCurrentConfig();
	_userProfile.makeCall(phoneNumber, config.getVideoEnable());
}

void CUserProfile::makeCall(const std::string & phoneNumber, bool enableVideo) {
	typedef ThreadEvent2<void (std::string phoneNumber, bool enableVideo), std::string, bool> MyThreadEvent;
	MyThreadEvent * event = 
		new MyThreadEvent(boost::bind(&CUserProfile::makeCallThreadSafe, this, _1, _2), phoneNumber, enableVideo);

	_modelThread.postEvent(event);
}

void CUserProfile::makeCallThreadSafe(std::string phoneNumber, bool enableVideo) {
	_userProfile.makeCall(phoneNumber, enableVideo);
}

void CUserProfile::startIM(const std::string & contactId) {
	typedef ThreadEvent1<void (std::string contactId), std::string> MyThreadEvent;
	MyThreadEvent * event = 
		new MyThreadEvent(boost::bind(&CUserProfile::startIMThreadSafe, this, _1), contactId);

	_modelThread.postEvent(event);
}

void CUserProfile::startIMThreadSafe(std::string contactId) {
	Contact * contact = _cContactList.getContact(contactId);
	if (contact) {
		_userProfile.startIM(*contact);
	}
}

void CUserProfile::setWengoAccount(const WengoAccount & wengoAccount) {
	typedef ThreadEvent1<void (WengoAccount wengoAccount), WengoAccount> MyThreadEvent;
	MyThreadEvent * event = 
		new MyThreadEvent(boost::bind(&CUserProfile::setWengoAccountThreadSafe, this, _1), wengoAccount);

	_modelThread.postEvent(event);
}

void CUserProfile::setWengoAccountThreadSafe(WengoAccount wengoAccount) {
	_userProfile.setWengoAccount(wengoAccount);
}

std::set<IMAccount *> CUserProfile::getIMAccountsOfProtocol(EnumIMProtocol::IMProtocol protocol) const {
	std::set<IMAccount *> result;

	for (IMAccountHandler::const_iterator it = _userProfile.getIMAccountHandler().begin();
		it != _userProfile.getIMAccountHandler().end();
		it++) {
		if ((*it).getProtocol() == protocol) {
			result.insert((IMAccount *)(&(*it)));
		}
	}

	return result;
}
