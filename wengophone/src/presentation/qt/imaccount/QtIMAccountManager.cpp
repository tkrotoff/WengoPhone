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

#include "QtIMAccountManager.h"

#include "ui_IMAccountManager.h"

#include "QtIMAccountItem.h"
#include "QtIMAccountSettings.h"

#include <model/profile/UserProfile.h>

#include <util/Logger.h>

#include <QtGui/QtGui>

static const int COLUMN_ENABLE_BUTTON = 2;

QtIMAccountManager::QtIMAccountManager(UserProfile & userProfile, bool showAsDialog, QWidget * parent)
	: QObject(parent),
	_userProfile(userProfile) {

	if (showAsDialog) {
		_imAccountManagerWidget = new QDialog(parent);
	} else {
		_imAccountManagerWidget = new QWidget(parent);
	}

	_ui = new Ui::IMAccountManager();
	_ui->setupUi(_imAccountManagerWidget);

	QMenu * addIMAccountMenu = new QMenu(_ui->addIMAccountButton);
	connect(addIMAccountMenu, SIGNAL(triggered(QAction *)), SLOT(addIMAccount(QAction *)));

	addIMAccountMenu->addAction(QIcon(":pics/protocols/msn.png"),
				QtEnumIMProtocol::toString(QtEnumIMProtocol::IMProtocolMSN));
	addIMAccountMenu->addAction(QIcon(":pics/protocols/aim.png"),
				QtEnumIMProtocol::toString(QtEnumIMProtocol::IMProtocolAIMICQ));
	addIMAccountMenu->addAction(QIcon(":pics/protocols/yahoo.png"),
				QtEnumIMProtocol::toString(QtEnumIMProtocol::IMProtocolYahoo));
	addIMAccountMenu->addAction(QIcon(":pics/protocols/jabber.png"),
				QtEnumIMProtocol::toString(QtEnumIMProtocol::IMProtocolJabber));
	addIMAccountMenu->addAction(QIcon(":pics/protocols/jabber.png"),
				QtEnumIMProtocol::toString(QtEnumIMProtocol::IMProtocolGoogleTalk));
	_ui->addIMAccountButton->setMenu(addIMAccountMenu);

	connect(_ui->modifyIMAccountButton, SIGNAL(clicked()), SLOT(modifyIMAccount()));

	connect(_ui->deleteIMAccountButton, SIGNAL(clicked()), SLOT(deleteIMAccount()));

	if (showAsDialog) {
		connect(_ui->closeButton, SIGNAL(clicked()), _imAccountManagerWidget, SLOT(accept()));
	} else {
		_imAccountManagerWidget->layout()->setMargin(0);
		_imAccountManagerWidget->layout()->setSpacing(0);
		_ui->closeButton->hide();
	}

	connect(_ui->treeWidget, SIGNAL(itemDoubleClicked(QTreeWidgetItem *, int)),
			SLOT(itemDoubleClicked(QTreeWidgetItem *, int)));

	connect(_ui->treeWidget, SIGNAL(itemClicked(QTreeWidgetItem *, int)),
			SLOT(itemClicked(QTreeWidgetItem *, int)));

	loadIMAccounts();

	if (showAsDialog) {
		((QDialog *) _imAccountManagerWidget)->exec();
	}
}

QtIMAccountManager::~QtIMAccountManager() {
	delete _ui;
}

void QtIMAccountManager::loadIMAccounts() {
	_ui->treeWidget->clear();

	IMAccountHandler & imAccountHandler = _userProfile.getIMAccountHandler();

	for (IMAccountHandler::iterator it = imAccountHandler.begin(); it != imAccountHandler.end(); it++) {
		IMAccount * imAccount = (IMAccount *) &(*it);
		QStringList accountStrList;
		accountStrList << QString::fromStdString(imAccount->getLogin());
		EnumIMProtocol::IMProtocol imProtocol = imAccount->getProtocol();

		if (imProtocol == EnumIMProtocol::IMProtocolWengo) {
			//This protocol is internal to WengoPhone, should not be shown to the user
			continue;
		}

		accountStrList << QString::fromStdString(EnumIMProtocol::toString(imProtocol));
		accountStrList << QString::null;

		QtIMAccountItem * item = new QtIMAccountItem(_ui->treeWidget, accountStrList);
		item->setCheckState(COLUMN_ENABLE_BUTTON,
				(imAccount->getPresenceState() == EnumPresenceState::PresenceStateOnline) ? Qt::Checked : Qt::Unchecked);
		item->setIMAccount(imAccount);
	}
}

void QtIMAccountManager::addIMAccount(QAction * action) {
	QString protocolName = action->text();
	LOG_DEBUG(protocolName.toStdString());

	QtEnumIMProtocol::IMProtocol imProtocol = QtEnumIMProtocol::toIMProtocol(protocolName);
	QtIMAccountSettings * qtIMAccountSettings = new QtIMAccountSettings(_userProfile, imProtocol, _imAccountManagerWidget);
	loadIMAccounts();
}

void QtIMAccountManager::deleteIMAccount() {
	QtIMAccountItem * imAccountItem = (QtIMAccountItem *) _ui->treeWidget->currentItem();
	if (imAccountItem) {
		IMAccount * imAccount = imAccountItem->getIMAccount();

		int buttonClicked = QMessageBox::question(_imAccountManagerWidget,
					"WengoPhone",
					tr("Are sure you want to delete this account?\n") +
					QString::fromStdString(imAccount->getLogin()),
					tr("&Delete"), tr("Cancel"));

		//Button delete clicked
		if (buttonClicked == 0) {
			_userProfile.removeIMAccount(*imAccount);
		}
	}

	loadIMAccounts();
}

void QtIMAccountManager::modifyIMAccount() {
	QtIMAccountItem * imAccountItem = (QtIMAccountItem *) _ui->treeWidget->currentItem();
	if (imAccountItem) {
		IMAccount * imAccount = imAccountItem->getIMAccount();
		QtIMAccountSettings * qtIMAccountSettings = new QtIMAccountSettings(_userProfile, imAccount, _imAccountManagerWidget);
	}
}

void QtIMAccountManager::itemClicked(QTreeWidgetItem * item, int column) {
	if (column == COLUMN_ENABLE_BUTTON) {
		QtIMAccountItem * imAccountItem = dynamic_cast<QtIMAccountItem *>(item);
		IMAccount * imAccount = imAccountItem->getIMAccount();

		if (item->checkState(column) == Qt::Checked) {
			_userProfile.setPresenceState(EnumPresenceState::PresenceStateOffline, imAccount);
			_userProfile.getConnectHandler().disconnect(*imAccount);
		} else {
			_userProfile.getConnectHandler().connect(*imAccount);
		}
	}
}

void QtIMAccountManager::itemDoubleClicked(QTreeWidgetItem * item, int column) {
	_ui->treeWidget->setCurrentItem(item);
	modifyIMAccount();
}
