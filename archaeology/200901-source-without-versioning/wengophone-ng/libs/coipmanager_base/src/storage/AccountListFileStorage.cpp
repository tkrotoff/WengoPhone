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

#include <coipmanager_base/storage/AccountListFileStorage.h>

#include <coipmanager_base/serializer/AccountListXMLSerializer.h>

#include <util/File.h>
#include <util/Logger.h>

#include <string>

static const std::string ACCOUNT_FILENAME = "accountlist.xml";

AccountListFileStorage::AccountListFileStorage() {
}

AccountListFileStorage::~AccountListFileStorage() {
}

bool AccountListFileStorage::load(const std::string & url, AccountList & accountList) {
	FileReader file(url + ACCOUNT_FILENAME);

	if (file.open()) {
		std::string data = file.read();
		file.close();

		AccountListXMLSerializer serializer(accountList);
		serializer.unserialize(data);

		return true;
	}

	return false;
}

bool AccountListFileStorage::save(const std::string & url, const AccountList & accountList) {
	FileWriter file(url + ACCOUNT_FILENAME);

	AccountListXMLSerializer serializer(const_cast<AccountList &>(accountList));
	file.write(serializer.serialize());
	file.close();

	return true;
}
