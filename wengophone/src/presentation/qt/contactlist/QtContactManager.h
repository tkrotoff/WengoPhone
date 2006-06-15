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

#ifndef OWQTCONTACTMANAGER_H
#define OWQTCONTACTMANAGER_H

#include <QObject>
#include <QMutex>
#include <QList>

#define MAX_ITEM_SIZE 100
#define MIN_ITEM_SIZE 28

class CUserProfile;
class CWengoPhone;
class QtContactList;
class QtContactInfo;
class PhoneCall;
class ContactProfile;

class QString;
class QTreeWidgetItem;
class QTimerEvent;
class QMenu;
class QTreeWidget;
class QSize;
class QAction;

/**
 * Qt Presentation manager for the ContactList
 *
 * @author Mr K.
 */
class QtContactManager : public QObject {

	Q_OBJECT

public:

	typedef QList <QtContactInfo> QtContactInfoList;

	QtContactManager(CUserProfile & cUserProfile, CWengoPhone & cWengoPhone,
		QtContactList & qtContactList, QObject * parent = 0, QTreeWidget * target = 0);

	void removeContact(const QString & contactId);

	void moveContact(const QString & contactId,
		const QString & srcContactGroupId, const QString & dstContactGroupId);

	bool groupsAreHiden();

	virtual bool event (QEvent * e);

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

	void hideGroups();

	void showAllUsers();

	void sortContacts(bool bypassTimer = false);

	void startChat(bool checked);

	void startSMS(bool checked);

	void startMobileCall(bool checked);

	void startHomeCall(bool checked);

	void startWorkCall(bool checked);

	void startWengoCall(bool checked);

	void setMouseButton(Qt::MouseButton button);

	void inviteToConference();

	void defaultAction(QTreeWidgetItem * item);



Q_SIGNALS:

	void groupRightClicked(const QString & groupName);

	void inviteToConferenceClicked(QString phone, PhoneCall * target);

protected:

	void safeUserStateChanged();

	void safeHideOffLineUsers();

	void safeHideGroup();

	void retranslateUi();

	bool canShowUser(const ContactProfile * cprofile);

	QMenu * createConferenceMenu();

	QMenu * createMenu();

	virtual void timerEvent ( QTimerEvent * event ) ;

	bool _hideUsers;

	bool _sortContacts;

	bool _hideGroups;

	QTreeWidget * _tree;

	QTreeWidgetItem * _previous;

	QTreeWidgetItem * _lastClicked;

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

	int _sortTimerId;

	int _showTimerId;

	bool _canSort;

	bool _wantSort;

	bool _canShow;

	bool _wantShow;

	/**
	* Translated strings
	*/
	QString _trStringCall;
	QString _trStringMobilePhone;
	QString _trStringHomePhone;
	QString _trStringWorkPhone;
	QString _trStringWengoPhone;
	QString _trStringSendSMS;
	QString _trStringEditContact;
	QString _trStringDeleteContact;
	QString _trStringBlockContact;
	QString _trStringForwardToCellPhone;
	QString _trStringStartChat;
	QString _trStringInviteToConference;
};

#endif	//OWQTUSERMANAGER_H
