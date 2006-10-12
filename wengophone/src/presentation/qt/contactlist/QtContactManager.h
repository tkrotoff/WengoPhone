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

#include <QtCore/QList>
#include <QtCore/QObject>

class ContactProfile;
class CUserProfile;
class CWengoPhone;
class PhoneCall;
class QtContactList;
class QtContactInfo;
class QtContact;

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
 * @author Philippe Bernery
 */
class QtContactManager : public QObject {
	Q_OBJECT
public:

	QtContactManager(CUserProfile & cUserProfile, CWengoPhone & cWengoPhone,
		QtContactList & qtContactList, QObject * parent, QTreeWidget * target);

	/**
	 * Removes a Contact from the QtContactManager.
	 *
	 * @param contactId the Id of the Contact to remove
	 */
	void removeContact(const QString & contactId);

	/**
	 * Moves a Contact from one group to another.
	 *
	 * @param dstGroupId the Id of the destination group
	 * @param srcGroupId the Id of the source group
	 * @param contactId the Id of the Contact to move
	 */
	void moveContact(const std::string & dstGroupId,
		const std::string & srcGroupId, const std::string & contactId);

	/**
	 * @return true if groups are hidden (not displayed)
	 */
	bool groupsAreHidden() const;

public Q_SLOTS:

	void hideGroups();

	void hideOffLineContacts();

	void setMouseButton(Qt::MouseButton button);

	void userStateChanged();

	void editContact(QString contactId);

private Q_SLOTS:

	void closeUserInfo();

	void defaultAction(QTreeWidgetItem * item);

	void deleteContact();

	void editContact(bool checked);

	void inviteToConference();

	void itemClicked(QTreeWidgetItem * item, int column);

	void openUserInfo(QTreeWidgetItem * item);

	void redrawContacts();

	void sortContacts(bool bypassTimer = false);

	void startChat(bool checked);

	void sendFile(bool checked);

	void startHomeCall(bool checked);

	void startMobileCall(bool checked);

	void startSMS(bool checked);

	void startWengoCall(bool checked);

	void startWorkCall(bool checked);

	void treeViewSelectionChanged();

	void languageChanged();

Q_SIGNALS:

	void groupRightClicked(const QString & groupName);

	void inviteToConferenceClicked(QString phone, PhoneCall * target);

private:

	typedef QList <QtContactInfo> QtContactInfoList;

	void safeSortContacts(bool bypassTimer);

	void safeUserStateChanged();

	void clearTreeSelection();

	void groupClicked(QTreeWidgetItem * item);

	void itemRightClicked(QTreeWidgetItem * item);

	bool canShowUser(const ContactProfile * cprofile);

	QTreeWidgetItem * findQtreeWidgetItem(const QString & data) const;

	QtContact * findContactInGroup(const QTreeWidgetItem * group, const QString & contactId) const;

	QMenu * createConferenceMenu();

	QMenu * createMenu();

	void timerEvent(QTimerEvent * event);

	bool _hideUsers;

	bool _sortContacts;

	bool _hideGroups;

	QTreeWidget * _tree;

	QTreeWidgetItem * _previous;

	QTreeWidgetItem * _lastClicked;

	QSize * _itemSize;

	QMenu * _menu;

	QAction * _callAction;

	Qt::MouseButton _button;

	int _timerId;

	bool _waitForDoubleClick;

	int _sortTimerId;

	int _showTimerId;

	bool _canSort;

	bool _wantSort;

	bool _canShow;

	bool _wantShow;

	CWengoPhone & _cWengoPhone;

	CUserProfile & _cUserProfile;

	QtContactList & _qtContactList;

	/**
	 * Translated strings
	 */
	QString _trStringCall;
	QString _trStringMobilePhone;
	QString _trStringHomePhone;
	QString _trStringWorkPhone;
	QString _trStringWengoPhone;
	QString _trStringSendSMS;
	QString _trStringSendFile;
	QString _trStringEditContact;
	QString _trStringDeleteContact;
	QString _trStringBlockContact;
	QString _trStringForwardToCellPhone;
	QString _trStringStartChat;
	QString _trStringInviteToConference;
};

#endif	//OWQTCONTACTMANAGER_H
