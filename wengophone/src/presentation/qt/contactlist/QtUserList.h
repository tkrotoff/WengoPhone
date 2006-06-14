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

#ifndef QTUSERLIST_H
#define QTUSERLIST_H

#include <QtGui>
#include "QtContact.h"
#include "QtContactPixmap.h"
#include "QtUserList.h"

class CContact;

class QtUserList {
public:

	static QtUserList * getInstance();

	void addUser(QtContact * user);

	void removeUser(QtContact* user);

	void paintUser(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index);

	void mouseOn(const QString & userid);

	bool hasIM(const QString & userid) const;

	bool hasCall(const QString & userid) const;

	bool hasVideo(const QString & userid) const;

	QtContact * getUser(const QString & userid) const;

	void mouseClicked(const QString & userid, const QPoint pos, const QRect & rect);

	int getIconsStartPosition(const QString & userid) const;

	void setButton(const QString & userid, const Qt::MouseButton button);

	void setOpenStatus(const QString & userid, bool value);

	Qt::MouseButton getButton(const QString & userid) const;

	int getHeight(const QString & userid) const;

	void setTreeWidget(QTreeWidget * tree) { _tree = tree; }

	QTreeWidget * getTreeWidget() const { return _tree; }

	void resetMouseStatus();

	void startChat(const QString & userid);

	void startSMS(const QString & userid);

	void startCall(const QString & userid, const QString & number);

	void startCall(const QString & userid);

	void clear();

	/**
	 * Will start a call with a free phone number.
	 */
	void startFreeCall(const QString & userid);

protected:

	QtUserList();

	QtUserList(const QtUserList & other) : _tree(other._tree), _userList(other._userList) { }

	QtUserList & operator = (const QtUserList & other) {
		_tree = other._tree;
		_userList = other._userList;
		return * this;
	}

	QTreeWidget * _tree;

	QHash <QString,QtContact *> _userList;

private:

	static QtUserList * _instance;

	QString _lastMouseOn;

	QMutex _mutex;
};

#endif	//QTUSERLIST_H
