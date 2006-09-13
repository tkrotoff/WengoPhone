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

#include "QtGoogleTalkSettings.h"

#include "ui_GoogleTalkSettings.h"

#include <model/profile/UserProfile.h>
#include <model/config/ConfigManager.h>
#include <model/config/Config.h>

#include <util/WebBrowser.h>
#include <util/Logger.h>
#include <util/SafeDelete.h>

#include <qtutil/SafeConnect.h>

#include <QtGui/QtGui>

static const std::string GOOGLETALK_SERVER = "talk.google.com";
static const std::string GOOGLETALK_LOGIN_EXTENSION = "gmail.com";
static const std::string GOOGLETALK_FORGOT_PASSWORD_LINK_EN = "https://www.google.com/accounts/ForgotPasswd?hl=en&continue=http%3A%2F%2Fmail.google.com";
static const std::string GOOGLETALK_FORGOT_PASSWORD_LINK_FR = "https://www.google.com/accounts/ForgotPasswd?hl=fr&continue=http%3A%2F%2Fmail.google.com";
static const std::string GOOGLETALK_CREATE_NEW_ACCOUNT_LINK_EN = "http://www.google.com/talk/index.html";
static const std::string GOOGLETALK_CREATE_NEW_ACCOUNT_LINK_FR = "http://www.google.com/talk/intl/fr/";
static const int GOOGLETALK_PORT = 80;

QtGoogleTalkSettings::QtGoogleTalkSettings(UserProfile & userProfile, IMAccount * imAccount, QWidget * parent)
	: QtIMAccountPlugin(userProfile, imAccount, parent) {

	init();
}

QtGoogleTalkSettings::~QtGoogleTalkSettings() {
	OWSAFE_DELETE(_ui);
}

void QtGoogleTalkSettings::init() {
	_IMSettingsWidget = new QWidget(_parentWidget);

	_ui = new Ui::GoogleTalkSettings();
	_ui->setupUi(_IMSettingsWidget);

	SAFE_CONNECT(_ui->forgotPasswordButton, SIGNAL(clicked()), SLOT(forgotPasswordButtonClicked()));

	SAFE_CONNECT(_ui->createAccountButton, SIGNAL(clicked()), SLOT(createAccountButtonClicked()));

	if (!_imAccount) {
		return;
	}

	IMAccountParameters & params = _imAccount->getIMAccountParameters();

	_ui->loginLineEdit->setText(QString::fromStdString(_imAccount->getLogin()));
	_ui->passwordLineEdit->setText(QString::fromStdString(_imAccount->getPassword()));
}

void QtGoogleTalkSettings::save(UserProfile & userProfile, IMAccount * imAccount,
		const std::string & login, const std::string & password) {

	static const String AT = "@";

	String tmpLogin = login;
	//Test if login ends with @gmail.com
	if (!tmpLogin.contains(AT)) {
		tmpLogin = tmpLogin + AT + GOOGLETALK_LOGIN_EXTENSION;
	}

	if (!imAccount) {
		imAccount = new IMAccount(tmpLogin, password, EnumIMProtocol::IMProtocolJabber);
	} else {
		//Check if the same account is not present already
		if (imAccount->getLogin() == tmpLogin &&
			imAccount->getPassword() == password) {
			return;
		}
	}

	IMAccountParameters & params = imAccount->getIMAccountParameters();

	imAccount->setLogin(tmpLogin);
	imAccount->setPassword(password);
	params.set(IMAccountParameters::JABBER_USE_TLS_KEY, true);
	params.set(IMAccountParameters::JABBER_CONNECTION_SERVER_KEY, GOOGLETALK_SERVER);
	params.set(IMAccountParameters::JABBER_PORT_KEY, GOOGLETALK_PORT);

	userProfile.addIMAccount(*imAccount);
	userProfile.getConnectHandler().connect(*imAccount);
}

void QtGoogleTalkSettings::save() {
	save(_userProfile, _imAccount, _ui->loginLineEdit->text().toStdString(), _ui->passwordLineEdit->text().toStdString());
}

void QtGoogleTalkSettings::forgotPasswordButtonClicked() {
	Config & config = ConfigManager::getInstance().getCurrentConfig();
	std::string lang = config.getLanguage();

	if (lang == "fr") {
		WebBrowser::openUrl(GOOGLETALK_FORGOT_PASSWORD_LINK_FR);
	} else {
		WebBrowser::openUrl(GOOGLETALK_FORGOT_PASSWORD_LINK_EN);
	}
}

void QtGoogleTalkSettings::createAccountButtonClicked() {
	Config & config = ConfigManager::getInstance().getCurrentConfig();
	std::string lang = config.getLanguage();

	if (lang == "fr") {
		WebBrowser::openUrl(GOOGLETALK_CREATE_NEW_ACCOUNT_LINK_FR);
	} else {
		WebBrowser::openUrl(GOOGLETALK_CREATE_NEW_ACCOUNT_LINK_EN);
	}
}
