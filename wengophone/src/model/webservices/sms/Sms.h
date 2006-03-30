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

#ifndef OW_SMSWEBSERVICE_H
#define OW_SMSWEBSERVICE_H

#include <model/webservices/WengoWebService.h>

/**
 * @class Sms Wengo sms web service
 *
 * @author Mathieu Stute
 */
class Sms : public WengoWebService {

public:
	
	enum SmsStatus {
		/** The SMS was not sent. */
		SmsStatusError,

		/** The SMS was sent. */
		SmsStatusOk
	};
	
	/**
	 * default constructor
	 *
	 * @param wengoAccount the WengoAccount used for web services
	 */
	Sms(WengoAccount & wengoAccount);

	/**
	 * Callback to check if the SMS was received or not.
	 *
	 * @param sender this class
	 * @param smsId SMS unique identifier
	 * @param status SMS status (ok or error)
	 */
	Event<void (Sms & sender, int smsId, SmsStatus status)> smsStatusEvent;

	/**
	 * Sends a SMS given a destination phone number and a message.
	 *
	 * @param phoneNumber phone that will receive the SMS
	 * @param message SMS message
	 * @return unique SMS ID
	 */
	int sendSMS(const std::string & phoneNumber, const std::string & message);

private:

	/**
	 * @see WengoWebService
	 */
	void answerReceived(std::string answer, int id);
};

#endif //OW_SMSWEBSERVICE_H
