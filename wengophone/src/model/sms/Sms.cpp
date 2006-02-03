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

#include "Sms.h"

#include <model/account/wengo/WengoAccount.h>

#include <Logger.h>
#include <http/HttpRequestFactory.h>
#include <http/CurlHttpRequestFactory.h>

static const std::string WENGO_SERVER_HOSTNAME = "ws.wengo.fr";
static const std::string WENGO_SMS_PATH = "/sms/sendsms.php";

Sms::Sms(WengoAccount & wengoAccount)
	: _wengoAccount(wengoAccount) {
}

/* If you modify this please modify all use of HttpRequest (WengoAccount for example). */
void Sms::sendSMS(const std::string & phoneNumber, const std::string & message) {

	std::string data = "login=" + String::encodeUrl(_wengoAccount.getWengoLogin()) +
				"&password=" + String::encodeUrl(_wengoAccount.getWengoPassword()) +
				"&message=" + String::encodeUrl(message) +
				"&target=" + phoneNumber +
				"&wl=" + QString(WL_TAG);

	LOG_DEBUG("sending SMS");

	//HTTPS + POST method
	sendRequest(true, Softphone::WENGO_SERVER_HOSTNAME, 443, Softphone::WENGO_SMS_PATH, data, true);

	////

	//Url::encode(_wengoLogin);
	//Url::encode(_wengoPassword);
	std::string data = "login=" + _wengoLogin + "&password=" + _wengoPassword + "&wl=" + WengoPhoneBuildId::SOFTPHONE_NAME;

	//FIXME if not static it crashes inside boost::thread, do not know why
	static HttpRequest httpRequest;

	httpRequest.answerReceivedEvent += boost::bind(&Sms::answerReceivedEventHandler, this, _1, _2);
	httpRequest.setFactory(new CurlHttpRequestFactory());
	//TODO not implemented inside libutil
	//httpRequest.setProxy("proxy.wengo.fr", 8080, "myProxyUsername", "myProxyPassword");

	//First parameter: true = HTTPS, false = HTTP
	//Last parameter: true = POST method, false = GET method
	httpRequest.sendRequest(true, WENGO_SERVER_HOSTNAME, 443, WENGO_SMS_PATH, data, true);
}

void Sms::answerReceivedEventHandler(const std::string & answer, HttpRequest::Error error) {
	static const std::string STATUS_UNAUTHORIZED = "401";
	static const std::string STATUS_OK = "200";

	String tmp(answer);

	if (error == HttpRequest::NoError && !tmp.empty()) {
		if (tmp.contains(STATUS_OK) && !tmp.contains(STATUS_UNAUTHORIZED)) {
			LOG_DEBUG("SMS sent");
			loginEvent(*this, SmsStatusOk);
			return;
		}
	}

	LOG_DEBUG("SMS unsent");
	smsStatusEvent(*this, SmsStatusError);
}
