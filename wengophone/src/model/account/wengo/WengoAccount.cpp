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

#include "WengoAccount.h"

#include "WengoPhoneBuildId.h"
#include "WengoAccountParser.h"
#include "WengoAccountSerializer.h"
#include "model/WengoPhoneLogger.h"

#include <http/HttpRequestFactory.h>
#include <http/CurlHttpRequestFactory.h>
#include <Timer.h>
#include <StringList.h>

#include <sstream>
#include <exception>

static const std::string WENGO_SERVER_HOSTNAME = "ws.wengo.fr";
static const std::string WENGO_LOGIN_PATH = "/softphone-sso/sso.php";

WengoAccount::WengoAccount(const std::string & login, const std::string & password, bool autoLogin) {
	_wengoLogin = login;
	_wengoPassword = password;
	_autoLogin = autoLogin;
	_timer = new Timer();
	_answerReceivedAlready = false;
}

WengoAccount::~WengoAccount() {
	delete _timer;
}

void WengoAccount::init() {
	static const unsigned LOGIN_TIMEOUT = 10000;
	static const unsigned LIMIT_RETRY = 5;

	_timer->timeoutEvent += boost::bind(&WengoAccount::timeoutEventHandler, this);
	_timer->lastTimeoutEvent += boost::bind(&WengoAccount::lastTimeoutEventHandler, this);
	_timer->start(0, LOGIN_TIMEOUT, LIMIT_RETRY);
}

void WengoAccount::timeoutEventHandler() {
	//Url::encode(_wengoLogin);
	//Url::encode(_wengoPassword);
	std::string data = "login=" + _wengoLogin + "&password=" + _wengoPassword + "&wl=" + WengoPhoneBuildId::SOFTPHONE_NAME;

	//FIXME if not static it crashes inside boost::thread, do not know why
	static HttpRequest httpRequest;

	httpRequest.answerReceivedEvent += boost::bind(&WengoAccount::answerReceivedEventHandler, this, _1, _2);
	httpRequest.setFactory(new CurlHttpRequestFactory());
	//TODO not implemented inside libutil
	//httpRequest.setProxy("proxy.wengo.fr", 8080, "myProxyUsername", "myProxyPassword");

	//First parameter: false = HTTP, true = HTTPS
	httpRequest.sendRequest(false, WENGO_SERVER_HOSTNAME, 80, WENGO_LOGIN_PATH, data, true);
}

void WengoAccount::lastTimeoutEventHandler() {
	loginEvent(*this, LoginNetworkError, _wengoLogin, _wengoPassword);
}

void WengoAccount::answerReceivedEventHandler(const std::string & answer, HttpRequest::Error error) {
	if (_answerReceivedAlready) {
		return;
	}

	_answerReceivedAlready = true;
	if (error == HttpRequest::NoError && !answer.empty()) {
		_timer->stop();
		WengoAccountParser parser(*this, answer);
		if (parser.isLoginPasswordOk()) {
			loginEvent(*this, LoginOk, _wengoLogin, _wengoPassword);
		} else {
			loginEvent(*this, LoginPasswordError, _wengoLogin, _wengoPassword);
		}
	}
}

std::string WengoAccount::serialize() {
	const WengoAccountSerializer serializer(*this);
	std::stringstream ss;
	try {
		boost::archive::xml_oarchive oa(ss);
		oa << BOOST_SERIALIZATION_NVP(serializer);
		return ss.str();
	} catch (boost::archive::archive_exception & e) {
		LOG_DEBUG(e.what());
		return String::null;
	}
}

bool WengoAccount::unserialize(const std::string & data) {
	WengoAccountSerializer serializer(*this);
	std::stringstream ss(data);
	try {
		boost::archive::xml_iarchive ia(ss);
		ia >> BOOST_SERIALIZATION_NVP(serializer);
	} catch (boost::archive::archive_exception & e) {
		LOG_DEBUG(e.what());
		return false;
	}
	return true;
}
