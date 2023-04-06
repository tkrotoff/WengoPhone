/*
 * WengoPhone, voice over Internet phone
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

#ifndef CONTACTLISTWIDGET_H
#define CONTACTLISTWIDGET_H

#include "ContactList.h"

#include <observer/Observer.h>

#include <qobject.h>

class ContactWindow;
class ContactPopupMenu;
class Subject;
class Event;
class QWidget;
class QListView;
class QListViewItem;
class QMainWindow;
class QResizeEvent;
class ContactId;

/**
 * Main widget of the gui: graphical representation of ContactList.
 *
 * Design Pattern Observer.
 * ContactListWidget is updated each time there is a modification to ContactList
 * or to one of the Contact (for example if the name of the Contact has been changed).
 *
 * @see ContactListWidgetForm.ui
 * @author Tanguy Krotoff
 */
class ContactListWidget : public QObject, public Observer {
	Q_OBJECT
public:

	/**
	 * QListView Column number that contains the Contact ID.
	 */
	static const int LISTVIEW_COLUMN_CONTACT_ID;

	/**
	 * Constructs a ContactListWidget using a ContactList.
	 *
	 * @param contactList ContactList that will represent ContactListWidget
	 * @param mainWindow pointer to the main window of the softphone
	 * @param parent pointer to the parent widget
	 */
	ContactListWidget(ContactList & contactList, QMainWindow * mainWindow, QWidget * parent);

	~ContactListWidget();

	/**
	 * Gets the low-level gui widget of this class.
	 *
	 * @return low-level widget
	 */
	QWidget * getWidget() const {
		return _contactListWidget;
	}

	/**
	 * Callback, called each time a Subject has changed.
	 *
	 * @param subject Subject that has been changed
	 * @param event Event
	 * @see Observer::update();
	 */
	virtual void update(const Subject & subject, const Event & event);

	static void removeContact(const ContactId & contactId, QWidget * parent);
	
	ContactWindow * getContactWindow(){
		return _contactWindow;
	}

	/**
	 * QListView Column number that contains the fullname of the Contact.
	 */
	static const int LISTVIEW_COLUMN_CONTACT_FULLNAME;

private slots:

	/**
	 * A new QListViewItem (Contact) has been selected from the ContactListWidget.
	 *
	 * @param contactSelected QListViewItem (Contact) that has been selected
	 */
	void selectionChanged(QListViewItem * contactSelected);

	/**
	 * The user double clicked somewhere on the ContactListWidget.
	 *
	 * @see
	 */
	void doubleClicked(QListViewItem * item, const QPoint & point, int column);

	void deletePressed();

	void returnPressed(QListViewItem * item);

	void resizeEventSlot();

signals:

	/**
	 * Signal when the user click in the gui for adding a Contact.
	 */
	void addContact();

private:

	ContactListWidget(const ContactListWidget &);
	ContactListWidget & operator=(const ContactListWidget &);

	static const int LISTVIEW_RIGHT_MARGIN;

	/**
	 * QListView Column number that contains the wengo phone number of the Contact.
	 */
	static const int LISTVIEW_COLUMN_WENGO_PHONE;

	/**
	 * QListView Column number that contains the mobile phone number of the Contact.
	 */
	static const int LISTVIEW_COLUMN_MOBILE_PHONE;

	/**
	 * QListView Column number that contains the home phone number of the Contact.
	 */
	static const int LISTVIEW_COLUMN_HOME_PHONE;

	/**
	 * QListView Column number that contains the work phone number of the Contact.
	 */
	static const int LISTVIEW_COLUMN_WORK_PHONE;

	/**
	 * ContactList that is represented graphically by this class.
	 */
	ContactList & _contactList;

	/**
	 * Low-level gui widget of this class.
	 */
	QWidget * _contactListWidget;

	/**
	 * QListView component: the graphical list with all the Contact.
	 */
	QListView * _contactListView;

	/**
	 * The Contact that is currently selected in the QListView.
	 */
	Contact * _contactSelected;

	/**
	 * Only for memory leak purpose.
	 */
	ContactWindow * _contactWindow;
	ContactPopupMenu * _contactPopupMenu;

	/**
	 * Needed so that we can resize properly ContactListView.
	 */
	QMainWindow *_mainWindow;
};

#endif	//CONTACTLISTWIDGET_H
