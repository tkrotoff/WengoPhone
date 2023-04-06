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

#include <coipmanager_base/serializer/SipAccountXMLSerializer.h>

#include <coipmanager_base/serializer/SipPeerXMLSerializer.h>

#include <coipmanager_base/account/SipAccount.h>

#include <util/String.h>

#include <tinyxml.h>

SipAccountXMLSerializer::SipAccountXMLSerializer(SipAccount & account) 
	: IAccountXMLSerializer(account) {
}

std::string SipAccountXMLSerializer::serialize() {
	SipAccount & myAccount = static_cast<SipAccount &>(_account);

	SipPeerXMLSerializer sipPeerXMLSerializer(myAccount);
	std::string result = sipPeerXMLSerializer.serialize();
	
	result += "<register.server>" + myAccount.getRegisterServerHostname() + "</register.server>\n";
	result += "<register.serverport>" + String::fromNumber(myAccount.getRegisterServerPort()) + "</register.serverport>\n";
	result += "<sipproxy.server>" + myAccount.getSIPProxyServerHostname() + "</sipproxy.server>\n";
	result += "<sipproxy.serverport>" + String::fromNumber(myAccount.getSIPProxyServerPort()) + "</sipproxy.serverport>\n";
	result += "<httptunnel.needed>" + String::fromBoolean(myAccount.isHttpTunnelNeeded()) + "</httptunnel.needed>\n";
	result += "<httptunnel.server>" + myAccount.getHttpTunnelServerHostname() + "</httptunnel.server>\n";
	result += "<httptunnel.sslserver>" + myAccount.getHttpsTunnelServerHostname() + "</httptunnel.sslserver>\n";
	result += "<httptunnel.serverport>" + String::fromNumber(myAccount.getHttpTunnelServerPort()) + "</httptunnel.serverport>\n";
	result += "<httptunnel.useSSL>" + String::fromBoolean(myAccount.useSSLForHttpTunnel()) + "</httptunnel.useSSL>\n";
	result += "<nattype>" + EnumNatType::toString(myAccount.getNatType()) + "</nattype>\n";

	return result;
}

bool SipAccountXMLSerializer::unserialize(TiXmlHandle & rootElt) {
	SipAccount & myAccount = static_cast<SipAccount &>(_account);

	SipPeerXMLSerializer sipPeerXMLSerializer(myAccount);
	sipPeerXMLSerializer.unserializeContent(rootElt);

	// Retrieving register server
	TiXmlNode * registerServer = rootElt.FirstChild("register.server").FirstChild().Node();
	if (registerServer) {
		myAccount.setRegisterServerHostname(registerServer->Value());
	}
	////

	// Retrieving register server port
	TiXmlNode * registerServerPort = rootElt.FirstChild("register.serverport").FirstChild().Node();
	if (registerServerPort) {
		String sRegisterServerPort = registerServerPort->Value();
		myAccount.setRegisterServerPort(sRegisterServerPort.toInteger());
	}
	////

	// Retrieving sip proxy server
	TiXmlNode * sipProxyServer = rootElt.FirstChild("sipproxy.server").FirstChild().Node();
	if (sipProxyServer) {
		myAccount.setSIPProxyServerHostname(sipProxyServer->Value());
	}
	////

	// Retrieving sip proxy server port
	TiXmlNode * sipProxyServerPort = rootElt.FirstChild("sipproxy.serverport").FirstChild().Node();
	if (sipProxyServerPort) {
		String sSipProxyServerPort = sipProxyServerPort->Value();
		myAccount.setSIPProxyServerPort(sSipProxyServerPort.toInteger());
	}
	////

	// Retrieving http tunnel server
	TiXmlNode * httpTunnelServer = rootElt.FirstChild("httptunnel.server").FirstChild().Node();
	if (httpTunnelServer) {
		myAccount.setHttpTunnelServerHostname(httpTunnelServer->Value());
	}
	////

	// Retrieving http tunnel server
	TiXmlNode * httpsTunnelServer = rootElt.FirstChild("httptunnel.sslserver").FirstChild().Node();
	if (httpsTunnelServer) {
		myAccount.setHttpsTunnelServerHostname(httpsTunnelServer->Value());
	}
	////

	// Retrieving http tunnel server port
	TiXmlNode * httpTunnelServerPort = rootElt.FirstChild("httptunnel.serverport").FirstChild().Node();
	if (httpTunnelServerPort) {
		String sHttpTunnelServerPort = httpTunnelServerPort->Value();
		myAccount.setHttpTunnelServerPort(sHttpTunnelServerPort.toInteger());
	}
	////

	// Retrieving http tunnel needed
	TiXmlNode * httpTunnelNeeded = rootElt.FirstChild("httptunnel.needed").FirstChild().Node();
	if (httpTunnelNeeded) {
		String sHttpTunnelNeeded = httpTunnelNeeded->Value();
		myAccount.setHttpTunnelNeeded(sHttpTunnelNeeded.toBoolean());
	}
	////

	// Retrieving http tunnel has SSL
	TiXmlNode * httpTunnelUseSSL = rootElt.FirstChild("httptunnel.useSSL").FirstChild().Node();
	if (httpTunnelUseSSL) {
		String sHttpTunnelUseSSL = httpTunnelUseSSL->Value();
		myAccount.setUseSSLForHttpTunnel(sHttpTunnelUseSSL.toBoolean());
	}
	////

	// Retrieving Nat type
	TiXmlNode * natType = rootElt.FirstChild("nattype").FirstChild().Node();
	if (natType) {
		myAccount.setNatType(EnumNatType::toNatType(natType->Value()));
	}
	////

	return true;
}
