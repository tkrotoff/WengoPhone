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
#include <model/config/ConfigManager.h>
#include <model/config/Config.h>
#include <WengoPhoneBuildId.h>

#include <Logger.h>
#include <http/HttpRequestFactory.h>
#include <http/CurlHttpRequestFactory.h>

Sms::Sms(WengoAccount & wengoAccount)
	: _wengoAccount(wengoAccount) {

	_answerReceivedAlready = false;
}

int Sms::sendSMS(const std::string & phoneNumber, const std::string & message) {
	//FIXME QUrl::encode() from Qt3 does not encode .
	//String::encodeUrl() encodes . to %2e
	//see http://www.w3schools.com/tags/ref_urlencode.asp
	String login = String::encodeUrl(_wengoAccount.getWengoLogin());
	login.replace("%2e", ".", false);
	String password = String::encodeUrl(_wengoAccount.getWengoPassword());
	password.replace("%2e", ".", false);
	String message2 = String::encodeUrl(message);
	message2.replace("%2e", ".", false);

	std::string data = "login=" + login +
				"&password=" + password +
				"&message=" + message2 +
				"&target=" + phoneNumber +
				"&wl=" + WengoPhoneBuildId::SOFTPHONE_NAME;

	LOG_DEBUG("sending SMS");

	//FIXME if not static it crashes inside boost::thread, do not know why
	static HttpRequest httpRequest;

	httpRequest.answerReceivedEvent += boost::bind(&Sms::answerReceivedEventHandler, this, _1, _2, _3);
	httpRequest.setFactory(new CurlHttpRequestFactory());
	//TODO not implemented inside libutil
	//httpRequest.setProxy("proxy.wengo.fr", 8080, "myProxyUsername", "myProxyPassword");

	_answerReceivedAlready = false;

	Config & config = ConfigManager::getInstance().getCurrentConfig();

	//First parameter: true = HTTPS, false = HTTP
	//Last parameter: true = POST method, false = GET method
	return httpRequest.sendRequest(true, config.getWengoServerHostname(), 443, config.getWengoSMSPath(), data, true);
}

void Sms::answerReceivedEventHandler(int requestId, const std::string & answer, HttpRequest::Error error) {
	if (_answerReceivedAlready) {
		return;
	}

	_answerReceivedAlready = true;

	static const std::string STATUS_UNAUTHORIZED = "401";
	static const std::string STATUS_OK = "200";

	String tmp(answer);

	if (error == HttpRequest::NoError && !tmp.empty()) {
		if (tmp.contains(STATUS_OK) && !tmp.contains(STATUS_UNAUTHORIZED)) {
			LOG_DEBUG("SMS sent");
			smsStatusEvent(*this, requestId, SmsStatusOk);
			return;
		}
	}

	LOG_DEBUG("SMS unsent");
	smsStatusEvent(*this, requestId, SmsStatusError);
}
