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

#include "LoginWidget.h"

#include "ui_LoginWidget.h"

#include "LogWidget.h"

#include <coipmanager_threaded/TCoIpManager.h>
#include <coipmanager_threaded/connectmanager/TConnectManager.h>
#include <coipmanager_threaded/datamanager/TUserProfileManager.h>

#include <coipmanager/CoIpManagerConfig.h>

#include <coipmanager_base/EnumAccountType.h>
#include <coipmanager_base/account/SipAccount.h>

#include <util/Logger.h>
#include <util/SafeConnect.h>
#include <util/SafeDelete.h>

#include <QtGui/QtGui>

LoginWidget::LoginWidget() 
	: QMainWindow() {

	_ui = new Ui::LoginWidget();
	QWidget *widget = new QWidget();
	_ui->setupUi(widget);
	setCentralWidget(widget);

	SAFE_CONNECT(_ui->loginButton, SIGNAL(clicked()), SLOT(loginButtonClicked()));
	SAFE_CONNECT(_ui->realmLineEdit, SIGNAL(editingFinished()), SLOT(realmLineEditingFinished()));

	statusBar()->showMessage("Disconnected");

	// Adding supported protocols in combo box
	_ui->protocolComboBox->addItem(QString::fromStdString(EnumAccountType::toString(EnumAccountType::AccountTypeAIM)));
	_ui->protocolComboBox->addItem(QString::fromStdString(EnumAccountType::toString(EnumAccountType::AccountTypeGTalk)));
	_ui->protocolComboBox->addItem(QString::fromStdString(EnumAccountType::toString(EnumAccountType::AccountTypeIAX)));
	_ui->protocolComboBox->addItem(QString::fromStdString(EnumAccountType::toString(EnumAccountType::AccountTypeICQ)));
	_ui->protocolComboBox->addItem(QString::fromStdString(EnumAccountType::toString(EnumAccountType::AccountTypeJabber)));
	_ui->protocolComboBox->addItem(QString::fromStdString(EnumAccountType::toString(EnumAccountType::AccountTypeMSN)));
	_ui->protocolComboBox->addItem(QString::fromStdString(EnumAccountType::toString(EnumAccountType::AccountTypeSIP)));
	_ui->protocolComboBox->addItem(QString::fromStdString(EnumAccountType::toString(EnumAccountType::AccountTypeWengo)));
	_ui->protocolComboBox->addItem(QString::fromStdString(EnumAccountType::toString(EnumAccountType::AccountTypeYahoo)));

	SAFE_CONNECT(_ui->protocolComboBox, SIGNAL(activated(int)), SLOT(setAccountType(int)));
	////

	// Initializing TCoIpManager
	CoIpManagerConfig config;
	_tCoIpManager = new TCoIpManager(config);
	SAFE_CONNECT(_tCoIpManager, SIGNAL(initialized()), SLOT(tCoIpManagerInitialized()));
	_tCoIpManager->start();
	////
}

LoginWidget::~LoginWidget() {
	while (!_childWidgetStack.empty()) {
		delete _childWidgetStack.top();
		_childWidgetStack.pop();
	}

	OWSAFE_DELETE(_tCoIpManager);
	OWSAFE_DELETE(_ui);
}

void LoginWidget::tCoIpManagerInitialized() {
	SAFE_CONNECT(&_tCoIpManager->getTUserProfileManager(),
		SIGNAL(userProfileSetSignal(UserProfile)),
		SLOT(userProfileSetSlot(UserProfile)));

	UserProfile userProfile;
	_tCoIpManager->getTUserProfileManager().setUserProfile(userProfile);
}

void LoginWidget::loginButtonClicked() {
	std::string accountId;
	switch(_ui->stackedWidget->currentIndex()) {
	case 0:
		accountId = addAccount();
		break;
	case 1:
		accountId = addSipAccount();
		break;
	default:
		LOG_FATAL("Unknown index");
	}

	LogWidget *logWidget = new LogWidget(*_tCoIpManager, accountId);
	logWidget->show();
}

void LoginWidget::setAccountType(int accountType) {
	if (accountType == (EnumAccountType::AccountTypeSIP - 1)) {
		_ui->stackedWidget->setCurrentIndex(1);
	} else {
		_ui->stackedWidget->setCurrentIndex(0);
	}
}

std::string LoginWidget::addAccount() {
	int accountTypeId = _ui->protocolComboBox->currentIndex() + 1;
	EnumAccountType::AccountType accountType = static_cast<EnumAccountType::AccountType>(accountTypeId);

	Account account(_ui->accountLoginLineEdit->text().toStdString(),
		_ui->accountPasswordLineEdit->text().toStdString(),
		accountType);

	_tCoIpManager->getTUserProfileManager().getTAccountManager().add(account);

	return account.getUUID();
}

std::string LoginWidget::addSipAccount() {
	Account account(_ui->usernameLineEdit->text().toStdString(),
		_ui->passwordLineEdit->text().toStdString(),
		EnumAccountType::AccountTypeSIP);
	SipAccount *sipAccount = static_cast<SipAccount *>(account.getPrivateAccount());
	sipAccount->setRealm(_ui->realmLineEdit->text().toStdString());
	sipAccount->setDisplayName(_ui->displaynameLineEdit->text().toStdString());
	sipAccount->setRegisterServerHostname(_ui->registerServerLineEdit->text().toStdString());
	sipAccount->setSIPProxyServerHostname(_ui->sipProxyLineEdit->text().toStdString());
	sipAccount->setSIPProxyServerPort(_ui->sipProxyPortLineEdit->text().toInt());

	_tCoIpManager->getTUserProfileManager().getTAccountManager().add(account);

	return account.getUUID();
}

void LoginWidget::realmLineEditingFinished() {
	if (_ui->registerServerLineEdit->text().isEmpty()) {
		_ui->registerServerLineEdit->setText(_ui->realmLineEdit->text());
	}

	if (_ui->sipProxyLineEdit->text().isEmpty()) {
		_ui->sipProxyLineEdit->setText(_ui->realmLineEdit->text());
	}
}

void LoginWidget::userProfileSetSlot(UserProfile userProfile) {
	_ui->loginButton->setEnabled(true);
}
