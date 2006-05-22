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

#include "QtHidenContact.h"
#include "QtUserList.h"
#include "QtUser.h"

QtHidenContact::QtHidenContact(QTreeWidgetItem * item, QTreeWidgetItem * parentItem, QtUser * user, int index, QObject * parent)
: QObject(parent) {
	_user = user;
	_item = item;
	_parentItem = parentItem;
	_index = index;
	_clear = false;
}

QtHidenContact::QtHidenContact(const QtHidenContact & other) {
	_user = other._user;
	_item = other._item;
	_parentItem = other._parentItem;
	_index = other._index;
	_clear = other._clear;
}

QtHidenContact::~QtHidenContact() {
}

void QtHidenContact::clear() {
	_clear = true;
}

bool QtHidenContact::isCleared() {
	return _clear;
}

QTreeWidgetItem * QtHidenContact::getItem() {
	return _item;
}

QTreeWidgetItem * QtHidenContact::getParentItem() {
	return _parentItem;
}

QtUser * QtHidenContact::getUser() const {
	return _user;
}

int QtHidenContact::getIndex() const {
	return _index;
}

bool QtHidenContact::operator < (const QtHidenContact & other) const {

    //return ((user->getStatus() == EnumPresenceState::PresenceStateOnline) &&
	return (_user->getUserName().toUpper() < other.getUser()->getUserName().toUpper());
}

QtHidenContact & QtHidenContact::operator = (const QtHidenContact & other) {
	_user = other._user;
	_item = other._item;
	_parentItem = other._parentItem;
	_index = other._index;
	_clear = other._clear;
	return * this;
}
