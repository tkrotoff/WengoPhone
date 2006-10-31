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

#ifndef OWQTLOGIN_H
#define OWQTLOGIN_H

#include <model/account/wengo/WengoAccount.h>

#include <QtCore/QObject>
#include <QtGui/QPalette>

#include <string>

class CUserProfileHandler;
class QWidget;
class QDialog;
class QLabel;

namespace Ui { class LoginWindow; }

/**
 * Qt login window.
 *
 * @author Tanguy Krotoff
 * @author Mathieu Stute
 */
class QtLogin : public QObject {
	Q_OBJECT
public:

	QtLogin(QWidget * parent, CUserProfileHandler & cUserProfileHandler);

	QDialog * getWidget() const {
		return _loginWindow;
	}

	std::string getLogin() const;

	std::string getPassword() const;

	bool hasAutoLogin() const;

public Q_SLOTS:

	/**
	 * Will display the login window normally.
	 */
	int show();

	/**
	 * Will display the login dialog and the given WengoAccount which is invalid
	 * (the password or the login is/are wrong).
	 * Thus a message on the login will say that the login/password is invalid.
	 */
	int showWithInvalidWengoAccount(WengoAccount wengoAccount);

	/**
	 * Will display the login dialog, add the given WengoAccount to the
	 * profile list and select it as default.
	 */
	int showWithWengoAccount(WengoAccount wengoAccount);

private Q_SLOTS:

	void createWengoAccountButtonClicked();

	void helpButtonClicked();

	void forgotPasswordButtonClicked();

	void currentIndexChanged(const QString & profileName);

	void loginClicked();

	void cancelClicked();

	void languageChanged();

	void loginTextEdited(const QString & text);

private:

	void setLogin(const QString & login);

	void setPassword(const QString & password);

	void setAutoLogin(bool autoLogin);

	/**
	 * Sets the text of infoLabel to 'message' and colorize it to normal color.
	 */
	void setInfoMessage(const QString & message);

	/**
	 * Sets the text of infoLabel to 'message' and colorize it to red.
	 */
	void setErrorMessage(const QString & message);

	/**
	 * Sets the login label.
	 */
	void setLoginLabel(const QString & message);

	/**
	 * Initializes the widgets.
	 */
	void init();

	bool _dontUpdateWidgets;

	Ui::LoginWindow * _ui;

	QDialog * _loginWindow;

	QPalette _infoPalette;

	QPalette _errorPalette;

	CUserProfileHandler & _cUserProfileHandler;
};

#endif	//OWQTLOGIN_H
