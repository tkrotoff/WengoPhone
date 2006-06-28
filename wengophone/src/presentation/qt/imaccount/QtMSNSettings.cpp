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

#include <qtutil/MouseEventFilter.h>

#include <util/WebBrowser.h>
#include <util/Logger.h>

#include <QtGui>

static const std::string MSN_LOGIN_DEFAULT_EXTENSION = "hotmail.com";
static const std::string MSN_FORGOT_PASSWORD_LINK_EN = "https://accountservices.passport.net/uiresetpw.srf?lc=1033";
static const std::string MSN_FORGOT_PASSWORD_LINK_FR = "https://accountservices.passport.net/uiresetpw.srf?lc=1036";
static const std::string MSN_CREATE_NEW_ACCOUNT_LINK_EN = "https://accountservices.passport.net/reg.srf?bk=1148037006&cru=https://accountservices.passport.net/uiresetpw.srf%3flc%3d1033&lc=1033&sl=1";
static const std::string MSN_CREATE_NEW_ACCOUNT_LINK_FR = "https://accountservices.passport.net/reg.srf?id=9&cbid=956&sl=1&lc=1036";

QtMSNSettings::QtMSNSettings(UserProfile & userProfile, IMAccount * imAccount, QWidget * parent)
	: QtIMAccountPlugin(userProfile, imAccount, parent) {

	init();
}

void QtMSNSettings::init() {
	_IMSettingsWidget = new QWidget(_parentWidget);

	_ui = new Ui::MSNSettings();
	_ui->setupUi(_IMSettingsWidget);

	MousePressEventFilter * mouseFilterForgotPassword = new MousePressEventFilter(
		this, SLOT(forgotPasswordLabelClicked()), Qt::LeftButton);
	_ui->forgotPasswordLabel->installEventFilter(mouseFilterForgotPassword);

	MousePressEventFilter * mouseFilterCreateAccount = new MousePressEventFilter(
		this, SLOT(createAccountLabelClicked()), Qt::LeftButton);
	_ui->createAccountLabel->installEventFilter(mouseFilterCreateAccount);


	if (!_imAccount) {
		return;
	}

	IMAccountParameters & params = _imAccount->getIMAccountParameters();

	_ui->loginLineEdit->setText(QString::fromStdString(_imAccount->getLogin()));
	_ui->passwordLineEdit->setText(QString::fromStdString(_imAccount->getPassword()));
}

void QtMSNSettings::save() {
	String login = _ui->loginLineEdit->text().toStdString();
	String password = _ui->passwordLineEdit->text().toStdString();
	static const String AT = "@";

	//Test if login ends with @hotmail.com
	if (!login.contains(AT)) {
		login = login + AT + MSN_LOGIN_DEFAULT_EXTENSION;
	}

	if (!_imAccount) {
		_imAccount = new IMAccount(login, password, EnumIMProtocol::IMProtocolMSN);
	}

	IMAccountParameters & params = _imAccount->getIMAccountParameters();

	_imAccount->setLogin(login);
	_imAccount->setPassword(password);
	//FIXME to remove, must be done inside model
	params.set(IMAccountParameters::MSN_USE_HTTP_KEY, true);

	_userProfile.addIMAccount(*_imAccount);
	_userProfile.getConnectHandler().connect(*_imAccount);
}

void QtMSNSettings::forgotPasswordLabelClicked() {
	Config & config = ConfigManager::getInstance().getCurrentConfig();
	std::string lang = config.getLanguage();

	if (lang == "fr") {
		WebBrowser::openUrl(MSN_FORGOT_PASSWORD_LINK_FR);
	} else {
		WebBrowser::openUrl(MSN_FORGOT_PASSWORD_LINK_EN);
	}
}

void QtMSNSettings::createAccountLabelClicked() {
	Config & config = ConfigManager::getInstance().getCurrentConfig();
	std::string lang = config.getLanguage();

	if (lang == "fr") {
		WebBrowser::openUrl(MSN_CREATE_NEW_ACCOUNT_LINK_FR);
	} else {
		WebBrowser::openUrl(MSN_CREATE_NEW_ACCOUNT_LINK_EN);
	}
}
