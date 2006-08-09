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

#ifndef OWQTCONTACTLIST_H
#define OWQTCONTACTLIST_H

#include <control/contactlist/CContactList.h>
#include <presentation/PContactList.h>

#include <qtutil/QObjectThreadSafe.h>

#include <QString>

class Contact;
class ContactPopupMenu;
class ContactGroupPopupMenu;
class CWengoPhone;
class QtContactManager;

class QTreeWidgetItem;
namespace Ui { class ContactList; }

/**
 * Qt Presentation component for ContactList.
 *
 * @author Tanguy Krotoff
 */
class QtContactList : public QObjectThreadSafe, public PContactList {
	Q_OBJECT
public:

	static const QString DEFAULT_GROUP_NAME;
	static const QString STATUS_UNKNOW_PIXMAP;
	static const QString STATUS_ONLINE_PIXMAP;
	static const QString STATUS_OFFLINE_PIXMAP;
	static const QString STATUS_DND_PIXMAP;
	static const QString STATUS_INVISIBLE_PIXMAP;
	static const QString STATUS_AWAY_PIXMAP;
	static const QString STATUS_FORWARD_PIXMAP;
	static const QString STATUS_CONTACT_IM_PIXMAP;
	static const QString STATUS_CONTACT_CALL_PIXMAP;
	static const QString STATUS_CONTACT_VIDEO_PIXMAP;
	static const QString STATUS_GROUP_OPEN_PIXMAP;
	static const QString STATUS_GROUP_CLOSE_PIXMAP;

	QtContactList(CContactList & cContactList, CWengoPhone & cWenghoPhone);

	virtual ~QtContactList();

	QWidget * getWidget() const {
		return _contactListWidget;
	}

	QtContactManager * getContactManager() const {
		return _contactManager;
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

	void cleanup();

	void redrawContacts();

	void hideOffLineContacts();

	void sortContacts();

	void showHideGroups();

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

	void mergeContactsSlot(QString dstContact, QString srcContact);

private:

	/**
	 * Initializes the Content of the QtContactList.
	 */
	void initContent();

	void initThreadSafe();

	void updatePresentationThreadSafe();

	QTreeWidgetItem * addGroup(QString contactGroupId);

	QtContactManager * _contactManager;

	CContactList & _cContactList;

	Ui::ContactList * _ui;

	QWidget * _contactListWidget;

	ContactPopupMenu * _contactPopupMenu;

	ContactGroupPopupMenu * _contactGroupPopupMenu;

	/** True when model is doing some things on contacts. */
	bool _waitingForModel;

	CWengoPhone & _cWengoPhone;
};

#endif	//OWQTCONTACTLIST_H

