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
#ifndef QtUserManager_H
#define QtUserManager_H

#include <QtGui>


#define MAX_ITEM_SIZE 100
#define MIN_ITEM_SIZE 28

class PContact;

class QtUserManager : public QObject {
		Q_OBJECT

	public:
		QtUserManager( QObject * parent = 0, QTreeWidget * target = 0 );

		void setContact(PContact * contact) { _pcontact = contact;};

	public Q_SLOTS:
		void treeViewSelectionChanged();
		void itemClicked ( QTreeWidgetItem * item, int column );
		void itemEntered ( QTreeWidgetItem * item );
		void openUserInfo( QTreeWidgetItem * i );
		void closeUserInfo();
		void editContact(bool checked);

	protected:
		QTreeWidget * _tree;

		QTreeWidgetItem * _previous;

		QTreeWidgetItem * _lastClicked;

		QSize * _itemSize;

		QMenu * _menu;

		PContact * _pcontact;
};

#endif

