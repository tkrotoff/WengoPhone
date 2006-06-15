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

#include <util/Logger.h>

#include "QtContactListManager.h"

QtContactListManager * QtContactListManager::_instance = 0;

QtContactListManager * QtContactListManager::getInstance() {
	if (_instance == 0) {
		_instance = new QtContactListManager;
	}
	return _instance;
}

QtContactListManager::QtContactListManager() {
	_mutex = new QMutex(QMutex::Recursive);
}

QtContactListManager::~QtContactListManager() {
	delete _mutex;
}
void QtContactListManager::paintUser(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index) {
	QMutexLocker locker(_mutex);
	QtContact * user;
	if (contains(index.data().toString())) {
		user = _userList[index.data().toString()];
		user->paint(painter, option, index);
	}
}

void QtContactListManager::addContact(QtContact * user) {
	QMutexLocker locker(_mutex);
	if (user) {
		_userList[user->getId()] = user;
	}
}

void QtContactListManager::removeUser(QtContact  * user) {
	QMutexLocker locker(_mutex);
	if (user) {
		_userList.remove(user->getId());
		delete user;
	}
}

void QtContactListManager::mouseOn(const QString & userid) {
	QMutexLocker locker(_mutex);
	QtContact * qtContact;
	qtContact = _userList[userid];
	if (qtContact)
		qtContact->setMouseOn(true);
	qtContact = _userList[_lastMouseOn];
	if (qtContact)
		qtContact->setMouseOn(false);
	_lastMouseOn = userid;
}

QtContact * QtContactListManager::getContact(const QString & userid) const {
	QMutexLocker locker(const_cast <QMutex *> (_mutex));
	if (contains(userid)) {
		return _userList[userid];
	}
	return NULL;
}

QString QtContactListManager::getPreferredNumber(const QString & userid) const {
	QMutexLocker locker(const_cast <QMutex *> (_mutex));
	if (contains(userid)) {
		return _userList[userid]->getPreferredNumber();
	}
	return QString();
}

QString QtContactListManager::getMobilePhone(const QString & userid) const {
	QMutexLocker locker(const_cast <QMutex *> (_mutex));
	if (contains(userid)) {
		return _userList[userid]->getMobilePhone();
	}
	return QString();
}

QString QtContactListManager::getHomePhone(const QString & userid) const {
	QMutexLocker locker(const_cast <QMutex *> (_mutex));
	if (contains(userid)) {
		return _userList[userid]->getHomePhone();
	}
	return QString();
}

QString QtContactListManager::getWorkPhone(const QString & userid) const {
	QMutexLocker locker(const_cast <QMutex *> (_mutex));
	if (contains(userid)) {
		return _userList[userid]->getHomePhone();
	}
	return QString();
}

bool QtContactListManager::contains(const QString & userid) const {
	QMutexLocker locker(const_cast <QMutex *> (_mutex));
	return _userList.contains(userid);
}

void QtContactListManager::contactUpdated(const QString & userid) {
	QMutexLocker locker(const_cast <QMutex *> (_mutex));
	if (contains(userid)) {
		_userList[userid]->contactUpdated();
	}
}

QString QtContactListManager::getWengoPhoneNumber(const QString & userid) const {
	QMutexLocker locker(const_cast <QMutex *> (_mutex));
	if (contains(userid)) {
		return _userList[userid]->getWengoPhoneNumber();
	}
	return QString();
}

bool QtContactListManager::hasIM(const QString & userid) const {
	QMutexLocker locker(const_cast <QMutex *> (_mutex));
	QtContact * user = _userList[userid];
	if (user)
		return user->hasIM();
	return false;
}

bool QtContactListManager::hasCall(const QString & userid) const {
	QMutexLocker locker(const_cast <QMutex *> (_mutex));
	QtContact * user = _userList[userid];
	if (user)
		return user->hasCall();
	return false;
}

bool QtContactListManager::hasVideo(const QString & userid) const {
	QMutexLocker locker(const_cast <QMutex *> (_mutex));
	QtContact * user = _userList[userid];
	if (user)
		return user->hasVideo();
	return false;
}

bool QtContactListManager::hasPhoneNumber(const QString & userid) const {
	QMutexLocker locker(const_cast <QMutex *> (_mutex));
	if (contains(userid)) {
		return _userList[userid]->hasPhoneNumber();
	}
	return false;
}

void QtContactListManager::mouseClicked(const QString & userid, const QPoint pos, const QRect & rect) {
	QMutexLocker locker(const_cast <QMutex *> (_mutex));
	QtContact * user = _userList[userid];
	if (user)
		user->mouseClicked(pos, rect);
}

void QtContactListManager::setButton(const QString & userid, const Qt::MouseButton button) {
	QMutexLocker locker(const_cast <QMutex *> (_mutex));
	QtContact * user = _userList[userid];
	if (user)
		user->setButton(button);
}

Qt::MouseButton QtContactListManager::getButton(const QString & userid) const {
	QMutexLocker locker(const_cast <QMutex *> (_mutex));
	QtContact * user = _userList[userid];
	if (user)
		return user->getButton();
	return Qt::NoButton;
}

void QtContactListManager::setOpenStatus(const QString & userid, bool value) {
	QMutexLocker locker(const_cast <QMutex *> (_mutex));
	QtContact * user = _userList[userid];
	if (user)
		user->setOpenStatus(value);
}

int QtContactListManager::getHeight(const QString & userid) const {
	QMutexLocker locker(const_cast <QMutex *> (_mutex));
	QtContact * user = _userList[userid];
	if (user)
		return user->getHeight();
	else return 0;
}

void QtContactListManager::resetMouseStatus() {
	QMutexLocker locker(_mutex);
	QHash <QString,QtContact *>::iterator iter;
	for (iter = _userList.begin(); iter != _userList.end(); iter++) {
		QtContact * user = iter.value();
		if (user != NULL) {
			user->setButton(Qt::NoButton);
		}
	}
}

void QtContactListManager::startChat(const QString & userid) {
	_mutex->lock();
	QtContact * user = _userList[userid];
	if (!user) {
		_mutex->unlock();
		return;
	}
	_mutex->unlock();
	user->startChat();
}

void QtContactListManager::startSMS(const QString & userid) {
	_mutex->lock();
	QtContact * user=_userList[userid];
	if (!user)
		LOG_FATAL("User lookup failed !!!");
	_mutex->unlock();
	user->startSMS();

}

void QtContactListManager::startCall(const QString & userid, const QString & number) {
	_mutex->lock();
	QtContact * user = _userList[userid];
	if (!user)
		LOG_FATAL("User lookup failed !!!");
	_mutex->unlock();
	user->startCall(number);
}


void QtContactListManager::startCall(const QString & userid) {
	_mutex->lock();
	QtContact * user = _userList[userid];
	if (!user)
		LOG_FATAL("User lookup failed !!!");
	_mutex->unlock();
	user->startCall();
}

void QtContactListManager::startFreeCall(const QString & userid) {
	_mutex->lock();
	QtContact * user = _userList[userid];
	if (!user)
		LOG_FATAL("User lookup failed !!!");
	_mutex->unlock();

	user->startFreeCall();
}


void QtContactListManager::clear(){
	QMutexLocker locker(_mutex);
	QHash <QString, QtContact *>::iterator iter;
	for (iter=_userList.begin();iter!=_userList.end();iter++){
		delete((*iter));
	}
	_userList.clear();
}
