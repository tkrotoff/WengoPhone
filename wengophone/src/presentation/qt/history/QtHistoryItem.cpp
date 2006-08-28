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

#include "QtHistoryItem.h"

#include <QtGui/QtGui>

const int QtHistoryItem::COLUMN_TYPE = 0;
const int QtHistoryItem::COLUMN_PEERS = 1;
const int QtHistoryItem::COLUMN_DATE = 2;
const int QtHistoryItem::COLUMN_DURATION = 3;

QtHistoryItem::QtHistoryItem(int type)
	: QTreeWidgetItem(type) {
}

QtHistoryItem::QtHistoryItem(const QStringList & strings, int type)
	: QTreeWidgetItem(strings, type) {
}

QtHistoryItem::QtHistoryItem(QTreeWidget * parent, int type)
	: QTreeWidgetItem(parent, type) {
}

QtHistoryItem::QtHistoryItem(QTreeWidget * parent, const QStringList & strings, int type)
	: QTreeWidgetItem(parent, strings, type) {
}

QtHistoryItem::QtHistoryItem(QTreeWidget * parent, QtHistoryItem * preceding, int type)
	: QTreeWidgetItem(parent, preceding, type) {
}

QtHistoryItem::QtHistoryItem(QtHistoryItem * parent, int type)
	: QTreeWidgetItem(parent, type) {
}

QtHistoryItem::QtHistoryItem(QtHistoryItem * parent, const QStringList & strings, int type)
	: QTreeWidgetItem(parent, strings, type) {
}

QtHistoryItem::QtHistoryItem(QtHistoryItem * parent, QtHistoryItem * preceding, int type)
	: QTreeWidgetItem(parent, preceding, type) {
}

QtHistoryItem::QtHistoryItem(const QtHistoryItem & other)
	: QTreeWidgetItem(other) {
}

QtHistoryItem::~QtHistoryItem() {
}

unsigned QtHistoryItem::getId() const {
	return _id;
}

void QtHistoryItem::setId(unsigned id) {
	_id = id;
}

void QtHistoryItem::setItemType(QtHistoryItem::HistoryType type) {
	_type = type;
}

QtHistoryItem::HistoryType QtHistoryItem::getItemType() const {
	return _type;
}
