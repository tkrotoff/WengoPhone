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

#include "QtHttpProxyLogin.h"

#include <WidgetFactory.h>
#include <Object.h>

#include <QtGui>

QtHttpProxyLogin::QtHttpProxyLogin(QWidget * parent, const std::string & proxyAddress, unsigned proxyPort)
	: QObject() {

	_httpProxyLoginWindow = qobject_cast<QDialog *>(WidgetFactory::create(":/forms/HttpProxyLoginWindow.ui", parent));

	//addressLineEdit
	_addressLineEdit = Object::findChild<QLineEdit *>(_httpProxyLoginWindow, "addressLineEdit");
	_addressLineEdit->setText(QString::fromStdString(proxyAddress));

	//portLineEdit
	_portLineEdit = Object::findChild<QLineEdit *>(_httpProxyLoginWindow, "portLineEdit");
	_portLineEdit->setText(QString::number(proxyPort));
}

std::string QtHttpProxyLogin::getLogin() const {
	QLineEdit * loginLineEdit = Object::findChild<QLineEdit *>(_httpProxyLoginWindow, "loginLineEdit");
	return loginLineEdit->text().toStdString();
}

std::string QtHttpProxyLogin::getPassword() const {
	QLineEdit * passwordLineEdit = Object::findChild<QLineEdit *>(_httpProxyLoginWindow, "passwordLineEdit");
	return passwordLineEdit->text().toStdString();
}

std::string QtHttpProxyLogin::getProxyAddress() const {
	return _addressLineEdit->text().toStdString();
}

unsigned QtHttpProxyLogin::getProxyPort() const {
	return _portLineEdit->text().toUInt();
}

int QtHttpProxyLogin::exec() {
	return _httpProxyLoginWindow->exec();
}
