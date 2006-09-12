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
	_smsWindow = new QDialog(_qtWengoPhone->getWidget());

	_ui = new Ui::SmsWindow();
	_ui->setupUi(_smsWindow);

	connect(_ui->sendButton, SIGNAL(clicked()), SLOT(sendButtonClicked()));
	connect(_ui->smsText, SIGNAL(textChanged()), SLOT(messageModified()));
	connect(_ui->signatureLineEdit, SIGNAL(textChanged(const QString &)), SLOT(signatureModified()));

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
	_ui->sendButton->setEnabled(false);

	//Converts to UTF-8
	std::string phoneNumber(_ui->phoneComboBox->currentText().toUtf8().constData());
	std::string sms(getCompleteMessage().toUtf8().constData());

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
		smsStatus = tr("Your SMS has not been sent");
		break;

	case Sms::SmsStatusOk:
		smsStatus = tr("Your SMS has been sent");
		break;

	default:
		LOG_FATAL("unknown status=" + String::fromNumber(status));
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

void QtSms::setText(const QString & smsMessage) {
	_ui->smsText->setPlainText(smsMessage);
}

void QtSms::messageModified() {
	if (!isSmsLengthOk()) {
		QString smsMessage = _ui->smsText->toPlainText();

		//Truncate the SMS message
		smsMessage.truncate(smsMessage.size() - 1);
		_ui->smsText->setPlainText(smsMessage);

		//Move the cursor to the end of the document
		QTextCursor textCursor = _ui->smsText->textCursor();
		textCursor.movePosition(QTextCursor::End);
		_ui->smsText->setTextCursor(textCursor);
	}
}

void QtSms::signatureModified() {
	if (!isSmsLengthOk()) {
		QString signature = _ui->signatureLineEdit->text();

		//Truncate the signature
		signature.truncate(signature.size() - 1);
		_ui->smsText->setPlainText(signature);

		//Move the cursor to the end of the document
		QTextCursor textCursor = _ui->smsText->textCursor();
		textCursor.movePosition(QTextCursor::End);
		_ui->smsText->setTextCursor(textCursor);
	}
}

bool QtSms::isSmsLengthOk() const {
	return (getCompleteMessage().size() < Sms::SMS_MAX_LENGTH);
}

QString QtSms::getCompleteMessage() const {
	QString completeMessage = _ui->smsText->toPlainText();
	completeMessage += " -- ";
	completeMessage += _ui->signatureLineEdit->text();

	return completeMessage;
}

void QtSms::setSignature(const QString & signature) {
	_ui->signatureLineEdit->setText(signature);
}
