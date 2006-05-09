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

#ifndef QTLOGIN_H
#define QTLOGIN_H

#include <QObject>

#include <string>

class QtWengoPhone;
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

	QtLogin(QWidget * parent, QtWengoPhone & qtWengoPhone);

	QDialog * getWidget() const {
		return _loginWindow;
	}

	std::string getLogin() const;

	std::string getPassword() const;

	bool hasAutoLogin() const;

	void setLogin(const QString & login);

	void setPassword(const QString & password);

	void setAutoLogin(bool autoLogin);

public Q_SLOTS:

	int show();

	void createAccountLabelClicked();

private:

	Ui::LoginWindow * _ui;

	QDialog * _loginWindow;

	QtWengoPhone & _qtWengoPhone;
};

#endif	//QTLOGIN_H
