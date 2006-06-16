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

#include <QMutex>
#include <QTreeWidget>

#include "QtContactListManager.h"

QtContactListManager * QtContactListManager::_instance = NULL;

QtContactListManager * QtContactListManager::getInstance() {
	if (!_instance) {
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
		user = _contactList[index.data().toString()];
		user->paint(painter, option, index);
	}
}

void QtContactListManager::addContact(QtContact * user) {
	QMutexLocker locker(_mutex);
	if (user) {
		_contactList[user->getId()] = user;
	}
}

void QtContactListManager::removeUser(QtContact  * user) {
	QMutexLocker locker(_mutex);
	if (user) {
		_contactList.remove(user->getId());
		delete user;
	}
}

void QtContactListManager::mouseOn(const QString & userid) {
	QMutexLocker locker(_mutex);
	QtContact * qtContact;
	if (contains(userid)) {
		qtContact = _contactList[userid];
		if (qtContact)
			qtContact->setMouseOn(true);
		qtContact = _contactList[_lastMouseOn];
		if (qtContact)
			qtContact->setMouseOn(false);
		_lastMouseOn = userid;
	}
}

QtContact * QtContactListManager::getContact(const QString & userid) const {
	QMutexLocker locker(const_cast <QMutex *> (_mutex));
	if (contains(userid)) {
		return _contactList[userid];
	}
	return NULL;
}

QString QtContactListManager::getPreferredNumber(const QString & userid) const {
	QMutexLocker locker(const_cast <QMutex *> (_mutex));
	if (contains(userid)) {
		return _contactList[userid]->getPreferredNumber();
	}
	return QString();
}

QString QtContactListManager::getMobilePhone(const QString & userid) const {
	QMutexLocker locker(const_cast <QMutex *> (_mutex));
	if (contains(userid)) {
		return _contactList[userid]->getMobilePhone();
	}
	return QString();
}

QString QtContactListManager::getHomePhone(const QString & userid) const {
	QMutexLocker locker(const_cast <QMutex *> (_mutex));
	if (contains(userid)) {
		return _contactList[userid]->getHomePhone();
	}
	return QString();
}

QString QtContactListManager::getWorkPhone(const QString & userid) const {
	QMutexLocker locker(const_cast <QMutex *> (_mutex));
	if (contains(userid)) {
		return _contactList[userid]->getHomePhone();
	}
	return QString();
}

bool QtContactListManager::contains(const QString & userid) const {
	QMutexLocker locker(const_cast <QMutex *> (_mutex));
	return _contactList.contains(userid);
}

void QtContactListManager::contactUpdated(const QString & userid) {
	QMutexLocker locker(const_cast <QMutex *> (_mutex));
	if (contains(userid)) {
		_contactList[userid]->contactUpdated();
	}
}

QString QtContactListManager::getWengoPhoneNumber(const QString & userid) const {
	QMutexLocker locker(const_cast <QMutex *> (_mutex));
	if (contains(userid)) {
		return _contactList[userid]->getWengoPhoneNumber();
	}
	return QString();
}

bool QtContactListManager::hasIM(const QString & userid) const {
	QMutexLocker locker(const_cast <QMutex *> (_mutex));
	if (contains(userid)) {
		return _contactList[userid]->hasIM();
	}
	return false;
}

bool QtContactListManager::hasCall(const QString & userid) const {
	QMutexLocker locker(const_cast <QMutex *> (_mutex));
	if (contains(userid)) {
		return _contactList[userid]->hasCall();
	}
	return false;
}

bool QtContactListManager::hasVideo(const QString & userid) const {
	QMutexLocker locker(const_cast <QMutex *> (_mutex));
	if (contains(userid)) {
		return _contactList[userid]->hasVideo();
	}
	return false;
}

bool QtContactListManager::hasPhoneNumber(const QString & userid) const {
	QMutexLocker locker(const_cast <QMutex *> (_mutex));
	if (contains(userid)) {
		return _contactList[userid]->hasPhoneNumber();
	}
	return false;
}

void QtContactListManager::mouseClicked(const QString & userid, const QPoint pos, const QRect & rect) {
	QMutexLocker locker(const_cast <QMutex *> (_mutex));
	if (contains(userid)) {
		_contactList[userid]->mouseClicked(pos, rect);
	}
}

void QtContactListManager::setButton(const QString & userid, const Qt::MouseButton button) {
	QMutexLocker locker(const_cast <QMutex *> (_mutex));
	if (contains(userid)) {
		_contactList[userid]->setButton(button);
	}
}

Qt::MouseButton QtContactListManager::getButton(const QString & userid) const {
	QMutexLocker locker(const_cast <QMutex *> (_mutex));
	if (contains(userid)) {
		return _contactList[userid]->getButton();
	}
	return Qt::NoButton;
}

void QtContactListManager::setOpenStatus(const QString & userid, bool value) {
	QMutexLocker locker(const_cast <QMutex *> (_mutex));
	if (contains(userid)) {
		_contactList[userid]->setOpenStatus(value);
	}
}

int QtContactListManager::getHeight(const QString & userid) const {
	QMutexLocker locker(const_cast <QMutex *> (_mutex));
	QtContact * user = _contactList[userid];
	if (contains(userid)) {
		return _contactList[userid]->getHeight();
	}
	else return 0;
}

void QtContactListManager::resetMouseStatus() {
	QMutexLocker locker(_mutex);
	QHash <QString,QtContact *>::iterator iter;
	for (iter = _contactList.begin(); iter != _contactList.end(); iter++) {
		QtContact * user = iter.value();
		if (user) {
			user->setButton(Qt::NoButton);
		}
	}
}

void QtContactListManager::startChat(const QString & userid) {
	QMutexLocker locker(_mutex);
	if (contains(userid)) {
		locker.unlock();
		_contactList[userid]->startChat();
		locker.relock();
	}
}

void QtContactListManager::startSMS(const QString & userid) {
	QMutexLocker locker(_mutex);
	if (contains(userid)) {
		locker.unlock();
		_contactList[userid]->startSMS();
		locker.relock();
	}
}

void QtContactListManager::startCall(const QString & userid, const QString & number) {
	QMutexLocker locker(_mutex);
	if (contains(userid)) {
		locker.unlock();
		_contactList[userid]->startCall(number);
		locker.relock();
	}
}

void QtContactListManager::startCall(const QString & userid) {
	QMutexLocker locker(_mutex);
	if (contains(userid)) {
		locker.unlock();
		_contactList[userid]->startCall();
		locker.relock();
	}
}

void QtContactListManager::startFreeCall(const QString & userid) {
	QMutexLocker locker(_mutex);
	if (contains(userid)) {
		locker.unlock();
		_contactList[userid]->startFreeCall();
		locker.relock();
	}
}

void QtContactListManager::clear(){
	QMutexLocker locker(_mutex);
	QHash <QString, QtContact *>::iterator iter;
	for (iter=_contactList.begin();iter!=_contactList.end();iter++){
		delete((*iter));
	}
	_contactList.clear();
}
