/*
 * WengoPhone, a voice over Internet phone
 * Copyright (C) 2004-2007  Wengo
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

#ifndef OWMANAGEACCOUNT_H
#define OWMANAGEACCOUNT_H

#include <QtGui/QDialog>

class EditAccount;
class QAction;
class TCoIpManager;

namespace Ui { class ManageAccount; }

/**
 *
 * @author Philippe Bernery
 */
class ManageAccount : public QDialog {
	Q_OBJECT
public:

	ManageAccount(TCoIpManager & tCoIpManager, QWidget * parent);

	~ManageAccount();

public Q_SLOTS:

	void show();

private Q_SLOTS:

	void addAccount(QAction *);

	void removeAccount();

private:

	void initializeAccountList();

	void initializeAddAccountMenu();

	Ui::ManageAccount * _ui;

	EditAccount * _editAccount;

	TCoIpManager & _tCoIpManager;
};

#endif	//OWMANAGEACCOUNT_H
