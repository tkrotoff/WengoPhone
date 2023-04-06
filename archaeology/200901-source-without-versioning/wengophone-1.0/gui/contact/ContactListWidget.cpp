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

#include "ContactWidget.h"

#include "MyWidgetFactory.h"
#include "ContactListWidget.h"
#include "AddContactEvent.h"
#include "RemoveContactEvent.h"
#include "UpdateContactEvent.h"
#include "PresenceContactEvent.h"
#include "util/EventFilter.h"
#include "ContactWindow.h"
#include "ContactPopupMenu.h"
#include "AudioCallManager.h"
#include "presence/PresenceStatus.h"
#include "sip/SipAddress.h"
#include "util/MyMessageBox.h"

#include <observer/Subject.h>
#include <observer/Event.h>

#include <qapplication.h>
#include <qwidget.h>
#include <qtooltip.h>
#include <qlistview.h>
#include <qheader.h>
#include <qstring.h>
#include <qtabwidget.h>
#include <qaction.h>
#include <qdialog.h>
#include <qevent.h>
#include <qmainwindow.h>
#include <qwidgetstack.h>

#include <cassert>
#include <iostream>
using namespace std;

const int ContactListWidget::LISTVIEW_COLUMN_CONTACT_ID = 5;
const int ContactListWidget::LISTVIEW_RIGHT_MARGIN = 130;
const int ContactListWidget::LISTVIEW_COLUMN_WENGO_PHONE = 0;
const int ContactListWidget::LISTVIEW_COLUMN_CONTACT_FULLNAME = 1;
const int ContactListWidget::LISTVIEW_COLUMN_MOBILE_PHONE = 2;
const int ContactListWidget::LISTVIEW_COLUMN_HOME_PHONE = 3;
const int ContactListWidget::LISTVIEW_COLUMN_WORK_PHONE = 4;

ContactListWidget::ContactListWidget(ContactList & contactList, QMainWindow * mainWindow, QWidget * parent)
:  QObject(parent), _contactList(contactList) {

	_mainWindow = mainWindow;

	_contactListWidget = MyWidgetFactory::create("ContactListWidgetForm.ui", this, parent);

	//_contactListView (QListView)
	_contactListView = (QListView *) _contactListWidget->child("contactListView", "QListView");
	_contactListView->setSorting(LISTVIEW_COLUMN_CONTACT_FULLNAME);
	//_contactListView->setBackgroundPixmap(QPixmap::fromMimeSource("Fond-W9engo.png"));
	_contactListView->setColumnWidthMode(LISTVIEW_COLUMN_CONTACT_FULLNAME, QListView::Manual);
	_contactListView->setColumnWidth(LISTVIEW_COLUMN_CONTACT_FULLNAME,
			mainWindow->width() - LISTVIEW_RIGHT_MARGIN);
	_contactListView->setColumnWidthMode(LISTVIEW_COLUMN_CONTACT_FULLNAME, QListView::Maximum);
	_contactListView->setColumnWidthMode(LISTVIEW_COLUMN_CONTACT_ID, QListView::Manual);
	_contactListView->setColumnAlignment(LISTVIEW_COLUMN_WENGO_PHONE, Qt::AlignHCenter);
	_contactListView->setColumnAlignment(LISTVIEW_COLUMN_MOBILE_PHONE, Qt::AlignHCenter);
	_contactListView->setColumnAlignment(LISTVIEW_COLUMN_HOME_PHONE, Qt::AlignHCenter);
	_contactListView->setColumnAlignment(LISTVIEW_COLUMN_WORK_PHONE, Qt::AlignHCenter);
	_contactListView->hideColumn(LISTVIEW_COLUMN_CONTACT_ID);
	_contactListView->header()->hide();

	//For the design pattern observer
	_contactList.addObserver(*this);

	_contactWindow = new ContactWindow(parent, NULL);

	QAction * addContactAction = (QAction *) mainWindow->child("addContactAction", "QAction");
	connect(addContactAction, SIGNAL(activated()),
		_contactWindow, SLOT(execAddContact()));
	QAction * addContactActionNoIcon = (QAction *) mainWindow->child("addContactActionNoIcon", "QAction");
	connect(addContactActionNoIcon, SIGNAL(activated()),
		_contactWindow, SLOT(execAddContact()));
	connect(this, SIGNAL(addContact()),
		_contactWindow, SLOT(execAddContact()));

	connect(_contactListView, SIGNAL(selectionChanged(QListViewItem *)),
		this, SLOT(selectionChanged(QListViewItem *)));

	connect(_contactListView, SIGNAL(doubleClicked(QListViewItem *, const QPoint &, int)),
		this, SLOT(doubleClicked(QListViewItem *, const QPoint &, int)));

	//Delete key press filter
	DeleteKeyEventFilter * deleteKeyEventFilter = new DeleteKeyEventFilter(this, SLOT(deletePressed()));
	_contactListView->installEventFilter(deleteKeyEventFilter);

	connect(_contactListView, SIGNAL(returnPressed(QListViewItem *)),
		this, SLOT(returnPressed(QListViewItem *)));

	_contactPopupMenu = new ContactPopupMenu(parent);

	connect(_contactListView, SIGNAL(rightButtonClicked(QListViewItem *, const QPoint &, int)),
		_contactPopupMenu, SLOT(showMenu(QListViewItem *, const QPoint &, int)));

	ResizeEventFilter * resizeEventFilter = new ResizeEventFilter(this, SLOT(resizeEventSlot()));
	_contactListView->installEventFilter(resizeEventFilter);

	// add a text if no contact has been found
	if( _contactListView->childCount() == 0 ) {
		QListViewItem * item = new QListViewItem(_contactListView);
		item->setText(LISTVIEW_COLUMN_CONTACT_FULLNAME, tr("No contact") );
	} else {
		QListViewItem * item = _contactListView->findItem( tr("No contact"), LISTVIEW_COLUMN_CONTACT_FULLNAME);
		if( item ) {
			_contactListView->takeItem(item);
		}
	}
}

void ContactListWidget::resizeEventSlot()
{
	_contactListView->setColumnWidth(LISTVIEW_COLUMN_CONTACT_FULLNAME, _mainWindow->width() - LISTVIEW_RIGHT_MARGIN);
}

ContactListWidget::~ContactListWidget() {
	delete _contactPopupMenu;
	delete _contactWindow;
}

void ContactListWidget::update(const Subject & /*subject*/, const Event & event) {
	QString typeEvent = event.getTypeEvent();

	cerr << "ContactListWidget::update" << endl;

	if (typeEvent == "AddContactEvent") {
		const AddContactEvent & evt = (const AddContactEvent &)event;
		Contact & contact = evt.getContact();
		contact.addObserver(*this);

		QListViewItem * item = new QListViewItem(_contactListView);
		item->setText(LISTVIEW_COLUMN_CONTACT_FULLNAME, contact.toString());
		item->setText(LISTVIEW_COLUMN_CONTACT_ID, contact.getId().toString());

		if (contact.isBlocked()) {
			item->setText(LISTVIEW_COLUMN_CONTACT_FULLNAME, contact.toString() +
					"\t(" + tr("Blocked") + ")");
		}

		if (!contact.getWengoPhone().isEmpty()) {
			item->setPixmap(LISTVIEW_COLUMN_WENGO_PHONE, QPixmap::fromMimeSource("contact_phone_wengo.png"));
		}

		if (!contact.getMobilePhone().isEmpty()) {
			item->setPixmap(LISTVIEW_COLUMN_MOBILE_PHONE, QPixmap::fromMimeSource("contact_phone_mobile.png"));
		}

		if (!contact.getHomePhone().isEmpty()) {
			item->setPixmap(LISTVIEW_COLUMN_HOME_PHONE, QPixmap::fromMimeSource("contact_phone_home.png"));
		}

		if (!contact.getWorkPhone().isEmpty()) {
			item->setPixmap(LISTVIEW_COLUMN_WORK_PHONE, QPixmap::fromMimeSource("contact_phone_work.png"));
		}
	}

	else if (typeEvent == "RemoveContactEvent") {
		const RemoveContactEvent & evt = (const RemoveContactEvent &)event;
		Contact & contact = evt.getContact();
		contact.removeObserver(*this);

		QListViewItemIterator it(_contactListView);
		while (it.current()) {
			QListViewItem * item = it.current();

			if (item->text(LISTVIEW_COLUMN_CONTACT_ID) == contact.getId().toString()) {
				_contactListView->takeItem(item);
				break;
			}
			++it;
		}
	}

	else if (typeEvent == "UpdateContactEvent") {
		const UpdateContactEvent & evt = (const UpdateContactEvent &)event;
		const Contact & contact = evt.getContact();

		QListViewItemIterator it(_contactListView);
		while (it.current()) {
			QListViewItem * item = it.current();

			if (item->text(LISTVIEW_COLUMN_CONTACT_ID) == contact.getId().toString()) {

				item->setText(LISTVIEW_COLUMN_CONTACT_FULLNAME, contact.toString());

				if (contact.isBlocked()) {
					item->setText(LISTVIEW_COLUMN_CONTACT_FULLNAME, contact.toString() +
							"\t(" + tr("Blocked") + ")");
					break;
				}

				if (!contact.getWengoPhone().isEmpty()) {
					const PresenceStatus & presenceStatus = contact.getPresenceStatus();
					item->setPixmap(LISTVIEW_COLUMN_WENGO_PHONE, presenceStatus.getStatusIcon());
					if (presenceStatus.getStatus() == PresenceStatus::UserDefine) {
						item->setText(LISTVIEW_COLUMN_CONTACT_FULLNAME, contact.toString() +
							"\t(" + QString::fromUtf8(presenceStatus.getStatusText()) + ")");
					} else {
						item->setText(LISTVIEW_COLUMN_CONTACT_FULLNAME, contact.toString() +
							"\t(" + presenceStatus.getStatusTextTranslated() + ")");
					}
				} else {
					item->setPixmap(LISTVIEW_COLUMN_WENGO_PHONE, NULL);
				}

				if (!contact.isWengoPhoneValid()) {
					item->setPixmap(LISTVIEW_COLUMN_WENGO_PHONE, NULL);
					item->setText(LISTVIEW_COLUMN_CONTACT_FULLNAME, contact.toString() +
							"\t(" + tr("Wengo address not valid!") + ")");
				}

				if (!contact.getMobilePhone().isEmpty()) {
					item->setPixmap(LISTVIEW_COLUMN_MOBILE_PHONE, QPixmap::fromMimeSource("contact_phone_mobile.png"));
				} else {
					item->setPixmap(LISTVIEW_COLUMN_MOBILE_PHONE, NULL);
				}

				if (!contact.getHomePhone().isEmpty()) {
					item->setPixmap(LISTVIEW_COLUMN_HOME_PHONE, QPixmap::fromMimeSource("contact_phone_home.png"));
				} else {
					item->setPixmap(LISTVIEW_COLUMN_HOME_PHONE, NULL);
				}

				if (!contact.getWorkPhone().isEmpty()) {
					item->setPixmap(LISTVIEW_COLUMN_WORK_PHONE, QPixmap::fromMimeSource("contact_phone_work.png"));
				} else {
					item->setPixmap(LISTVIEW_COLUMN_WORK_PHONE, NULL);
				}
				break;
			}
			++it;
		}
	}

	else if (typeEvent == "PresenceContactEvent") {
		/*const PresenceContactEvent & evt = dynamic_cast<const PresenceContactEvent &>(event);
		const Contact & contact = evt.getContact();*/
	}

	else {
		assert(NULL && "Unknown message event");
	}
	
	// add a text if no contact has been found
	if( _contactListView->childCount() == 0 ) {
		QListViewItem * item = new QListViewItem(_contactListView);
		item->setText(LISTVIEW_COLUMN_CONTACT_FULLNAME, tr("No contact") );
	} else {
		QListViewItem * item = _contactListView->findItem( tr("No contact"), LISTVIEW_COLUMN_CONTACT_FULLNAME);
		if( item ) {
			_contactListView->takeItem(item);
		}
	}
	
	resizeEventSlot();
	_contactListView->sort();
}

void ContactListWidget::selectionChanged(QListViewItem * contactSelected) {
	for (unsigned int i = 0; i < _contactList.size(); i++) {
		if (_contactList[i].getId().toString() ==
			contactSelected->text(LISTVIEW_COLUMN_CONTACT_ID)) {
				_contactSelected = & _contactList[i];
				break;
		}
	}
}

void ContactListWidget::doubleClicked(QListViewItem * item, const QPoint & /*point*/, int column) {
	if (item == NULL || column == -1) {
		//NULL if the user didn't click on an item
		emit addContact();
	} else {
		
		if( item->text(LISTVIEW_COLUMN_CONTACT_FULLNAME) != tr("No contact") ) {
			AudioCallManager & audioCallManager = AudioCallManager::getInstance();
	
			//The user clicked on an item
			ContactList & contactList = ContactList::getInstance();
			Contact & contact = contactList.getContact(ContactId(item->text(LISTVIEW_COLUMN_CONTACT_ID)));
	
			switch(column) {
			case LISTVIEW_COLUMN_WENGO_PHONE:
				if (!contact.getWengoPhone().isEmpty() && 
					contact.getPresenceStatus().getStatus() != PresenceStatus::Offline) {
	
					audioCallManager.startChat(contact);
				}
				break;
	
			case LISTVIEW_COLUMN_MOBILE_PHONE:
				if (!contact.getMobilePhone().isEmpty()) {
					audioCallManager.createAudioCall(SipAddress::fromPhoneNumber(contact.getMobilePhone()),
									&contact);
				}
				break;
	
			case LISTVIEW_COLUMN_HOME_PHONE:
				if (!contact.getHomePhone().isEmpty()) {
					audioCallManager.createAudioCall(SipAddress::fromPhoneNumber(contact.getHomePhone()),
									&contact);
				}
				break;
	
			case LISTVIEW_COLUMN_WORK_PHONE:
				if (!contact.getWorkPhone().isEmpty()) {
					audioCallManager.createAudioCall(SipAddress::fromPhoneNumber(contact.getWorkPhone()),
									&contact);
				}
				break;
	
			default:
				if (!contact.getDefaultPhone().isEmpty()) {
					audioCallManager.createAudioCall(SipAddress::fromPhoneNumber(contact.getDefaultPhone()),
									&contact);
				}
				break;
			}
		}
	}
}

void ContactListWidget::deletePressed() {
	QListViewItem * item = _contactListView->selectedItem();
	if (item == NULL) {
		return;
	}

	//No action for the "No contact" entry
	if( item->text(LISTVIEW_COLUMN_CONTACT_FULLNAME) == tr("No contact") ) {
		return;
	}

	removeContact(ContactId(item->text(LISTVIEW_COLUMN_CONTACT_ID)), _contactListView);
}

void ContactListWidget::removeContact(const ContactId & contactId, QWidget * parent) {
	ContactList & contactList = ContactList::getInstance();
	Contact & contact = contactList.getContact(contactId);

	int buttonClicked = MyMessageBox::question(parent,
			tr("Wengo"),
			tr("Are you sure you want to delete ") + contact.toString() + tr("?"),
			tr("&Delete"), tr("&Cancel"));

	switch(buttonClicked) {
	default:
	case QMessageBox::Yes: {
		contactList.removeContact(contact);
		break;
	}
	case QMessageBox::No:
		break;
	}
}

void ContactListWidget::returnPressed(QListViewItem * item) {
	if (item == NULL) {
		return;
	}

	//No action for the "No contact" entry
	if( item->text(LISTVIEW_COLUMN_CONTACT_FULLNAME) == tr("No contact") ) {
		return;
	}

	//The user clicked on an item
	ContactList & contactList = ContactList::getInstance();
	Contact & contact = contactList.getContact(ContactId(item->text(LISTVIEW_COLUMN_CONTACT_ID)));

	AudioCallManager & audioCallManager = AudioCallManager::getInstance();
	audioCallManager.createAudioCall(SipAddress::fromPhoneNumber(contact.getDefaultPhone()),
					&contact);
}
