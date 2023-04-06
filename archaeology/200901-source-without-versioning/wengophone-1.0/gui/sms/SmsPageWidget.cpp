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

#include "SmsPageWidget.h"

#include "MyWidgetFactory.h"
#include "contact/Contact.h"
#include "Softphone.h"
#include "sip/SipAddress.h"
#include "login/User.h"
#include "sms/Sms.h"

#include <qwidget.h>
#include <qtextedit.h>
#include <qpushbutton.h>
#include <qcombobox.h>
#include <qlineedit.h>
#include <qvalidator.h>
#include <qmessagebox.h>
#include <qregexp.h>

#include <iostream>
using namespace std;

const int SmsPageWidget::MAX_SMS_LENGTH = 144;
const unsigned int SmsPageWidget::MAX_SIGNATURE_LENGTH = 12;

SmsPageWidget::SmsPageWidget(QWidget * parent)
: QObject(parent) {
	_smsPageWidget = MyWidgetFactory::create("SmsPageWidgetForm.ui", this, parent);

	//Phone combobox
	_phoneComboBox = (QComboBox *) _smsPageWidget->child("phoneComboBox", "QComboBox");
	_phoneComboBox->setDuplicatesEnabled(false);
	QRegExpValidator * validator = new QRegExpValidator(QRegExp("([0-9])*"), _smsPageWidget);
	_phoneComboBox->lineEdit()->setValidator(validator);

	//Signature QLineEdit
	_signatureText = (QLineEdit *) _smsPageWidget->child("signatureText", "QLineEdit");

	//Send button
	_sendButton = (QPushButton *) _smsPageWidget->child("sendButton", "QPushButton");
	connect(_sendButton, SIGNAL(clicked()),
		this, SLOT(sendSms()));

	//SMS message
	_smsText = (QTextEdit *) _smsPageWidget->child("smsText", "QTextEdit");
	_smsText->setFocus();
	connect(_smsText, SIGNAL(textChanged()),
		this, SLOT(smsTextChanged()));
}

void SmsPageWidget::show(QWidget *) {
	//Loads the signature
	//Cannot do that in the constructor since Sms::readSignature()
	//cannot get the signature at that time
	cerr << "SmsPageWidget: readSignature()" << endl;
	QString signature = Sms::readSignature();
	if (signature.isEmpty()) {
		QStringList login = QStringList::split("@", Softphone::getInstance().getUser().getLogin());
		signature = login[0];
	}
	_signatureText->setText(signature);
}

void SmsPageWidget::setContact(const QString & phoneNumber, const Contact & contact) {
	_smsText->clear();
	_phoneComboBox->clear();

	QStringList numberList;
	numberList += phoneNumber;
	if (!contact.getMobilePhone().isEmpty() && numberList.grep(contact.getMobilePhone()).isEmpty()) {
		numberList += contact.getMobilePhone();
	}
	if (!contact.getWorkPhone().isEmpty() && numberList.grep(contact.getWorkPhone()).isEmpty()) {
		numberList += contact.getWorkPhone();
	}
	if (!contact.getHomePhone().isEmpty() && numberList.grep(contact.getHomePhone()).isEmpty()) {
		numberList += contact.getHomePhone();
	}
	if (!contact.getWengoPhone().isEmpty() && numberList.grep(contact.getWengoPhone()).isEmpty()) {
		numberList += contact.getWengoPhone();
	}
	if (!contact.getOtherPhone().isEmpty() && numberList.grep(contact.getOtherPhone()).isEmpty()) {
		numberList += contact.getOtherPhone();
	}

	_phoneComboBox->insertStringList(numberList);
}

void SmsPageWidget::setMessage(const QString & message) {
	_smsText->setText(message);
}

void SmsPageWidget::sendSms() const {
	static const QString SIGNATURE_SEPARATOR = " -- ";
	const User & user = Softphone::getInstance().getUser();

	QMessageBox * messageBox  = new QMessageBox (
		tr("SMS"),
		"",
		QMessageBox::Critical,
		QMessageBox::Ok,
		QMessageBox::NoButton,
		QMessageBox::NoButton,
		Softphone::getInstance().getActiveWindow());
	
	if( _phoneComboBox->lineEdit()->text().isEmpty() ) {
		messageBox->setText(tr("Phone number is empty !"));
		messageBox->show();
	} else {
		SipAddress tmp = SipAddress::fromPhoneNumber(_phoneComboBox->currentText());
		QString phoneNumber = tmp.getValidPhoneNumber();
		QString message = _smsText->text();
		QString signature = _signatureText->text();
		Sms::saveSignature(signature);
		
		//Always dynamic since there is a HTTP request
		//FIXME memory leak
		Sms * sms = new Sms(user, (SmsPageWidget*)this);
		sms->sendSms(phoneNumber, message + SIGNATURE_SEPARATOR + signature);
	}
}

void SmsPageWidget::smsTextChanged() {
	if (_smsText->length() >= MAX_SMS_LENGTH && _smsPageWidget->isVisible()) {
		_smsText->doKeyboardAction(QTextEdit::ActionBackspace);
	}
}

void SmsPageWidget::setFocus() {
	_smsText->setFocus();
}

void SmsPageWidget::setSendButtonEnabled(bool b) {
	_sendButton->setEnabled(b);
}
