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

#include <qtutil/LanguageChangeEventFilter.h>
#include <qtutil/WidgetBackgroundImage.h>
#include <qtutil/SafeConnect.h>

#include <util/Logger.h>
#include <util/WebBrowser.h>

#include <QtGui/QtGui>

QtLogin::QtLogin(QWidget * parent, CUserProfileHandler & cUserProfileHandler)
	: _cUserProfileHandler(cUserProfileHandler) {

	_loginWindow = new QDialog(parent);

	_ui = new Ui::LoginWindow();
	_ui->setupUi(_loginWindow);

	LANGUAGE_CHANGE(_loginWindow);

	WidgetBackgroundImage::setBackgroundImage(_ui->loginLabel, ":pics/headers/login.png", true);

	SAFE_CONNECT(_ui->createWengoAccountButton, SIGNAL(clicked()), SLOT(createWengoAccountButtonClicked()));
	SAFE_CONNECT(_ui->helpButton, SIGNAL(clicked()), SLOT(helpButtonClicked()));
	SAFE_CONNECT(_ui->forgotPasswordButton, SIGNAL(clicked()), SLOT(forgotPasswordButtonClicked()));
	SAFE_CONNECT(_ui->loginComboBox, SIGNAL(currentIndexChanged(const QString &)),
		SLOT(currentIndexChanged(const QString &)));
	SAFE_CONNECT(_ui->loginButton, SIGNAL(clicked()), SLOT(loginClicked()));
	SAFE_CONNECT(_ui->cancelButton, SIGNAL(clicked()), SLOT(cancelClicked()));
	SAFE_CONNECT(_ui->loginComboBox->lineEdit(), SIGNAL(textEdited(const QString &)), SLOT(loginTextEdited(const QString &)));

	_infoPalette = _ui->loginLabel->palette();

	_errorPalette = _infoPalette;
	_errorPalette.setColor(QPalette::WindowText, QColor(Qt::red));

	_dontUpdateWidgets = false;

	init();
}

std::string QtLogin::getLogin() const {
	return _ui->loginComboBox->currentText().toStdString();
}

std::string QtLogin::getPassword() const {
	return _ui->passwordLineEdit->text().toStdString();
}

bool QtLogin::hasAutoLogin() const {
	//return _ui->autoLoginCheckBox->isChecked();
	// FIXME: Needs to be changed to a 'Save my password' option.
	return true;
}

int QtLogin::show() {
	init();

	setInfoMessage(tr("Please enter your email address<br/>and your password"));

	_ui->passwordLineEdit->setEnabled(false);
	_loginWindow->show();

	return 0;
}

int QtLogin::showWithInvalidWengoAccount(WengoAccount wengoAccount) {
	init();

	setErrorMessage(tr("Wrong email/password entered"));

	_ui->passwordLineEdit->setEnabled(true);
	_loginWindow->show();

	return 0;
}

int QtLogin::showWithWengoAccount(WengoAccount wengoAccount) {
	init();

	// Add and select the given WengoAccount
	_dontUpdateWidgets = true;
	_ui->loginComboBox->addItem(QString::fromStdString(wengoAccount.getWengoLogin()));
	setPassword(QString::fromStdString(wengoAccount.getWengoPassword()));
	setAutoLogin(wengoAccount.hasAutoLogin());

	_ui->loginComboBox->setCurrentIndex(_ui->loginComboBox->findText(QString::fromStdString(wengoAccount.getWengoLogin())));

	_ui->passwordLineEdit->setEnabled(false);

	_dontUpdateWidgets = false;
	////

	setInfoMessage(tr("Click on Login to connect to Wengo"));

	_loginWindow->show();

	return 0;
}

void QtLogin::createWengoAccountButtonClicked() {
	/*if (_qtWengoPhone.getSubscribe()) {
		_qtWengoPhone.getSubscribe()->show();
	}*/
	Config & config = ConfigManager::getInstance().getCurrentConfig();
	std::string lang = config.getLanguage();

	if (lang == "fr") {
		WebBrowser::openUrl("http://www.wengo.com/publipasswordLineEditc/public.php?page=subscribe_wengos&lang=fra");
	} else {
		WebBrowser::openUrl("http://www.wengo.com/public/public.php?page=subscribe_wengos&lang=eng");
	}
}

void QtLogin::helpButtonClicked() {
	Config & config = ConfigManager::getInstance().getCurrentConfig();
	std::string lang = config.getLanguage();

	if (lang == "fr") {
		WebBrowser::openUrl("http://wiki.wengo.fr/index.php/Accueil");
	} else {
		WebBrowser::openUrl("http://wiki.wengo.com/index.php/Main_Page");
	}
}

void QtLogin::forgotPasswordButtonClicked() {
	Config & config = ConfigManager::getInstance().getCurrentConfig();
	std::string lang = config.getLanguage();

	if (lang == "fr") {
		WebBrowser::openUrl("https://www.wengo.fr/index.php?yawl[S]=wengo.public.download&yawl[K]=wengo.public.displayLogin");
	} else {
		WebBrowser::openUrl("https://www.wengo.com/index.php?yawl[S]=wengo.public.download&yawl[K]=wengo.public.displayLogin");
	}
}

void QtLogin::setLogin(const QString & login) {
	_ui->loginComboBox->setEditText(login);
}

void QtLogin::setPassword(const QString & password) {
	_ui->passwordLineEdit->setText(password);
}

void QtLogin::setAutoLogin(bool autoLogin) {
	//_ui->autoLoginCheckBox->setChecked(autoLogin);
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
	if (!_dontUpdateWidgets) {
		WengoAccount wengoAccount = _cUserProfileHandler.getWengoAccountOfUserProfile(profileName.toStdString());
		setPassword(QString::fromStdString(wengoAccount.getWengoPassword()));
		setAutoLogin(wengoAccount.hasAutoLogin());
	}
}

void QtLogin::loginClicked() {
	std::string login = _ui->loginComboBox->currentText().toStdString();

	if (!login.empty()) {
		WengoAccount wengoAccount(login, _ui->passwordLineEdit->text().toStdString(), true);

		// Looking for the selected profile
		if (!_cUserProfileHandler.userProfileExists(login)) {
			// The selected profile does not exist. Creating a new one.
			_cUserProfileHandler.createAndSetUserProfile(wengoAccount);
		} else {
			_cUserProfileHandler.setCurrentUserProfile(login, wengoAccount);
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

void QtLogin::loginTextEdited(const QString & text) {
	_ui->passwordLineEdit->setEnabled(true);
}

void QtLogin::languageChanged() {
	_ui->retranslateUi(_loginWindow);
}
