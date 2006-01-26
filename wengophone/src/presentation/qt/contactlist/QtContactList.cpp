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
#include "model/WengoPhoneLogger.h"
#include "TreeModel.h"
#include "TreeItem.h"
#include "QtContactGroup.h"
#include "QtContact.h"
#include "ContactListDelegate.h"
#include "ContactListUtil.h"

#include <StringList.h>
#include <Object.h>
#include <WidgetFactory.h>
#include <MouseEventFilter.h>

#include <QtGui>

QtContactList::QtContactList(CContactList & cContactList) 
	: QObjectThreadSafe(), _cContactList(cContactList) {

	_treeView = NULL;
	
	typedef PostEvent0<void ()> MyPostEvent;
	MyPostEvent * event = new MyPostEvent(boost::bind(&QtContactList::initThreadSafe, this));
	postEvent(event);
}

void QtContactList::initThreadSafe() {
	_contactListWidget = WidgetFactory::create(":/forms/contactlist/ContactListWidget.ui", NULL);
	
	//treeView
	_treeView = Object::findChild<QTreeView *>(_contactListWidget, "treeView");
	_delegate = new ContactListDelegate(_treeView, _treeView);
	_treeView->setRootIsDecorated(false);
	_treeView->setDragEnabled(true);
	_treeView->setItemDelegate(_delegate);
	_treeView->setAttribute(Qt::WA_MouseTracking);
	_treeView->setAttribute(Qt::WA_Hover);
	
	//model
	_model = new TreeModel();
	_treeView->setModel(_model);
	
	//Popup Menus
	_contactPopupMenu = new ContactPopupMenu(_treeView);
	_contactGroupPopupMenu = new ContactGroupPopupMenu(_treeView);

	//Connect events
	_treeView->viewport()->installEventFilter(new MousePressEventFilter(this, SLOT(mousePressEventHandler(QEvent *))));
	connect(_treeView, SIGNAL(entered(const QModelIndex &)), SLOT(entered(const QModelIndex &)), Qt::DirectConnection);
	//connect(_treeView, SIGNAL(doubleClicked(const QModelIndex &)), SLOT(doubleClicked(const QModelIndex &)), Qt::DirectConnection);
	
	QtWengoPhone * qtWengoPhone = (QtWengoPhone *) _cContactList.getCWengoPhone().getPresentation();
	qtWengoPhone->setContactList(this);
	
	_treeView->viewport()->setFocus();
}

void QtContactList::updatePresentation() {
	typedef PostEvent0<void ()> MyPostEvent;
	MyPostEvent * event = new MyPostEvent(boost::bind(&QtContactList::updatePresentationThreadSafe, this));
	postEvent(event);
}

void QtContactList::updatePresentationThreadSafe() {
	int row = 0;
	
	_model->reset();

	QModelIndex rootIndex = _treeView->rootIndex();
	_treeView->setExpanded(rootIndex, true);

	QModelIndex index = rootIndex.child(0, 0);
	while (index.isValid()) {
		_treeView->setExpanded(index, true);
		index = rootIndex.child(++row, 0);
	}

	_treeView->update();
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
	QModelIndex index = _treeView->indexAt(event->pos());

	if (!index.parent().isValid()) {
		_treeView->setExpanded(index, !_treeView->isExpanded(index));
	} else {
		IQtContact *iQtContact = ContactListUtil::getIQtContact(index.data(Qt::UserRole));
		iQtContact->setDisplayDetails(!iQtContact->isDisplayDetailsEnabled());
	}
}

void QtContactList::rightButtonPressEventHandler(QMouseEvent *event) {
	QModelIndex index = _treeView->indexAt(event->pos());

	if (!index.parent().isValid()) {
		_contactGroupPopupMenu->showMenu(
			ContactListUtil::getContactGroup(index.data(Qt::UserRole)));		
	} else {
		_contactPopupMenu->showMenu(
			ContactListUtil::getIQtContact(index.data(Qt::UserRole))->getContact());		
	}
}
