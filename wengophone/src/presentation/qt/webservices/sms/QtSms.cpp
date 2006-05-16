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

#include "QtSms.h"

#include <presentation/qt/QtWengoPhone.h>

#include <control/CWengoPhone.h>

#include <qtutil/Object.h>
#include <qtutil/WidgetFactory.h>

#include <QtGui>
#include <QMessageBox>

#include <util/Logger.h>

QtSms::QtSms(CSms & cSms)
	: QObjectThreadSafe(NULL),
	_cSms(cSms) {

	_cSms.smsStatusEvent += boost::bind(&QtSms::smsStatusEventHandler, this, _1, _2, _3);
	_qtWengoPhone = (QtWengoPhone *) _cSms.getCWengoPhone().getPresentation();

	typedef PostEvent0<void ()> MyPostEvent;
	MyPostEvent * event = new MyPostEvent(boost::bind(&QtSms::initThreadSafe, this));
	postEvent(event);
}

void QtSms::initThreadSafe() {
	_smsWindow = WidgetFactory::create(":/forms/webservices/sms/SmsWindow.ui", _qtWengoPhone->getWidget());

	_sendButton = Object::findChild<QPushButton *>(_smsWindow, "sendButton");
	connect(_sendButton, SIGNAL(clicked()), SLOT(sendButtonClicked()));

	_smsText = Object::findChild<QTextEdit *>(_smsWindow, "smsText");

	_qtWengoPhone->setSms(this);
}

void QtSms::updatePresentation() {
}

void QtSms::updatePresentationThreadSafe() {
}

void QtSms::sendButtonClicked() {
	static QComboBox * phoneComboBox = Object::findChild<QComboBox *>(_smsWindow, "phoneComboBox");

	_sendButton->setEnabled(false);

	//Converts to UTF-8
	std::string phoneNumber(phoneComboBox->currentText().toUtf8().constData());
	std::string sms(_smsText->toPlainText().toUtf8().constData());

	_cSms.sendSMS(phoneNumber, sms);
}

void QtSms::smsStatusEventHandler(Sms & sender, int smsId, Sms::SmsStatus status) {
	typedef PostEvent0<void ()> MyPostEvent;
	MyPostEvent * event = new MyPostEvent(boost::bind(&QtSms::smsStatusEventHandlerThreadSafe, this, status));
	postEvent(event);
}

void QtSms::smsStatusEventHandlerThreadSafe(Sms::SmsStatus status) {
	QString smsStatus = String::null;
	switch (status) {
	case Sms::SmsStatusError:
		smsStatus = tr("SmsStatusError");
		break;
	case Sms::SmsStatusOk:
		smsStatus = tr("SmsStatusOk");
		break;
	default:
		LOG_FATAL("Unknown SmsStatus");
	}

	_sendButton->setEnabled(true);
	QMessageBox::information(_smsWindow, tr("Sms"), smsStatus);
}

void QtSms::setPhoneNumber(const QString & phoneNumber) {
	QComboBox * phoneComboBox = Object::findChild<QComboBox *>(_smsWindow, "phoneComboBox");
	if (!phoneNumber.isEmpty()) {
		phoneComboBox->clear();
		phoneComboBox->addItem(phoneNumber);
	}
}

void QtSms::setText(const QString & text) {
	_smsText->setPlainText(text);
}
