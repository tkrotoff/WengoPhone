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

#ifndef TREEITEM_H
#define TREEITEM_H

#include <QList>
#include <QVariant>

/**
 * A container for items of data supplied by the simple tree model.
 *
 * Inspired from Qt4's simpletreemodel example.
 *
 * @author Tanguy Krotoff
 */
class TreeItem {
public:

	TreeItem(const QVariant & data);

	~TreeItem();

	void appendChild(TreeItem * child);

	TreeItem * getChild(int row);

	int childCount() const;

	void setData(const QVariant & data);

	QVariant getData() const;

	int getRow() const;

	TreeItem * getParent();

private:

	QList < TreeItem * > _childItems;

	QVariant _itemData;

	TreeItem * _parentItem;
};

#endif	//TREEITEM_H
