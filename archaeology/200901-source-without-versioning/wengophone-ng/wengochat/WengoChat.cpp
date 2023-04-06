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

#include "WengoChat.h"

#include "ChatWidget.h"
#include "IMContactDialog.h"
#include "LoggerWidget.h"
#include "ManageAccount.h"
#include "Config.h"
#include "Widget.h"

#include "ui_MainWindow.h"

#include <coipmanager_threaded/TCoIpManager.h>
#include <coipmanager_threaded/chatsessionmanager/TChatSession.h>
#include <coipmanager_threaded/chatsessionmanager/TChatSessionManager.h>
#include <coipmanager_threaded/connectmanager/TConnectManager.h>
#include <coipmanager_threaded/datamanager/TUserProfileManager.h>
#include <coipmanager_base/storage/UserProfileFileStorage.h>

#include <settings/SettingsFileStorage.h>

#include <util/SafeConnect.h>

#include <util/Logger.h>
#include <util/Path.h>
#include <util/SafeDelete.h>
#include <cutil/global.h>

#include <QtGui/QtGui>

WengoChat::WengoChat() {
	_ui = new Ui::MainWindow();
	_ui->setupUi(this);

	_manageAccount = NULL;

	initializeLogTab();
	initializeCoIpManager();

	SAFE_CONNECT(_ui->actionStart_a_chat, SIGNAL(triggered()), SLOT(startChat()));
	SAFE_CONNECT(_ui->actionManage_account, SIGNAL(triggered()), SLOT(manageAccount()));
}

WengoChat::~WengoChat() {
	Config::getInstance().set(Config::PROFILE_LAST_USED_ID_KEY,
		_tCoIpManager->getTUserProfileManager().getCopyOfUserProfile().getUUID());

	SettingsFileStorage::save(_configFilename, Config::getInstance());

	OWSAFE_DELETE(_manageAccount);
	OWSAFE_DELETE(_tCoIpManager);
	OWSAFE_DELETE(_ui);
}

void WengoChat::showStatusBarMessage(const std::string & message) {
	statusBar()->showMessage(QString::fromStdString(message));
}

void WengoChat::initializeCoIpManager() {
	// Initializing CoIpManager
	Config & config = Config::getInstance();
	std::string endOfPath = "WengoPhone-coip/";
#if defined(OS_LINUX)
	endOfPath = ".wengophone-coip/";
#endif
	config.set(Config::CONFIG_PATH_KEY, Path::getConfigurationDirPath() + endOfPath);

	_configFilename = config.getConfigPath() + "config.xml";
	SettingsFileStorage::load(_configFilename, config);

	_tCoIpManager = new TCoIpManager(config);

	SAFE_CONNECT(_tCoIpManager, SIGNAL(initialized()), SLOT(tCoIpManagerInitialized()));

	_tCoIpManager->start();
	////
}

void WengoChat::tCoIpManagerInitialized() {
	// Registering to events
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

	SAFE_CONNECT(&_tCoIpManager->getTChatSessionManager(),
		SIGNAL(newChatSessionCreatedSignal(TChatSession *)),
		SLOT(newChatSessionCreatedSlot(TChatSession *)));
	////


	// Loading last used profile
	Config & config = Config::getInstance();
	UserProfile userProfile;
	std::string lastProfile = config.getProfileLastUsedId();
	if (!lastProfile.empty()) {
		UserProfileFileStorage fileStorage(config.getConfigPath());
		if (!fileStorage.load(lastProfile, userProfile) == UserProfileFileStorage::UserProfileStorageErrorNoError) {
			LOG_ERROR("cannot load profile");
		}
	}

	_tCoIpManager->getTUserProfileManager().setUserProfile(userProfile);
	////
}

void WengoChat::userProfileSetSlot(UserProfile userProfile) {
	_tCoIpManager->getTConnectManager().connectAllAccounts();
}

void WengoChat::initializeLogTab() {
	QWidget * logTab = _ui->tabWidget->widget(0);
	Widget::createLayout(logTab);
	logTab->layout()->setMargin(10);
	logTab->layout()->addWidget(new LoggerWidget(_ui->tabWidget, this));
}

void WengoChat::manageAccount() {
	if (!_manageAccount) {
		_manageAccount = new ManageAccount(*_tCoIpManager, this);
	}

	_manageAccount->show();
}

void WengoChat::startChat() {
	IMContactDialog dialog(this);
	if (dialog.exec()) {
		IMContact imContact(dialog.accountType(), dialog.contactId().toStdString());
		Contact contact;
		contact.addIMContact(imContact);
		TChatSession * session = _tCoIpManager->getTChatSessionManager().createTChatSession();
		EnumSessionError::SessionError error = session->addContact(contact);
		if (error != EnumSessionError::SessionErrorNoError) {
			QMessageBox::warning(this, "Session creation error",
				"This contact could not be added to the Session",
				QMessageBox::Ok, QMessageBox::NoButton);
			OWSAFE_DELETE(session);
			return;
		}
		addSession(session);
	}
}

void WengoChat::newChatSessionCreatedSlot(TChatSession * tChatSession) {
	addSession(tChatSession);
}

void WengoChat::accountConnectionStateSlot(std::string accountId, EnumConnectionState::ConnectionState state) {
	showStatusBarMessage("** Account status changed: " + EnumConnectionState::toString(state));
}

void WengoChat::accountConnectionErrorSlot(std::string accountId,
	EnumConnectionError::ConnectionError errorCode) {

	showStatusBarMessage(EnumConnectionError::toString(errorCode));
}

void WengoChat::accountConnectionProgressSlot(std::string accountId,
	unsigned currentStep, unsigned totalSteps, std::string infoMessage) {

	showStatusBarMessage(infoMessage);
}

void WengoChat::addSession(TChatSession * session) {
	ChatWidget * chatTab = new ChatWidget(this, session);
	int i = _ui->tabWidget->addTab(chatTab, "name" /*session->getName()*/);
	_ui->tabWidget->setCurrentIndex(i);
}
