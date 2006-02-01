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

#include "WengoAccountParser.h"

#include "model/WengoPhoneLogger.h"

#include <StringList.h>

#include <tinyxml.h>

#include <iostream>
using namespace std;

static const std::string STATUS_CODE_OK = "200";

WengoAccountParser::WengoAccountParser(WengoAccount & account, const std::string & data) {
	/*_identity = "robobob1534";
	_username = "robobob1534";
	_realm = "voip.wengo.fr";
	_displayName = "robobob1534";
	_registerServerHostname = "voip.wengo.fr";
	_registerServerPort = 5060;
	_proxyServerHostname = "proxy1.host.wengo.fr";
	_proxyServerPort = 5060;
	_password = "8j1hvrekmxzr94lh";*/

	TiXmlDocument doc;
	doc.Parse(data.c_str());

	TiXmlHandle docHandle(& doc);

	TiXmlHandle sso = docHandle.FirstChild("sso");

	TiXmlElement * elem = sso.FirstChild("status").Element();
	if (elem) {
		std::string statusCode = elem->Attribute("code");
		LOG_DEBUG("SSO status code: " + statusCode);
		if (statusCode != STATUS_CODE_OK) {
			_loginPasswordOk = false;
			return;
		} else {
			_loginPasswordOk = true;
		}
	}

	TiXmlText * text = sso.FirstChild("user").FirstChild().Text();
	if (text) {
		account._username = text->Value();
	}

	text = sso.FirstChild("userid").FirstChild().Text();
	if (text) {
		account._identity = text->Value();
	}

	text = sso.FirstChild("displayname").FirstChild().Text();
	if (text) {
		account._displayName = text->Value();
	}

	text = sso.FirstChild("password").FirstChild().Text();
	if (text) {
		account._password = text->Value();
	}

	text = sso.FirstChild("realm").FirstChild().Text();
	if (text) {
		account._realm = text->Value();
	}

	text = sso.FirstChild("proxy").FirstChild("host").FirstChild().Text();
	if (text) {
		account._proxyServerHostname = text->Value();
		//account._proxyServerHostname = "213.91.9.210";
	}

	text = sso.FirstChild("proxy").FirstChild("port").FirstChild().Text();
	if (text) {
		String tmp(text->Value());
		account._proxyServerPort = tmp.toInteger();
	}

	text = sso.FirstChild("server").FirstChild("host").FirstChild().Text();
	if (text) {
		account._registerServerHostname = text->Value();
	}

	text = sso.FirstChild("server").FirstChild("port").FirstChild().Text();
	if (text) {
		String tmp(text->Value());
		account._registerServerPort = tmp.toInteger();
	}

	text = sso.FirstChild("httptunnel").FirstChild("host").FirstChild().Text();
	if (text) {
		//account._registerServerHostname = text->Value();
	}
}
