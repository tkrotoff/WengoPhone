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

#ifndef QTCONTACTLIST_H
#define QTCONTACTLIST_H

#include <control/contactlist/CContactList.h>
#include <presentation/PContactList.h>

#include <qtutil/QObjectThreadSafe.h>

#include <map>

#include <QTreeWidgetItem>

class Contact;
class ContactPopupMenu;
class ContactGroupPopupMenu;
class CWengoPhone;
class QTreeWidget;
class QMouseEvent;
class QtUserManager;

/**
 * Qt Presentation component for ContactList.
 *
 * @author Tanguy Krotoff
 */
class QtContactList : public QObjectThreadSafe, public PContactList {
	Q_OBJECT
public:

	QtContactList(CContactList & cContactList, CWengoPhone & cWenghoPhone);

	QWidget * getWidget() const {
		return _contactListWidget;
	}

	QtUserManager * getUserManager() {
		return _userManager;
	}

	void updatePresentation();

	void contactGroupAddedEvent(std::string contactGroupId);

	void contactGroupRemovedEvent(std::string contactGroupId);

	void contactGroupRenamedEvent(std::string contactGroupId);

	void contactAddedEvent(std::string contactId);

	void contactRemovedEvent(std::string contactId);

	void contactMovedEvent(std::string dstContactGroupId,
	std::string srcContactGroupId, std::string contactId);

	void contactChangedEvent(std::string contactId);

	CContactList & getCContactList() const;

Q_SIGNALS:

	void contactGroupAddedEventSignal(QString contactGroupId);

	void contactGroupRemovedEventSignal(QString contactGroupId);

	void contactGroupRenamedEventSignal(QString contactGroupId);

	void contactAddedEventSignal(QString contactId);

	void contactRemovedEventSignal(QString contactId);

	void contactMovedEventSignal(QString dstContactGroupId,
	QString srcContactGroupId, QString contactId);

	void contactChangedEventSignal(QString contactId);

public Q_SLOTS:

	void showAllUsers();

	void hideOffLineUser();

	void sortUsers();

	void contactGroupAddedEventSlot(QString contactGroupId);

private Q_SLOTS:

	void groupRightClickedSlot(const QString & groupName);

	void contactGroupRemovedEventSlot(QString contactGroupId);

	void contactGroupRenamedEventSlot(QString contactGroupId);

	void contactAddedEventSlot(QString contactId);

	void contactRemovedEventSlot(QString contactId);

	void contactMovedEventSlot(QString dstContactGroupId,
	QString srcContactGroupId, QString contactId);

	void contactChangedEventSlot(QString contactId);

private:

	/**
	 * Initializes the Content of the QtContactList.
	 */
	void initContent();

	void initThreadSafe();

	void updatePresentationThreadSafe();

	QtUserManager * _userManager;

	CContactList & _cContactList;

	QWidget * _contactListWidget;

	ContactPopupMenu * _contactPopupMenu;

	ContactGroupPopupMenu * _contactGroupPopupMenu;

	QTreeWidget * _treeWidget;

	CWengoPhone & _cWengoPhone;

	bool _groupHiden;
};

#endif	//QTCONTACTLIST_H
