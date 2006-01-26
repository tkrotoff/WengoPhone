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

#include "TreeModel.h"
#include "TreeItem.h"
#include "MetaContact.h"
#include "MetaContactGroup.h"
#include <model/contactlist/Contact.h>
#include <model/contactlist/ContactGroup.h>
#include "ContactListUtil.h"
#include "QtContact.h"

#include <QtGui>

TreeModel::TreeModel(QObject * parent) : QAbstractItemModel(parent) {
	_rootItem = new TreeItem(QVariant());
}

TreeModel::~TreeModel() {
	delete _rootItem;
}

int TreeModel::columnCount(const QModelIndex & parent) const {
	return 1;
}

QVariant TreeModel::data(const QModelIndex & index, int role) const {
	QVariant v;

	if (!index.isValid()) {
		return QVariant();
	}

	TreeItem * item = static_cast < TreeItem * > (index.internalPointer());
	IQtContact * iQtContact = ContactListUtil::getIQtContact(item->getData());
	Contact * contact = NULL;
	if (iQtContact) {
		contact = iQtContact->getContact();
	}
	ContactGroup *contactGroup = ContactListUtil::getContactGroup(item->getData());

	switch (role) {

	case Qt::WhatsThisRole: {
		if (iQtContact) {
			v = tr("Contact name. Left click to get more information");
		} else if (contactGroup) {
			v = tr("Group name. Left click to collapse the group");
		}
		break;
	}

	case Qt::ToolTipRole: {
		if (iQtContact) {
			v = QString::fromStdString((contact->getFirstName() + " " + contact->getLastName()));
		} else if (contactGroup) {
			v = QString::fromStdString(contactGroup->toString());
		}
		break;
	}

	case Qt::DisplayRole: {
		if (iQtContact) {
			v = QString::fromStdString(contact->getFirstName());
		} else if (contactGroup) {
			v = QString::fromStdString(contactGroup->toString());
		}
		break;
	}

	case Qt::DecorationRole: {
		if (iQtContact) {
			//v = contact->getIcon();
			static QIcon iconContact(":/pics/contact_phone_wengo.png");
			v = iconContact;
		} else if (contactGroup) {
			//v = contactGroup->getIcon();
			static QIcon iconGroup(":/pics/contact_phone_wengo.png");
			v = iconGroup;
		}
		break;
	}

	case Qt::UserRole: {
		return item->getData();
	}

	}

	return v;
}

QVariant TreeModel::headerData(int section, Qt::Orientation orientation, int role) const {
	if (section == 0) {
		return "Contact list";
	} else {
		return "";
	}
}

QModelIndex TreeModel::index(int row, int column, const QModelIndex & parent) const {
	TreeItem * parentItem;

	if (!parent.isValid()) {
		parentItem = _rootItem;
	} else {
		parentItem = static_cast < TreeItem * > (parent.internalPointer());
	}

	TreeItem * childItem = parentItem->getChild(row);
	if (childItem) {
		return createIndex(row, column, childItem);
	} else {
		return QModelIndex();
	}
}

QModelIndex TreeModel::parent(const QModelIndex & index) const {
	if (!index.isValid()) {
		return QModelIndex();
	}

	TreeItem * childItem = static_cast < TreeItem * > (index.internalPointer());
	TreeItem * parentItem = childItem->getParent();

	if (parentItem == _rootItem) {
		return QModelIndex();
	}

	return createIndex(parentItem->getRow(), 0, parentItem);
}

int TreeModel::rowCount(const QModelIndex & parent) const {
	TreeItem * parentItem;

	if (!parent.isValid()) {
		parentItem = _rootItem;
	} else {
		parentItem = static_cast < TreeItem * > (parent.internalPointer());
	}

	return parentItem->childCount();
}

Qt::ItemFlags TreeModel::flags(const QModelIndex & index) const {
	//return Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsDragEnabled | Qt::ItemIsEnabled;
	return Qt::ItemIsSelectable | Qt::ItemIsDragEnabled | Qt::ItemIsEnabled;
}

bool TreeModel::setData(const QModelIndex & index, const QVariant & value, int role) {
	Contact * contact = NULL;
	IQtContact *iQtContact = ContactListUtil::getIQtContact(index.data(Qt::UserRole));
	if (iQtContact) {
		contact = iQtContact->getContact();
	}
	
	ContactGroup *contactGroup = ContactListUtil::getContactGroup(index.data(Qt::UserRole));

	if (iQtContact) {
		contact->setFirstName(value.toString().toStdString());
		return true;
	} else if (contactGroup) {
		contactGroup->setName(value.toString().toStdString());
		return true;
	} else {
		return false;
	}
}
