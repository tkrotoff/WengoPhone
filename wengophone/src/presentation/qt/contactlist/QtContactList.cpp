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
#include "QtContactListManager.h"
#include "QtContactPixmap.h"
#include "QtContactManager.h"
#include "QtTreeViewDelegate.h"

#include <presentation/qt/QtWengoPhone.h>

#include <control/CWengoPhone.h>
#include <control/profile/CUserProfileHandler.h>

#include <model/contactlist/Contact.h>
#include <model/contactlist/ContactGroup.h>

#include <util/StringList.h>
#include <util/Logger.h>

#include <qtutil/Object.h>
#include <qtutil/WidgetFactory.h>
#include <qtutil/MouseEventFilter.h>

#include <QPixmap>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QString>
#include <QHeaderView>

const QString QtContactList::DEFAULT_GROUP_NAME = "WENGO2006CLISTHIDE";
const QString QtContactList::STATUS_UNKNOW_PIXMAP = ":/pics/status/unknown.png";
const QString QtContactList::STATUS_ONLINE_PIXMAP = ":/pics/status/online.png";
const QString QtContactList::STATUS_OFFLINE_PIXMAP = ":/pics/status/offline.png";
const QString QtContactList::STATUS_DND_PIXMAP = ":/pics/status/donotdisturb.png";
const QString QtContactList::STATUS_INVISIBLE_PIXMAP = ":/pics/status/invisible.png";
const QString QtContactList::STATUS_AWAY_PIXMAP = ":/pics/status/away.png";
const QString QtContactList::STATUS_FORWARD_PIXMAP = ":/pics/status/forward.png";
const QString QtContactList::STATUS_CONTACT_IM_PIXMAP = ":/pics/contact_im.png";
const QString QtContactList::STATUS_CONTACT_CALL_PIXMAP = ":/pics/contact_call.png";
const QString QtContactList::STATUS_CONTACT_VIDEO_PIXMAP = ":/pics/contact_video.png";
const QString QtContactList::STATUS_GROUP_OPEN_PIXMAP = ":/pics/group_open.png";
const QString QtContactList::STATUS_GROUP_CLOSE_PIXMAP = ":/pics/group_close.png";

static inline QPixmap scalePixmap(const QString name) {
	return QPixmap(name).scaled(QSize(16, 16), Qt::KeepAspectRatio, Qt::SmoothTransformation);
}

QtContactList::QtContactList(CContactList & cContactList, CWengoPhone & cWengoPhone)
	: QObjectThreadSafe(NULL),
	_cContactList(cContactList),
	_cWengoPhone(cWengoPhone) {

	_treeWidget = NULL;

	typedef PostEvent0 < void() > MyPostEvent;
	MyPostEvent * event = new MyPostEvent(boost::bind(& QtContactList::initThreadSafe, this));
	postEvent(event);
}

QtContactList::~QtContactList() {
}

void QtContactList::cleanup() {
	QtContactListManager::getInstance()->clear();

	if (_contactListWidget) {
		delete _contactListWidget;
		_contactListWidget = NULL;
	}
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

	// icons
	QtContactPixmap * spx = QtContactPixmap::getInstance();
	spx->setPixmap(QtContactPixmap::ContactUnknown, scalePixmap(STATUS_UNKNOW_PIXMAP));
	spx->setPixmap(QtContactPixmap::ContactOnline, scalePixmap(STATUS_ONLINE_PIXMAP));
	spx->setPixmap(QtContactPixmap::ContactOffline, scalePixmap(STATUS_OFFLINE_PIXMAP ));
	spx->setPixmap(QtContactPixmap::ContactDND, scalePixmap(STATUS_DND_PIXMAP));
	spx->setPixmap(QtContactPixmap::ContactInvisible, scalePixmap(STATUS_INVISIBLE_PIXMAP));
	spx->setPixmap(QtContactPixmap::ContactAway, scalePixmap(STATUS_AWAY_PIXMAP));
	spx->setPixmap(QtContactPixmap::ContactForward, scalePixmap(STATUS_FORWARD_PIXMAP));

	// Functions icons
	spx->setPixmap(QtContactPixmap::ContactIM, scalePixmap(STATUS_CONTACT_IM_PIXMAP));
	spx->setPixmap(QtContactPixmap::ContactCall, scalePixmap(STATUS_CONTACT_CALL_PIXMAP));
	spx->setPixmap(QtContactPixmap::ContactVideo, scalePixmap(STATUS_CONTACT_VIDEO_PIXMAP));

	// Group icons
	spx->setPixmap(QtContactPixmap::ContactGroupOpen, QPixmap(STATUS_GROUP_OPEN_PIXMAP));
	spx->setPixmap(QtContactPixmap::ContactGroupClose, QPixmap(STATUS_GROUP_CLOSE_PIXMAP));

	_contactManager=new QtContactManager(*_cWengoPhone.getCUserProfileHandler().getCUserProfile(), _cWengoPhone,
			* this, _treeWidget, _treeWidget);

	QtTreeViewDelegate * delegate = new QtTreeViewDelegate(_cWengoPhone, _treeWidget);
	delegate->setParent(_treeWidget->viewport());

	_treeWidget->setItemDelegate(delegate);
	_treeWidget->setUniformRowHeights(false);
	_treeWidget->header()->hide();

	//Popup Menus
	_contactGroupPopupMenu = new ContactGroupPopupMenu(_cContactList, _treeWidget);
	connect(_contactManager, SIGNAL(groupRightClicked(const QString &)),
		SLOT(groupRightClickedSlot(const QString &)));

	// Events from the Control
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


	QtWengoPhone * qtWengoPhone = (QtWengoPhone *) _cWengoPhone.getPresentation();
	qtWengoPhone->setContactList(this);
	_treeWidget->viewport()->setFocus();

	initContent();
}

void QtContactList::initContent() {
	std::vector <std::string> contacts = _cContactList.getContactIds();
	std::vector <std::string>::const_iterator it;

	for (it = contacts.begin(); it != contacts.end(); ++it) {
		contactAddedEventSlot(QString::fromStdString(*it));
	}
}

void QtContactList::updatePresentation() {
	typedef PostEvent0 <void()> MyPostEvent;
	MyPostEvent * event = new MyPostEvent(boost::bind(&QtContactList::updatePresentationThreadSafe, this));
	postEvent(event);
}

void QtContactList::updatePresentationThreadSafe() {
	_contactManager->closeUserInfo();
	_contactManager->userStateChanged();
	_treeWidget->viewport()->update();
}

void QtContactList::contactGroupAddedEvent(std::string contactGroupId) {
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
	if (_contactManager->groupsAreHiden()){
		contactGroupId = DEFAULT_GROUP_NAME;
	}
	addGroup(contactGroupId);
}

QTreeWidgetItem * QtContactList::addGroup(QString contactGroupId) {
	QList <QTreeWidgetItem *> list;
	QTreeWidgetItem * group;

	list = _treeWidget->findItems(contactGroupId, Qt::MatchExactly);
	if (list.isEmpty()) {
		group = new QTreeWidgetItem(_treeWidget);
		group->setText(0, contactGroupId);
		_treeWidget->setItemExpanded(group, true);
	}
	return group;
}

void QtContactList::contactGroupRemovedEventSlot(QString contactGroupId) {
	QList <QTreeWidgetItem *> list = _treeWidget->findItems(contactGroupId, Qt::MatchExactly);
	if (!list.isEmpty()) {
		_treeWidget->takeTopLevelItem(_treeWidget->indexOfTopLevelItem(list[0]));
	}
}

void QtContactList::contactGroupRenamedEventSlot(QString contactGroupId) {
	updatePresentationThreadSafe();
}

void QtContactList::contactAddedEventSlot(QString contactId) {
	QtContactListManager * ul = QtContactListManager::getInstance();
	QString groupId;
	QTreeWidgetItem * newContact = NULL;
	QtContact * qtContact = NULL;
	QString contactName;

	// If the contact is not already in QtContactList
	if (!ul->contains(contactId)) {
		ContactProfile contactProfile = _cContactList.getContactProfile(contactId.toStdString());
		if (_contactManager->groupsAreHiden()) {
			groupId = DEFAULT_GROUP_NAME;
		} else {
			groupId = QString::fromStdString(contactProfile.getGroupId());
		}
		QList <QTreeWidgetItem *> list;
		// If the Contact has a group
		if (!contactProfile.getGroupId().empty()) {
			list = _treeWidget->findItems(groupId, Qt::MatchExactly);
			// No group exists. Creating the group
			if (list.empty()) {
				contactGroupAddedEventSlot(groupId);
				list = _treeWidget->findItems(groupId,Qt::MatchExactly);
			}
			newContact = new QTreeWidgetItem(list[0]);
			newContact->setText(0, contactId);
			newContact->setFlags(newContact->flags() | Qt::ItemIsEditable);

			qtContact = new QtContact(contactId.toStdString(), _cWengoPhone);
			ul->addContact(qtContact);
			updatePresentationThreadSafe();
		}
	}
}

void QtContactList::contactRemovedEventSlot(QString contactId) {
	_contactManager->removeContact(contactId);
	updatePresentationThreadSafe();
}

void QtContactList::contactMovedEventSlot(QString dstContactGroupId,
	QString srcContactGroupId, QString contactId) {
	_contactManager->moveContact(contactId, srcContactGroupId, dstContactGroupId);
	updatePresentationThreadSafe();
}

void QtContactList::contactChangedEventSlot(QString contactId) {
	QtContactListManager * ul = QtContactListManager::getInstance();
	if (ul->contains(contactId)) {
		ul->contactUpdated(contactId);
		updatePresentationThreadSafe();
	} else {
		contactAddedEventSlot(contactId);
	}
}

void QtContactList::redrawContacts() {
	_contactManager->redrawContacts();
}

void QtContactList::hideOffLineContacts() {
	_contactManager->hideOffLineContacts();
}

void QtContactList::sortContacts() {
	_contactManager->sortContacts();
}

CContactList & QtContactList::getCContactList() const {
	return _cContactList;
}

void QtContactList::groupRightClickedSlot(const QString & groupId) {
	_contactGroupPopupMenu->showMenu(groupId);
}

void QtContactList::showHideGroups(){
	_contactManager->hideGroups();
}
