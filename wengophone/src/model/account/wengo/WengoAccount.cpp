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

#include <model/config/ConfigManager.h>
#include <model/config/Config.h>
#include <model/network/NetworkObserver.h>
#include <model/network/NetworkProxyDiscovery.h>

#include <thread/Thread.h>
#include <util/StringList.h>
#include <util/Logger.h>
#include <http/HttpRequest.h>

#include <sstream>
#include <exception>

using namespace std;

WengoAccount WengoAccount::empty;
unsigned short WengoAccount::_testSIPRetry = 3;
unsigned short WengoAccount::_testStunRetry = 2;

WengoAccount::WengoAccount()
	: SipAccount(),
	WengoWebService(this) {

	_protocol = EnumIMProtocol::IMProtocolWengo;
	_ssoRequestOk = false;
	_wengoLoginOk = false;
	_ssoWithSSL = false;
	_isValid = false;
	_discoveringNetwork = false;
	_needsHttpTunnel = false;
	_stunServer = "stun.wengo.fr";
	_lastNetworkDiscoveryState = NetworkDiscoveryStateError;

	_ssoTimer.timeoutEvent += boost::bind(&WengoAccount::ssoTimeoutEventHandler, this);
	_ssoTimer.lastTimeoutEvent += boost::bind(&WengoAccount::ssoLastTimeoutEventHandler, this);

	_initTimer.timeoutEvent += boost::bind(&WengoAccount::initTimeoutEventHandler, this);
	_initTimer.lastTimeoutEvent += boost::bind(&WengoAccount::initLastTimeoutEventHandler, this);
}

WengoAccount::WengoAccount(const std::string & login, const std::string & password, bool autoLogin)
	: SipAccount(),
	WengoWebService(this) {

	_protocol = EnumIMProtocol::IMProtocolWengo;
	_wengoLogin = login;
	_wengoPassword = password;
	_autoLogin = autoLogin;
	_ssoRequestOk = false;
	_wengoLoginOk = false;
	_ssoWithSSL = false;
	_needsHttpTunnel = false;
	_stunServer = "stun.wengo.fr";
	_discoveringNetwork = false;
	_lastNetworkDiscoveryState = NetworkDiscoveryStateError;

	_ssoTimer.timeoutEvent += boost::bind(&WengoAccount::ssoTimeoutEventHandler, this);
	_ssoTimer.lastTimeoutEvent += boost::bind(&WengoAccount::ssoLastTimeoutEventHandler, this);

	_initTimer.timeoutEvent += boost::bind(&WengoAccount::initTimeoutEventHandler, this);
	_initTimer.lastTimeoutEvent += boost::bind(&WengoAccount::initLastTimeoutEventHandler, this);
}

WengoAccount::WengoAccount(const WengoAccount & wengoAccount) : WengoWebService(this) {
	copy(wengoAccount);
}

WengoAccount & WengoAccount::operator = (const WengoAccount & wengoAccount) {
	copy(wengoAccount);

	return *this;
}

Account * WengoAccount::createCopy() const {
	Account * result = new WengoAccount();

	copyTo(result);

	return result;
}

void WengoAccount::copyTo(Account * account) const {
	SipAccount::copyTo(account);

	WengoAccount * wengoAccount = dynamic_cast<WengoAccount *>(account);
	wengoAccount->_wengoLogin = _wengoLogin;
	wengoAccount->_wengoPassword = _wengoPassword;
	wengoAccount->_autoLogin = _autoLogin;
	wengoAccount->_ssoRequestOk = _ssoRequestOk;
	wengoAccount->_wengoLoginOk = _wengoLoginOk;
	wengoAccount->_ssoWithSSL = _ssoWithSSL;
	wengoAccount->_stunServer = _stunServer;
	wengoAccount->_lastNetworkDiscoveryState = _lastNetworkDiscoveryState;
}

void WengoAccount::copy(const WengoAccount & wengoAccount) {
	SipAccount::copy(wengoAccount);
	_wengoLogin = wengoAccount._wengoLogin;
	_wengoPassword = wengoAccount._wengoPassword;
	_autoLogin = wengoAccount._autoLogin;
	_ssoRequestOk = wengoAccount._ssoRequestOk;
	_wengoLoginOk = wengoAccount._wengoLoginOk;
	_ssoWithSSL = wengoAccount._ssoWithSSL;
	_stunServer = wengoAccount._stunServer;
	_discoveringNetwork = false;
	_lastNetworkDiscoveryState = wengoAccount._lastNetworkDiscoveryState;

	_ssoTimer.timeoutEvent += boost::bind(&WengoAccount::ssoTimeoutEventHandler, this);
	_ssoTimer.lastTimeoutEvent += boost::bind(&WengoAccount::ssoLastTimeoutEventHandler, this);

	_initTimer.timeoutEvent += boost::bind(&WengoAccount::initTimeoutEventHandler, this);
	_initTimer.lastTimeoutEvent += boost::bind(&WengoAccount::initLastTimeoutEventHandler, this);
}

WengoAccount::~WengoAccount() {
	_ssoTimer.stop();
	_initTimer.stop();
}

void WengoAccount::init() {

	if (NetworkObserver::getInstance().isConnected()) {
		_initTimer.start(0, 5000, 5);
	}

	NetworkObserver::getInstance().connectionIsDownEvent +=
		boost::bind(&WengoAccount::connectionIsDownEventHandler, this, _1);

	NetworkObserver::getInstance().connectionIsUpEvent +=
		boost::bind(&WengoAccount::connectionIsUpEventHandler, this, _1);

}

void WengoAccount::initTimeoutEventHandler() {
	if (!_discoveringNetwork) {

		_discoveringNetwork = true;
		_lastLoginState = discoverNetwork();
		if (_lastLoginState != EnumSipLoginState::SipLoginStateNetworkError) {
			_initTimer.stop();
			loginStateChangedEvent(*this, _lastLoginState);
			networkDiscoveryStateChangedEvent(*this, _lastNetworkDiscoveryState);

			NetworkObserver::getInstance().connectionIsDownEvent -=
				boost::bind(&WengoAccount::connectionIsDownEventHandler, this, _1);

			NetworkObserver::getInstance().connectionIsUpEvent -=
				boost::bind(&WengoAccount::connectionIsUpEventHandler, this, _1);
		}
		_discoveringNetwork = false;
	}
}

void WengoAccount::initLastTimeoutEventHandler() {
	loginStateChangedEvent(*this, EnumSipLoginState::SipLoginStateNetworkError);
	networkDiscoveryStateChangedEvent(*this, _lastNetworkDiscoveryState);
}

void WengoAccount::connectionIsUpEventHandler(NetworkObserver & sender) {
	_initTimer.start(0, 5000, 5);
}

void WengoAccount::connectionIsDownEventHandler(NetworkObserver & sender) {
	_initTimer.stop();
}

EnumSipLoginState::SipLoginState WengoAccount::discoverNetwork() {
	static const unsigned LOGIN_TIMEOUT = 10000;
	static const unsigned LIMIT_RETRY = 5;

	Config & config = ConfigManager::getInstance().getCurrentConfig();
	config.set(Config::PROFILE_LAST_USED_NAME_KEY, _wengoLogin);

	if (!discoverForSSO()) {
		LOG_DEBUG("error while discovering network for SSO");
		_lastNetworkDiscoveryState = NetworkDiscoveryStateHTTPError;
		return EnumSipLoginState::SipLoginStateNetworkError;
	}

	_ssoTimerFinished = false;
	_ssoTimer.start(0, LOGIN_TIMEOUT, LIMIT_RETRY);
	while (!_ssoTimerFinished) {
		Thread::msleep(100);
	}

	if (!_ssoRequestOk) {
		LOG_DEBUG("error while doing SSO request");
		_lastNetworkDiscoveryState = NetworkDiscoveryStateError;
		return EnumSipLoginState::SipLoginStateNetworkError;
	} else if (_ssoRequestOk && !_wengoLoginOk) {
		LOG_DEBUG("SSO request Ok but login/password are invalid");
		return EnumSipLoginState::SipLoginStatePasswordError;
	}

	_isValid = true;

	if (!discoverForSIP()) {
		LOG_DEBUG("error while discovering network for SIP");
		_lastNetworkDiscoveryState = NetworkDiscoveryStateSIPError;
		return EnumSipLoginState::SipLoginStateNetworkError;
	}

	LOG_DEBUG("initialization Ok");
	_lastNetworkDiscoveryState = NetworkDiscoveryStateOk;
	return EnumSipLoginState::SipLoginStateReady;
}

bool WengoAccount::discoverForSSO() {
	//
	// Please contact network@openwengo.com before any modifications.
	//

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
	//
	// Please contact network@openwengo.com before any modifications.
	//

	LOG_DEBUG("discovering network parameters for SIP connection");

	Config & config = ConfigManager::getInstance().getCurrentConfig();

	_localSIPPort = _networkDiscovery.getFreeLocalPort();
	
	if (!config.getNetWorkTunnelNeeded()) {
		LOG_DEBUG("SIP will use " + String::fromNumber(_localSIPPort) + " as local SIP port");

		// Stun test
		unsigned short iTestStun;
		for (iTestStun = 0; iTestStun < _testStunRetry; iTestStun++) {
			LOG_DEBUG("testUDP (Stun): " + String::fromNumber(iTestStun + 1));
			if (_networkDiscovery.testUDP(_stunServer)) {
				break;
			}
		}

		if (iTestStun == _testStunRetry) {
			// Stun test failed
			_networkDiscovery.setNatConfig(EnumNatType::NatTypeFullCone);
		}
		////

		// SIP test with UDP
		for (unsigned short i = 0; i < _testSIPRetry; i++) {
			LOG_DEBUG("testSIP test number: " + String::fromNumber(i + 1));

			if (_networkDiscovery.testSIP(_sipProxyServerHostname, _sipProxyServerPort, _localSIPPort)) {
				LOG_DEBUG("SIP can connect via UDP");
				_needsHttpTunnel = false;
				return true;
			}
		}
		////
	}

	LOG_DEBUG("cannot connect via UDP");

	// SIP test with Http
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
	////

	LOG_DEBUG("SIP cannot connect");
	return false;
}

void WengoAccount::ssoTimeoutEventHandler() {
	Config & config = ConfigManager::getInstance().getCurrentConfig();
	setHostname(config.getWengoServerHostname());
	setGet(false);
	setHttps(_ssoWithSSL);
	setServicePath(config.getWengoSSOPath());
	setWengoAuthentication(true);

	LOG_DEBUG("setting proxy settings for SSO request");
	NetworkProxy networkProxy = NetworkProxyDiscovery::getInstance().getNetworkProxy();

	HttpRequest::setProxy(networkProxy.getServer(), networkProxy.getServerPort(),
		networkProxy.getLogin(), networkProxy.getPassword());

	if (_ssoWithSSL) {
		setPort(443);
		LOG_DEBUG("sending SSO request with SSL");
	} else {
		setPort(80);
		LOG_DEBUG("sending SSO request without SSL");
	}

	call(this);
}

void WengoAccount::ssoLastTimeoutEventHandler() {
	_ssoTimerFinished = true;
}

void WengoAccount::answerReceived(const std::string & answer, int requestId) {
	if (!answer.empty()) {
		LOG_DEBUG("SSO request has been processed successfully");
		_ssoRequestOk = true;
		WengoAccountParser parser(*this, answer);
		if (parser.isLoginPasswordOk()) {
			LOG_DEBUG("login/password Ok");
			_wengoLoginOk = true;
			_ssoTimer.stop();
			_ssoTimerFinished = true;
			//SIP connection test can now be launched as _ssoTimer has been joined in init()
		} else {
			LOG_DEBUG("login/password not Ok");
			_wengoLoginOk = false;
			_ssoTimer.stop();
			_ssoTimerFinished = true;
		}
	}
}
