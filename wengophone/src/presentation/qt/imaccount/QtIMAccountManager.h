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

#ifndef QTIMACCOUNTMANAGER_H
#define QTIMACCOUNTMANAGER_H

#include <QObject>

#include <string>

class UserProfile;
class QWidget;
class QDialog;
class QAction;
class QTreeWidget;
class QTreeWidgetItem;

/**
 * Manages IM accounts: add/delete/modify IM accounts.
 *
 * @author Tanguy Krotoff
 */
class QtIMAccountManager : public QObject {
	Q_OBJECT
public:

	QtIMAccountManager(UserProfile & userProfile, QWidget * parent);

	QWidget * getWidget() const {
		return _imAccountManagerWidget;
	}

	void show();

private Q_SLOTS:

	void addIMAccount(QAction * action);

	void deleteIMAccount();

	void modifyIMAccount();

	void itemDoubleClicked(QTreeWidgetItem * item, int column);

private:

	void loadIMAccounts();

	UserProfile & _userProfile;

	QWidget * _imAccountManagerWidget;

	QDialog * _imAccountManagerWindow;

	QTreeWidget * _treeWidget;
};

#endif	//QTIMACCOUNTMANAGER_H
