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

#include <util/Logger.h>

#include <QtGui>

QtAIMSettings::QtAIMSettings(UserProfile & userProfile, IMAccount * imAccount, QWidget * parent)
	: QtIMAccountPlugin(userProfile, imAccount, parent) {

	init();
}

void QtAIMSettings::init() {
	_IMSettingsWidget = new QWidget(_parentWidget);

	_ui = new Ui::AIMSettings();
	_ui->setupUi(_IMSettingsWidget);

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
