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

#ifndef QTUSERMANAGER_H
#define QTUSERMANAGER_H

#include <QtGui>

#define MAX_ITEM_SIZE 100
#define MIN_ITEM_SIZE 28

class CUserProfile;
class CWengoPhone;
class QtContactList;
class QtHidenContact;
class PhoneCall;

class QtUserManager : public QObject {
	Q_OBJECT
public:

	QtUserManager(CUserProfile & cUserProfile, CWengoPhone & cWengoPhone,
	QtContactList & qtContactList, QObject * parent = 0, QTreeWidget * target = 0);

	void removeContact(const QString & contactId);

	void moveContact(const QString & contactId,
	const QString & srcContactGroupId, const QString & dstContactGroupId);

public Q_SLOTS:

	void treeViewSelectionChanged();

	void itemClicked(QTreeWidgetItem * item, int column);

    void itemDoubleClicked(QTreeWidgetItem *,int);

	void itemEntered(QTreeWidgetItem * item);

	void openUserInfo(QTreeWidgetItem * i);

	void closeUserInfo();

	void editContact(bool checked);

	void deleteContact();

	void userStateChanged();

	void hideOffLineUsers();

	void showAllUsers();

	void sortUsers();

	void startChat(bool checked);

	void startSMS(bool checked);

	void startMobileCall(bool checked);

	void startHomeCall(bool checked);

	void startWorkCall(bool checked);

	void startWengoCall(bool checked);

	void setMouseButton(Qt::MouseButton button);

	void inviteToConference();

Q_SIGNALS:

	void groupRightClicked(const QString & groupName);

	void inviteToConferenceClicked(QString phone, PhoneCall * target);

protected:

	void safeRemoveContact(const QString & contactId);

	void safeUserStateChanged();

	void safeHideOffLineUsers();

	void safeSortUsers();

	void safeShowAllUsers();

	void removeFromHidenContact(const QString & contactId);

    void defaultAction(QTreeWidgetItem * item);

	virtual void timerEvent ( QTimerEvent * event ) ;

	QMenu * createConferenceMenu();

	QMenu * createMenu();

	QList < QtHidenContact * > clearList(QList < QtHidenContact * > list);

	bool _hideUsers;

	bool _sortUsers;

	QTreeWidget * _tree;

	QTreeWidgetItem * _previous;

	QTreeWidgetItem * _lastClicked;

	QList < QtHidenContact * > _hidenContacts;

	QSize * _itemSize;

	QMenu * _menu;

	QAction * _callAction;

	QMutex _mutex;

	QtContactList & _qtContactList;

	CUserProfile & _cUserProfile;

	CWengoPhone & _cWengoPhone;

	Qt::MouseButton _button;

	int _timerId;

	bool _waitForDoubleClick;

};

#endif	//QTUSERMANAGER_H
