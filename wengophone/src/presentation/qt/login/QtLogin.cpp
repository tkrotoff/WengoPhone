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

#include <WidgetFactory.h>
#include <Object.h>

#include <QtGui>

QtLogin::QtLogin(QWidget * parent) {
	_loginWindow = qobject_cast<QDialog *>(WidgetFactory::create(":/forms/login/LoginWindow.ui", parent));
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
