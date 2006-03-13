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

#ifndef WENGOACCOUNTPARSER_H
#define WENGOACCOUNTPARSER_H

#include "WengoAccount.h"

#include <string>

/**
 * Parses the XML datas from the Single Sign On authentication on the Wengo platform (SSO).
 *
 * This a separated class rather than to be inside the class WengoAccount,
 * this allows more modularity and encapsulates the use of TinyXML.
 *
 * XML scheme
 *
 * http://ws.wengo.fr/softphone-sso/sso.php?login=LOGIN&password=PASSWORD
 *
 * Replace LOGIN and PASSWORD by the login/password provided by Wengo.
 *
 * If the login/password are wrong it will return:
 * <pre>
 * <?xml version="1.0" ?>
 * <sso vapi="1">
 *     <status code="401">Unauthorized</status>
 * </sso>
 * </pre>
 *
 * If the login/password are correct it will return:
 * <pre>
 * <?xml version="1.0" ?>
 * <sso vapi="1">
 *     <status code="200">OK</status>
 *     <user>xxxx</user>
 *     <userid>xxxx</userid>
 *     <displayname>xxxx</displayname>
 *     <password>xxxx</password>
 *     <outbound>1</outbound>
 *     <realm>voip.wengo.fr</realm>
 *     <proxy>
 *         <host>213.91.9.210</host>
 *         <port>5060</port>
 *     </proxy>
 *     <server>
 *         <host>voip.wengo.fr</host>
 *         <port>5060</port>
 *     </server>
 *     <httptunnel>
 *         <host>80.118.99.31</host>
 *     </httptunnel>
 * </sso>
 * </pre>
 *
 * @ingroup model
 * @author Tanguy Krotoff
 */
class WengoAccountParser {
public:

	WengoAccountParser(WengoAccount & account, const std::string & data);

	bool isLoginPasswordOk() const {
		return _loginPasswordOk;
	}

private:

	bool _loginPasswordOk;
};

#endif	//WENGOACCOUNTPARSER_H
