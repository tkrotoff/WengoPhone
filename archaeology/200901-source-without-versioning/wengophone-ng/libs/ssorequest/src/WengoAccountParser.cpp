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

#include <ssorequest/WengoAccountParser.h>

#include <util/String.h>
#include <util/StringList.h>
#include <util/Logger.h>

#include <tinyxml.h>

#include <cstdlib>
#include <ctime>

#include <iostream>
using namespace std;

static const std::string STATUS_CODE_OK = "200";

WengoAccountParser::WengoAccountParser(WengoAccount & account, const std::string & data) {
	TiXmlDocument doc;
	doc.Parse(data.c_str());

	TiXmlHandle docHandle(& doc);

	TiXmlHandle sso = docHandle.FirstChild("sso");

	//SSO status code
	TiXmlElement * elem = sso.FirstChild("status").Element();
	if (elem) {
		std::string statusCode = elem->Attribute("code");
		LOG_DEBUG("SSO status code=" + statusCode);
		if (statusCode != STATUS_CODE_OK) {
			_loginPasswordOk = false;
			return;
		} else {
			_loginPasswordOk = true;
		}
	}

	//Iterate over "d" element
	elem = sso.FirstChild("d").Element();
	while (elem) {

		std::string key = std::string(elem->Attribute("k"));
		std::string value;
		const char * tmp = elem->Attribute("v");
		if (tmp) {
			value = std::string(tmp);
		}

		if (key == "sip.auth.userid") {
			account.setIdentity(value);
		} else if (key == "sip.auth.password") {
			account.setPassword(value);
		} else if (key == "sip.auth.realm") {
			account.setRealm(value);
		} else if (key == "sip.address.name") {
			account.setUsername(value);
		} else if (key == "sip.address.displayname") {
			account.setDisplayName(value);
		} else if (key == "sip.address.server.host") {
			account.setRegisterServerHostname(value);
		} else if (key == "sip.address.server.port") {
			account.setRegisterServerPort(String(value).toInteger());
		} else if (key == "sip.outbound") {
			account.setUseSIPProxyServer(String(value).toBoolean());
		} else if (key == "sip.outbound.proxy.host") {
			account.setSIPProxyServerHostname(value);
		} else if (key == "sip.outbound.proxy.port") {
			account.setSIPProxyServerPort(String(value).toInteger());
		} else if (key == "netlib.stun.host") {
			account.setStunServer(value);
		} else if (key == "netlib.tunnel.http") {
			StringList httpTunnels;
			TiXmlElement * elt = elem->FirstChildElement("l");
			while (elt) {

				const char * tmp = elt->Attribute("v");
				if (tmp) {
					httpTunnels += std::string(tmp);
				}

				elt = elt->NextSiblingElement("l");
			}
			account.setHttpTunnelServerHostname(chooseHttpTunnel(httpTunnels));

		} else if (key == "netlib.tunnel.https") {
			StringList httpsTunnels;
			TiXmlElement * elt = elem->FirstChildElement("l");
			while (elt) {

				const char * tmp = elt->Attribute("v");
				if (tmp) {
					httpsTunnels += std::string(tmp);
				}

				elt = elt->NextSiblingElement("l");
			}

			account.setHttpsTunnelServerHostname(chooseHttpTunnel(httpsTunnels));
		}

		elem = elem->NextSiblingElement("d");
	}

	account.setSSORequestMade(true);
}

std::string WengoAccountParser::chooseHttpTunnel(const StringList & httpTunnels) {
	srand(time(NULL));
	return httpTunnels[rand() % httpTunnels.size()];
}

bool WengoAccountParser::isLoginPasswordOk() const {
	return _loginPasswordOk;
}
