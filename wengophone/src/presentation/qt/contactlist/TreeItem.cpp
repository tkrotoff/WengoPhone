/*
 * Copyright (C) 2005-2005 Trolltech AS. All rights reserved.
 *
 * This file is part of the example classes of the Qt Toolkit.
 *
 * Licensees holding a valid Qt License Agreement may use this file in
 * accordance with the rights, responsibilities and obligations
 * contained therein.  Please consult your licensing agreement or
 * contact sales@trolltech.com if any conditions of this licensing
 * agreement are not clear to you.
 *
 * Further information about Qt licensing is available at:
 * http://www.trolltech.com/products/qt/licensing.html or by
 * contacting info@trolltech.com.
 *
 * This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 * WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 */

#include "TreeItem.h"

#include <QStringList>

TreeItem::TreeItem(const QVariant & data) {
	_parentItem = NULL;
	_itemData = data;
}

TreeItem::~TreeItem() {
	qDeleteAll(_childItems);
}

void TreeItem::appendChild(TreeItem * item) {
	_childItems.append(item);
	item->_parentItem = this;
}

TreeItem * TreeItem::getChild(int row) {
	return _childItems.value(row);
}

int TreeItem::childCount() const {
	return _childItems.count();
}

void TreeItem::setData(const QVariant & data) {
	_itemData = data;
}

QVariant TreeItem::getData() const {
	return _itemData;
}

TreeItem * TreeItem::getParent() {
	return _parentItem;
}

int TreeItem::getRow() const {
	if (_parentItem) {
		return _parentItem->_childItems.indexOf(const_cast < TreeItem * > (this));
	}

	return 0;
}
