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

#include "ui_SmsWindow.h"

#include <control/CWengoPhone.h>

#include <presentation/qt/QtWengoPhone.h>

#include <QtGui>

#include <util/Logger.h>

const QString & QtSms::signatureSeparator = " -- ";

QtSms::QtSms(CSms & cSms)
	: QObjectThreadSafe(NULL),
	_cSms(cSms) {

	_cSms.smsStatusEvent += boost::bind(&QtSms::smsStatusEventHandler, this, _1, _2, _3);
	_qtWengoPhone = (QtWengoPhone *) _cSms.getCWengoPhone().getPresentation();

	typedef PostEvent0<void ()> MyPostEvent;
	MyPostEvent * event = new MyPostEvent(boost::bind(&QtSms::initThreadSafe, this));
	postEvent(event);
}

QtSms::~QtSms() {
	//TODO: unregister events, delete created objects
	_qtWengoPhone->setSms(NULL);
}

void QtSms::initThreadSafe() {
	_smsWindow = new QDialog(_qtWengoPhone->getWidget());

	_ui = new Ui::SmsWindow();
	_ui->setupUi(_smsWindow);

	connect(_ui->sendButton, SIGNAL(clicked()), SLOT(sendButtonClicked()));

	_qtWengoPhone->setSms(this);
}

QWidget * QtSms::getWidget() const {
	return _smsWindow;
}

void QtSms::updatePresentation() {
}

void QtSms::updatePresentationThreadSafe() {
}

void QtSms::sendButtonClicked() {

	//Validate SMS length before sending
	if (!checkSmsLength()) {

		QMessageBox::warning(_smsWindow,
			tr("Wengo SMS service"),
			tr("Your message is too long.\n"
				"The length can not exeed 156 characters.\n"
				"Don't forget to add your signature length."
			  ));

		return;
	}

	_ui->sendButton->setEnabled(false);

	//Converts to UTF-8
	std::string phoneNumber(_ui->phoneComboBox->currentText().toUtf8().constData());
	std::string sms(getCompleteMessage().toUtf8().constData());

	_cSms.sendSMS(phoneNumber, sms);
}

void QtSms::smsStatusEventHandler(WsSms & sender, int smsId, WsSms::SmsStatus status) {
	typedef PostEvent0<void ()> MyPostEvent;
	MyPostEvent * event = new MyPostEvent(boost::bind(&QtSms::smsStatusEventHandlerThreadSafe, this, status));
	postEvent(event);
}

void QtSms::smsStatusEventHandlerThreadSafe(WsSms::SmsStatus status) {
	QString smsStatus = String::null;
	switch (status) {
	case WsSms::SmsStatusError:
		smsStatus = tr("Your SMS has not been sent");
		break;
	case WsSms::SmsStatusOk:
		smsStatus = tr("Your SMS has been sent");
		break;
	default:
		LOG_FATAL("unknown SmsStatus=" + String::fromNumber(status));
	}

	_ui->sendButton->setEnabled(true);
	QMessageBox::information(_smsWindow, tr("Wengo SMS service"), smsStatus);
}

void QtSms::setPhoneNumber(const QString & phoneNumber) {
	_ui->phoneComboBox->clear();
	if (!phoneNumber.isEmpty()) {
		_ui->phoneComboBox->addItem(phoneNumber);
	}
}

void QtSms::setText(const QString & text) {
	int pos = text.lastIndexOf(signatureSeparator);
	QString mess = text;
	if(pos != -1) {
		//extract the signature
		setSignature(text.right(text.length() - pos - 4));
		mess = text.left(pos);
	}

	_ui->smsText->clear();
	_ui->smsText->setHtml("<html><head><meta name=\"qrichtext\" content=\"1\" /></head><body style=\" white-space: pre-wrap; font-family:Sans Serif; font-size:9pt; font-weight:400; font-style:normal; text-decoration:none;\"><p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">" + mess + "</p></body></html>");
}

bool QtSms::checkSmsLength() {

	QString mess = getCompleteMessage();

	return (mess.length() < 160);
}

QString QtSms::getCompleteMessage() {

	QString completeMessage = _ui->smsText->toPlainText();
	completeMessage += signatureSeparator;
	completeMessage += _ui->signatureLineEdit->text();

	return completeMessage;
}

void QtSms::setSignature(const QString & signature) {
	_ui->signatureLineEdit->setText(signature);
}
