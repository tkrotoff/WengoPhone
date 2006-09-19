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

#include "QtMSNSettings.h"

#include "ui_MSNSettings.h"

#include <model/profile/UserProfile.h>
#include <model/config/ConfigManager.h>
#include <model/config/Config.h>

#include <util/WebBrowser.h>
#include <util/Logger.h>
#include <util/SafeDelete.h>

#include <qtutil/SafeConnect.h>

#include <QtGui/QtGui>

static const std::string MSN_LOGIN_DEFAULT_EXTENSION = "hotmail.com";
static const std::string MSN_FORGOT_PASSWORD_LINK_EN = "https://accountservices.passport.net/uiresetpw.srf?lc=1033";
static const std::string MSN_FORGOT_PASSWORD_LINK_FR = "https://accountservices.passport.net/uiresetpw.srf?lc=1036";
static const std::string MSN_CREATE_NEW_ACCOUNT_LINK_EN = "https://accountservices.passport.net/reg.srf?bk=1148037006&cru=https://accountservices.passport.net/uiresetpw.srf%3flc%3d1033&lc=1033&sl=1";
static const std::string MSN_CREATE_NEW_ACCOUNT_LINK_FR = "https://accountservices.passport.net/reg.srf?id=9&cbid=956&sl=1&lc=1036";

QtMSNSettings::QtMSNSettings(UserProfile & userProfile, IMAccount * imAccount, QWidget * parent)
	: QtIMAccountPlugin(userProfile, imAccount, parent) {

	init();
}

QtMSNSettings::~QtMSNSettings() {
	OWSAFE_DELETE(_ui);
}

void QtMSNSettings::init() {
	_IMSettingsWidget = new QWidget(_parentWidget);

	_ui = new Ui::MSNSettings();
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

void QtMSNSettings::save(UserProfile & userProfile, IMAccount * imAccount,
	const std::string & login, const std::string & password) {

	static const String AT = "@";

	String tmpLogin = login;

	//Test if login ends with @hotmail.com
	if (!tmpLogin.contains(AT)) {
		tmpLogin = tmpLogin + AT + MSN_LOGIN_DEFAULT_EXTENSION;
	}

	if (!imAccount) {
		imAccount = new IMAccount(tmpLogin, password, EnumIMProtocol::IMProtocolMSN);
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
	//FIXME to remove, must be done inside model
	params.set(IMAccountParameters::MSN_USE_HTTP_KEY, true);

	userProfile.addIMAccount(*imAccount);
	userProfile.getConnectHandler().connect(*imAccount);
}

void QtMSNSettings::save() {
	if (_ui->loginLineEdit->text().isEmpty()) {
		return;
	}

	save(_userProfile, _imAccount, _ui->loginLineEdit->text().toStdString(), _ui->passwordLineEdit->text().toStdString());
}

void QtMSNSettings::forgotPasswordButtonClicked() {
	Config & config = ConfigManager::getInstance().getCurrentConfig();
	std::string lang = config.getLanguage();

	if (lang == "fr") {
		WebBrowser::openUrl(MSN_FORGOT_PASSWORD_LINK_FR);
	} else {
		WebBrowser::openUrl(MSN_FORGOT_PASSWORD_LINK_EN);
	}
}

void QtMSNSettings::createAccountButtonClicked() {
	Config & config = ConfigManager::getInstance().getCurrentConfig();
	std::string lang = config.getLanguage();

	if (lang == "fr") {
		WebBrowser::openUrl(MSN_CREATE_NEW_ACCOUNT_LINK_FR);
	} else {
		WebBrowser::openUrl(MSN_CREATE_NEW_ACCOUNT_LINK_EN);
	}
}
