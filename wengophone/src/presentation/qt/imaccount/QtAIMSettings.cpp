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

#include "QtAIMSettings.h"

#include "ui_AIMSettings.h"

#include <model/profile/UserProfile.h>
#include <model/config/ConfigManager.h>
#include <model/config/Config.h>

#include <qtutil/MouseEventFilter.h>

#include <util/WebBrowser.h>
#include <util/Logger.h>

#include <QtGui>

static const std::string AIM_FORGOT_PASSWORD_LINK_EN = "http://www.aol.co.uk/aim/faqs/AIM_FAQ_Passwords.htm";
static const std::string AIM_FORGOT_PASSWORD_LINK_FR = "http://www.aim.aol.fr/oubli.htm";
static const std::string AIM_CREATE_NEW_ACCOUNT_LINK_EN = "https://my.screenname.aol.com/_cqr/login/login.psp?mcState=initialized&seamless=n&createSn=1&sitedomain=www.aim.com&siteState=http%3A//www.aim.com/get_aim/congratsd2.adp&triedAimAuth=y&promo=380464";
static const std::string AIM_CREATE_NEW_ACCOUNT_LINK_FR = "https://reg.my.screenname.aol.com/_cqr/registration/initRegistration.psp?mcState=initialized&seamless=n&createSn=1&siteId=aimregPROD-fr&siteState=http%3A%2F%2Faim%2Eaol%2Efr%2Fupgrade%2Ejsp&mcAuth=%2FBcAG0RtoewAAPdvAHyUj0RtoigIxtPj0NfjWwgAAA%3D%3D";


QtAIMSettings::QtAIMSettings(UserProfile & userProfile, IMAccount * imAccount, QWidget * parent)
	: QtIMAccountPlugin(userProfile, imAccount, parent) {

	init();
}

void QtAIMSettings::init() {
	_IMSettingsWidget = new QWidget(_parentWidget);

	_ui = new Ui::AIMSettings();
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

void QtAIMSettings::save() {
	std::string login = _ui->loginLineEdit->text().toStdString();
	std::string password = _ui->passwordLineEdit->text().toStdString();

	if (!_imAccount) {
		_imAccount = new IMAccount(login, password, EnumIMProtocol::IMProtocolAIMICQ);
	}

	IMAccountParameters & params = _imAccount->getIMAccountParameters();

	_imAccount->setLogin(login);
	_imAccount->setPassword(password);
	//FIXME to remove, must be done inside model
	params.set(IMAccountParameters::OSCAR_PORT_KEY, 443);

	_userProfile.addIMAccount(*_imAccount);
	_userProfile.getConnectHandler().connect(*_imAccount);
}

void QtAIMSettings::forgotPasswordLabelClicked() {
	Config & config = ConfigManager::getInstance().getCurrentConfig();
	std::string lang = config.getLanguage();

	if (lang == "fr") {
		WebBrowser::openUrl(AIM_FORGOT_PASSWORD_LINK_FR);
	} else {
		WebBrowser::openUrl(AIM_FORGOT_PASSWORD_LINK_EN);
	}
}

void QtAIMSettings::createAccountLabelClicked() {
	Config & config = ConfigManager::getInstance().getCurrentConfig();
	std::string lang = config.getLanguage();

	if (lang == "fr") {
		WebBrowser::openUrl(AIM_CREATE_NEW_ACCOUNT_LINK_FR);
	} else {
		WebBrowser::openUrl(AIM_CREATE_NEW_ACCOUNT_LINK_EN);
	}
}
