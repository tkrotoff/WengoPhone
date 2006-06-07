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

#include <presentation/qt/webservices/subscribe/QtSubscribe.h>

#include <control/profile/CUserProfileHandler.h>

#include <model/account/wengo/WengoAccount.h>
#include <model/config/ConfigManager.h>
#include <model/config/Config.h>

#include <qtutil/MouseEventFilter.h>
#include <qtutil/WidgetBackgroundImage.h>

#include <util/WebBrowser.h>

#include <QtGui>

QtLogin::QtLogin(QWidget * parent, CUserProfileHandler & cUserProfileHandler)
	: _cUserProfileHandler(cUserProfileHandler) {

	_loginWindow = new QDialog(parent);

	_ui = new Ui::LoginWindow();
	_ui->setupUi(_loginWindow);

	WidgetBackgroundImage::setBackgroundImage(_ui->loginLabel, ":pics/headers/login.png", true);

	MousePressEventFilter * mouseFilter = new MousePressEventFilter(
		this, SLOT(createAccountLabelClicked()), Qt::LeftButton);
	_ui->linkWengoAccountLabel->installEventFilter(mouseFilter);

	connect(_ui->loginComboBox, SIGNAL(currentIndexChanged(const QString &)),
		SLOT(currentIndexChanged(const QString &)));
	connect(_ui->loginButton, SIGNAL(clicked()), SLOT(loginClicked()));
	connect(_ui->cancelButton, SIGNAL(clicked()), SLOT(cancelClicked()));

	_infoPalette = _ui->loginLabel->palette();

	_errorPalette = _infoPalette;
	_errorPalette.setColor(QPalette::WindowText, QColor(Qt::red));

	init();
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
	init();

	setInfoMessage(tr("Please enter your email address<br/>and your password"));

	return _loginWindow->exec();
}

int QtLogin::showWithInvalidWengoAccount(WengoAccount wengoAccount) {
	init();

	setErrorMessage(tr("Wrong email/password entered"));

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

void QtLogin::init() {
	_ui->loginComboBox->clear();
	
	std::vector<std::string> profileNames = _cUserProfileHandler.getUserProfileNames();

	for (std::vector<std::string>::const_iterator it = profileNames.begin();
		it != profileNames.end();
		++it) {
		_ui->loginComboBox->addItem(QString::fromStdString(*it));
	}

	if (profileNames.size() > 0) {
		Config & config = ConfigManager::getInstance().getCurrentConfig();
		_ui->loginComboBox->setCurrentIndex(_ui->loginComboBox->findText(QString::fromStdString(config.getProfileLastUsedName())));
	}
}

void QtLogin::currentIndexChanged(const QString & profileName) {
	WengoAccount wengoAccount = _cUserProfileHandler.getWengoAccountOfUserProfile(profileName.toStdString());
	setPassword(QString::fromStdString(wengoAccount.getWengoPassword()));
	setAutoLogin(wengoAccount.hasAutoLogin());
}

void QtLogin::slotUpdatedTranslation() {
  _ui->retranslateUi(_loginWindow);
}

void QtLogin::loginClicked() {
	std::string login = _ui->loginComboBox->currentText().toStdString();
	
	//FIXME: if login is empty we should create a default profile
	if (!login.empty()) {
		// Looking for the selected profile
		if (!_cUserProfileHandler.userProfileExists(login)) {
			// The selected profile does not exist. Creating a new one.
			WengoAccount wengoAccount(login, _ui->passwordLineEdit->text().toStdString(), _ui->autoLoginCheckBox->isChecked());
			_cUserProfileHandler.createAndSetUserProfile(wengoAccount);
		} else {
			_cUserProfileHandler.setCurrentUserProfile(login);
		}

		_loginWindow->accept();
	}
}

void QtLogin::cancelClicked() {
	_loginWindow->reject();
}

void QtLogin::setInfoMessage(const QString & message) {
	//_ui->loginLabel->setPalette(_infoPalette);
	setLoginLabel(message);
}

void QtLogin::setErrorMessage(const QString & message) {
	//_ui->loginLabel->setPalette(_errorPalette);
	setLoginLabel(QString("<font color=\"red\">%1</font>").arg(message));
}

void QtLogin::setLoginLabel(const QString & message) {
	QString loginLabel = QString("<font size=\"18\">Login</font><br/>%1").arg(message);

	_ui->loginLabel->setText(loginLabel);
}
