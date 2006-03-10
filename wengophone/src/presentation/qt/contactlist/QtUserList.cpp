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
#include "QtUserList.h"

QtUserList * QtUserList::_instance = 0;

QtUserList * QtUserList::getInstance()
{
	if (_instance == 0)
	{
		_instance = new QtUserList;
	}
	return _instance;
}

QtUserList::QtUserList()
{
}

void QtUserList::paintUser(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index)
{
	QtUser * user;
	user = _userList[index.data().toString()];
	if (user)
		user->paint(painter, option, index);
}

void QtUserList::addUser(QtUser * user)
{
	if (user)
	{
		_userList[user->getId()] = user;
	}
}

void QtUserList::removeUser(const QString & userid)
{
	_userList.remove(userid);
}

void QtUserList::mouseOn(const QString & userid)
{
	QtUser * user;
	user = _userList[userid];
	
	if (user)
		user->setMouseOn(true);
		
	user = _userList[_lastMouseOn];
	if (user)
		user->setMouseOn(false);
	_lastMouseOn = userid;
}

QtUser * QtUserList::getUser(const QString & userid)
{
	QtUser * user;
	user = _userList[userid];
	return user;
}

bool QtUserList::hasIM(const QString & userid){
	QtUser * user = _userList[userid];
	if (user)
		return user->hasIM();
	return false;
}

bool QtUserList::hasCall(const QString & userid){

	QtUser * user = _userList[userid];
	if (user)
		return user->hasCall();
	return false;
}
	
bool QtUserList::hasVideo(const QString & userid){
	QtUser * user = _userList[userid];
	if (user)
		return user->hasVideo();
	return false;
}

void QtUserList::mouseClicked(const QString & userid,const QPoint pos, const QRect & rect)
{
	QtUser * user = _userList[userid];
	if (user)
		user->mouseClicked(pos,rect);
}

int	QtUserList::getIconsStartPosition(const QString & userid)
{
	QtUser * user = _userList[userid];
	if (user)
		return user->getIconsStartPosition();
	return -1;
}

void QtUserList::setButton(const QString & userid,const Qt::MouseButton button)
{
	QtUser * user = _userList[userid];
	if (user)
		user->setButton(button);
}

Qt::MouseButton QtUserList::getButton(const QString & userid)
{
	QtUser * user = _userList[userid];
	if (user)
		return user->getButton();
	return Qt::NoButton;
}

void QtUserList::setOpenStatus(const QString & userid,bool value)
{
	QtUser * user = _userList[userid];
	if (user)
		user->setOpenStatus(value);
}

int	QtUserList::getHeight(const QString & userid){

	QtUser * user = _userList[userid];
	if (user)
		return user->getHeight();
 	else return 0;
}
