/*
 * WengoPhone, a voice over Internet phone
 * Copyright (C) 2004-2005  Wengo
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

#include "LoginWindow.h"

#include "login/Login.h"
#include "Softphone.h"
#include "config/Authentication.h"
#include "config/Config.h"
#include "MyWidgetFactory.h"
#include "util/EventFilter.h"

#include "softphone-config.h"

#include <WebBrowser.h>

#include <qdialog.h>
#include <qcombobox.h>
#include <qdir.h>
#include <qpushbutton.h>
#include <qlineedit.h>
#include <qlabel.h>
#include <qcheckbox.h>
#include <qregexp.h>
#include <qvalidator.h>
#include <qstringlist.h>

const char * LoginWindow::REG_EXP_ALPHANUM = "([0-9]|[a-z]|[A-Z]|[@]|[.]|[-]|[_])*";

LoginWindow::LoginWindow(QWidget * parent) : QObject(parent) {
	_loginWindow = (QDialog *) MyWidgetFactory::create("LoginWindowForm.ui", this, parent);

	QRegExp regexp(REG_EXP_ALPHANUM);
	QRegExpValidator * validator = new QRegExpValidator(regexp, _loginWindow);

	_loginComboBox = (QComboBox *) _loginWindow->child("loginComboBox", "QComboBox");
	_loginComboBox->setValidator(validator);
	_loginComboBox->insertStringList(getListLogin());
	_passwordLineEdit = (QLineEdit *) _loginWindow->child("passwordLineEdit", "QLineEdit");
	_autoLoginCheckBox = (QCheckBox *) _loginWindow->child("autoLoginCheckBox", "QCheckBox");
	_autoLoginCheckBox->setChecked(true);

	//Ok button
	QPushButton * okButton = (QPushButton *) _loginWindow->child("okButton", "QPushButton");
	connect(okButton, SIGNAL(clicked()),
			this, SLOT(login()));

	//Create new account button
	LeftMouseClickEventFilter * newAccountEventFilter = new LeftMouseClickEventFilter(this, SLOT(createNewWengoAccount()));
	QLabel * newAccountLabel = (QLabel *) _loginWindow->child("newAccountLabel", "QLabel");
	newAccountLabel->installEventFilter(newAccountEventFilter);

	//Help label
	LeftMouseClickEventFilter * helpEventFilter = new LeftMouseClickEventFilter(this, SLOT(showHelp()));
	QLabel * helpLabel = (QLabel *) _loginWindow->child("helpLabel", "QLabel");
	helpLabel->installEventFilter(helpEventFilter);

	//Password label
	LeftMouseClickEventFilter * passwordEventFilter = new LeftMouseClickEventFilter(this, SLOT(showForgotPassword()));
	QLabel * passwordLabel = (QLabel *) _loginWindow->child("passwordLabel", "QLabel");
	passwordLabel->installEventFilter(passwordEventFilter);

	//Logo pixmap
	LeftMouseClickEventFilter * logoWengoEventFilter = new LeftMouseClickEventFilter(this, SLOT(showWengoWebpage()));
	QLabel * logoWengoPixmap = (QLabel *) _loginWindow->child("logoWengoPixmap", "QLabel");
	logoWengoPixmap->installEventFilter(logoWengoEventFilter);
}

LoginWindow::~LoginWindow() {
	delete _loginWindow;
}

void LoginWindow::login() {
	QString loginString = _loginComboBox->currentText();
	QString passwordString = _passwordLineEdit->text();
	bool autoLogin = _autoLoginCheckBox->isChecked();
	_loginWindow->hide();

	if (loginString.isEmpty() || passwordString.isEmpty()) {
		return;
	}

	Softphone & softphone = Softphone::getInstance();
	softphone.setUser(User(loginString, passwordString, autoLogin));

	Login * login = new Login(softphone.getUser());
	login->reinit();
	login->saveAsXml();
}

void LoginWindow::createNewWengoAccount() {
	Config & conf = Config::getInstance();
	const QString langCode = conf.getLanguageCode();
	WebBrowser::openUrl(Softphone::URL_CREATE_WENGO_ACCOUNT + "&lang=" + langCode + "&wl=" + QString(WL_TAG));
}

QStringList LoginWindow::getListLogin() const {
	QDir dir(Softphone::getInstance().getWengoDir());
	QStringList listLogin = dir.entryList(QDir::Dirs);
	QStringList::Iterator it;

	it = listLogin.find(".");
	if (it != listLogin.end())
		listLogin.remove(it);
	it = listLogin.find("..");
	if (it != listLogin.end())
		listLogin.remove(it);

	return listLogin;
}

void LoginWindow::showHelp() {
	WebBrowser::openUrl(Softphone::URL_HELP_WENGO);
}

void LoginWindow::showForgotPassword() {
	WebBrowser::openUrl("http://wengo.fr/index.php?yawl[S]=wengo.public.homePage&yawl[K]=wengo.public.displayLogin");
}

void LoginWindow::showWengoWebpage() {
	WebBrowser::openUrl("http://www.wengo.fr/");
}
