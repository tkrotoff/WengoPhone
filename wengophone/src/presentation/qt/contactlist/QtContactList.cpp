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

#include "QtContactList.h"

#include "ContactPopupMenu.h"
#include "ContactGroupPopupMenu.h"
#include "QtContactGroup.h"
#include "QtContact.h"
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

	typedef PostEvent0<void ()> MyPostEvent;
	MyPostEvent * event = new MyPostEvent(boost::bind(&QtContactList::initThreadSafe, this));
	postEvent(event);
}

void QtContactList::initThreadSafe() {
	_contactListWidget = WidgetFactory::create(":/forms/contactlist/contactList.ui", NULL);

	//treeWidget
	_treeWidget = Object::findChild<QTreeWidget *>(_contactListWidget, "treeWidget");

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

	spx->setPixmap (QtContactPixmap::ContactOnline, scalePixmap(":/pics/status/online.png"));
	spx->setPixmap (QtContactPixmap::ContactOffline, scalePixmap(":/pics/status/offline.png"));
	spx->setPixmap (QtContactPixmap::ContactDND, scalePixmap(":/pics/status/donotdisturb.png"));
	spx->setPixmap (QtContactPixmap::ContactInvisible, scalePixmap(":/pics/status/invisible.png"));
	spx->setPixmap (QtContactPixmap::ContactAway, scalePixmap(":/pics/status/away.png"));
	spx->setPixmap (QtContactPixmap::ContactForward, scalePixmap(":/pics/status/forward.png"));

	// Fonctions icons
	spx->setPixmap (QtContactPixmap::ContactIM, scalePixmap(":/pics/contact_im.png"));
	spx->setPixmap (QtContactPixmap::ContactCall, scalePixmap(":/pics/contact_call.png"));
	spx->setPixmap (QtContactPixmap::ContactVideo, scalePixmap(":/pics/contact_video.png"));

	// Group icons
	spx->setPixmap (QtContactPixmap::ContactGroupOpen,QPixmap(":/pics/group_open.png"));
	spx->setPixmap (QtContactPixmap::ContactGroupClose,QPixmap(":/pics/group_close.png"));

	_usermanager = new QtUserManager(_cWengoPhone, _treeWidget,_treeWidget);

	_previous = NULL;
	_lastClicked = NULL;

	QtTreeViewDelegate * delegate = new QtTreeViewDelegate(_treeWidget);
	delegate->setParent(_treeWidget->viewport());

	_treeWidget->setItemDelegate(delegate);
	_treeWidget->setUniformRowHeights(false);
	_treeWidget->header()->hide();


	//Popup Menus
	_contactPopupMenu = new ContactPopupMenu(_treeWidget, _cContactList.getCWengoPhone().getWengoPhone());
	_contactGroupPopupMenu = new ContactGroupPopupMenu(_treeWidget);

	connect(_usermanager, SIGNAL(groupRightClicked(const QString &)), SLOT(groupRightClickedSlot(const QString &)));

	//Connect events
	QtWengoPhone * qtWengoPhone = (QtWengoPhone *) _cContactList.getCWengoPhone().getPresentation();
	qtWengoPhone->setContactList(this);
	_treeWidget->viewport()->setFocus();


	/**********************************************************************************************************************************/
	/*
		DELETE THIS !
	*/
	/*
	QtUserList * ul = QtUserList::getInstance();
	QTreeWidgetItem *group = new QTreeWidgetItem(_treeWidget);
	group->setText(0, "Group 1");

	QTreeWidgetItem *wengo;
	QString uname;
	QString userId;
	QtUser * user;

	int counter = 0;

	wengo = new QTreeWidgetItem(group);
	uname = QString("Kavous");
	wengo->setText(0,uname);
	wengo->setFlags(wengo->flags() | Qt::ItemIsEditable);

	user = new QtUser();
	user->setId(uname);
	user->setUserName(uname);
	user->setStatus(QtContactPixmap::ContactOnline);
	user->haveIM(true);
	user->haveCall(true);
	user->haveVideo(true);
	ul->addUser(user);

	wengo = new QTreeWidgetItem(group);
	uname = QString("Guirec");
	wengo->setText(0,uname);
	wengo->setFlags(wengo->flags() | Qt::ItemIsEditable);

	user = new QtUser();
	user->setId(uname);
	user->setUserName(uname);
	user->setStatus(QtContactPixmap::ContactAway);
	user->haveIM(true);
	user->haveVideo(true);
	ul->addUser(user);

	for (int i=1; i< 10; i++){
		wengo = new QTreeWidgetItem(group);
		uname = QString("User %1 in group 1").arg(i);
		wengo->setText(0,uname);
		wengo->setFlags(wengo->flags() | Qt::ItemIsEditable);
		userId = QString("USER%1").arg(counter);
		user = new QtUser();
		user->setId(uname);
		user->setUserName(uname);
		user->setStatus(QtContactPixmap::ContactOnline);
		user->haveIM(true);

		ul->addUser(user);
		counter++;
	}

	group = new QTreeWidgetItem(_treeWidget);
	group->setText(0,"Group 2");

	for (int i=1; i< 10; i++){
		wengo = new QTreeWidgetItem(group);
		wengo->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled);
		uname = QString("User %1 in group 2").arg(i);
		wengo->setText(0,uname);
		wengo->setFlags(wengo->flags() | Qt::ItemIsEditable);
		userId = QString("USER%1").arg(counter);
		user = new QtUser();
		user->setId(uname);
		user->setUserName(uname);
		user->setStatus(QtContactPixmap::ContactNotAvailable);
		user->haveIM(true);
		user->haveVideo(true);
		ul->addUser(user);
		counter++;
	}*/
}

void QtContactList::updatePresentation() {
	typedef PostEvent0<void ()> MyPostEvent;
	MyPostEvent * event = new MyPostEvent(boost::bind(&QtContactList::updatePresentationThreadSafe, this));
	postEvent(event);
}

void QtContactList::updatePresentationThreadSafe() {
	//TODO: Add code to manage hiden users
	_usermanager->userStateChanged();
	_treeWidget->viewport()->update();
}

void QtContactList::addContactGroup(PContactGroup * pContactGroup) {
	typedef PostEvent1<void (PContactGroup *), PContactGroup *> MyPostEvent;
	MyPostEvent * event = new MyPostEvent(boost::bind(&QtContactList::addContactGroupThreadSafe, this, _1), pContactGroup);
	postEvent(event);
}

void QtContactList::addContactGroupThreadSafe(PContactGroup * pContactGroup) {
	//LOG_DEBUG("contact group added:" + pContactGroup->getName());
	//QString displayName = QString::fromStdString(pContactGroup->getDisplayName();
	QString displayName = QString::fromUtf8(pContactGroup->getDisplayName().c_str());
	QTreeWidgetItem *group = new QTreeWidgetItem(_treeWidget);
	group->setText(0, displayName );
	_treeWidget->setItemExpanded ( group, true);
}

void QtContactList::showAllUsers(){
	_usermanager->showAllUsers();
}

void QtContactList::hideOffLineUser(){
	_usermanager->hideOffLineUsers();
}

void QtContactList::sortUsers(){
	_usermanager->sortUsers();
}
CContactList & QtContactList::getContactList() const{
	return _cContactList;
}

void QtContactList::groupRightClickedSlot(const QString & groupName) {
	_contactGroupPopupMenu->showMenu(_cContactList.getContactGroup(groupName.toStdString()));
}
