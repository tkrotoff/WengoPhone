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
#ifndef QTUSERLIST_H
#define QTUSERLIST_H

#include <QtGui>
#include "QtUser.h"
#include "QtContactPixmap.h"
#include "QtUserList.h"

class QtUserList 
{
public:
	static QtUserList * getInstance();
	
	void	addUser(QtUser * user);
	void	removeUser(const QString & userid);
	
	void	paintUser(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index);
	void	mouseOn(const QString & userid);
	void	setUserStatus(const QString & userid,QtContactPixmap::contactPixmap status);
	
	
	bool	haveIM(const QString & userid);
	
	bool	haveCall(const QString & userid);
	
	bool	haveVideo(const QString & userid);

	QtUser * getUser(const QString & userid);
	
	void	mouseClicked(const QString & userid,const QPoint pos, const QRect & rect); 
	
	int		getIconsStartPosition(const QString & userid);
	
	void	setButton(const QString & userid,const Qt::MouseButton button);
	
	void	setOpenStatus(const QString & userid,bool value);
	
	Qt::MouseButton getButton(const QString & userid);
protected:
	QtUserList ( );
	QtUserList (const QtUserList&) {};
	QtUserList & operator= (const QtUserList &) {};
protected:
	QHash<QString,QtUser *>	_userList;
private:
	static QtUserList * _instance;
	QString			    _lastMouseOn;
};
#endif 
