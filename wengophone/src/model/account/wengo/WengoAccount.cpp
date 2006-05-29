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

#include "WengoAccount.h"

#include "WengoPhoneBuildId.h"
#include "WengoAccountParser.h"
#include "WengoAccountSerializer.h"

#include <model/account/NetworkObserver.h>
#include <model/config/ConfigManager.h>
#include <model/config/Config.h>

#include <thread/Thread.h>
#include <util/StringList.h>
#include <util/Logger.h>

#include <sstream>
#include <exception>

using namespace std;

WengoAccount::WengoAccount(const std::string & login, const std::string & password, bool autoLogin)
	: SipAccount() {
	_wengoLogin = login;
	_wengoPassword = password;
	_autoLogin = autoLogin;
	_ssoRequestOk = false;
	_wengoLoginOk = false;
	_ssoWithSSL = false;
	_stunServer = "stun.wengo.fr";
	_discoveringNetwork	= false;

	_ssoTimer.timeoutEvent += boost::bind(&WengoAccount::ssoTimeoutEventHandler, this);
	_ssoTimer.lastTimeoutEvent += boost::bind(&WengoAccount::ssoLastTimeoutEventHandler, this);

	_initTimer.timeoutEvent += boost::bind(&WengoAccount::initTimeoutEventHandler, this);
	_initTimer.lastTimeoutEvent += boost::bind(&WengoAccount::initLastTimeoutEventHandler, this);
}

WengoAccount::WengoAccount(const WengoAccount & wengoAccount) {
	SipAccount::copy(wengoAccount);
	_wengoLogin = wengoAccount._wengoLogin;
	_wengoPassword = wengoAccount._wengoPassword;
	_autoLogin = wengoAccount._autoLogin;
	_ssoRequestOk = wengoAccount._ssoRequestOk;
	_wengoLoginOk = wengoAccount._wengoLoginOk;
	_ssoWithSSL = wengoAccount._ssoWithSSL;
	_stunServer = wengoAccount._stunServer;
	_discoveringNetwork	= false;

	_ssoTimer.timeoutEvent += boost::bind(&WengoAccount::ssoTimeoutEventHandler, this);
	_ssoTimer.lastTimeoutEvent += boost::bind(&WengoAccount::ssoLastTimeoutEventHandler, this);

	_initTimer.timeoutEvent += boost::bind(&WengoAccount::initTimeoutEventHandler, this);
	_initTimer.lastTimeoutEvent += boost::bind(&WengoAccount::initLastTimeoutEventHandler, this);
}

WengoAccount::~WengoAccount() {
}

void WengoAccount::init() {

	if (NetworkObserver::getInstance().isConnected())
		_initTimer.start(0, 5000, 5);

	NetworkObserver::getInstance().connectionIsDownEvent += 
		boost::bind(&WengoAccount::connectionIsDownEventHandler, this, _1);

	NetworkObserver::getInstance().connectionIsUpEvent += 
		boost::bind(&WengoAccount::connectionIsUpEventHandler, this, _1);

}

void WengoAccount::initTimeoutEventHandler() {
	if (!_discoveringNetwork) {

		_discoveringNetwork	= true;
		SipAccount::LoginState result = discoverNetwork();
		if (result != LoginStateNetworkError) {
			_initTimer.stop();
			loginStateChangedEvent(*this, result);
			networkDiscoveryStateChangedEvent(*this, _lastNetworkDiscoveryState);
		}
		_discoveringNetwork	= false;
	}
}

void WengoAccount::initLastTimeoutEventHandler() {
	loginStateChangedEvent(*this, LoginStateNetworkError);
	networkDiscoveryStateChangedEvent(*this, _lastNetworkDiscoveryState);
}

void WengoAccount::connectionIsUpEventHandler(NetworkObserver & sender) {
	_initTimer.start(0, 5000, 5);
}

void WengoAccount::connectionIsDownEventHandler(NetworkObserver & sender) {
	_initTimer.stop();
}

SipAccount::LoginState WengoAccount::discoverNetwork() {
	static const unsigned LOGIN_TIMEOUT = 10000;
	static const unsigned LIMIT_RETRY = 5;

	Config & config = ConfigManager::getInstance().getCurrentConfig();
	config.set(Config::PROFILE_LAST_USED_NAME_KEY, _wengoLogin);

	if (!discoverForSSO()) {
		LOG_DEBUG("error while discovering network for SSO");
		_lastNetworkDiscoveryState = NetworkDiscoveryStateHTTPError;
		return LoginStateNetworkError;
	}

	_ssoTimerFinished = false;
	_ssoTimer.start(0, LOGIN_TIMEOUT, LIMIT_RETRY);
	while (!_ssoTimerFinished) {
		Thread::msleep(100);
	}

	if (!_ssoRequestOk) {
		LOG_DEBUG("error while doing SSO request");
		_lastNetworkDiscoveryState = NetworkDiscoveryStateError;
		return LoginStateNetworkError;
	} else if (_ssoRequestOk && !_wengoLoginOk) {
		LOG_DEBUG("SSO request Ok but login/password are invalid");
		return LoginStatePasswordError;
	}

	if (!discoverForSIP()) {
		LOG_DEBUG("error while discovering network for SIP");
		_lastNetworkDiscoveryState = NetworkDiscoveryStateSIPError;
		return LoginStateNetworkError;
	}

	LOG_DEBUG("initialization Ok");
	_lastNetworkDiscoveryState = NetworkDiscoveryStateOk;
	return LoginStateReady;
}

bool WengoAccount::discoverForSSO() {
	LOG_DEBUG("discovering network parameters for SSO connection");

	Config & config = ConfigManager::getInstance().getCurrentConfig();

	string url = config.getWengoServerHostname() + ":" + String::fromNumber(443) + config.getWengoSSOPath();
	if (_networkDiscovery.testHTTP(url, true)) {
		_ssoWithSSL = true;
		LOG_DEBUG("SSO can connect on port 443 with SSL");
		return true;
	}

	url = config.getWengoServerHostname() + ":" + String::fromNumber(80) + config.getWengoSSOPath();
	if (_networkDiscovery.testHTTP(url, false)) {
		_ssoWithSSL = false;
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

void WengoAccount::ssoTimeoutEventHandler() {
	//Url::encode(_wengoLogin);
	//Url::encode(_wengoPassword);
	std::string data = "login=" + _wengoLogin + "&password=" + _wengoPassword + "&wl=" + WengoPhoneBuildId::SOFTPHONE_NAME;

	HttpRequest * httpRequest = new HttpRequest();

	httpRequest->answerReceivedEvent += boost::bind(&WengoAccount::answerReceivedEventHandler, this, _1, _2, _3, _4);

	LOG_DEBUG("setting proxy settings for SSO request");
	httpRequest->setProxy(_networkDiscovery.getProxyServer(), _networkDiscovery.getProxyServerPort(),
			_networkDiscovery.getProxyLogin(), _networkDiscovery.getProxyPassword());

	Config & config = ConfigManager::getInstance().getCurrentConfig();

	//First parameter: true = HTTPS, false = HTTP
	//Last parameter: true = POST method, false = GET method
	if (_ssoWithSSL) {
		LOG_DEBUG("sending SSO request with SSL");
		httpRequest->sendRequest(true, config.getWengoServerHostname(), 443, config.getWengoSSOPath(), data, true);
	} else {
		LOG_DEBUG("sending SSO request without SSL");
		httpRequest->sendRequest(false, config.getWengoServerHostname(), 80, config.getWengoSSOPath(), data, true);
	}
}

void WengoAccount::ssoLastTimeoutEventHandler() {
	_ssoTimerFinished = true;
}

void WengoAccount::answerReceivedEventHandler(IHttpRequest * sender, int requestId, const std::string & answer, HttpRequest::Error error) {
	if (error == HttpRequest::NoError && !answer.empty()) {
		LOG_DEBUG("SSO request has been processed successfully");
		_ssoRequestOk = true;
		WengoAccountParser parser(*this, answer);
		if (parser.isLoginPasswordOk()) {
			LOG_DEBUG("login/password is Ok");
			_wengoLoginOk = true;
			_ssoTimer.stop();
			_ssoTimerFinished = true;
			//SIP connection test can now be launched as _ssoTimer has been joined in init()
		} else {
			LOG_DEBUG("login/password is not Ok");
			_ssoTimer.stop();
			_ssoTimerFinished = true;
		}
	}
	//FIXME (crashes on Linux/MacOS X): delete sender;
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
