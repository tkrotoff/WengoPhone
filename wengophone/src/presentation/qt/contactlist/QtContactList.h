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

#ifndef QTCONTACTLIST_H
#define QTCONTACTLIST_H

#include "control/contactlist/CContactList.h"
#include "presentation/PContactList.h"

#include <qtutil/QObjectThreadSafe.h>

#include <map>

#include <QTreeWidgetItem>

class Contact;
class ContactPopupMenu;
class ContactGroupPopupMenu;
class QTreeWidget;
class QMouseEvent;
class QtContactGroup;
class QtUserManager;

/**
 * Qt Presentation component for ContactList.
 *
 * @author Tanguy Krotoff
 */
class QtContactList : public QObjectThreadSafe, public PContactList {
	Q_OBJECT
	friend class QtContactGroup;
public:

	QtContactList(CContactList & cContactList);

	QWidget * getWidget() const {
		return _contactListWidget;
	}

	void updatePresentation ();

	void addContactGroup(PContactGroup * pContactGroup);

	CContactList & getContactList() const;

public Q_SLOTS:
	void showAllUsers();

	void hideOffLineUser();

	void sortUsers();

private:

	void initThreadSafe();

	void updatePresentationThreadSafe();

	void addContactGroupThreadSafe(PContactGroup * pContactGroup);

	QtUserManager * _usermanager;

	CContactList & _cContactList;

	QWidget * _contactListWidget;

	ContactPopupMenu * _contactPopupMenu;

	ContactGroupPopupMenu * _contactGroupPopupMenu;

	QTreeWidget * _treeWidget;

	QTreeWidgetItem * _previous;

	QTreeWidgetItem * _lastClicked;

};

#endif	//QTCONTACTLIST_H
