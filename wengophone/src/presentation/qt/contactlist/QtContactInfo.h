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

#ifndef OWCONTACTINFO_H
#define OWCONTACTINFO_H

#include "QtContactPixmap.h"

#include <QtCore/QObject>

class QTreeWidgetItem;
class QtContact;

/**
 * Store contact informations for sorting
 *
 * @author Mr K
 */
class QtContactInfo : public QObject {
	Q_OBJECT
public:

	QtContactInfo(QTreeWidgetItem * item, QTreeWidgetItem * parentItem, const QString & contactId, int index, QObject * parent);

	QtContactInfo(const QtContactInfo & other);

	~QtContactInfo();

	QTreeWidgetItem * getItem() const;

	QTreeWidgetItem * getParentItem() const;

	QString getContactId() const;

	int getIndex() const;

	void clear();

	bool isCleared() const;

	QtContactPixmap::ContactPixmap getStatus() const;

	bool operator<(const QtContactInfo & other) const;

	QtContactInfo & operator=(const QtContactInfo & other);

private:

	QTreeWidgetItem * _item;

	QTreeWidgetItem * _parentItem;

	QString _contactId;

	int _index;

	bool _clear;
};

#endif	//OWCONTACTINFO_H
