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
#include "model/contactlist/Contact.h"
#include "model/contactlist/ContactGroup.h"
#include "presentation/qt/QtWengoPhone.h"
#include "control/CWengoPhone.h"
#include "TreeModel.h"
#include "TreeItem.h"
#include "QtContactGroup.h"
#include "QtContact.h"
#include "ContactListDelegate.h"
#include "ContactListUtil.h"
#include "QtUserList.h"
#include "QtContactPixmap.h"
#include "QtUserManager.h"
#include "QtTreeViewDelegate.h"
#include <StringList.h>
#include <Logger.h>

#include <Object.h>
#include <WidgetFactory.h>
#include <MouseEventFilter.h>

#include <QtGui>
#include <QDebug>

QtContactList::QtContactList(CContactList & cContactList) 
	: QObjectThreadSafe(), _cContactList(cContactList) {

	_treeWidget = NULL;
	
	typedef PostEvent0<void ()> MyPostEvent;
	MyPostEvent * event = new MyPostEvent(boost::bind(&QtContactList::initThreadSafe, this));
	postEvent(event);
}

void QtContactList::initThreadSafe() {


	
	_contactListWidget = WidgetFactory::create(":/forms/contactlist/contactList.ui", NULL);
	
	//treeWidget
	_treeWidget = Object::findChild<QTreeWidget *>(_contactListWidget, "treeWidget");
	
	
	_delegate = new ContactListDelegate(_treeWidget, _treeWidget);
    
	
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

	spx->setPixmap (QtContactPixmap::ContactOnline,QPixmap("images/online.png"));
	spx->setPixmap (QtContactPixmap::ContactDND,QPixmap("images/dnd.png"));
	spx->setPixmap (QtContactPixmap::ContactInvisible,QPixmap("images/offline.png"));
	spx->setPixmap (QtContactPixmap::ContactBRB,QPixmap("images/dnd.png"));
	spx->setPixmap (QtContactPixmap::ContactAway,QPixmap("images/away.png"));
	spx->setPixmap (QtContactPixmap::ContactNotAvailable,QPixmap("images/dnd.png"));
	spx->setPixmap (QtContactPixmap::ContactForward,QPixmap("images/online.png"));
	// Fonctions icons
	spx->setPixmap (QtContactPixmap::ContactIM,QPixmap("images/contact_phone_home.png"));
	spx->setPixmap (QtContactPixmap::ContactCall,QPixmap("images/contact_phone_mobile.png"));
	spx->setPixmap (QtContactPixmap::ContactVideo,QPixmap("images/call_video_button.png"));
	// Group icons
	spx->setPixmap (QtContactPixmap::ContactGroupOpen,QPixmap("images/group_open.png"));
	spx->setPixmap (QtContactPixmap::ContactGroupClose,QPixmap("images/group_close.png"));
	
    new QtUserManager(_treeWidget,_treeWidget);
    
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

	//Connect events
	QtWengoPhone * qtWengoPhone = (QtWengoPhone *) _cContactList.getCWengoPhone().getPresentation();
	qtWengoPhone->setContactList(this);
	_treeWidget->viewport()->setFocus();
	/**********************************************************************************************************************************/
	/*
	
		DELETE THIS !
	
	*/
	QtUserList * ul = QtUserList::getInstance();
    QTreeWidgetItem *group = new QTreeWidgetItem(_treeWidget);
    group->setText(0,"Group 1");

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
    }


}

void QtContactList::updatePresentation() {
	typedef PostEvent0<void ()> MyPostEvent;
	MyPostEvent * event = new MyPostEvent(boost::bind(&QtContactList::updatePresentationThreadSafe, this));
	postEvent(event);
}

void QtContactList::updatePresentationThreadSafe() {
	int row = 0;
	
	_model->reset();

	QModelIndex rootIndex = _treeWidget->rootIndex();
	_treeWidget->setExpanded(rootIndex, true);

	QModelIndex index = rootIndex.child(0, 0);
	while (index.isValid()) {
		_treeWidget->setExpanded(index, true);
		index = rootIndex.child(++row, 0);
	}

	_treeWidget->update();
}

void QtContactList::addContactGroup(PContactGroup * pContactGroup) {
	typedef PostEvent1<void (PContactGroup *), PContactGroup *> MyPostEvent;
	MyPostEvent * event = new MyPostEvent(boost::bind(&QtContactList::addContactGroupThreadSafe, this, _1), pContactGroup);
	postEvent(event);
}

void QtContactList::addContactGroupThreadSafe(PContactGroup * pContactGroup) {
	QtContactGroup * qtContactGroup = (QtContactGroup *) pContactGroup;

	QString tmp = qtContactGroup->getTreeItem()->getData().toString();
	LOG_DEBUG("contact: " + tmp.toStdString());

	_model->getRootItem()->appendChild(qtContactGroup->getTreeItem());
	
	updatePresentationThreadSafe();
}

void QtContactList::doubleClicked(const QModelIndex & index) {
	LOG_DEBUG("double click");
}

void QtContactList::entered(const QModelIndex & index) {
	LOG_DEBUG("mouse entered");
}

void QtContactList::mousePressEventHandler(QEvent *event) {
	QMouseEvent * mouseEvent = (QMouseEvent *)event;
	
	if (mouseEvent->button() == Qt::LeftButton) {
		leftButtonPressEventHandler(mouseEvent);
	} else if (mouseEvent->button() == Qt::RightButton) {
		rightButtonPressEventHandler(mouseEvent);
	}
}

void QtContactList::leftButtonPressEventHandler(QMouseEvent *event) {
	QModelIndex index = _treeWidget->indexAt(event->pos());

	if (!index.parent().isValid()) {
		_treeWidget->setExpanded(index, !_treeWidget->isExpanded(index));
	} else {
		IQtContact *iQtContact = ContactListUtil::getIQtContact(index.data(Qt::UserRole));
		iQtContact->setDisplayDetails(!iQtContact->isDisplayDetailsEnabled());
	}
}

void QtContactList::rightButtonPressEventHandler(QMouseEvent *event) {
	QModelIndex index = _treeWidget->indexAt(event->pos());

	if (!index.parent().isValid()) {
		_contactGroupPopupMenu->showMenu(
			ContactListUtil::getContactGroup(index.data(Qt::UserRole)));		
	} else {
		_contactPopupMenu->showMenu(
			ContactListUtil::getIQtContact(index.data(Qt::UserRole))->getContact());		
	}
}
