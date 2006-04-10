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

#include <model/profile/UserProfile.h>

#include <util/Logger.h>

#include <qtutil/WidgetFactory.h>
#include <qtutil/Object.h>

#include <QtGui>

QtMSNSettings::QtMSNSettings(UserProfile & userProfile, IMAccount * imAccount, QWidget * parent)
	: QtIMAccountPlugin(userProfile, imAccount, parent) {

	init();
}

void QtMSNSettings::init() {
	_MSNSettingsWidget = WidgetFactory::create(":/forms/imaccount/MSNSettings.ui", _parentWidget);
	_MSNSettingsWidget->setWindowTitle("MSN " + tr("Settings"));

	//loginLineEdit
	_loginLineEdit = Object::findChild<QLineEdit *>(_MSNSettingsWidget, "loginLineEdit");

	//passwordLineEdit
	_passwordLineEdit = Object::findChild<QLineEdit *>(_MSNSettingsWidget, "passwordLineEdit");

	if (!_imAccount) {
		return;
	}

	IMAccountParameters & param = _imAccount->getIMAccountParameters();

	_loginLineEdit->setText(QString::fromStdString(_imAccount->getLogin()));

	_passwordLineEdit->setText(QString::fromStdString(_imAccount->getPassword()));
}

void QtMSNSettings::save() {
	std::string login = _loginLineEdit->text().toStdString();
	std::string password = _passwordLineEdit->text().toStdString();

	if (!_imAccount) {
		_imAccount = new IMAccount(login, password, EnumIMProtocol::IMProtocolMSN);
	}

	IMAccountParameters & param = _imAccount->getIMAccountParameters();

	_imAccount->setLogin(login);
	_imAccount->setPassword(password);

	_userProfile.addIMAccount(*_imAccount);
	_userProfile.getConnectHandler().connect(*_imAccount);
}
