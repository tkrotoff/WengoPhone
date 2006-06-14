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

#include "QtUserList.h"

QtUserList * QtUserList::_instance = 0;

QtUserList * QtUserList::getInstance() {
	if (_instance == 0) {
		_instance = new QtUserList;
	}
	return _instance;
}

QtUserList::QtUserList() {
	_mutex = new QMutex(QMutex::Recursive);
}

QtUserList::~QtUserList() {
	delete _mutex;
}
void QtUserList::paintUser(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index) {
	QMutexLocker locker(_mutex);
	QtContact * user;
	if (contains(index.data().toString())) {
		user = _userList[index.data().toString()];
		user->paint(painter, option, index);
	}
}

void QtUserList::addContact(QtContact * user) {
	QMutexLocker locker(_mutex);
	if (user) {
		_userList[user->getId()] = user;
	}
}

void QtUserList::removeUser(QtContact  * user) {
	QMutexLocker locker(_mutex);
	if (user) {
		_userList.remove(user->getId());
		delete user;
	}
}

void QtUserList::mouseOn(const QString & userid) {
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

QtContact * QtUserList::getContact(const QString & userid) const {
	QMutexLocker locker(const_cast <QMutex *> (_mutex));
	if (contains(userid)) {
		return _userList[userid];
	}
	return NULL;
}

QString QtUserList::getPreferredNumber(const QString & userid) const {
	QMutexLocker locker(const_cast <QMutex *> (_mutex));
	if (contains(userid)) {
		return _userList[userid]->getPreferredNumber();
	}
	return QString();
}

QString QtUserList::getMobilePhone(const QString & userid) const {
	QMutexLocker locker(const_cast <QMutex *> (_mutex));
	if (contains(userid)) {
		return _userList[userid]->getMobilePhone();
	}
	return QString();
}

QString QtUserList::getHomePhone(const QString & userid) const {
	QMutexLocker locker(const_cast <QMutex *> (_mutex));
	if (contains(userid)) {
		return _userList[userid]->getHomePhone();
	}
	return QString();
}

QString QtUserList::getWorkPhone(const QString & userid) const {
	QMutexLocker locker(const_cast <QMutex *> (_mutex));
	if (contains(userid)) {
		return _userList[userid]->getHomePhone();
	}
	return QString();
}

bool QtUserList::contains(const QString & userid) const {
	QMutexLocker locker(const_cast <QMutex *> (_mutex));
	return _userList.contains(userid);
}

void QtUserList::contactUpdated(const QString & userid) {
	QMutexLocker locker(const_cast <QMutex *> (_mutex));
	if (contains(userid)) {
		_userList[userid]->contactUpdated();
	}
}

QString QtUserList::getWengoPhoneNumber(const QString & userid) const {
	QMutexLocker locker(const_cast <QMutex *> (_mutex));
	if (contains(userid)) {
		return _userList[userid]->getWengoPhoneNumber();
	}
	return QString();
}

bool QtUserList::hasIM(const QString & userid) const {

	QMutexLocker locker(const_cast <QMutex *> (_mutex));
	QtContact * user = _userList[userid];
	if (user)
		return user->hasIM();
	return false;
}

bool QtUserList::hasCall(const QString & userid) const {

	QMutexLocker locker(const_cast <QMutex *> (_mutex));
	QtContact * user = _userList[userid];
	if (user)
		return user->hasCall();
	return false;
}

bool QtUserList::hasVideo(const QString & userid) const {
	QMutexLocker locker(const_cast <QMutex *> (_mutex));
	QtContact * user = _userList[userid];
	if (user)
		return user->hasVideo();
	return false;
}

bool QtUserList::hasPhoneNumber(const QString & userid) const {
	QMutexLocker locker(const_cast <QMutex *> (_mutex));
	if (contains(userid)) {
		return _userList[userid]->hasPhoneNumber();
	}
	return false;
}

void QtUserList::mouseClicked(const QString & userid, const QPoint pos, const QRect & rect) {
	QMutexLocker locker(const_cast <QMutex *> (_mutex));
	QtContact * user = _userList[userid];
	if (user)
		user->mouseClicked(pos, rect);
}

void QtUserList::setButton(const QString & userid, const Qt::MouseButton button) {
	QMutexLocker locker(const_cast <QMutex *> (_mutex));
	QtContact * user = _userList[userid];
	if (user)
		user->setButton(button);
}

Qt::MouseButton QtUserList::getButton(const QString & userid) const {
	QMutexLocker locker(const_cast <QMutex *> (_mutex));
	QtContact * user = _userList[userid];
	if (user)
		return user->getButton();
	return Qt::NoButton;
}

void QtUserList::setOpenStatus(const QString & userid, bool value) {
	QMutexLocker locker(const_cast <QMutex *> (_mutex));
	QtContact * user = _userList[userid];
	if (user)
		user->setOpenStatus(value);
}

int QtUserList::getHeight(const QString & userid) const {

	QMutexLocker locker(const_cast <QMutex *> (_mutex));
	QtContact * user = _userList[userid];
	if (user)
		return user->getHeight();
	else return 0;
}

void QtUserList::resetMouseStatus() {

	QMutexLocker locker(_mutex);

	QHash <QString,QtContact *>::iterator iter;

	for (iter = _userList.begin(); iter != _userList.end(); iter++) {
		QtContact * user = iter.value();
		if (user != NULL) {
			user->setButton(Qt::NoButton);
		}
	}
}

void QtUserList::startChat(const QString & userid) {

	_mutex->lock();
	QtContact * user = _userList[userid];
	if (!user) {
		_mutex->unlock();
		return;
	}
	_mutex->unlock();
	user->startChat();
}

void QtUserList::startSMS(const QString & userid) {
	_mutex->lock();
	QtContact * user=_userList[userid];
	if (!user)
		LOG_FATAL("User lookup failed !!!");
	_mutex->unlock();
	user->startSMS();

}

void QtUserList::startCall(const QString & userid, const QString & number) {
	_mutex->lock();
	QtContact * user = _userList[userid];
	if (!user)
		LOG_FATAL("User lookup failed !!!");
	_mutex->unlock();
	user->startCall(number);
}


void QtUserList::startCall(const QString & userid) {
	_mutex->lock();
	QtContact * user = _userList[userid];
	if (!user)
		LOG_FATAL("User lookup failed !!!");
	_mutex->unlock();
	user->startCall();
}

void QtUserList::startFreeCall(const QString & userid) {
	_mutex->lock();
	QtContact * user = _userList[userid];
	if (!user)
		LOG_FATAL("User lookup failed !!!");
	_mutex->unlock();

	user->startFreeCall();
}


void QtUserList::clear(){
	QMutexLocker locker(_mutex);
	QHash <QString, QtContact *>::iterator iter;
	for (iter=_userList.begin();iter!=_userList.end();iter++){
		delete((*iter));
	}
	_userList.clear();
}
