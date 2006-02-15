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

#include <http/HttpRequestFactory.h>
#include <http/CurlHttpRequestFactory.h>

#include <StringList.h>
#include <Logger.h>

#include <sstream>
#include <exception>

using namespace std;

const string WengoAccount::_SSOServer = "ws.wengo.fr";
const string WengoAccount::_SSOLoginPath = "/softphone-sso/sso.php";

WengoAccount::WengoAccount(const std::string & login, const std::string & password, bool autoLogin)
	: SipAccount() {
	_wengoLogin = login;
	_wengoPassword = password;
	_autoLogin = autoLogin;
	_answerReceivedAlready = false;
	_ssoRequestOk = false;
	_wengoLoginOk = false;
	_stunServer = "stun.wengo.fr";

	_SSOWithSSL = false;
}

WengoAccount::~WengoAccount() {
}

bool WengoAccount::init() {
	static const unsigned LOGIN_TIMEOUT = 10000;
	static const unsigned LIMIT_RETRY = 5;

	if (!discoverForSSO()) {
		LOG_DEBUG("error while discovering netowork for SSO");
		networkDiscoveryStateEvent(*this, NetworkDiscoveryStateHTTPError);
		return false;
	}

	_timer.timeoutEvent += boost::bind(&WengoAccount::timeoutEventHandler, this);
	_timer.start(0, LOGIN_TIMEOUT, LIMIT_RETRY);
	_timer.join();

	if (!_ssoRequestOk) {
		LOG_DEBUG("error while doing SSO request");
		networkDiscoveryStateEvent(*this, NetworkDiscoveryStateError);
		return false;
	} else if (_ssoRequestOk && !_wengoLoginOk) {
		LOG_DEBUG("SSO request Ok but login/password are invalid");
		loginStateChangedEvent(*this, LoginStatePasswordError);
		return false;
	}

	if (!discoverForSIP()) {
		LOG_DEBUG("error while discovering network for SIP");
		networkDiscoveryStateEvent(*this, NetworkDiscoveryStateSIPError);
		return false;
	}

	LOG_DEBUG("initialization Ok");
	loginStateChangedEvent(*this, LoginStateReady);
	return true;
}

bool WengoAccount::discoverForSSO() {
	string url;

	LOG_DEBUG("discovering network parameters for SSO connection");

	url = _SSOServer + ":" + String::fromNumber(443) + _SSOLoginPath;
	if (_networkDiscovery.testHTTP(url, true)) {
		_SSOWithSSL = true;
		LOG_DEBUG("SSO can connect on port 443 with SSL");
		return true;
	}

	url = _SSOServer + ":" + String::fromNumber(80) + _SSOLoginPath;
	if (_networkDiscovery.testHTTP(url, false)) {
		_SSOWithSSL = false;
		LOG_DEBUG("SSO can connect on port 80 without SSL");
		return true;
	}

	LOG_DEBUG("SSO cannot connect");
	return false;
}

bool WengoAccount::discoverForSIP() {
	LOG_DEBUG("discovering network parameters for SIP connection");

	_localSIPPort = _networkDiscovery.getFreeLocalPort();
	LOG_DEBUG("SIP will use " + String::fromNumber(_localSIPPort) + " as local SIP port");

	if (_networkDiscovery.testUDP(_stunServer)
		&& _networkDiscovery.testSIP(_sipProxyServerHostname, _sipProxyServerPort)) {

		_needsHttpTunnel = false;

		LOG_DEBUG("SIP can connect via UDP");
		return true;
	}

	LOG_DEBUG("cannot connect via UDP");

	if (_networkDiscovery.testSIPHTTPTunnel(_httpTunnelServerHostname, 80, false,
		_sipProxyServerHostname, _sipProxyServerPort)) {

		_needsHttpTunnel = true;
		_httpTunnelServerPort = 80;
		_httpTunnelWithSSL = false;

		LOG_DEBUG("SIP can connect via a tunnel on port 80 without SSL");
		return true;
	}

	if (_networkDiscovery.testSIPHTTPTunnel(_httpTunnelServerHostname, 443, false,
		_sipProxyServerHostname, _sipProxyServerPort)) {

		_needsHttpTunnel = true;
		_httpTunnelServerPort = 443;
		_httpTunnelWithSSL = false;

		LOG_DEBUG("SIP can connect via a tunnel on port 443 without SSL");
		return true;
	}

	if (_networkDiscovery.testSIPHTTPTunnel(_httpTunnelServerHostname, 443, true,
		_sipProxyServerHostname, _sipProxyServerPort)) {

		_needsHttpTunnel = true;
		_httpTunnelServerPort = 443;
		_httpTunnelWithSSL = true;

		LOG_DEBUG("SIP can connect via a tunnel on port 443 with SSL");
		return true;
	}

	LOG_DEBUG("SIP cannot connect");
	return false;
}

void WengoAccount::timeoutEventHandler() {
	//Url::encode(_wengoLogin);
	//Url::encode(_wengoPassword);
	std::string data = "login=" + _wengoLogin + "&password=" + _wengoPassword + "&wl=" + WengoPhoneBuildId::SOFTPHONE_NAME;

	//FIXME if not static it crashes inside boost::thread, do not know why
	static HttpRequest httpRequest;

	httpRequest.answerReceivedEvent += boost::bind(&WengoAccount::answerReceivedEventHandler, this, _1, _2);
	httpRequest.setFactory(new CurlHttpRequestFactory());

	LOG_DEBUG("setting proxy settings for SSO request");
	httpRequest.setProxy(_networkDiscovery.getProxyServer(), _networkDiscovery.getProxyServerPort(),
		 _networkDiscovery.getProxyLogin(), _networkDiscovery.getProxyPassword());

	//First parameter: true = HTTPS, false = HTTP
	//Last parameter: true = POST method, false = GET method
	if (_SSOWithSSL) {
		LOG_DEBUG("sending SSO request with SSL");
		httpRequest.sendRequest(true, _SSOServer, 443, _SSOLoginPath, data, true);
	} else {
		LOG_DEBUG("sending SSO request without SSL");
		httpRequest.sendRequest(false, _SSOServer, 80, _SSOLoginPath, data, true);
	}
}

void WengoAccount::answerReceivedEventHandler(const std::string & answer, HttpRequest::Error error) {
	if (_answerReceivedAlready) {
		return;
	}

	_answerReceivedAlready = true;
	if (error == HttpRequest::NoError && !answer.empty()) {
		LOG_DEBUG("SSO request has been processed successfully");
		_ssoRequestOk = true;
		WengoAccountParser parser(*this, answer);
		if (parser.isLoginPasswordOk()) {
			LOG_DEBUG("login/password is Ok");
			_wengoLoginOk = true;
			_timer.stop();
			//SIP connection test can now be launched as _timer has been joined in init()
		} else {
			LOG_DEBUG("login/password is not Ok");
			_timer.stop();
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
