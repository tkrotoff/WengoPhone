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

#ifndef OWCONTACTLISTMANAGER_H
#define OWCONTACTLISTMANAGER_H


#include "QtContact.h"
#include "QtContactPixmap.h"

#include <QString>

class CContact;
class QMutex;
class QTreeWidget;

/**
 * QtContact storage class for fast lookup
 *
 * @author Mr K.
 */
class QtContactListManager {
public:

	static QtContactListManager * getInstance();

	virtual ~QtContactListManager();

	void addContact(QtContact * user);

	void removeUser(QtContact* user);

	void paintUser(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index);

	void mouseOn(const QString & userid);

	bool contains(const QString & userid) const;

	void contactUpdated(const QString & userid);

	bool hasIM(const QString & userid) const;

	bool hasCall(const QString & userid) const;

	bool hasVideo(const QString & userid) const;

	bool hasPhoneNumber(const QString & userid) const;

	QtContact * getContact(const QString & userid) const;

	QString getPreferredNumber(const QString & userid) const;

	QString getMobilePhone(const QString & userid) const;

	QString getHomePhone(const QString & userid) const;

	QString getWorkPhone(const QString & userid) const;

	void mouseClicked(const QString & userid, const QPoint pos, const QRect & rect);

	void setButton(const QString & userid, const Qt::MouseButton button);

	void setOpenStatus(const QString & userid, bool value);

	Qt::MouseButton getButton(const QString & userid) const;

	int getHeight(const QString & userid) const;

	QString getWengoPhoneNumber(const QString & userid) const;

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

	QtContactListManager();

	QtContactListManager(const QtContactListManager & other) : _tree(other._tree), _contactList(other._contactList) { }

	QtContactListManager & operator = (const QtContactListManager & other) {
		_tree = other._tree;
		_contactList = other._contactList;
		return * this;
	}

private:

	QTreeWidget * _tree;

	QHash <QString,QtContact *> _contactList;

	static QtContactListManager * _instance;

	QString _lastMouseOn;

	QMutex * _mutex;
};

#endif	//OWCONTACTLISTMANAGER_H
