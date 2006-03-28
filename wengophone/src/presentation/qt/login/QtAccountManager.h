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

#ifndef QTACCOUNTMANAGERWIDGET_H
#define QTACCOUNTMANAGERWIDGET_H

#include <QtGui>

class WengoPhone;

class QtAccountManager : public QDialog
{

	Q_OBJECT
public:

	QtAccountManager(WengoPhone & wengoPhone, QWidget * parent=0);

protected:

	void loadImAccounts();

	WengoPhone & _wengoPhone;

	QWidget * _widget;

	QPushButton * _imAddPushButton;

	QPushButton * _imDeletePushButton;

	QPushButton * _imModifyPushButton;

	QPushButton * _closePushButton;

	QTreeWidget * _treeWidget;

protected Q_SLOTS:

	void addImAccount();
	void deleteImAccount();
	void modifyImAccount();
	void itemDoubleClicked ( QTreeWidgetItem * item, int column );
};
#endif
