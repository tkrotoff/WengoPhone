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

#include <coipmanager/session/ISession.h>

#include <util/Logger.h>
#include <util/SafeDelete.h>

ISession::ISession(CoIpManager & coIpManager)
	: CoIpModule(coIpManager) {

	LOG_DEBUG("ISession created");
	_mutex = new QMutex(QMutex::Recursive);
}

ISession::ISession(const ISession & iSession)
	: CoIpModule(iSession._coIpManager) {

	_imContactList = iSession._imContactList;
	_account = iSession._account;
	_mutex = new QMutex(QMutex::Recursive);
}

ISession::~ISession() {
	OWSAFE_DELETE(_mutex);
}

void ISession::setAccount(const Account & account) {
	QMutexLocker lock(_mutex);

	_account = account;
}

Account ISession::getAccount() const {
	if (!_account.getPrivateAccount()) {
		LOG_FATAL("account is empty, call setAccount() first");
	}

	return _account;
}

void ISession::addIMContact(const IMContact & imContact) {
	addIMContactLocally(imContact);
}

void ISession::removeIMContact(const IMContact & imContact) {
	removeIMContactLocally(imContact);
}

void ISession::addIMContactLocally(const IMContact & imContact) {
	QMutexLocker lock(_mutex);

	if (!IMContactListHelper::has(_imContactList, imContact)) {
		_imContactList.push_back(imContact);
		LOG_DEBUG("IMContact added=" + imContact.getContactId());
		contactAddedSignal(imContact);
	}
}

void ISession::removeIMContactLocally(const IMContact & imContact) {
	QMutexLocker lock(_mutex);

	for (IMContactList::iterator it = _imContactList.begin();
		it != _imContactList.end();
		++it) {
		if (((*it).getUUID() == imContact.getUUID())
			|| (((*it).getContactId() == imContact.getContactId())
				&& ((*it).getAccountType() == imContact.getAccountType()))) {
			LOG_DEBUG("IMContact removed=" + (*it).getContactId());
			contactRemovedSignal(*it);
			_imContactList.erase(it);
		}
	}
}

void ISession::setIMContactList(const IMContactList & imContactList) {
	QMutexLocker lock(_mutex);
	_imContactList = imContactList;
}

IMContactList ISession::getIMContactList() const {
	QMutexLocker lock(_mutex);
	return _imContactList;
}
