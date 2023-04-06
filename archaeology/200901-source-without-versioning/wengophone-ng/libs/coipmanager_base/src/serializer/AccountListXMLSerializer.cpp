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

#include <coipmanager_base/serializer/AccountListXMLSerializer.h>

#include <coipmanager_base/account/AccountList.h>
#include <coipmanager_base/serializer/AccountXMLSerializer.h>

#include <tinyxml.h>

AccountListXMLSerializer::AccountListXMLSerializer(AccountList & accountList)
	: _accountList(accountList) {
}

std::string AccountListXMLSerializer::serialize() {
	std::string result;

	result += "<accountlist>\n";

	for (AccountList::iterator it = _accountList.begin() ; it != _accountList.end() ; it++) {
		AccountXMLSerializer serializer(*it);
		result += serializer.serialize();
	}

	result += "</accountlist>\n";

	return result;	
}

bool AccountListXMLSerializer::unserialize(const std::string & data) {
	TiXmlDocument doc;
	doc.Parse(data.c_str());
	
	TiXmlHandle docHandle(& doc);
	TiXmlNode * accounts = docHandle.FirstChild("accountlist").Node();
	
	if (accounts) {
		TiXmlNode * lastChild = NULL;
		while ((lastChild = accounts->IterateChildren("account", lastChild))) {
			std::string accountData;
			accountData << *lastChild;

			Account account;
			AccountXMLSerializer serializer(account);
			if (serializer.unserialize(accountData)) {
				_accountList.push_back(account);
			}
		}
		return true;
	}
	return false;
}
