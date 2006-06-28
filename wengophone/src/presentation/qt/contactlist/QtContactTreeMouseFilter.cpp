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

#include "QtContactTreeMouseFilter.h"
#include "QtContactListManager.h"

#include <control/contactlist/CContactList.h>

#include <model/config/ConfigManager.h>
#include <model/config/Config.h>

#include <QTreeWidgetItem>
#include <QMouseEvent>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QDragMoveEvent>
#include <QApplication>
#include <QMessageBox>

const QString QtContactTreeMouseFilter::WENGO_MIME_TYPE = "application/x-wengo-user-data";

QtContactTreeMouseFilter::QtContactTreeMouseFilter(CContactList & cContactList, QObject * parent, QTreeWidget * treeWidget)
	: QObject(parent),
	_cContactList(cContactList) {
	_tree = treeWidget;

	/* The event filter must be installed in the viewport of the QTreeWidget */
	_tree->viewport()->installEventFilter(this);
	_inDrag = false;
	_selectedItem = QString::null;

}

bool QtContactTreeMouseFilter::eventFilter(QObject * obj, QEvent * event) {
	switch (event->type()) {
	case QEvent::MouseButtonPress:
		mousePressEvent(dynamic_cast <QMouseEvent *> (event));
		return false;
	case QEvent::MouseButtonRelease:
		mouseReleaseEvent(dynamic_cast <QMouseEvent *> (event));
		return false;
	case QEvent::MouseMove:
		mouseMoveEvent(dynamic_cast <QMouseEvent *> (event));
		event->accept();
		return true;
	case QEvent::DragEnter:
		dragEnterEvent(dynamic_cast <QDragEnterEvent *> (event));
		event->accept();
		return true;
	case QEvent::Drop:
		dropEvent(dynamic_cast <QDropEvent *> (event));
		event->accept();
		return true;
	case QEvent::DragMove:
		dragMoveEvent(dynamic_cast <QDragMoveEvent *> (event));
		event->accept();
		return true;
	default:
		return QObject::eventFilter(obj, event);
	}
}

void QtContactTreeMouseFilter::mousePressEvent(QMouseEvent * event) {

	mouseClicked(event->button());
	QTreeWidgetItem * item = _tree->itemAt(event->pos());
	QtContactListManager * ul = QtContactListManager::getInstance();
	if (item) {
		if (!item->parent()) {
			return;
		}
		ul->resetMouseStatus();
		_selectedItem = item->text(0);
		ul->setButton(_selectedItem, event->button());
		if (event->button() == Qt::LeftButton)
			_dstart = event->pos();
	}
}

void QtContactTreeMouseFilter::mouseReleaseEvent(QMouseEvent * event) {
	if (event->button() == Qt::RightButton) {
		mouseClicked(Qt::RightButton);
	} else {
		mouseClicked(Qt::NoButton);
	}
}

void QtContactTreeMouseFilter::mouseMoveEvent(QMouseEvent * event) {
	if (_selectedItem.isNull()) {
		return;
	}

	if (!(event->buttons() & Qt::LeftButton)) {
		return;
	}

	if ((event->pos() - _dstart).manhattanLength() < QApplication::startDragDistance()) {
		return;
	}

	// Define a new empty custom data
	QByteArray custom;
	QDrag * drag = new QDrag(_tree);
	QMimeData * mimeData = new QMimeData;
	QtContactListManager * ul = QtContactListManager::getInstance();

	QtContact * contact = ul->getContact(_selectedItem);
	if (contact) {
		mimeData->setText(contact->getUserName());
		mimeData->setData(WENGO_MIME_TYPE, custom);
		drag->setMimeData(mimeData);
		_inDrag = true;
		drag->start(Qt::MoveAction);
	}
}

void QtContactTreeMouseFilter::dragEnterEvent(QDragEnterEvent * event) {
	if (event->mimeData()->hasFormat(WENGO_MIME_TYPE)) {
		event->acceptProposedAction();
	}
}

void QtContactTreeMouseFilter::dropEvent(QDropEvent * event) {

	QTreeWidgetItem * item = _tree->itemAt(event->pos());

	_inDrag = false;

	if (!event->mimeData()->hasFormat(WENGO_MIME_TYPE)) {
		return;
	}

	if (item) {

		if (!_selectedItem.isNull()) {
			if (_selectedItem == item->text(0)) {
				_selectedItem = QString::null;
				return;
			}
			if (_cContactList.getContactProfile(_selectedItem.toStdString()).getGroupId() ==
				_cContactList.getContactProfile(item->text(0).toStdString()).getGroupId() ) {
				// The destination and the source groups are the same
				// This is a contact combination
				mergeContacts(_selectedItem, item->text(0));
			} else {
				if (item->parent()) {
					// The destination group and the source group are different
					// This is a Contact move
					ContactProfile contactProfile = _cContactList.getContactProfile(_selectedItem.toStdString());
					QString groupId = item->parent()->text(0);
					contactProfile.setGroupId(groupId.toStdString());
					_cContactList.updateContact(contactProfile);
				} else {
					// The destination is a group, not a contact, add the contact to the group
					// This is a Contact move
					ContactProfile contactProfile = _cContactList.getContactProfile(_selectedItem.toStdString());
					QString groupId = item->text(0);
					contactProfile.setGroupId(groupId.toStdString());
					_cContactList.updateContact(contactProfile);
				}

			}
		}
		event->acceptProposedAction();
	}

	_selectedItem = QString::null;
	return;
}

void QtContactTreeMouseFilter::dragMoveEvent(QDragMoveEvent * event) {
	QTreeWidgetItem * item = _tree->itemAt(event->pos());
	event->setDropAction(Qt::IgnoreAction);
	if (item) {
		event->setDropAction(Qt::MoveAction);
		if (event->mimeData()->hasFormat(WENGO_MIME_TYPE)) {
			event->acceptProposedAction();
		}
	}
}
