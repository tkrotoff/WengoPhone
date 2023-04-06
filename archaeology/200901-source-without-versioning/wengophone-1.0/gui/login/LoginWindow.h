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

#ifndef LOGINWINDOW_H
#define LOGINWINDOW_H

#include <qobject.h>

class QDialog;
class QWidget;
class QComboBox;
class QLineEdit;
class QCheckBox;
class QStringList;

/**
 * Login window of the softphone.
 *
 * @author Tanguy Krotoff
 */
class LoginWindow : public QObject {
	Q_OBJECT
public:

	LoginWindow(QWidget * parent);

	~LoginWindow();

	/**
	 * Gets the low-level gui widget of this class.
	 *
	 * @return low-level widget
	 */
	QDialog * getWidget() {
		return _loginWindow;
	}

	void crash() const {
	}

private slots:

	void login();

	void createNewWengoAccount();

	void showHelp();

	void showWengoWebpage();

	void showForgotPassword();

private:

	LoginWindow(const LoginWindow &);
	LoginWindow & operator=(const LoginWindow &);

	QStringList getListLogin() const;

	/**
	 * Low-level widget of this class.
	 */
	QDialog * _loginWindow;

	QComboBox * _loginComboBox;

	QLineEdit * _passwordLineEdit;

	QCheckBox * _autoLoginCheckBox;

	static const char * REG_EXP_ALPHANUM;
};

#endif	//LOGINWINDOW_H
