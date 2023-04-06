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

#include "Sms.h"

#include "Softphone.h"
#include "database/Database.h"
using namespace database;

#include "callhistory/CallHistory.h"
#include "callhistory/CallLog.h"
#include "login/User.h"
#include "contact/ContactList.h"
#include "contact/Contact.h"
#include "SessionWindow.h"
#include "config/AdvancedConfig.h"

#include <qstring.h>
#include <qmessagebox.h>
#include <qurl.h>
#include <timer/Timer.h>
#include <qapplication.h>

#include "softphone-config.h"


#include <iostream>
using namespace std;

/**
 * Custom event for sms.
 *
 * @author Mathieu Stute
 */
class SmsEvent : public QCustomEvent {
	public:
		enum Type { Sms = (QEvent::User + 203) };
		SmsEvent(Type t, QString answer) : QCustomEvent(t), _answer(answer) {}
		~SmsEvent() {}
		QString anwser() {
			return _answer;
		}
	private:
		QString _answer;
};

Sms::Sms(const User & user, SmsPageWidget * parent)
	: _user(user), _parent(parent) {
}

Sms::~Sms() {
}

void Sms::sendSms(const QString & phoneNumber, const QString & message) {
	//Adds the SMS to the CallHistory
	CallHistory & history = CallHistory::getInstance();
	CallLog * call = new CallLog();
	call->setType(CallLog::SmsOutgoing);
	call->setSms(message);
	call->setPhoneNumber(phoneNumber);
	Contact * contact = ContactList::getInstance().getContact(phoneNumber);
	if (contact == NULL) {
		call->setContactName(phoneNumber);
	} else {
		call->setContactName(contact->toString());
	}
	history.addCallLog(*call);

	_messageBox  = new QMessageBox (
		tr("SMS"),
		"",
		QMessageBox::Critical,
		QMessageBox::Ok,
		QMessageBox::NoButton,
		QMessageBox::NoButton,
		Softphone::getInstance().getActiveWindow());
	
	QString login = _user.getLogin();
	QUrl::encode(login);
	QString password = _user.getPassword();
	QUrl::encode(password);
	QString msg = message;
	QUrl::encode(msg);
	
	QString data = "login=" + login +
				"&password=" + password +
				"&message=" + msg +
				"&target=" + phoneNumber + "&wl=" + QString(WL_TAG);
	
	cerr << "SendSms: send the SMS" << endl;
	_parent->setSendButtonEnabled(false);
	
	
	AdvancedConfig & conf = AdvancedConfig::getInstance();
	if( conf.useSSL() ) {
		sendRequest(true, Softphone::WENGO_SERVER_HOSTNAME, 443, Softphone::WENGO_SMS_PATH, data, true);
	} else {
		sendRequest(false, Softphone::WENGO_SERVER_HOSTNAME, 80, Softphone::WENGO_SMS_PATH, data, true);	
	}
}

void Sms::answerReceived(const std::string & answer, Error error){
	QApplication::postEvent(this, new SmsEvent(SmsEvent::Sms, answer));
}

void Sms::saveSignature(const QString & signature) {
	QFile file(Softphone::getInstance().getSignatureFile());
	if (file.open(IO_WriteOnly)) {
		QTextStream stream(&file);
		stream << signature << endl;
		file.close();
	}
}

const QString Sms::readSignature() {
	QString signature = QString::null;
	QFile file(Softphone::getInstance().getSignatureFile());
	if (file.open(IO_ReadOnly)) {
		QTextStream stream(&file);
		signature = stream.read();
		file.close();
	}
	return signature;
}

void Sms::customEvent(QCustomEvent *ev) {
	static const QString STATUS_UNAUTHORIZED = "401";
	static const QString STATUS_OK = "200";
	
	_parent->setSendButtonEnabled(true);
	
	SmsEvent *e = (SmsEvent*) ev;
	QString ack = e->anwser();

	if (ack.find(STATUS_OK) != -1) {
	_messageBox->setText(tr("The SMS was sent"));
	_messageBox->setIcon(QMessageBox::Information);
	_messageBox->show();
	} else {
		if (ack.find(STATUS_UNAUTHORIZED) != -1) {
		_messageBox->setText(tr("You cannot send this SMS, this login is unauthorized"));
		_messageBox->setIcon(QMessageBox::Warning );
		_messageBox->show();
		} else {
			_messageBox->setText(tr("You cannot send this SMS"));
			_messageBox->setIcon(QMessageBox::Warning );
			_messageBox->show();
		}
	}
}
