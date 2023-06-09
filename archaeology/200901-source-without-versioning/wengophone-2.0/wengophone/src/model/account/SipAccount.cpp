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

SipAccount::SipAccount() 
	: Account(EnumIMProtocol::IMProtocolSIP) {
	_registerServerPort = 0;
	_sipProxyServerPort = 0;
	_httpTunnelServerPort = 0;
	_needsHttpTunnel = false;
	_httpTunnelWithSSL = false;
	_localSIPPort = 0;
	_isConnected = false;
	_autoLogin = false;

	_lastNetworkDiscoveryState = NetworkDiscoveryStateUnknown;
	_lastLoginState = EnumSipLoginState::SipLoginStateUnknown;
}

SipAccount::SipAccount(const SipAccount & sipAccount)
	: Account(sipAccount) {
	copy(sipAccount);
}

SipAccount::~SipAccount() {
}

void SipAccount::copyTo(Account * account) const {
	Account::copyTo(account);

	SipAccount * sipAccount = dynamic_cast<SipAccount *>(account);
	sipAccount->_registerServerHostname = _registerServerHostname;
	sipAccount->_registerServerPort = _registerServerPort;
	sipAccount->_sipProxyServerHostname = _sipProxyServerHostname;
	sipAccount->_sipProxyServerPort = _sipProxyServerPort;
	sipAccount->_httpTunnelServerHostname = _httpTunnelServerHostname;
	sipAccount->_httpTunnelServerPort = _httpTunnelServerPort;
	sipAccount->_needsHttpTunnel = _needsHttpTunnel;
	sipAccount->_httpTunnelWithSSL = _httpTunnelWithSSL;
	sipAccount->_localSIPPort = _localSIPPort;
	sipAccount->_isConnected = _isConnected;
	sipAccount->_autoLogin = _autoLogin;
	sipAccount->_displayName = _displayName;
	sipAccount->_identity = _identity;
	sipAccount->_username = _username;
	sipAccount->_password = _password;
	sipAccount->_realm = _realm;
	sipAccount->_lastNetworkDiscoveryState = _lastNetworkDiscoveryState;
	sipAccount->_lastLoginState = _lastLoginState;
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

void SipAccount::setConnected(bool connected) {
	if (_isConnected != connected) {
		_isConnected = connected;
		if (_isConnected) {
			loginStateChangedEvent(*this, EnumSipLoginState::SipLoginStateConnected);
		} else {
			loginStateChangedEvent(*this, EnumSipLoginState::SipLoginStateDisconnected);
		}
	}
}
