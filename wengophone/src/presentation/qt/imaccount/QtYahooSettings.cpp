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

#include "QtYahooSettings.h"

#include "ui_YahooSettings.h"

#include <model/profile/UserProfile.h>
#include <model/config/ConfigManager.h>
#include <model/config/Config.h>

#include <util/WebBrowser.h>
#include <util/Logger.h>

#include <QtGui/QtGui>

static const std::string YAHOO_FORGOT_PASSWORD_LINK_EN = "http://edit.yahoo.com/config/eval_forgot_pw?.src=pg&.done=http://messenger.yahoo.com/&.redir_from=MESSENGER";
static const std::string YAHOO_FORGOT_PASSWORD_LINK_FR = "http://edit.yahoo.com/config/eval_forgot_pw?.src=pg&.done=http://fr.messenger.yahoo.com&.intl=fr&.redir_from=MESSENGER";
static const std::string YAHOO_CREATE_NEW_ACCOUNT_LINK_EN = "http://login.yahoo.com/config/login?.done=http://messenger.yahoo.com&.src=pg";
static const std::string YAHOO_CREATE_NEW_ACCOUNT_LINK_FR = "https://edit.yahoo.com/config/eval_register?.intl=fr&.done=http://fr.messenger.yahoo.com&.src=pg";

QtYahooSettings::QtYahooSettings(UserProfile & userProfile, IMAccount * imAccount, QWidget * parent)
	: QtIMAccountPlugin(userProfile, imAccount, parent) {

	init();
}

void QtYahooSettings::init() {
	_IMSettingsWidget = new QWidget(_parentWidget);

	_ui = new Ui::YahooSettings();
	_ui->setupUi(_IMSettingsWidget);

	connect(_ui->forgotPasswordButton, SIGNAL(clicked()), SLOT(forgotPasswordButtonClicked()));

	connect(_ui->createAccountButton, SIGNAL(clicked()), SLOT(createAccountButtonClicked()));

	if (!_imAccount) {
		return;
	}

	IMAccountParameters & param = _imAccount->getIMAccountParameters();

	_ui->loginLineEdit->setText(QString::fromStdString(_imAccount->getLogin()));
	_ui->passwordLineEdit->setText(QString::fromStdString(_imAccount->getPassword()));
}

void QtYahooSettings::save() {
	String login = _ui->loginLineEdit->text().toStdString();
	String password = _ui->passwordLineEdit->text().toStdString();
	static const String AT = "@";

	//Test if login ends with @
	if (login.contains(AT)) {
		std::string::size_type posAT = login.find(AT);
		if (posAT != std::string::npos) {
			login = login.substr(0, posAT);
		}
	}

	if (!_imAccount) {
		_imAccount = new IMAccount(login, password, EnumIMProtocol::IMProtocolYahoo);
	}

	IMAccountParameters & params = _imAccount->getIMAccountParameters();

	_imAccount->setLogin(login);
	_imAccount->setPassword(password);
	params.set(IMAccountParameters::YAHOO_IS_JAPAN_KEY, _ui->useJapanServerCheckBox->isChecked());
	//FIXME to remove, must be done inside model
	params.set(IMAccountParameters::YAHOO_PORT_KEY, 23);

	_userProfile.addIMAccount(*_imAccount);
	_userProfile.getConnectHandler().connect(*_imAccount);
}

void QtYahooSettings::forgotPasswordButtonClicked() {
	Config & config = ConfigManager::getInstance().getCurrentConfig();
	std::string lang = config.getLanguage();

	if (lang == "fr") {
		WebBrowser::openUrl(YAHOO_FORGOT_PASSWORD_LINK_FR);
	} else {
		WebBrowser::openUrl(YAHOO_FORGOT_PASSWORD_LINK_EN);
	}
}

void QtYahooSettings::createAccountButtonClicked() {
	Config & config = ConfigManager::getInstance().getCurrentConfig();
	std::string lang = config.getLanguage();

	if (lang == "fr") {
		WebBrowser::openUrl(YAHOO_CREATE_NEW_ACCOUNT_LINK_FR);
	} else {
		WebBrowser::openUrl(YAHOO_CREATE_NEW_ACCOUNT_LINK_EN);
	}
}
