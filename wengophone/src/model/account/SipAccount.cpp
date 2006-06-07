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

#include "SipAccount.h"

SipAccount::SipAccount() {
	_registerServerPort = 0;
	_sipProxyServerPort = 0;
	_httpTunnelServerPort = 0;
	_needsHttpTunnel = false;
	_httpTunnelWithSSL = false;
	_localSIPPort = 0;
	_isConnected = false;
	_autoLogin = false;

	_lastNetworkDiscoveryState = NetworkDiscoveryStateUnknown;
	_lastLoginState = LoginStateUnknown;

	_networkDiscovery.proxyNeedsAuthenticationEvent +=
		boost::bind(&SipAccount::proxyNeedsAuthenticationEventHandler, this, _1, _2, _3);

	_networkDiscovery.wrongProxyAuthenticationEvent +=
		boost::bind(&SipAccount::wrongProxyAuthenticationEventHandler, this, _1, _2, _3, _4, _5);
}

SipAccount::SipAccount(const SipAccount & sipAccount) {
	copy(sipAccount);
}

SipAccount::~SipAccount() {
}

void SipAccount::copy(const SipAccount & sipAccount) {
	_registerServerHostname = sipAccount._registerServerHostname;
	_registerServerPort = sipAccount._registerServerPort;
	_sipProxyServerHostname = sipAccount._sipProxyServerHostname;
	_sipProxyServerPort = sipAccount._sipProxyServerPort;
	_httpTunnelServerHostname = sipAccount._httpTunnelServerHostname;
	_httpTunnelServerPort = sipAccount._httpTunnelServerPort;
	_needsHttpTunnel = sipAccount._needsHttpTunnel;
	_httpTunnelWithSSL = sipAccount._httpTunnelWithSSL;
	_localSIPPort = sipAccount._localSIPPort;
	_isConnected = sipAccount._isConnected;
	_autoLogin = sipAccount._autoLogin;
	_displayName = sipAccount._displayName;
	_identity = sipAccount._identity;
	_username = sipAccount._username;
	_password = sipAccount._password;
	_realm = sipAccount._realm;
	_lastNetworkDiscoveryState = sipAccount._lastNetworkDiscoveryState;
	_lastLoginState = sipAccount._lastLoginState;
}

void SipAccount::setProxySettings(const std::string & proxyAddress, unsigned proxyPort,
	const std::string & proxyLogin, const std::string & proxyPassword) {

	_networkDiscovery.setProxySettings(proxyAddress, proxyPort, proxyLogin, proxyPassword);
}

void SipAccount::proxyNeedsAuthenticationEventHandler(NetworkDiscovery & sender, const std::string & proxyUrl, int proxyPort) {
	proxyNeedsAuthenticationEvent(*this, proxyUrl, proxyPort);
}

void SipAccount::wrongProxyAuthenticationEventHandler(NetworkDiscovery & sender,
	const std::string & proxyUrl, int proxyPort,
	const std::string & proxyLogin, const std::string & proxyPassword) {

	wrongProxyAuthenticationEvent(*this, proxyUrl, proxyPort, proxyLogin, proxyPassword);
}

void SipAccount::setConnected(bool connected) {
	_isConnected = connected;
	if (_isConnected) {
		loginStateChangedEvent(*this, LoginStateConnected);
	} else {
		loginStateChangedEvent(*this, LoginStateDisconnected);
	}
}
