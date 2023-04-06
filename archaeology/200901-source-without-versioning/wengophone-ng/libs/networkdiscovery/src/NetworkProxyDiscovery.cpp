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

#include <networkdiscovery/NetworkProxyDiscovery.h>

#include <util/Logger.h>

NetworkProxy::NetworkProxy() {
	_proxyAuthType = EnumProxyAuthType::ProxyAuthTypeUnknown;
	_serverPort = 0;
}

NetworkProxy::NetworkProxy(const NetworkProxy & networkProxy) {
	copy(networkProxy);
}

NetworkProxy & NetworkProxy::operator = (const NetworkProxy & networkProxy) {
	copy(networkProxy);
	return *this;
}

void NetworkProxy::copy(const NetworkProxy & networkProxy) {
	_login = networkProxy._login;
	_password = networkProxy._password;
	_server = networkProxy._server;
	_serverPort = networkProxy._serverPort;
	_proxyAuthType = networkProxy._proxyAuthType;
}

std::string NetworkProxy::getLogin() const {
	return _login;
}

void NetworkProxy::setLogin(const std::string & login) {
	_login = login;
}

std::string NetworkProxy::getPassword() const {
	return _password;
}

void NetworkProxy::setPassword(const std::string & password) {
	_password = password;
}

std::string NetworkProxy::getServer() const {
	return _server;
}

void NetworkProxy::setServer(const std::string & server) {
	_server = server;
}

int NetworkProxy::getServerPort() const {
	return _serverPort;
}

void NetworkProxy::setServerPort(int port) {
	_serverPort = port;
}

EnumProxyAuthType::ProxyAuthType NetworkProxy::getProxyAuthType() const {
	return _proxyAuthType;
}

void NetworkProxy::setProxyAuthType(EnumProxyAuthType::ProxyAuthType proxyAuthType) {
	_proxyAuthType = proxyAuthType;
}

static const unsigned PROXY_TIMEOUT = 10;

NetworkProxyDiscovery::NetworkProxyDiscovery() {
}

NetworkProxyDiscovery::~NetworkProxyDiscovery() {
}

NetworkProxyDiscovery::NetworkProxyDiscoveryError NetworkProxyDiscovery::getNetworkProxy(NetworkProxy & networkProxy) const {
	NetworkProxyDiscoveryError result = NetworkProxyDiscoveryErrorCannotBeDetected;

	// Trying to ping an http server. We do this because sometimes
	// a system can be set to use a proxy whereas it could connect directly.
	if (is_http_conn_allowed("www.google.com",
		NULL, 0, NULL, NULL,
		NETLIB_FALSE, 10) == HTTP_OK) {

		LOG_DEBUG("can connect without proxy");

		networkProxy.setServer("");
		networkProxy.setServerPort(0);
		networkProxy.setLogin("");
		networkProxy.setPassword("");

		return NetworkProxyDiscoveryErrorDetected;
	}

	// Detecting proxy
	char * localProxyUrl = get_local_http_proxy_address();
	int localProxyPort = get_local_http_proxy_port();

	networkProxy.setServer(localProxyUrl ? std::string(localProxyUrl) : std::string(""));
	networkProxy.setServerPort(localProxyPort);

	result = NetworkProxyDiscoveryErrorDetected;
	////

	if (!localProxyUrl) {
		LOG_DEBUG("discovery made. No proxy found.");
		networkProxy.setLogin("");
		networkProxy.setPassword("");
		return result;
	}

	LOG_DEBUG("proxy found. Testing if authentication is needed.");
	if (is_proxy_auth_needed(networkProxy.getServer().c_str(),
		networkProxy.getServerPort(), PROXY_TIMEOUT)) {

		LOG_DEBUG("proxy authentication needed");

		// Getting EnumAuthType
		EnumAuthType proxyAuthType = get_proxy_auth_type(networkProxy.getServer().c_str(),
			networkProxy.getServerPort(), PROXY_TIMEOUT);
		networkProxy.setProxyAuthType(EnumProxyAuthType::toProxyAuthType(proxyAuthType));
		////

		// Testing login/password
		if (!is_proxy_auth_ok(networkProxy.getServer().c_str(), networkProxy.getServerPort(),
			networkProxy.getLogin().c_str(), networkProxy.getPassword().c_str(), PROXY_TIMEOUT)) {

			LOG_DEBUG("proxy needs valid login/password");

			result = NetworkProxyDiscoveryErrorNeedsAuthentication;
		}
		////
	}

	return result;
}
