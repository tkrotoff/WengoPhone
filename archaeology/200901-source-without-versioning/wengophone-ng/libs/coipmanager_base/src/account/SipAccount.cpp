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

#include <coipmanager_base/account/SipAccount.h>

#include <util/Logger.h>
#include <util/String.h>

SipAccount::SipAccount()
	: IAccount(String::null, String::null, EnumAccountType::AccountTypeSIP),
	SipPeer() {
	_registerServerPort = 0;
	_sipProxyServerPort = 0;
	_httpTunnelServerPort = 0;
	_needsHttpTunnel = false;
	_httpTunnelWithSSL = false;
	_lineId = 0;
	_httpTunnelSupported = false;
	_natType = EnumNatType::NatTypeUnknown;
}

SipAccount::SipAccount(const std::string & identity, const std::string & password)
	: IAccount(identity, password, EnumAccountType::AccountTypeSIP),
	SipPeer() {

	_identity = identity;
	_registerServerPort = 0;
	_sipProxyServerPort = 0;
	_httpTunnelServerPort = 0;
	_needsHttpTunnel = false;
	_httpTunnelWithSSL = false;
	_lineId = 0;
	_httpTunnelSupported = false;
	_natType = EnumNatType::NatTypeUnknown;
}

SipAccount::SipAccount(const SipAccount & sipAccount)
	: IAccount(sipAccount),
	SipPeer(sipAccount) {
	copy(sipAccount);
}

SipAccount::SipAccount(const IAccount * iAccount) {
	const SipAccount * account = dynamic_cast<const SipAccount *>(iAccount);
	if (account) {
		copy(*account);
	} else {
		LOG_FATAL("given IAccount is not an SipAccount");
	}
}

SipAccount * SipAccount::clone() const {
	return new SipAccount(*this);
}

SipAccount::~SipAccount() {
}

void SipAccount::copy(const SipAccount & sipAccount) {
	IAccount::copy(sipAccount);
	SipPeer::copy(sipAccount);

	_registerServerHostname = sipAccount._registerServerHostname;
	_registerServerPort = sipAccount._registerServerPort;
	_sipProxyServerHostname = sipAccount._sipProxyServerHostname;
	_sipProxyServerPort = sipAccount._sipProxyServerPort;
	_httpTunnelServerHostname = sipAccount._httpTunnelServerHostname;
	_httpTunnelServerPort = sipAccount._httpTunnelServerPort;
	_needsHttpTunnel = sipAccount._needsHttpTunnel;
	_httpTunnelWithSSL = sipAccount._httpTunnelWithSSL;
	_lineId = sipAccount._lineId;
	_httpTunnelSupported = sipAccount._httpTunnelSupported;
	_natType = sipAccount._natType;
}

void SipAccount::setRegisterServerHostname(const std::string & server) {
	_registerServerHostname = server;
}

const std::string & SipAccount::getRegisterServerHostname() const {
	return _registerServerHostname;
}

void SipAccount::setRegisterServerPort(unsigned port) {
	_registerServerPort = port;
}

unsigned SipAccount::getRegisterServerPort() const {
	return _registerServerPort;
}

void SipAccount::setUseSIPProxyServer(bool useSipProxy) {
	_useSipProxyServer = useSipProxy;
}

bool SipAccount::useSIPProxyServer() const {
	return _useSipProxyServer;
}

void SipAccount::setSIPProxyServerHostname(const std::string & server) {
	_sipProxyServerHostname = server;
}

const std::string & SipAccount::getSIPProxyServerHostname() const {
	return _sipProxyServerHostname;
}

void SipAccount::setSIPProxyServerPort(unsigned port) {
	_sipProxyServerPort = port;
}

unsigned SipAccount::getSIPProxyServerPort() const {
	return _sipProxyServerPort;
}

void SipAccount::setHttpTunnelSupported(bool supported) {
	_httpTunnelSupported = supported;
}

bool SipAccount::isHttpTunnelSupported() const {
	return _httpTunnelSupported;
}

void SipAccount::setHttpTunnelServerHostname(const std::string & server) {
	_httpTunnelServerHostname = server;
}

const std::string & SipAccount::getHttpTunnelServerHostname() const {
	return _httpTunnelServerHostname;
}

void SipAccount::setHttpsTunnelServerHostname(const std::string & server) {
	_httpsTunnelServerHostname = server;
}

const std::string & SipAccount::getHttpsTunnelServerHostname() const {
	return _httpsTunnelServerHostname;
}

void SipAccount::setHttpTunnelServerPort(unsigned port) {
	_httpTunnelServerPort = port;
}

unsigned SipAccount::getHttpTunnelServerPort() const {
	return _httpTunnelServerPort;
}

void SipAccount::setUseSSLForHttpTunnel(bool useSSL) {
	_httpTunnelWithSSL = useSSL;
}

bool SipAccount::useSSLForHttpTunnel() const {
	return _httpTunnelWithSSL;
}

void SipAccount::setHttpTunnelNeeded(bool needed) {
	_needsHttpTunnel = needed;
}

bool SipAccount::isHttpTunnelNeeded() const {
	return _needsHttpTunnel;
}

void SipAccount::setVirtualLineId(int lineId) {
	_lineId = lineId;
}

int SipAccount::getVirtualLineId() const {
	return _lineId;
}

void SipAccount::setNatType(EnumNatType::NatType natType) {
	_natType = natType;
}

EnumNatType::NatType SipAccount::getNatType() const {
	return _natType;
}

std::string SipAccount::getPeerId() const {
	return getFullIdentity();
}

