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

#ifndef TREEMODEL_H
#define TREEMODEL_H

#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVariant>

class TreeItem;
class Contact;
class ContactGroup;
class QtContactList;

/**
 * Provides a simple tree model to show how to create and use hierarchical models.
 *
 * Inspired from Qt4's simpletreemodel example.
 *
 * @author Tanguy Krotoff
 */
class TreeModel : public QAbstractItemModel {
	Q_OBJECT
	friend class QtContactList;
public:

	TreeModel(QObject * parent = 0);

	~TreeModel();

	QVariant data(const QModelIndex & index, int role) const;

	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

	QModelIndex index(int row, int column,	const QModelIndex & parent = QModelIndex()) const;

	QModelIndex parent(const QModelIndex & index) const;

	int rowCount(const QModelIndex & parent = QModelIndex()) const;

	int columnCount(const QModelIndex & parent = QModelIndex()) const;

	Qt::ItemFlags flags(const QModelIndex & index) const;

	bool setData(const QModelIndex & index, const QVariant & value, int role = Qt::EditRole);

	TreeItem * getRootItem() const {
		return _rootItem;
	}

private:

	TreeItem * _rootItem;
};

#endif	//TREEMODEL_H
