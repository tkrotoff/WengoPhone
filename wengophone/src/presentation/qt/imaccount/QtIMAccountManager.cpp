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

#include <QtGui>

static const QString PROTOCOL_MSN_NAME = "MSN";
static const QString PROTOCOL_AIMICQ_NAME = "AIM/ICQ";
static const QString PROTOCOL_YAHOO_NAME = "Yahoo";
static const QString PROTOCOL_JABBER_NAME = "Jabber";
static const QString PROTOCOL_SIPSIMPLE_NAME = "SIP/SIMPLE";

QLayout * createLayout(QWidget * parent) {
	QGridLayout * layout = new QGridLayout(parent);
	layout->setSpacing(0);
	layout->setMargin(0);
	return layout;
}

QDialog * createWindowFromWidget(QWidget * widget) {
	QDialog * dialog = new QDialog(widget->parentWidget());
	widget->setParent(NULL);
	createLayout(dialog)->addWidget(widget);
	return dialog;
}


QtIMAccountManager::QtIMAccountManager(UserProfile & userProfile, QWidget * parent)
	: QObject(parent),
	_userProfile(userProfile) {

	_imAccountManagerWidget = WidgetFactory::create(":/forms/imaccount/IMAccountManager.ui", parent);
	_imAccountManagerWindow = createWindowFromWidget(_imAccountManagerWidget);
	_imAccountManagerWindow->setWindowTitle(_imAccountManagerWidget->windowTitle());

	QPushButton * addIMAccountButton = Object::findChild<QPushButton *>(_imAccountManagerWidget, "addIMAccountButton");
	QMenu * addIMAccountMenu = new QMenu();
	connect(addIMAccountMenu, SIGNAL(triggered(QAction *)), SLOT(addIMAccount(QAction *)));

	addIMAccountMenu->addAction(QIcon(":pics/protocol_msn.png"), PROTOCOL_MSN_NAME);
	addIMAccountMenu->addAction(QIcon(":pics/protocol_aim.png"), PROTOCOL_AIMICQ_NAME);
	addIMAccountMenu->addAction(QIcon(":pics/protocol_yahoo.png"), PROTOCOL_YAHOO_NAME);
	addIMAccountMenu->addAction(QIcon(":pics/protocol_jabber.png"), PROTOCOL_JABBER_NAME);
	addIMAccountButton->setMenu(addIMAccountMenu);

	QPushButton * modifyIMAccountButton = Object::findChild<QPushButton *>(_imAccountManagerWidget, "modifyIMAccountButton");
	connect(modifyIMAccountButton, SIGNAL(clicked()), SLOT(modifyIMAccount()));

	QPushButton * deleteIMAccountButton = Object::findChild<QPushButton *>(_imAccountManagerWidget, "deleteIMAccountButton");
	connect(modifyIMAccountButton, SIGNAL(clicked()), SLOT(deleteIMAccount()));

	QPushButton * closeButton = Object::findChild<QPushButton *>(_imAccountManagerWidget, "closeButton");
	connect(closeButton, SIGNAL(clicked()), _imAccountManagerWindow, SLOT(accept()));

	_treeWidget = Object::findChild<QTreeWidget *>(_imAccountManagerWidget, "treeWidget");
	connect(_treeWidget, SIGNAL(itemDoubleClicked(QTreeWidgetItem *, int)),
			SLOT(itemDoubleClicked(QTreeWidgetItem *, int)));

	loadIMAccounts();
}

void QtIMAccountManager::show() {
	_imAccountManagerWindow->exec();
}

void QtIMAccountManager::loadIMAccounts() {
	_treeWidget->clear();

	IMAccountHandler & imAccountHandler = _userProfile.getIMAccountHandler();

	for (IMAccountHandler::iterator it = imAccountHandler.begin(); it != imAccountHandler.end(); it++) {
		IMAccount * imAccount = (IMAccount *) &(*it);
		QStringList accountStrList;
		accountStrList << QString::fromStdString(imAccount->getLogin());
		EnumIMProtocol::IMProtocol imProtocol = imAccount->getProtocol();

		switch (imProtocol) {
		case EnumIMProtocol::IMProtocolMSN:
			accountStrList << PROTOCOL_MSN_NAME;
			break;
		case EnumIMProtocol::IMProtocolYahoo:
			accountStrList << PROTOCOL_YAHOO_NAME;
			break;
		case EnumIMProtocol::IMProtocolAIMICQ:
			accountStrList << PROTOCOL_AIMICQ_NAME;
			break;
		case EnumIMProtocol::IMProtocolJabber:
			accountStrList << PROTOCOL_JABBER_NAME;
			break;
		case EnumIMProtocol::IMProtocolSIPSIMPLE:
			accountStrList << PROTOCOL_SIPSIMPLE_NAME;
			break;
		default:
			LOG_FATAL("unknown IM protocol=" + String::fromNumber(imProtocol));
		}

		accountStrList << "";

		QtIMAccountItem * item = new QtIMAccountItem(_treeWidget, accountStrList);
		item->setCheckState(2, Qt::Checked);
		item->setIMAccount(imAccount);
	}
}

void QtIMAccountManager::addIMAccount(QAction * action) {
	QString protocolName = action->text();
	LOG_DEBUG(protocolName.toStdString());

	EnumIMProtocol::IMProtocol imProtocol;
	if (protocolName == PROTOCOL_MSN_NAME) {
		imProtocol = EnumIMProtocol::IMProtocolMSN;
	}
	else if (protocolName == PROTOCOL_AIMICQ_NAME) {
		imProtocol = EnumIMProtocol::IMProtocolAIMICQ;
	}
	else if (protocolName == PROTOCOL_YAHOO_NAME) {
		imProtocol = EnumIMProtocol::IMProtocolYahoo;
	}
	else if (protocolName == PROTOCOL_JABBER_NAME) {
		imProtocol = EnumIMProtocol::IMProtocolJabber;
	}
	else {
		LOG_FATAL("unknown IM protocol=" + protocolName.toStdString());
	}
	QtIMAccountSettings * qtIMAccountSettings = new QtIMAccountSettings(_userProfile, imProtocol, _imAccountManagerWindow);
}

void QtIMAccountManager::deleteIMAccount() {
}

void QtIMAccountManager::modifyIMAccount() {
}

void QtIMAccountManager::itemDoubleClicked(QTreeWidgetItem * item, int column) {
	QtIMAccountItem * imAccountItem = dynamic_cast<QtIMAccountItem *>(item);

	IMAccount * imAccount = imAccountItem->getIMAccount();

	QtIMAccountSettings * qtIMAccountSettings = new QtIMAccountSettings(_userProfile, imAccount, _imAccountManagerWindow);
}
