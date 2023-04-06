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

#include "WengoAccountXMLSerializer.h"

#include <model/account/wengo/WengoAccount.h>

#include <util/Base64.h>
#include <util/Logger.h>
#include <util/String.h>

#include <tinyxml.h>

WengoAccountXMLSerializer::WengoAccountXMLSerializer(WengoAccount & wengoAccount)
	: _wengoAccount(wengoAccount) {
}

std::string WengoAccountXMLSerializer::serialize() {
	std::string result;

	result += "<wengoaccount>\n";

	result += "<login>" + _wengoAccount.getWengoLogin() + "</login>\n";

	result += "<password>" + Base64::encode(_wengoAccount.getWengoPassword()) + "</password>\n";

	result += "<autologin>" + String::fromBoolean(_wengoAccount.hasAutoLogin()) + "</autologin>\n";

	result += "<valid>" + String::fromBoolean(_wengoAccount.isValid()) + "</valid>\n";

	result += "</wengoaccount>\n";

	return result;
}

bool WengoAccountXMLSerializer::unserialize(const std::string & data) {
	TiXmlDocument doc;

	doc.Parse(data.c_str());

	TiXmlHandle docHandle(&doc);
	TiXmlHandle wengoaccount = docHandle.FirstChild("wengoaccount");

	// Retrieving login
	TiXmlNode * login = wengoaccount.FirstChild("login").Node();
	if (login && login->FirstChild()) {
		_wengoAccount._wengoLogin = login->FirstChild()->Value();
	} else {
		LOG_ERROR("Invalid WengoAccount: no login set");
		return false;
	}
	////

	// Retrieving password
	TiXmlNode * password = wengoaccount.FirstChild("password").Node();
	if (password && password->FirstChild()) {
		_wengoAccount._wengoPassword = Base64::decode(password->FirstChild()->Value());
	}
	////

	// Retrieving autologin
	TiXmlNode * autologin = wengoaccount.FirstChild("autologin").Node();
	if (autologin && autologin->FirstChild()) {
		String autologinStr = autologin->FirstChild()->Value();
		_wengoAccount._autoLogin = autologinStr.toBoolean();
	}
	////

	// Retrieving valid
	TiXmlNode * valid = wengoaccount.FirstChild("valid").Node();
	if (valid && valid->FirstChild()) {
		String validStr = valid->FirstChild()->Value();
		_wengoAccount._isValid = validStr.toBoolean();
	}
	////

	return true;
}
