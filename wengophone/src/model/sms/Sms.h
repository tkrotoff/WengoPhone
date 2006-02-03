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

#ifndef SMS_H
#define SMS_H

#include <NonCopyable.h>
#include <Event.h>
#include <http/HttpRequest.h>

#include <string>

class WengoAccount;

/**
 * Sends a SMS using a web service from Wengo.
 *
 * SMS stands for Short Message Service.
 *
 * SMS sending works, reception not.
 *
 * - Command
 *
 * Everything should be done using HTTPS + POST method
 *
 * https://ws.wengo.fr/sms/sendsms.php?login=LOGIN&password=PASSWORD&message=MESSAGE&target=PHONE_NUMBER&wl=WL_TAG
 *
 * - LOGIN
 * user email address (the one from the Wengo login procedure)
 * URL encode
 * cf http://www.blooberry.com/indexdot/html/topics/urlencoding.htm
 * cf http://doc.trolltech.com/3.3/qurl.html#encode
 *
 * - PASSWORD
 * user password (the one from the Wengo login procedure)
 * URL encode
 *
 * - MESSAGE
 * SMS message
 * URL encode
 * maximum length: 160 caractères after URL encode
 * an automatic signature is added to the SMS message
 *
 * - PHONE_NUMBER
 * phone number that will receive the SMS
 *
 * - WL_TAG
 * Always wengo if the software comes from Wengo
 *
 *
 * - Listener
 *
 * - Error
 * <pre>
 * <?xml version="1.0"?>
 * <status code="401"/>
 * </pre>
 *
 * - Ok
 * <pre>
 * <?xml version="1.0"?>
 * <status code="200"/>
 * </pre>
 *
 *
 * - Example
 *
 * LOGIN=robobob1502@toto.com
 * PASSWORD=XXXXXXX
 * MESSAGE=
 * éééé èèèè àààà çççç
 * new line
 * new line -- robobob1502
 * TARGET=0678427025
 * WL_TAG=wengo
 *
 * <pre>
 * /sms/sendsms.php
 * ?login=robobob1502%40toto.com
 * &password=XXXXXXX
 * &message=%C3%A9%C3%A9%C3%A9%C3%A9
 *          %20%C3%A8%C3%A8%C3%A8%C3%A8
 *          %20%C3%A0%C3%A0%C3%A0%C3%A0
 *          %20%C3%A7%C3%A7%C3%A7%C3%A7
 *          %0Anew%20line%0Anew%20line
 *          %20--%20robobob1502
 * &target=0678427025
 * &wl=wengo
 * </pre>
 *
 * @ingroup model
 * @author Tanguy Krotoff
 */
class Sms : NonCopyable {
public:

	enum SmsStatus {
		/** The SMS was not sent. */
		SmsStatusError,

		/** The SMS was sent. */
		SmsStatusOk
	};

	/**
	 * Callback to check if the SMS was received or not.
	 *
	 * @param sender this class
	 * @param status SMS status (ok or error)
	 */
	Event <void (Sms & sender, SmsStatus status)> smsStatusEvent;

	Sms(WengoAccount & wengoAccount);

	/**
	 * Sends a SMS given a destination phone number and a message.
	 *
	 * @param phoneNumber phone that will receive the SMS
	 * @param message SMS message
	 */
	void sendSMS(const std::string & phoneNumber, const std::string & message);

private:

	void answerReceivedEventHandler(const std::string & answer, HttpRequest::Error error);

	WengoAccount & _wengoAccount;
};

#endif	//SMS_H
