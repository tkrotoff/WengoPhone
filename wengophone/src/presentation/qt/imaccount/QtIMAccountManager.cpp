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

#include "QtIMAccountItem.h"
#include "QtIMAccountSettings.h"

#include <model/profile/UserProfile.h>

#include <util/Logger.h>

#include <qtutil/WidgetFactory.h>
#include <qtutil/Object.h>
#include <qtutil/Widget.h>

#include <QtGui>

QtIMAccountManager::QtIMAccountManager(UserProfile & userProfile, QWidget * parent)
	: QObject(parent),
	_userProfile(userProfile) {

	_imAccountManagerWidget = WidgetFactory::create(":/forms/imaccount/IMAccountManager.ui", parent);
	_imAccountManagerWindow = Widget::transformToWindow(_imAccountManagerWidget);
	_imAccountManagerWindow->setWindowTitle(_imAccountManagerWidget->windowTitle());

	QPushButton * addIMAccountButton = Object::findChild<QPushButton *>(_imAccountManagerWidget, "addIMAccountButton");
	QMenu * addIMAccountMenu = new QMenu(addIMAccountButton);
	connect(addIMAccountMenu, SIGNAL(triggered(QAction *)), SLOT(addIMAccount(QAction *)));

	addIMAccountMenu->addAction(QIcon(":pics/protocol_msn.png"),
				QString::fromStdString(EnumIMProtocol::toString(EnumIMProtocol::IMProtocolMSN)));
	addIMAccountMenu->addAction(QIcon(":pics/protocol_aim.png"),
				QString::fromStdString(EnumIMProtocol::toString(EnumIMProtocol::IMProtocolAIMICQ)));
	addIMAccountMenu->addAction(QIcon(":pics/protocol_yahoo.png"),
				QString::fromStdString(EnumIMProtocol::toString(EnumIMProtocol::IMProtocolYahoo)));
	addIMAccountMenu->addAction(QIcon(":pics/protocol_jabber.png"),
				QString::fromStdString(EnumIMProtocol::toString(EnumIMProtocol::IMProtocolJabber)));
	addIMAccountMenu->addAction(QIcon(":pics/protocol_googletalk.png"),
				QString::fromStdString(EnumIMProtocol::toString(EnumIMProtocol::IMProtocolGoogleTalk)));
	addIMAccountButton->setMenu(addIMAccountMenu);

	QPushButton * modifyIMAccountButton = Object::findChild<QPushButton *>(_imAccountManagerWidget, "modifyIMAccountButton");
	connect(modifyIMAccountButton, SIGNAL(clicked()), SLOT(modifyIMAccount()));

	QPushButton * deleteIMAccountButton = Object::findChild<QPushButton *>(_imAccountManagerWidget, "deleteIMAccountButton");
	connect(deleteIMAccountButton, SIGNAL(clicked()), SLOT(deleteIMAccount()));

	QPushButton * closeButton = Object::findChild<QPushButton *>(_imAccountManagerWidget, "closeButton");
	connect(closeButton, SIGNAL(clicked()), _imAccountManagerWindow, SLOT(accept()));

	_treeWidget = Object::findChild<QTreeWidget *>(_imAccountManagerWidget, "treeWidget");
	connect(_treeWidget, SIGNAL(itemDoubleClicked(QTreeWidgetItem *, int)),
			SLOT(itemDoubleClicked(QTreeWidgetItem *, int)));

	connect(_treeWidget, SIGNAL(itemClicked(QTreeWidgetItem *, int)),
			SLOT(itemClicked(QTreeWidgetItem *, int)));

	loadIMAccounts();
}

void QtIMAccountManager::show() {
	_imAccountManagerWindow->exec();
}

void QtIMAccountManager::loadIMAccounts() {
	static const int COLUMN_ENABLE_BUTTON = 2;

	_treeWidget->clear();

	IMAccountHandler & imAccountHandler = _userProfile.getIMAccountHandler();

	for (IMAccountHandler::iterator it = imAccountHandler.begin(); it != imAccountHandler.end(); it++) {
		IMAccount * imAccount = (IMAccount *) &(*it);
		QStringList accountStrList;
		accountStrList << QString::fromStdString(imAccount->getLogin());
		EnumIMProtocol::IMProtocol imProtocol = imAccount->getProtocol();

		if (imProtocol == EnumIMProtocol::IMProtocolSIPSIMPLE) {
			//This protocol is internal to WengoPhone, should not be shown to the user
			continue;
		}

		accountStrList << QString::fromStdString(EnumIMProtocol::toString(imProtocol));
		accountStrList << QString::null;

		QtIMAccountItem * item = new QtIMAccountItem(_treeWidget, accountStrList);
		item->setCheckState(COLUMN_ENABLE_BUTTON,
				(imAccount->getPresenceState() == EnumPresenceState::PresenceStateOnline) ? Qt::Checked : Qt::Unchecked);
		item->setIMAccount(imAccount);
	}
}

void QtIMAccountManager::addIMAccount(QAction * action) {
	QString protocolName = action->text();
	LOG_DEBUG(protocolName.toStdString());

	EnumIMProtocol::IMProtocol imProtocol = EnumIMProtocol::toIMProtocol(protocolName.toStdString());
	QtIMAccountSettings * qtIMAccountSettings = new QtIMAccountSettings(_userProfile, imProtocol, _imAccountManagerWindow);
	loadIMAccounts();
}

void QtIMAccountManager::deleteIMAccount() {
	QtIMAccountItem * imAccountItem = (QtIMAccountItem *) _treeWidget->currentItem();
	if (imAccountItem) {
		IMAccount * imAccount = imAccountItem->getIMAccount();

		int buttonClicked = QMessageBox::question(_imAccountManagerWindow,
					"WengoPhone",
					tr("Are sure you want to delete this account?\n") + QString::fromStdString(imAccount->getLogin()),
					tr("&Delete"), tr("Cancel"));

		//Button delete clicked
		if (buttonClicked == 0) {
			_userProfile.removeIMAccount(*imAccount);
		}
	}

	loadIMAccounts();
}

void QtIMAccountManager::modifyIMAccount() {
	loadIMAccounts();
}

void QtIMAccountManager::itemClicked(QTreeWidgetItem * item, int column) {
	if (column == 2) {
		QtIMAccountItem * imAccountItem = dynamic_cast<QtIMAccountItem *>(item);
		IMAccount * imAccount = imAccountItem->getIMAccount();

		if (item->checkState(column) == Qt::Checked) {
			item->setCheckState(column, Qt::Unchecked);
			_userProfile.setPresenceState(EnumPresenceState::PresenceStateOffline, imAccount);
			_userProfile.getConnectHandler().disconnect(*imAccount);
		} else {
			item->setCheckState(column, Qt::Checked);
			_userProfile.getConnectHandler().connect(*imAccount);
		}
	}
}

void QtIMAccountManager::itemDoubleClicked(QTreeWidgetItem * item, int column) {
	QtIMAccountItem * imAccountItem = dynamic_cast<QtIMAccountItem *>(item);

	IMAccount * imAccount = imAccountItem->getIMAccount();

	QtIMAccountSettings * qtIMAccountSettings = new QtIMAccountSettings(_userProfile, imAccount, _imAccountManagerWindow);
}
