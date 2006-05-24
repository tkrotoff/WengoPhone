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

#include "QtContactList.h"

#include "ContactGroupPopupMenu.h"
#include "QtUserList.h"
#include "QtContactPixmap.h"
#include "QtUserManager.h"
#include "QtTreeViewDelegate.h"

#include <presentation/qt/QtWengoPhone.h>

#include <control/CWengoPhone.h>

#include <model/contactlist/Contact.h>
#include <model/contactlist/ContactGroup.h>

#include <util/StringList.h>
#include <util/Logger.h>

#include <qtutil/Object.h>
#include <qtutil/WidgetFactory.h>
#include <qtutil/MouseEventFilter.h>

#include <QtGui>
#include <QDebug>

static inline QPixmap scalePixmap(const char * name) {
	return QPixmap(name).scaled(QSize(16, 16), Qt::KeepAspectRatio, Qt::SmoothTransformation);
}

QtContactList::QtContactList(CContactList & cContactList, CWengoPhone & cWengoPhone)
	: QObjectThreadSafe(NULL),
	_cContactList(cContactList),
	_cWengoPhone(cWengoPhone) {

	_treeWidget = NULL;

	connect(this, SIGNAL(contactGroupAddedEventSignal(QString)),
		SLOT(contactGroupAddedEventSlot(QString)), Qt::QueuedConnection);
	connect(this, SIGNAL(contactGroupRemovedEventSignal(QString)),
		SLOT(contactGroupRemovedEventSlot(QString)), Qt::QueuedConnection);
	connect(this, SIGNAL(contactGroupRenamedEventSignal(QString)),
		SLOT(contactGroupRenamedEventSlot(QString)), Qt::QueuedConnection);
	connect(this, SIGNAL(contactAddedEventSignal(QString)),
		SLOT(contactAddedEventSlot(QString)), Qt::QueuedConnection);
	connect(this, SIGNAL(contactRemovedEventSignal(QString)),
		SLOT(contactRemovedEventSlot(QString)), Qt::QueuedConnection);
	connect(this, SIGNAL(contactMovedEventSignal(QString, QString, QString)),
		SLOT(contactMovedEventSlot(QString, QString, QString)), Qt::QueuedConnection);
	connect(this, SIGNAL(contactChangedEventSignal(QString)),
		SLOT(contactChangedEventSlot(QString)), Qt::QueuedConnection);

	typedef PostEvent0 < void() > MyPostEvent;
	MyPostEvent * event = new MyPostEvent(boost::bind(& QtContactList::initThreadSafe, this));
	postEvent(event);
}

void QtContactList::initThreadSafe() {
	_contactListWidget = WidgetFactory::create(":/forms/contactlist/contactList.ui", NULL);

	//treeWidget
	_treeWidget = Object::findChild < QTreeWidget * > (_contactListWidget, "treeWidget");

	_treeWidget->setAcceptDrops(true);
	_treeWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
	_treeWidget->setProperty("showDropIndicator", QVariant(true));
	_treeWidget->setDragEnabled(true);
	_treeWidget->setAlternatingRowColors(false);
	_treeWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);
	_treeWidget->setSelectionBehavior(QAbstractItemView::SelectItems);
	_treeWidget->setIndentation(0);
	_treeWidget->setRootIsDecorated(false);
	// _treeWidget->setAlternatingRowColors ( true );

	// icons
	QtContactPixmap * spx = QtContactPixmap::getInstance();

	spx->setPixmap(QtContactPixmap::ContactUnknown, scalePixmap(":/pics/status/unknown.png"));
	spx->setPixmap(QtContactPixmap::ContactOnline, scalePixmap(":/pics/status/online.png"));
	spx->setPixmap(QtContactPixmap::ContactOffline, scalePixmap(":/pics/status/offline.png"));
	spx->setPixmap(QtContactPixmap::ContactDND, scalePixmap(":/pics/status/donotdisturb.png"));
	spx->setPixmap(QtContactPixmap::ContactInvisible, scalePixmap(":/pics/status/invisible.png"));
	spx->setPixmap(QtContactPixmap::ContactAway, scalePixmap(":/pics/status/away.png"));
	spx->setPixmap(QtContactPixmap::ContactForward, scalePixmap(":/pics/status/forward.png"));

	// Functions icons
	spx->setPixmap(QtContactPixmap::ContactIM, scalePixmap(":/pics/contact_im.png"));
	spx->setPixmap(QtContactPixmap::ContactCall, scalePixmap(":/pics/contact_call.png"));
	spx->setPixmap(QtContactPixmap::ContactVideo, scalePixmap(":/pics/contact_video.png"));

	// Group icons
	spx->setPixmap(QtContactPixmap::ContactGroupOpen, QPixmap(":/pics/group_open.png"));
	spx->setPixmap(QtContactPixmap::ContactGroupClose, QPixmap(":/pics/group_close.png"));

	_userManager = new QtUserManager(* _cWengoPhone.getCUserProfile(), _cWengoPhone,
			* this, _treeWidget, _treeWidget);

	QtTreeViewDelegate * delegate = new QtTreeViewDelegate(_cWengoPhone, _treeWidget);
	delegate->setParent(_treeWidget->viewport());

	_treeWidget->setItemDelegate(delegate);
	_treeWidget->setUniformRowHeights(false);
	_treeWidget->header()->hide();

	//Popup Menus
	_contactGroupPopupMenu = new ContactGroupPopupMenu(_cContactList, _treeWidget);

	connect(_userManager, SIGNAL(groupRightClicked(const QString &)), SLOT(groupRightClickedSlot(const QString &)));

	//Connect events
	QtWengoPhone * qtWengoPhone = (QtWengoPhone *) _cWengoPhone.getPresentation();
	qtWengoPhone->setContactList(this);
	_treeWidget->viewport()->setFocus();

	initContent();
}

void QtContactList::updatePresentation() {
	typedef PostEvent0 < void() > MyPostEvent;
	MyPostEvent * event = new MyPostEvent(boost::bind(& QtContactList::updatePresentationThreadSafe, this));
	postEvent(event);
}

void QtContactList::updatePresentationThreadSafe() {
	//TODO: Add code to manage hidden users
	_userManager->closeUserInfo();
	_userManager->userStateChanged();
	_treeWidget->viewport()->update();
}

void QtContactList::contactGroupAddedEvent(std::string contactGroupId) {
	LOG_DEBUG("contact group added. UUID: " + contactGroupId);
	contactGroupAddedEventSignal(QString::fromStdString(contactGroupId));
}

void QtContactList::contactGroupRemovedEvent(std::string contactGroupId) {
	contactGroupRemovedEventSignal(QString::fromStdString(contactGroupId));
}

void QtContactList::contactGroupRenamedEvent(std::string contactGroupId) {
	contactGroupRenamedEventSignal(QString::fromStdString(contactGroupId));
}

void QtContactList::contactAddedEvent(std::string contactId) {
	contactAddedEventSignal(QString::fromStdString(contactId));
}

void QtContactList::contactRemovedEvent(std::string contactId) {
	contactRemovedEventSignal(QString::fromStdString(contactId));
}

void QtContactList::contactMovedEvent(std::string dstContactGroupId,
std::string srcContactGroupId, std::string contactId) {
	contactMovedEventSignal(QString::fromStdString(dstContactGroupId),
	QString::fromStdString(srcContactGroupId), QString::fromStdString(contactId));
}

void QtContactList::contactChangedEvent(std::string contactId) {
	contactChangedEventSignal(QString::fromStdString(contactId));
}

void QtContactList::contactGroupAddedEventSlot(QString contactGroupId) {
	LOG_DEBUG("contact group added. UUID: " + contactGroupId.toStdString());

	QList < QTreeWidgetItem * > list = _treeWidget->findItems(contactGroupId, Qt::MatchExactly);

	// If no group exists, creating the group
	if (list.isEmpty()) {
		QTreeWidgetItem * group = new QTreeWidgetItem(_treeWidget);
		group->setText(0, contactGroupId);
		_treeWidget->setItemExpanded(group, true);
	}
}

void QtContactList::contactGroupRemovedEventSlot(QString contactGroupId) {
	LOG_DEBUG("contact group removed. UUID: " + contactGroupId.toStdString());

	QList < QTreeWidgetItem * > list = _treeWidget->findItems(contactGroupId, Qt::MatchExactly);

	if (!list.isEmpty()) {
		_treeWidget->takeTopLevelItem(_treeWidget->indexOfTopLevelItem(list[0]));
	}
}

void QtContactList::contactGroupRenamedEventSlot(QString contactGroupId) {
	LOG_DEBUG("contact group renamed. UUID: " + contactGroupId.toStdString());

	updatePresentationThreadSafe();
}

void QtContactList::contactAddedEventSlot(QString contactId) {
	LOG_DEBUG("contact added. UUID: " + contactId.toStdString());
	QtUserList * ul = QtUserList::getInstance();

	// If User is not already in UserList
	if (!ul->getUser(contactId)) {
		ContactProfile contactProfile = _cContactList.getContactProfile(contactId.toStdString());
		QList < QTreeWidgetItem * > list;

		// If the Contact has a group
		if (!contactProfile.getGroupId().empty()) {
			list = _treeWidget->findItems(QString::fromStdString(contactProfile.getGroupId()), Qt::MatchExactly);

			// No group exists. Creating the group
			if (list.size() == 0) {
				contactGroupAddedEventSlot(QString::fromStdString(contactProfile.getGroupId()));
				list = _treeWidget->findItems(QString::fromStdString(contactProfile.getGroupId()),
				   Qt::MatchExactly);
			}

			QTreeWidgetItem * newContact = NULL;
			QtUser * user = NULL;
			QString contactName;

			LOG_DEBUG("display name: " + contactProfile.getDisplayName());

			newContact = new QTreeWidgetItem(list[0]);
			newContact->setText(0, contactId);
			newContact->setFlags(newContact->flags() | Qt::ItemIsEditable);

			user = new QtUser(contactId.toStdString(), _cWengoPhone);

			ul->addUser(user);

			updatePresentationThreadSafe();
		}
	}
}

void QtContactList::contactRemovedEventSlot(QString contactId) {
	LOG_DEBUG("contact removed. UUID: " + contactId.toStdString());

	_userManager->removeContact(contactId);
	updatePresentationThreadSafe();
}

void QtContactList::contactMovedEventSlot(QString dstContactGroupId,
	QString srcContactGroupId, QString contactId) {
	LOG_DEBUG("contact moved from " + srcContactGroupId.toStdString()
	+ " to " + dstContactGroupId.toStdString()
	+ ". UUID: " + contactId.toStdString());

	_userManager->moveContact(contactId,
		srcContactGroupId, dstContactGroupId);
	updatePresentationThreadSafe();
}

void QtContactList::contactChangedEventSlot(QString contactId) {
	LOG_DEBUG("contact changed: " + contactId.toStdString());

	QtUserList * ul = QtUserList::getInstance();
	QtUser * user = ul->getUser(contactId);

	if (user) {
		user->contactUpdated();
		updatePresentationThreadSafe();
	} else {
		contactAddedEventSlot(contactId);
	}
}

void QtContactList::showAllUsers() {
	_userManager->showAllUsers();
}

void QtContactList::hideOffLineUser() {
	_userManager->hideOffLineUsers();
}

void QtContactList::sortUsers() {
	_userManager->sortUsers();
}

CContactList & QtContactList::getCContactList() const {
	return _cContactList;
}

void QtContactList::groupRightClickedSlot(const QString & groupId) {
	_contactGroupPopupMenu->showMenu(groupId);
}

void QtContactList::initContent() {
	std::vector < std::string > contacts = _cContactList.getContactIds();
	for (std::vector < std::string >::const_iterator it = contacts.begin();
		it != contacts.end(); ++it) {
		contactAddedEventSlot(QString::fromStdString(* it));
	}
}
