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

#include "ManageAccount.h"

#include "EditAccount.h"

#include "ui_ManageAccount.h"

#include <coipmanager_threaded/TCoIpManager.h>
#include <coipmanager_threaded/connectmanager/TConnectManager.h>
#include <coipmanager_threaded/datamanager/TUserProfileManager.h>

#include <util/SafeConnect.h>

#include <util/SafeDelete.h>

#include <QtGui/QtGui>

#include <iostream>

ManageAccount::ManageAccount(TCoIpManager & tCoIpManager, QWidget * parent)
	: QDialog(parent),
	_tCoIpManager(tCoIpManager) {

	_ui = new Ui::ManageAccount();
	_ui->setupUi(this);

	_editAccount = NULL;

	initializeAccountList();
	initializeAddAccountMenu();

	SAFE_CONNECT(_ui->removeButton, SIGNAL(clicked()), SLOT(removeAccount()));
}

ManageAccount::~ManageAccount() {
	OWSAFE_DELETE(_ui);
}

void ManageAccount::initializeAddAccountMenu() {
	QMenu * menu = new QMenu(_ui->addButton);

	menu->addAction(QString::fromStdString(EnumAccountType::toString(EnumAccountType::AccountTypeMSN)));
	menu->addAction(QString::fromStdString(EnumAccountType::toString(EnumAccountType::AccountTypeYahoo)));
	menu->addAction(QString::fromStdString(EnumAccountType::toString(EnumAccountType::AccountTypeAIM)));
	menu->addAction(QString::fromStdString(EnumAccountType::toString(EnumAccountType::AccountTypeICQ)));
	menu->addAction(QString::fromStdString(EnumAccountType::toString(EnumAccountType::AccountTypeGTalk)));
	menu->addAction(QString::fromStdString(EnumAccountType::toString(EnumAccountType::AccountTypeJabber)));
	// FIXME: SIP is hidden because it needs a special window to set parameters on the Account
	//menu->addAction(QString::fromStdString(EnumAccountType::toString(EnumAccountType::AccountTypeSIP)));
	menu->addAction(QString::fromStdString(EnumAccountType::toString(EnumAccountType::AccountTypeWengo)));
	menu->addAction(QString::fromStdString(EnumAccountType::toString(EnumAccountType::AccountTypeIAX)));

	_ui->addButton->setMenu(menu);
	_ui->addButton->setPopupMode(QToolButton::InstantPopup);

	SAFE_CONNECT(menu, SIGNAL(triggered(QAction *)), SLOT(addAccount(QAction *)));
}

void ManageAccount::initializeAccountList() {
	_ui->accountList->clear();

	// Updating list of Account
	AccountList accountList = _tCoIpManager.getTUserProfileManager().getCopyOfUserProfile().getAccountList();
	for (AccountList::const_iterator it = accountList.begin();
		it != accountList.end();
		++it) {

		QStringList accountStrList;
		accountStrList << QString::fromStdString((*it).getLogin());
		accountStrList << QString::fromStdString(EnumAccountType::toString((*it).getAccountType()));

		QTreeWidgetItem * item = new QTreeWidgetItem(_ui->accountList, accountStrList);
		item->setData(0, Qt::UserRole, QString::fromStdString((*it).getUUID()));
	}
	////
}

void ManageAccount::show() {
	initializeAccountList();

	QDialog::show();
}

void ManageAccount::addAccount(QAction * action) {
	if (!_editAccount) {
		_editAccount = new EditAccount(this);
	}

	_editAccount->setLogin(QString::null);
	_editAccount->setPassword(QString::null);

	if (_editAccount->exec()) {
		std::string login = _editAccount->login().toStdString();
		std::string password = _editAccount->password().toStdString();
		EnumAccountType::AccountType accountType = EnumAccountType::toAccountType(action->text().toStdString());

		Account account(login, password, accountType);
		account.setSavePassword(true);
		_tCoIpManager.getTUserProfileManager().getTAccountManager().add(account);
		_tCoIpManager.getTConnectManager().connect(account.getUUID());

		initializeAccountList();
	}
}

void ManageAccount::removeAccount() {
	QTreeWidgetItem * item = _ui->accountList->currentItem();
	if (item) {
		std::string accountId = item->data(0, Qt::UserRole).toString().toStdString();
		_tCoIpManager.getTUserProfileManager().getTAccountManager().remove(accountId);
		initializeAccountList();
	}
}
