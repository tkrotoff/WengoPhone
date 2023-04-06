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

#ifndef SMS_H
#define SMS_H

#include "login/User.h"

#include <qobject.h>
//#include "database/HttpSource.h"

#include <http/HttpRequest.h>
#include "SmsPageWidget.h"

class User;
class QMessageBox;
namespace database {
	class HttpSource;
}
class QString;

/**
 * Sends a SMS using HTTP.
 *
 * @author Tanguy Krotoff
 * @author Mathieu Stute
 */
class Sms : public QObject, public HttpRequest {
	Q_OBJECT
public:

	/**
	 * Creates a Sms object given the User sending the SMS.
	 *
	 * @param user User who wants to send the SMS, we need the login/password of the User
	 */
	Sms(const User & user, SmsPageWidget * parent);

	~Sms();

	/**
	 * Sends the SMS message to a given phone number.
	 *
	 * Uses the login/password of the User associated with the Sms.
	 * @param phoneNumber receiver phone number
	 * @param message SMS message to send
	 */
	void sendSms(const QString & phoneNumber, const QString & message);

	/**
	 * Saves the SMS signature into a file.
	 *
	 * @param signature text signature to save into a file
	 */
	static void saveSignature(const QString & signature);

	/**
	 * Reads the signature from a file.
	 *
	 * @return signature read from the file or QString::null if no signature file available
	 */
	static const QString readSignature();
	
	virtual void answerReceived(const std::string & answer, Error error);
		
protected:
	
	void customEvent(QCustomEvent *ev);
	
private:

	/**
	 * User who wants to send a SMS.
	 */
	const User & _user;

	QMessageBox * _messageBox;
	
	SmsPageWidget * _parent;
};

#endif	//SMS_H
