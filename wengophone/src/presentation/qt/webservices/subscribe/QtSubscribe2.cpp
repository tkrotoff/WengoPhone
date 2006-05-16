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

#include "QtSubscribe2.h"

#include "ui_SubscribeWengo2.h"

#include <presentation/qt/QtWengoPhone.h>
#include <presentation/qt/login/QtLogin.h>

#include <util/Logger.h>

#include <QtGui>

QtSubscribe2::QtSubscribe2(const QString & nickName, const QString & password, const QString & email, QWidget * parent)
	: QObject(parent) {

	_subscribeWindow = new QDialog(parent);

	_ui = new Ui::SubscribeWengo2();
	_ui->setupUi(_subscribeWindow);

	_ui->nicknameLineEdit->setText(nickName);
	_ui->passwordLineEdit->setText(password);
	_ui->emailLineEdit->setText(email);

	connect(_ui->addIMAccountButton, SIGNAL(clicked()), SLOT(addIMAccount()));
	connect(_ui->finishButton, SIGNAL(clicked()), SLOT(configureLogin()));

	show();
}

QtSubscribe2::~QtSubscribe2() {
	delete _ui;
}

void QtSubscribe2::show() {
	_subscribeWindow->exec();
}

void QtSubscribe2::configureLogin() {
	/*_qtWengoPhone->getLogin()->setLogin(_ui->emailLineEdit->text().toStdString());
	_qtWengoPhone->getLogin()->setPassword(_ui->passwordLineEdit->text().toStdString());
	_qtWengoPhone->getLogin()->setAutoLogin(true);
	_qtWengoPhone->getLogin()->getWidget()->exec();*/
	_subscribeWindow->close();
}

void QtSubscribe2::addIMAccount() {
}
