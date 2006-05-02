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
#ifndef QtUserManager_H
#define QtUserManager_H

#include <QtGui>

class CContact;

#define MAX_ITEM_SIZE 100
#define MIN_ITEM_SIZE 28

class PContact;
class CWengoPhone;
class QtHidenContact;

class QtUserManager : public QObject {
	Q_OBJECT

public:

	QtUserManager(CWengoPhone & cWengoPhone, QObject * parent = 0, QTreeWidget * target = 0 );

	void setContact(PContact * contact) { _pcontact = contact;};

	void removeContact(CContact & cContact);

public Q_SLOTS:

	void treeViewSelectionChanged();

	void itemClicked ( QTreeWidgetItem * item, int column );

	void itemEntered ( QTreeWidgetItem * item );

	void openUserInfo( QTreeWidgetItem * i );

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

Q_SIGNALS:

	void groupRightClicked(const QString & groupName);

protected:

	void safeRemoveContact(CContact & cContact);

	void safeUserStateChanged();

	void safeHideOffLineUsers();

	void safeSortUsers();

	void safeShowAllUsers();

	void removeFromHidenContact(const CContact & cContact);

	QMenu * createMenu();

	QList<QtHidenContact *>  clearList(QList<QtHidenContact *> list);

	bool _hideUsers;

	bool _sortUsers;

	QTreeWidget * _tree;

	QTreeWidgetItem * _previous;

	QTreeWidgetItem * _lastClicked;

	QList<QtHidenContact *> _hidenContacts;

	QSize * _itemSize;

	QMenu * _menu;

	QAction * _callAction;

	QMutex _mutex;

	PContact * _pcontact;

	CWengoPhone & _cWengoPhone;

	Qt::MouseButton _button;
};
/*
class UserManagerEventManager : public QObject {
	Q_OBJECT
public:
	UserManagerEventManager(QTreeWidget * target, QtUserManager * userManager);

protected:
	bool eventFilter(QObject *obj, QEvent *event);

private:
	QtUserManager * _userManager;
};
*/
#endif
