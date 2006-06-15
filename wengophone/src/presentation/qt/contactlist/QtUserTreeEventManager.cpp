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

#include "QtUserTreeEventManager.h"
#include "QtContactListManager.h"

#include <control/contactlist/CContactList.h>

#include <model/config/ConfigManager.h>
#include <model/config/Config.h>

QtUserTreeEventManager::QtUserTreeEventManager(CContactList & cContactList, QObject * parent, QTreeWidget * target)
	: QObject(parent),
	_cContactList(cContactList) {
	_tree = target;

	/* We need to install the event filter in the viewport of the QTreeWidget */
	target->viewport()->installEventFilter(this);
	_timer.setSingleShot(true);
	_inDrag = false;
	_selectedItem = NULL;
}

bool QtUserTreeEventManager::eventFilter(QObject * obj, QEvent * event) {
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

void QtUserTreeEventManager::mousePressEvent(QMouseEvent * event) {
	mouseClicked(event->button());
	QTreeWidgetItem * item = _tree->itemAt(event->pos());
	QtContactListManager * ul = QtContactListManager::getInstance();

	if (_timer.isActive()) {
		_timer.stop();
	}
	if (item) {
		ul->resetMouseStatus();
		ul->setButton(item->text(0), event->button());
		_selectedItem = item;
		if (event->button() == Qt::LeftButton)
			_dstart = event->pos();
	}
}

void QtUserTreeEventManager::mouseReleaseEvent(QMouseEvent * event) {
	if (event->button() == Qt::RightButton) {
		mouseClicked(Qt::RightButton);
	} else {
		mouseClicked(Qt::NoButton);
	}
}

void QtUserTreeEventManager::mouseMoveEvent(QMouseEvent * event) {

	QTreeWidgetItem * item=_selectedItem;

	if (!_selectedItem) {
		return;
	}

	if (!(event->buttons() & Qt::LeftButton) && !(event->buttons() & Qt::RightButton)) {
		QTreeWidgetItem * tmp = _tree->itemAt(event->pos());
		if ((tmp != _entered)) {
			if (_timer.isActive()) {
				_timer.stop();
			}
			_entered = tmp;
			if (_entered) {
				itemEntered(_entered);
				_timer.start(1500);
			}
		}
	}
	item=_selectedItem;

	if (!(event->buttons() & Qt::LeftButton)) {
		return;
	}

	if ((event->pos() - _dstart).manhattanLength() < QApplication::startDragDistance()) {
		return;
	}

	if (!item) {
		return;
	}

	//If item->parent() == NULL then the item is a group (parent item for contact), a group cannot be moved
	if (!item->parent()) {
		return;
	}

	QByteArray custom; // Define a new empty custom data
	QDrag * drag = new QDrag(_tree);
	QMimeData * mimeData = new QMimeData;
	// FIXME
	mimeData->setText(item->text(0));
	mimeData->setData("application/x-wengo-user-data", custom);
	drag->setMimeData(mimeData);
	_inDrag = true;
	drag->start(Qt::MoveAction);

}

void QtUserTreeEventManager::dragEnterEvent(QDragEnterEvent * event) {
	if (event->mimeData()->hasFormat("application/x-wengo-user-data")) {
		event->acceptProposedAction();
	}
}

void QtUserTreeEventManager::dropEvent(QDropEvent * event) {
	QTreeWidgetItem * p;
	QTreeWidgetItem * item = _tree->itemAt(event->pos());
	_inDrag = false;

	if (!event->mimeData()->hasFormat("application/x-wengo-user-data")) {
		return;
	}
	if (item) {
		if (_selectedItem) {
			if (_selectedItem == item) {
				return;
			}

			if (_selectedItem->parent() == item->parent()) {
				// The destination and the source groups are the same
				// This is a contact combination
				ContactProfile dstContactProfile = _cContactList.getContactProfile(_selectedItem->text(0).toStdString());
				ContactProfile srcContactProfile = _cContactList.getContactProfile(item->text(0).toStdString());
				if (QMessageBox::question(NULL,
					tr("Merge Contacts -- WengoPhone"),
					tr("Merge %1 with %2?")
					.arg(QString::fromStdString(dstContactProfile.getDisplayName()))
					.arg(QString::fromStdString(srcContactProfile.getDisplayName())),
					tr("&Yes"), tr("&No"),
					QString(), 0, 1) == 0) {
					_cContactList.merge(_selectedItem->text(0).toStdString(),
						item->text(0).toStdString());
				}
			} else if (item->parent()) {
				// The destination group and the source group are different
				// This is a Contact move
				ContactProfile contactProfile = _cContactList.getContactProfile(_selectedItem->text(0).toStdString());
				QString groupId = item->parent()->text(0);
				contactProfile.setGroupId(groupId.toStdString());
				_cContactList.updateContact(contactProfile);
			}
		}
		event->acceptProposedAction();
	}
	return;
}

void QtUserTreeEventManager::dragMoveEvent(QDragMoveEvent * event) {
	QTreeWidgetItem * item = _tree->itemAt(event->pos());
	event->setDropAction(Qt::IgnoreAction);
	if (item) {
		event->setDropAction(Qt::MoveAction);
		if (event->mimeData()->hasFormat("application/x-wengo-user-data"))
			event->acceptProposedAction();
	}
}

void QtUserTreeEventManager::timerTimeout() {

	if (_inDrag) {
		return;
	}

	QTreeWidgetItem * item = _tree->itemAt(_tree->mapFromGlobal(QCursor::pos()));

	if (item) {
		itemTimeout(item);
	}
}