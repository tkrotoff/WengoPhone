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

#include <coipmanager_base/serializer/WengoAccountXMLSerializer.h>

#include <coipmanager_base/account/WengoAccount.h>

#include <util/Base64.h>
#include <util/Logger.h>
#include <util/String.h>

#include <tinyxml.h>

WengoAccountXMLSerializer::WengoAccountXMLSerializer(WengoAccount & wengoAccount)
	: SipAccountXMLSerializer(wengoAccount) {
}

std::string WengoAccountXMLSerializer::serialize() {
	std::string result;

	WengoAccount & myAccount = (WengoAccount &) _account;

	result += "<wengologin>" + myAccount.getWengoLogin() + "</wengologin>\n";

	if (myAccount.isPasswordSaved()) {
		result += "<wengopassword>" + Base64::encode(myAccount.getWengoPassword()) + "</wengopassword>\n";
		result += "<ssorequestmade>" + String::fromBoolean(myAccount.isSSORequestMade()) + "</ssorequestmade>\n";
		result += "<stunserver>" + myAccount.getStunServer() + "</stunserver>\n";
		result += SipAccountXMLSerializer::serialize();
	}

	return result;
}

bool WengoAccountXMLSerializer::unserialize(TiXmlHandle & rootElt) {
	WengoAccount & myAccount = (WengoAccount &) _account;

	// Retrieving wengo login
	TiXmlNode * wengoLogin = rootElt.FirstChild("wengologin").FirstChild().Node();
	if (wengoLogin) {
		myAccount.setWengoLogin(wengoLogin->Value());
	}
	////

	// Retrieving wengo password
	TiXmlNode * wengoPassword = rootElt.FirstChild("wengopassword").FirstChild().Node();
	if (wengoPassword) {
		myAccount.setWengoPassword(Base64::decode(wengoPassword->Value()));
		myAccount.setSavePassword(true);
	} else {
		myAccount.setSavePassword(false);
	}
	////

	// Retrieving 'sso request made'
	TiXmlNode * made = rootElt.FirstChild("ssorequestmade").FirstChild().Node();
	if (made) {
		String sMade = made->Value();
		myAccount.setSSORequestMade(sMade.toBoolean());
	}
	////

	// Retrieving stun server
	TiXmlNode * stunServer = rootElt.FirstChild("stunserver").FirstChild().Node();
	if (stunServer) {
		myAccount.setStunServer(stunServer->Value());
	}
	////

	// SipAccountXMLSerializer::unserialize has to be done after
	// WengoAccount::setPassword and WengoPassword::setLogin because
	// calling these methods reset Sip parameters in WengoAccount
	return SipAccountXMLSerializer::unserialize(rootElt);
}
