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

#include "CallWidget.h"

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
	SAFE_CONNECT(_ui->callButton, SIGNAL(clicked()), SLOT(callButtonClicked()));
	SAFE_CONNECT(_ui->realmLineEdit, SIGNAL(editingFinished()), SLOT(realmLineEditingFinished()));

	statusBar()->showMessage("Disconnected");

	// Adding supported protocols in combo box
	_ui->protocolComboBox->addItem(QString::fromStdString(EnumAccountType::toString(EnumAccountType::AccountTypeWengo)));
	_ui->protocolComboBox->addItem(QString::fromStdString(EnumAccountType::toString(EnumAccountType::AccountTypeSIP)));
	SAFE_CONNECT_RECEIVER(_ui->protocolComboBox, SIGNAL(activated(int)),
		_ui->stackedWidget, SLOT(setCurrentIndex(int)));
	////

	// Initializing TCoIpManager
	CoIpManagerConfig config;
	_tCoIpManager = new TCoIpManager(config);
	SAFE_CONNECT(_tCoIpManager, SIGNAL(initialized()), SLOT(tCoIpManagerInitialized()));
	_tCoIpManager->start();
	////
}

LoginWidget::~LoginWidget() {
	while (!_callWidgetStack.empty()) {
		delete _callWidgetStack.top();
		_callWidgetStack.pop();
	}

	OWSAFE_DELETE(_tCoIpManager);
	OWSAFE_DELETE(_ui);
}

void LoginWidget::tCoIpManagerInitialized() {
	SAFE_CONNECT(&_tCoIpManager->getTUserProfileManager(),
		SIGNAL(userProfileSetSignal(UserProfile)),
		SLOT(userProfileSetSlot(UserProfile)));

	SAFE_CONNECT(&_tCoIpManager->getTConnectManager(),
		SIGNAL(accountConnectionStateSignal(std::string, EnumConnectionState::ConnectionState)),
		SLOT(accountConnectionStateSlot(std::string, EnumConnectionState::ConnectionState)));
	SAFE_CONNECT(&_tCoIpManager->getTConnectManager(),
		SIGNAL(accountConnectionErrorSignal(std::string, EnumConnectionError::ConnectionError)),
		SLOT(accountConnectionErrorSlot(std::string, EnumConnectionError::ConnectionError)));
	SAFE_CONNECT(&_tCoIpManager->getTConnectManager(),
		SIGNAL(accountConnectionProgressSignal(std::string, unsigned, unsigned, std::string)),
		SLOT(accountConnectionProgressSlot(std::string, unsigned, unsigned, std::string)));

	UserProfile userProfile;
	_tCoIpManager->getTUserProfileManager().setUserProfile(userProfile);
}

void LoginWidget::loginButtonClicked() {
	if (_accountId.empty()) {
		switch(_ui->stackedWidget->currentIndex()) {
		case 0:
			_accountId = addWengoAccount();
			break;
		case 1:
			_accountId = addSipAccount();
			break;
		default:
			LOG_FATAL("Unknown index");
		}
	}

	_tCoIpManager->getTConnectManager().connect(_accountId);

	_ui->loginButton->setEnabled(false);
	_ui->loginButton->setText(tr("Connecting..."));
}

std::string LoginWidget::addWengoAccount() {
	Account account(_ui->wengoLoginLineEdit->text().toStdString(),
		_ui->wengoPasswordLineEdit->text().toStdString(),
		EnumAccountType::AccountTypeWengo);

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

void LoginWidget::callButtonClicked() {
	CallWidget *callWidget = new CallWidget(*_tCoIpManager);
	_callWidgetStack.push(callWidget);
	callWidget->show();
}

void LoginWidget::userProfileSetSlot(UserProfile userProfile) {
	_ui->loginButton->setEnabled(true);
}

void LoginWidget::accountConnectionStateSlot(std::string accountId,
	EnumConnectionState::ConnectionState state) {

	statusBar()->showMessage(QString::fromStdString(EnumConnectionState::toString(state)));

	if (state == EnumConnectionState::ConnectionStateConnected) {
		_ui->callButton->setEnabled(true);
		_ui->loginButton->setEnabled(false);
		_ui->loginButton->setText(tr("Connected"));
	} else {
		_ui->callButton->setEnabled(false);
		_ui->loginButton->setEnabled(true);
		_ui->loginButton->setText(tr("Login"));
	}
}

void LoginWidget::accountConnectionErrorSlot(std::string accountId,
	EnumConnectionError::ConnectionError errorCode) {

	statusBar()->showMessage(QString::fromStdString(EnumConnectionError::toString(errorCode)));
}

void LoginWidget::accountConnectionProgressSlot(std::string accountId,
	unsigned currentStep, unsigned totalSteps, std::string infoMessage) {

	statusBar()->showMessage(QString::fromStdString(infoMessage));
}
