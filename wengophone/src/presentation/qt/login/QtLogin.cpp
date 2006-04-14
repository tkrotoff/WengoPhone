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

#include "QtLogin.h"

#include <presentation/qt/QtWengoPhone.h>
#include <presentation/qt/webservices/subscribe/QtSubscribe.h>

#include <qtutil/WidgetFactory.h>
#include <qtutil/Object.h>
#include <qtutil/MouseEventFilter.h>

#include <QtGui>

QtLogin::QtLogin(QWidget * parent, QtWengoPhone & qtWengoPhone) 
	: _qtWengoPhone(qtWengoPhone) {
	
	_loginWindow = qobject_cast<QDialog *>(WidgetFactory::create(":/forms/login/LoginWindow.ui", parent));
	_createAccountLabel = Object::findChild<QLabel *>(_loginWindow, "linkWengoAccountLabel");
	
	MousePressEventFilter * mouseFilter = new MousePressEventFilter(
		this, SLOT(createAccountLabelClicked()), Qt::LeftButton);
	_createAccountLabel->installEventFilter(mouseFilter);
}

QDialog * QtLogin::getWidget() const {
	return _loginWindow;
}

std::string QtLogin::getLogin() const {
	QComboBox * loginComboBox = Object::findChild<QComboBox *>(_loginWindow, "loginComboBox");
	return loginComboBox->currentText().toStdString();
}

std::string QtLogin::getPassword() const {
	QLineEdit * passwordLineEdit = Object::findChild<QLineEdit *>(_loginWindow, "passwordLineEdit");
	return passwordLineEdit->text().toStdString();
}

bool QtLogin::hasAutoLogin() const {
	QCheckBox * autoLoginCheckBox = Object::findChild<QCheckBox *>(_loginWindow, "autoLoginCheckBox");
	return autoLoginCheckBox->isChecked();
}

int QtLogin::exec() {
	return _loginWindow->exec();
}

void QtLogin::createAccountLabelClicked() {
	if( _qtWengoPhone.getSubscribe() ) {
		_qtWengoPhone.getSubscribe()->exec();
	}
}

void QtLogin::setLogin(const QString & login) {
	QComboBox * loginComboBox = Object::findChild<QComboBox *>(_loginWindow, "loginComboBox");
	loginComboBox->setEditText(login);
}
	
void QtLogin::setPassword(const QString & password) {
	QLineEdit * passwordLineEdit = Object::findChild<QLineEdit *>(_loginWindow, "passwordLineEdit");
	passwordLineEdit->setText(password);
}

void QtLogin::setAutoLogin(bool autoLogin) {
	QCheckBox * autoLoginCheckBox = Object::findChild<QCheckBox *>(_loginWindow, "autoLoginCheckBox");
	if( autoLogin ) {
		autoLoginCheckBox->setCheckState(Qt::Checked);
	} else {
		autoLoginCheckBox->setCheckState(Qt::Unchecked);
	}
}
