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

#include "QtJabberSettings.h"

#include "ui_JabberSettings.h"

#include <model/profile/UserProfile.h>

#include <util/Logger.h>
#include <util/SafeDelete.h>

#include <qtutil/SafeConnect.h>

#include <QtGui/QtGui>

QtJabberSettings::QtJabberSettings(UserProfile & userProfile, IMAccount * imAccount, QDialog * parent)
	: QtIMAccountPlugin(userProfile, imAccount, parent) {

	init();
}

QtJabberSettings::~QtJabberSettings() {
	OWSAFE_DELETE(_ui);
}

void QtJabberSettings::init() {
	_IMSettingsWidget = new QWidget(_parentDialog);

	_ui = new Ui::JabberSettings();
	_ui->setupUi(_IMSettingsWidget);

	if (!_imAccount) {
		return;
	}

	IMAccountParameters & params = _imAccount->getIMAccountParameters();

	_ui->loginLineEdit->setText(QString::fromStdString(_imAccount->getLogin()));
	_ui->passwordLineEdit->setText(QString::fromStdString(_imAccount->getPassword()));
	_ui->useTLSCheckBox->setChecked(params.isJabberTLSUsed());
	_ui->requireTLSCheckBox->setChecked(params.isJabberTLSRequired());
	_ui->forceOldSSLCheckBox->setChecked(params.isJabberOldSSLUsed());
	_ui->allowPlainTextAuthenticationCheckBox->setChecked(params.isJabberAuthPlainInClearUsed());
	_ui->connectServerLineEdit->setText(QString::fromStdString(params.getJabberConnectionServer()));
	_ui->portLineEdit->setText(QString("%1").arg(params.getJabberServerPort()));
}

bool QtJabberSettings::isValid() const {
	return !_ui->loginLineEdit->text().isEmpty();
}

void QtJabberSettings::save() {
	std::string login = _ui->loginLineEdit->text().toStdString();
	std::string password = _ui->passwordLineEdit->text().toStdString();
	bool newAccount = false;
	
	if (!_imAccount) {
		// new account because no pre-existing account is found
		_imAccount = new IMAccount(login, password, EnumIMProtocol::IMProtocolJabber);
		newAccount = true;
	} else if (_imAccount->getLogin() != login) {
		// new account because the user changed the login
		_userProfile.removeIMAccount(*_imAccount);
		_imAccount = new IMAccount(login, password, EnumIMProtocol::IMProtocolJabber);
		newAccount = true;
	} else {
		// pre-existing account, but password may have been modified
		_imAccount->setPassword(password);
	}


	IMAccountParameters & params = _imAccount->getIMAccountParameters();

	params.set(IMAccountParameters::JABBER_USE_TLS_KEY, _ui->useTLSCheckBox->isChecked());
	params.set(IMAccountParameters::JABBER_REQUIRE_TLS_KEY, _ui->requireTLSCheckBox->isChecked());
	params.set(IMAccountParameters::JABBER_USE_OLD_SSL_KEY, _ui->forceOldSSLCheckBox->isChecked());
	params.set(IMAccountParameters::JABBER_AUTH_PLAIN_IN_CLEAR_KEY, _ui->allowPlainTextAuthenticationCheckBox->isChecked());
	params.set(IMAccountParameters::JABBER_CONNECTION_SERVER_KEY, _ui->connectServerLineEdit->text().toStdString());
	params.set(IMAccountParameters::JABBER_PORT_KEY, _ui->portLineEdit->text().toInt());

	if (newAccount) {
		_userProfile.addIMAccount(*_imAccount);
	}

	_userProfile.getConnectHandler().connect(*_imAccount);
}
