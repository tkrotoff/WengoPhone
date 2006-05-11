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

#include "QtUserManager.h"

#include "QtContactList.h"
#include "QtContactPixmap.h"
#include "QtHidenContact.h"
#include "QtUserList.h"
#include "QtUserTreeEventFilter.h"
#include "UserTreeEventManager.h"
#include "QtConferenceAction.h"
#include "../QtWengoPhone.h"

#include <presentation/qt/profile/QtProfileDetails.h>

#include <control/CWengoPhone.h>
#include <control/contactlist/CContactList.h>
#include <control/profile/CUserProfile.h>

#include <model/contactlist/ContactList.h>
#include <model/phoneline/PhoneLine.h>
#include <model/phonecall/PhoneCall.h>
#include <model/contactlist/Contact.h>

#include <sipwrapper/EnumPhoneCallState.h>

#include <util/Logger.h>

QtUserManager::QtUserManager(CUserProfile & cUserProfile, CWengoPhone & cWengoPhone, QtContactList & qtContactList,
   QObject * parent, QTreeWidget * target)
   : QObject(parent), _cUserProfile(cUserProfile), _cWengoPhone(cWengoPhone), _qtContactList(qtContactList) {
	   _tree = target;
	   _previous = NULL;
	   _lastClicked = NULL;
	   _hideUsers = false;
	   _sortUsers = false;
	   _menu = NULL;

	   QtUserList::getInstance()->setTreeWidget(target);
	   target->setMouseTracking(true);
	   UserTreeEventManager * dnd = new UserTreeEventManager(qtContactList.getCContactList(), this, target);
	   QtUserTreeEventFilter * keyFilter = new QtUserTreeEventFilter(this, target);

	   connect(target, SIGNAL(itemSelectionChanged()), this, SLOT(treeViewSelectionChanged()));
	   connect(target, SIGNAL(itemClicked(QTreeWidgetItem *, int)), this, SLOT(itemClicked(QTreeWidgetItem *, int)));
	   connect(dnd, SIGNAL(itemEntered(QTreeWidgetItem *)), this, SLOT(itemEntered(QTreeWidgetItem *)));
	   connect(dnd, SIGNAL(itemTimeout(QTreeWidgetItem *)), this, SLOT(openUserInfo(QTreeWidgetItem *)));
	   connect(dnd, SIGNAL(mouseClicked(Qt::MouseButton)), SLOT(setMouseButton(Qt::MouseButton)));

	   connect(keyFilter, SIGNAL(openItem(QTreeWidgetItem *)), SLOT(openUserInfo(QTreeWidgetItem *)));
	   connect(keyFilter, SIGNAL(closeItem(QTreeWidgetItem *)), SLOT(closeUserInfo()));
	   connect(keyFilter, SIGNAL(deleteItem(QTreeWidgetItem *)), SLOT(deleteContact()));

	   QtWengoPhone * qtWengoPhone = dynamic_cast < QtWengoPhone * > (_cWengoPhone.getPresentation());

	   if (!connect(this, SIGNAL(inviteToConferenceClicked(QString, PhoneCall *)),
	   qtWengoPhone, SLOT(addToConference(QString, PhoneCall *)))) {
		   LOG_DEBUG("Unable to connect signal\n");
	   }
}

void QtUserManager::startSMS(bool checked) {

	QtUserList * ul = QtUserList::getInstance();
	QtUser * user;

	// The current selected item
	QTreeWidgetItem * item = _tree->currentItem();

	user = ul->getUser(item->text(0));

	user->startSMS();
}

void QtUserManager::startChat(bool) {
	QtUserList * ul = QtUserList::getInstance();
	QtUser * user;

	//The current selected item
	QTreeWidgetItem * item = _tree->currentItem();

	user = ul->getUser(item->text(0));

	user->startChat();
}

void QtUserManager::editContact(bool) {
	QtUserList * ul = QtUserList::getInstance();

	//The current selected item
	QTreeWidgetItem * item = _tree->currentItem();

	ContactProfile contactProfile = _cUserProfile.getCContactList().getContactProfile(item->text(0).toStdString());
	QtProfileDetails qtProfileDetails(*_cWengoPhone.getCUserProfile(), contactProfile, _tree);
	if (qtProfileDetails.show()) {
		_cUserProfile.getCContactList().updateContact(contactProfile);
	}

	LOG_DEBUG("edit contact");
}

void QtUserManager::deleteContact() {
	QtUserList * ul = QtUserList::getInstance();
	QTreeWidgetItem * item = _tree->currentItem();
	_cUserProfile.getCContactList().removeContact(item->text(0).toStdString());
}

void QtUserManager::treeViewSelectionChanged() {
	if (_button == Qt::NoButton)
		closeUserInfo();
}

void QtUserManager::itemEntered(QTreeWidgetItem * item) {
	QtUserList * ul = QtUserList::getInstance();

	if (ul) {
		ul->mouseOn(item->text(0));
		//		closeUserInfo();

	}
	_tree->viewport()->update();
}

void QtUserManager::closeUserInfo() {
	QtUserList * ul = QtUserList::getInstance();
	if (_previous != NULL) {
		_tree->closePersistentEditor(_previous, 0);
		ul->setOpenStatus(_previous->text(0), false);
		_previous->setSizeHint(0, QSize(-1, ul->getHeight(_previous->text(0))));
		_previous = NULL;
	}
	_tree->viewport()->update();
}

void QtUserManager::openUserInfo(QTreeWidgetItem * i) {
	QTreeWidgetItem * item = i;

	QtUserList * ul = QtUserList::getInstance();
	if (_previous != NULL) {
		closeUserInfo();
		_previous = item;
		if (item->parent()) {
			ul->setOpenStatus(_previous->text(0), true);
			item->setSizeHint(0, QSize(-1, ul->getHeight(item->text(0))));
			_tree->openPersistentEditor(item);
		}
	}
	else {
		_previous = item;
		if (item->parent()) {
			ul->setOpenStatus(_previous->text(0), true);
			item->setSizeHint(0, QSize(-1, ul->getHeight(item->text(0))));
			_tree->openPersistentEditor(item);
		}
	}
	_tree->viewport()->update();
}

void QtUserManager::itemClicked(QTreeWidgetItem *, int) {

	QtUserList * ul = QtUserList::getInstance();
	QTreeWidgetItem * item = _tree->currentItem();

	QRect widgetSize = _tree->rect();
	QPoint mousepos = _tree->mapFromGlobal(QCursor::pos());
	if (!item->parent()) {
		if (_button == Qt::RightButton) {
			groupRightClicked(item->text(0));
			return;
		}

		if (_tree->isItemExpanded(item)) {
			_tree->collapseItem(item);
		}
		else {
			_tree->expandItem(item);
		}
	}

	ul->mouseClicked(item->text(0), mousepos, widgetSize);

	if (mousepos.x() > ul->getIconsStartPosition(item->text(0)))
		return;

	if (ul->getButton(item->text(0)) == Qt::RightButton) {
		if (!_menu) {
			_menu = createMenu();
			_menu->popup(QCursor::pos());
		}
		else {
			delete _menu;
			_menu = createMenu();
			_menu->popup(QCursor::pos());
		}
		return;
	}

	if (_previous == item) {
		closeUserInfo();
	}
	else if (_previous != NULL) {
		closeUserInfo();
		_previous = item;

		if (item->parent()) {
			openUserInfo(item);
		}

	}
	else {
		_previous = item;
		if (item->parent()) {
			openUserInfo(item);
		}
	}
	_tree->viewport()->update();
}

QList < QtHidenContact * > QtUserManager::clearList(QList < QtHidenContact * > list) {

	QList < QtHidenContact * >::iterator iter;
	QList < QtHidenContact * > tmp;

	for (iter = list.begin(); iter != list.end(); iter++) {

		if (!(* iter)->isCleared())
			tmp.append((* iter));
	}
	return tmp;
}


void QtUserManager::safeUserStateChanged() {

	safeHideOffLineUsers();

	QtHidenContact * hidenContact;
	QList < QtHidenContact * >::iterator iter;

	QList < QtHidenContact * > newHidenList;

	for (iter = _hidenContacts.begin(); iter != _hidenContacts.end(); iter++) {

		hidenContact = (QtHidenContact *) (* iter);

		if (hidenContact->getUser()->getStatus() != QtContactPixmap::ContactOffline) {

			if (hidenContact->getIndex() > hidenContact->getParentItem()->childCount())
				hidenContact->getParentItem()->insertChild(hidenContact->getParentItem()->childCount(),
				hidenContact->getItem());
			else hidenContact->getParentItem()->insertChild(hidenContact->getIndex(), hidenContact->getItem());
		}
		else {
			newHidenList.append(hidenContact);
		}
	}
	_hidenContacts = newHidenList;

	safeSortUsers();
}

void QtUserManager::userStateChanged() {
	//QMutexLocker lock(&_mutex);
	safeUserStateChanged();
}

void QtUserManager::safeHideOffLineUsers() {
	//TODO: Add the code to manage hidden groups

	if (!_hideUsers)
		return;
	QtUserList * ul = QtUserList::getInstance();
	QtUser * user;
	QList < QTreeWidgetItem * > itemList = _tree->findItems("*", Qt::MatchWildcard);
	QList < QTreeWidgetItem * > deleteList;
	QList < QTreeWidgetItem * >::iterator i;

	for (i = itemList.begin(); i != itemList.end(); i++) {

		QTreeWidgetItem * item;
		QTreeWidgetItem * group = (QTreeWidgetItem *) (* i);

		if (group->parent() == 0) {
			// We have all parents (if groups are not hiden)
			deleteList.clear();
			int count = group->childCount();

			for (int t = 0; t < count; t++) {
				item = group->child(t);
				user = ul->getUser(item->text(0));
				if (user->getStatus() == QtContactPixmap::ContactOffline) {
					// Take the widget and put it in _hidenContacts
					int index = group->indexOfChild(item);
					QtHidenContact * hiden = new QtHidenContact(item, item->parent(), user, index, this);
					_hidenContacts.append(hiden);
					deleteList.append(item);
				}
			}
			// Delete the childs
			QList < QTreeWidgetItem * >::iterator deleteIterator;
			for (deleteIterator = deleteList.begin(); deleteIterator != deleteList.end(); deleteIterator++) {
				group->takeChild(group->indexOfChild((* deleteIterator)));
			}
			// Work around Qt bug ...
			_tree->setItemExpanded(group, false);
			_tree->setItemExpanded(group, true);
		}

	}

}

void QtUserManager::hideOffLineUsers() {

	//QMutexLocker lock(&_mutex);
	_hideUsers = true;
	safeHideOffLineUsers();
}

void QtUserManager::safeSortUsers() {
	//FIXME: only use sortList or deleteList
	if (!_sortUsers)
		return;

	QtUserList * ul = QtUserList::getInstance();
	QtUser * user;

	QList < QTreeWidgetItem * > itemList = _tree->findItems("*", Qt::MatchWildcard);
	QList < QTreeWidgetItem * > deleteList;
	QList < QtHidenContact > sortList;
	QList < QTreeWidgetItem * >::iterator i;

	for (i = itemList.begin(); i != itemList.end(); i++) {

		QTreeWidgetItem * item;
		QTreeWidgetItem * group = (QTreeWidgetItem *) (* i);

		if (group->parent() == 0) {
			// We have all parents (if groups are not hiden)
			deleteList.clear();
			sortList.clear();

			int count = group->childCount();

			for (int t = 0; t < count; t++) {
				item = group->child(t);
				user = ul->getUser(item->text(0));
				// Take the widget and put it in sortList
				int index = group->indexOfChild(item);
				QtHidenContact hiden = QtHidenContact(item, item->parent(), user, index, this);
				sortList.append(hiden);
				deleteList.append(item);
			}
			// Delete the childs
			QList < QTreeWidgetItem * >::iterator deleteIterator;
			for (deleteIterator = deleteList.begin(); deleteIterator != deleteList.end(); deleteIterator++) {
				group->takeChild(group->indexOfChild((* deleteIterator)));
			}
			// Sort and reinsert items
			qSort(sortList.begin(), sortList.end());

			QList < QtHidenContact >::iterator insertIterator;
			for (insertIterator = sortList.begin(); insertIterator != sortList.end(); insertIterator++) {
				group->insertChild(group->childCount(), (* insertIterator).getItem());
			}
		}
	}
}

void QtUserManager::sortUsers() {
	//QMutexLocker lock(&_mutex);
	_sortUsers = true;
	safeSortUsers();
}

void QtUserManager::safeShowAllUsers() {
	QtHidenContact * hidenContact;
	QList < QtHidenContact * >::iterator iter;

	for (iter = _hidenContacts.begin(); iter != _hidenContacts.end(); iter++) {

		hidenContact = (QtHidenContact *) (* iter);

		if (hidenContact->getIndex() > hidenContact->getParentItem()->childCount())
			hidenContact->getParentItem()->insertChild(hidenContact->getParentItem()->childCount(),
			hidenContact->getItem());
		else hidenContact->getParentItem()->insertChild(hidenContact->getIndex(), hidenContact->getItem());

		delete hidenContact;
	}
	_hidenContacts.clear();
}

void QtUserManager::showAllUsers() {
	//QMutexLocker lock(&_mutex);
	_hideUsers = false;
	safeShowAllUsers();
}

QMenu * QtUserManager::createConferenceMenu() {

	PhoneLine * phoneLine = dynamic_cast < PhoneLine * >
	   (_cWengoPhone.getCUserProfile()->getUserProfile().getActivePhoneLine());

	QMenu * menu = new QMenu(tr("Invite to conference"));
	if (phoneLine) {
		PhoneLine::PhoneCallList phoneCallList = phoneLine->getPhoneCallList();

		PhoneLine::PhoneCallList::iterator it;

		for (it = phoneCallList.begin(); it != phoneCallList.end(); it++) {

			if ((* it)->getState() != EnumPhoneCallState::PhoneCallStateClosed) {
				// QAction * action = menu->addAction(QString::fromStdString( (*it)->getPeerSipAddress().getUserName() ));
				QtConferenceAction * action = new
				   QtConferenceAction(QString::fromStdString((* it)->getPeerSipAddress().getUserName()), menu);
				action->setPhoneCall((* it));
				connect(action, SIGNAL(triggered(bool)), SLOT(inviteToConference()));
				menu->addAction(action);
			}
		}
	}
	return menu;
}

void QtUserManager::inviteToConference() { // SLOT

	QObject * source = sender();

	if (source) {
		QtConferenceAction * action = dynamic_cast < QtConferenceAction * > (source);
		QtUserList * ul = QtUserList::getInstance();
		QtUser * user;
		// The current selected item
		QTreeWidgetItem * item = _tree->currentItem();
		user = ul->getUser(item->text(0));
		if (user) {
			QString phone = user->getPreferredNumber();
			inviteToConferenceClicked(phone, action->getPhoneCall());
		}
	}
	else {
		LOG_DEBUG("Don't call this function directly !!! \n");
	}
}

QMenu * QtUserManager::createMenu() {

	QtUserList * ul = QtUserList::getInstance();
	QtUser * user;

	// The current selected item
	QTreeWidgetItem * item = _tree->currentItem();

	user = ul->getUser(item->text(0));

	QAction * action;

	QMenu * menu;

	menu = new QMenu(dynamic_cast < QWidget * > (parent()));
	// _callAction = menu->addAction(tr("Call"));
	// Call menu
	if (user->havePhoneNumber()) {

		QMenu * callMenu = menu->addMenu(tr("Call"));
		if (!user->getMobilePhone().isEmpty()) {
			action = callMenu->addAction(tr("Mobile phone"));
			connect(action, SIGNAL(triggered(bool)), SLOT(startMobileCall(bool)));
		}

		if (!user->getHomePhone().isEmpty()) {
			action = callMenu->addAction(tr("Home phone"));
			connect(action, SIGNAL(triggered(bool)), SLOT(startHomeCall(bool)));
		}

		if (!user->getWorkPhone().isEmpty()) {
			action = callMenu->addAction(tr("Work phone"));
			connect(action, SIGNAL(triggered(bool)), SLOT(startWorkCall(bool)));
		}

		if (!user->getWengoPhoneNumber().isEmpty()) {
			action = callMenu->addAction(tr("Wengophone"));
			connect(action, SIGNAL(triggered(bool)), SLOT(startWengoCall(bool)));
		}
	}


	action = menu->addAction(tr("Start Chat"));
	connect(action, SIGNAL(triggered(bool)), SLOT(startChat(bool)));

	action = menu->addAction(tr("Send SMS"));
	connect(action, SIGNAL(triggered(bool)), SLOT(startSMS(bool)));

	// menu->addAction(tr("Invite to conference"));
	menu->addMenu(createConferenceMenu());

	menu->addSeparator();

	action = menu->addAction(tr("Edit contact"));
	connect(action, SIGNAL(triggered(bool)), this, SLOT(editContact(bool)));

	action = menu->addAction(tr("Delete contact"));
	connect(action, SIGNAL(triggered(bool)), this, SLOT(deleteContact()));


	// _cWengoPhone.getCContactList()

	menu->addSeparator();

	menu->addAction(tr("Block contact"));

	menu->addAction(tr("Forward to Cell phone"));

	menu->setWindowOpacity(0.97);

	return menu;

}

void QtUserManager::startMobileCall(bool checked) {

	QtUserList * ul = QtUserList::getInstance();
	QtUser * user;

	// The current selected item
	QTreeWidgetItem * item = _tree->currentItem();

	user = ul->getUser(item->text(0));
	user->startCall(user->getMobilePhone());
}

void QtUserManager::startHomeCall(bool checked) {
	QtUserList * ul = QtUserList::getInstance();
	QtUser * user;

	// The current selected item
	QTreeWidgetItem * item = _tree->currentItem();

	user = ul->getUser(item->text(0));
	user->startCall(user->getHomePhone());

}

void QtUserManager::startWorkCall(bool checked) {

	QtUserList * ul = QtUserList::getInstance();
	QtUser * user;

	// The current selected item
	QTreeWidgetItem * item = _tree->currentItem();

	user = ul->getUser(item->text(0));
	user->startCall(user->getWorkPhone());
}

void QtUserManager::startWengoCall(bool checked) {

	QtUserList * ul = QtUserList::getInstance();
	QtUser * user;

	// The current selected item
	QTreeWidgetItem * item = _tree->currentItem();

	user = ul->getUser(item->text(0));
	user->startCall(user->getWengoPhoneNumber());
}

void QtUserManager::removeContact(const QString & contactId) {
	QMutexLocker lock(& _mutex);

	safeRemoveContact(contactId);
}

void QtUserManager::safeRemoveContact(const QString & contactId) {
	QtUserList * ul = QtUserList::getInstance();
	QtUser * user = NULL;
	bool found = false;

	QList < QTreeWidgetItem * > itemList = _tree->findItems("*", Qt::MatchWildcard);
	QList < QTreeWidgetItem * >::iterator i;

	for (i = itemList.begin(); (i != itemList.end()) && !found; i++) {
		QTreeWidgetItem * item;
		QTreeWidgetItem * group = (QTreeWidgetItem *) (* i);

		if (group->parent() == 0) {
			int count = group->childCount();

			for (int t = 0; (t < count) && !found; t++) {
				item = group->child(t);
				user = ul->getUser(item->text(0));
				if (user->getId() == contactId) {
					group->takeChild(t);
					ul->removeUser(user);
					found = true;
				}
			}
		}
	}

	removeFromHidenContact(contactId);
}

void QtUserManager::removeFromHidenContact(const QString & contactId) {
	QList < QtHidenContact * >::iterator iter;

	for (iter = _hidenContacts.begin(); iter != _hidenContacts.end(); iter++) {
		if ((* iter)->getUser()->getId() == contactId) {
			_hidenContacts.erase(iter);
			break;
		}
	}
}

void QtUserManager::setMouseButton(Qt::MouseButton button) {
	_button = button;
}

void QtUserManager::moveContact(const QString & contactId,
const QString & srcContactGroupId, const QString & dstContactGroupId) {

	//	QMutexLocker lock(&_mutex);

	QtUserList * ul = QtUserList::getInstance();
	QtUser * user = NULL;
	bool found = false;

	// Removing the Contact from the old group
	// We should only one group
	QList < QTreeWidgetItem * > list = _tree->findItems(srcContactGroupId, Qt::MatchExactly);
	QTreeWidgetItem * group = list[0];
	int count = group->childCount();

	for (int t = 0; (t < count) && !found; t++) {
		user = ul->getUser(group->child(t)->text(0));
		if (user->getId() == contactId) {
			group->takeChild(t);
			found = true;
		}
	}

	// If not found
	if (!found) {
		for (QList < QtHidenContact * >::iterator iter = _hidenContacts.begin(); iter != _hidenContacts.end();
		   iter++) {
			   if ((* iter)->getUser()->getId() == contactId) {
				   user = (* iter)->getUser();
				   _hidenContacts.erase(iter);
				   break;
			   }
		}
	}
	////

	// If the user has been found
	if (user) {
		// Adding the user to the destination group
		list = _tree->findItems(dstContactGroupId, Qt::MatchExactly);

		// No group exists. Creating the group
		if (list.size() == 0) {
			_qtContactList.contactGroupAddedEventSlot(dstContactGroupId);
		}

		QTreeWidgetItem * newContact = NULL;
		newContact = new QTreeWidgetItem(list[0]);
		newContact->setText(0, contactId);
		newContact->setFlags(newContact->flags() | Qt::ItemIsEditable);
		////
	}
}
