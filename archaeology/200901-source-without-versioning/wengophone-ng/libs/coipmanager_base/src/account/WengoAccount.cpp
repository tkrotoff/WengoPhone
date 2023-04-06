/*
 * WengoPhone, a voice over Internet phone
 * Copyright (C) 2004-2007  Wengo
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

#include <coipmanager_base/account/WengoAccount.h>

#include <util/Logger.h>
#include <util/String.h>

WengoAccount::WengoAccount()
	: SipAccount() {

	_accountType = EnumAccountType::AccountTypeWengo;
	_httpTunnelSupported = true;
	_ssoRequestMade = false;
}

WengoAccount::WengoAccount(const std::string & wengoLogin, const std::string & wengoPassword)
	: SipAccount(String::null, String::null) {

	_accountType = EnumAccountType::AccountTypeWengo;
	_wengoLogin = wengoLogin;
	_wengoPassword = wengoPassword;
	_httpTunnelSupported = true;
	_ssoRequestMade = false;
}

WengoAccount::WengoAccount(const WengoAccount & wengoAccount)
	: SipAccount(wengoAccount) {
	copy(wengoAccount);
}

WengoAccount::WengoAccount(const IAccount * iAccount) {
	const WengoAccount * account = dynamic_cast<const WengoAccount *>(iAccount);
	if (account) {
		copy(*account);
	} else {
		LOG_FATAL("given IAccount is not an WengoAccount");
	}
}

WengoAccount * WengoAccount::clone() const {
	return new WengoAccount(*this);
}

void WengoAccount::copy(const WengoAccount & wengoAccount) {
	SipAccount::copy(wengoAccount);

	_wengoLogin = wengoAccount._wengoLogin;
	_wengoPassword = wengoAccount._wengoPassword;
	_ssoRequestMade = wengoAccount._ssoRequestMade;
	_stunServer = wengoAccount._stunServer;
}

void WengoAccount::reset() {
	_connectionState = EnumConnectionState::ConnectionStateDisconnected;
	_connectionConfigurationSet = false;
	_displayName = String::null;
	_httpTunnelServerHostname = String::null;
	_httpTunnelServerPort = 0;
	_httpTunnelSupported = false;
	_httpTunnelWithSSL = false;
	_identity = String::null;
	_lineId = 0;
	_natType = EnumNatType::NatTypeUnknown;	
	_needsHttpTunnel = false;
	_password = String::null;
	_presenceState = EnumPresenceState::PresenceStateUnknown;
	_realm = String::null;
	_registerServerHostname = String::null;
	_registerServerPort = 0;
	_sipProxyServerHostname = String::null;
	_sipProxyServerPort = 0;
	_ssoRequestMade = false;
	_stunServer = String::null;
	_username = String::null;
}

WengoAccount::~WengoAccount() {
}

void WengoAccount::setWengoLogin(const std::string & wengoLogin) {
	// Changing login/password makes data in SipAccount not valid anymore. They
	// have to be reset. reset() does not affect data in WengoAccount only.
	if (_wengoLogin != wengoLogin) {
		reset();
		_wengoLogin = wengoLogin;
		setPeerId(wengoLogin);
	}
}

const std::string & WengoAccount::getWengoLogin() const {
	return _wengoLogin;
}

void WengoAccount::setWengoPassword(const std::string & wengoPassword) {
	// Changing login/password makes data in SipAccount not valid anymore. They
	// have to be reset. reset() does not affect data in WengoAccount only.
	if (_wengoPassword != wengoPassword) {
		reset();
		_wengoPassword = wengoPassword;
	}
}

const std::string & WengoAccount::getWengoPassword() const {
	return _wengoPassword;
}

std::string WengoAccount::getLogin() const {
	return getWengoLogin();
}

void WengoAccount::setLogin(const std::string & login) {
	setWengoLogin(login);
}

void WengoAccount::setSSORequestMade(bool made) {
	_ssoRequestMade = made;
}

bool WengoAccount::isSSORequestMade() const {
	return _ssoRequestMade;
}

std::string WengoAccount::getWengoServerHostname() {
	return "ws.wengo.fr";
}

std::string WengoAccount::getWengoSMSServicePath() {
	return "/sms/sendsms.php";
}

std::string WengoAccount::getWengoSSOServicePath() {
	return "/softphone-sso/sso2.php";
}

void WengoAccount::setStunServer(const std::string & server) {
	_stunServer = server;
}

const std::string & WengoAccount::getStunServer() const {
	return _stunServer;
}
