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

#include <model/profile/UserProfile.h>

#include <util/Logger.h>

#include <qtutil/WidgetFactory.h>
#include <qtutil/Object.h>

#include <QtGui>

QtJabberSettings::QtJabberSettings(UserProfile & userProfile, IMAccount * imAccount, QWidget * parent)
	: QtIMAccountPlugin(userProfile, imAccount, parent) {

	init();
}

void QtJabberSettings::init() {
	_IMSettingsWidget = WidgetFactory::create(":/forms/imaccount/JabberSettings.ui", _parentWidget);
	_IMSettingsWidget->setWindowTitle("Jabber " + tr("Settings"));

	//loginLineEdit
	_loginLineEdit = Object::findChild<QLineEdit *>(_IMSettingsWidget, "loginLineEdit");

	//passwordLineEdit
	_passwordLineEdit = Object::findChild<QLineEdit *>(_IMSettingsWidget, "passwordLineEdit");

	//useTLSCheckBox
	_useTLSCheckBox = Object::findChild<QCheckBox *>(_IMSettingsWidget, "useTLSCheckBox");

	//requireTLSCheckBox
	_requireTLSCheckBox = Object::findChild<QCheckBox *>(_IMSettingsWidget, "requireTLSCheckBox");

	//forceOldSSLCheckBox
	_forceOldSSLCheckBox = Object::findChild<QCheckBox *>(_IMSettingsWidget, "forceOldSSLCheckBox");

	//allowPlainTextAuthenticationCheckBox
	_allowPlainTextAuthenticationCheckBox = Object::findChild<QCheckBox *>(_IMSettingsWidget, "allowPlainTextAuthenticationCheckBox");

	//connectServerLineEdit
	_connectServerLineEdit = Object::findChild<QLineEdit *>(_IMSettingsWidget, "connectServerLineEdit");

	//portLineEdit
	_portLineEdit = Object::findChild<QLineEdit *>(_IMSettingsWidget, "portLineEdit");

	if (!_imAccount) {
		return;
	}

	IMAccountParameters & params = _imAccount->getIMAccountParameters();

	_loginLineEdit->setText(QString::fromStdString(_imAccount->getLogin()));
	_passwordLineEdit->setText(QString::fromStdString(_imAccount->getPassword()));
	_useTLSCheckBox->setChecked(params.isJabberTLSUsed());
	_requireTLSCheckBox->setChecked(params.isJabberTLSRequired());
	_forceOldSSLCheckBox->setChecked(params.isJabberOldSSLUsed());
	_allowPlainTextAuthenticationCheckBox->setChecked(params.isJabberAuthPlainInClearUsed());
	_connectServerLineEdit->setText(QString::fromStdString(params.getJabberConnectionServer()));
	_portLineEdit->setText(QString("%1").arg(params.getJabberServerPort()));
}

void QtJabberSettings::save() {
	std::string login = _loginLineEdit->text().toStdString();
	std::string password = _passwordLineEdit->text().toStdString();

	if (!_imAccount) {
		_imAccount = new IMAccount(login, password, EnumIMProtocol::IMProtocolJabber);
	}

	IMAccountParameters & params = _imAccount->getIMAccountParameters();

	_imAccount->setLogin(login);
	_imAccount->setPassword(password);
	params.set(IMAccountParameters::JABBER_USE_TLS_KEY, _useTLSCheckBox->isChecked());
	params.set(IMAccountParameters::JABBER_REQUIRE_TLS_KEY, _requireTLSCheckBox->isChecked());
	params.set(IMAccountParameters::JABBER_USE_OLD_SSL_KEY, _forceOldSSLCheckBox->isChecked());
	params.set(IMAccountParameters::JABBER_AUTH_PLAIN_IN_CLEAR_KEY, _allowPlainTextAuthenticationCheckBox->isChecked());
	params.set(IMAccountParameters::JABBER_CONNECTION_SERVER_KEY, _connectServerLineEdit->text().toStdString());
	params.set(IMAccountParameters::JABBER_PORT_KEY, _portLineEdit->text().toInt());

	_userProfile.addIMAccount(*_imAccount);
	_userProfile.getConnectHandler().connect(*_imAccount);
}
