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

#include <imwrapper/IMAccountHandlerParser.h>

#include <imwrapper/IMAccountHandler.h>
#include <imwrapper/EnumIMProtocol.h>

#include <tinyxml.h>

using namespace std;

IMAccountHandlerParser::IMAccountHandlerParser(IMAccountHandler & imAccountHandler, const string & data)
: _imAccountHandler(imAccountHandler), _data(data) {
}

bool IMAccountHandlerParser::parse() {
	TiXmlDocument doc;
	doc.Parse(_data.c_str());
	
	TiXmlHandle docHandle(& doc);
	TiXmlNode * imaccounts = docHandle.FirstChild("imaccounts").Node();

	TiXmlNode * lastChild = NULL;
	while ((lastChild = imaccounts->IterateChildren("account", lastChild))) {
		string accountData;
		accountData << *lastChild;

		IMAccount account;
		account.unserialize(accountData);
		_imAccountHandler.insert(account);
	}

	return true;
}
