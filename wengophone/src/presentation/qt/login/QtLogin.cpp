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

#include "QtLogin.h"

#include "ui_LoginWindow.h"

#include <presentation/qt/QtWengoPhone.h>
#include <presentation/qt/webservices/subscribe/QtSubscribe.h>

#include <model/config/ConfigManager.h>
#include <model/config/Config.h>

#include <qtutil/MouseEventFilter.h>
#include <qtutil/WidgetBackgroundImage.h>

#include <util/WebBrowser.h>

#include <QtGui>

QtLogin::QtLogin(QWidget * parent, QtWengoPhone & qtWengoPhone)
	: _qtWengoPhone(qtWengoPhone) {

	_loginWindow = new QDialog(parent);

	_ui = new Ui::LoginWindow();
	_ui->setupUi(_loginWindow);

	WidgetBackgroundImage::setBackgroundImage(_ui->loginLabel, ":pics/headers/login.png", true);

	MousePressEventFilter * mouseFilter = new MousePressEventFilter(
		this, SLOT(createAccountLabelClicked()), Qt::LeftButton);
	_ui->linkWengoAccountLabel->installEventFilter(mouseFilter);
}

std::string QtLogin::getLogin() const {
	return _ui->loginComboBox->currentText().toStdString();
}

std::string QtLogin::getPassword() const {
	return _ui->passwordLineEdit->text().toStdString();
}

bool QtLogin::hasAutoLogin() const {
	return _ui->autoLoginCheckBox->isChecked();
}

int QtLogin::show() {
	return _loginWindow->exec();
}

void QtLogin::createAccountLabelClicked() {
	/*if (_qtWengoPhone.getSubscribe()) {
		_qtWengoPhone.getSubscribe()->show();
	}*/
	Config & config = ConfigManager::getInstance().getCurrentConfig();
	std::string lang = config.getLanguage();

	if (lang == "fr") {
		WebBrowser::openUrl("http://www.wengo.com/public/public.php?page=subscribe_wengos&lang=fra");
	} else {
		WebBrowser::openUrl("http://www.wengo.com/public/public.php?page=subscribe_wengos&lang=eng");
	}
}

void QtLogin::setLogin(const QString & login) {
	_ui->loginComboBox->setEditText(login);
}

void QtLogin::setPassword(const QString & password) {
	_ui->passwordLineEdit->setText(password);
}

void QtLogin::setAutoLogin(bool autoLogin) {
	_ui->autoLoginCheckBox->setChecked(autoLogin);
}

void QtLogin::slotUpdatedTranslation() {
  _ui->retranslateUi(_loginWindow);
}
