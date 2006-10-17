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

#ifndef OWCONFIGIMPORTER_H
#define OWCONFIGIMPORTER_H

#include <model/account/SipAccount.h>
#include <model/account/wengo/WengoAccount.h>

#include <string>

class Contact;
class UserProfile;
class UserProfileHandler;

/**
 * Imports the config from a previous WengoPhone version.
 *
 * @ingroup model
 * @author Julien Bossart
 * @author Philippe Bernery
 */
class ConfigImporter {
public:

	ConfigImporter(UserProfileHandler & userProfileHandler);

	/**
	 * Imports a config from a particular version to the last one.
	 *
	 * If this method fails it makes a LOG_FATAL() -> assertion failed
	 */
	void importConfig();

private:

	/**
	 * Detects the last available version.
	 *
	 * @return the version of the last found version.
	 */
	static unsigned detectLastVersion();

	static std::string getWengoClassicConfigPath();

	static bool classicVcardParser(const std::string & vcardFile, void * structVcard);

	static bool classicXMLParser(const std::string & xmlFile, void * structVcard);

	static std::string classicVCardToString(void * structVcard);

	static void * getLastWengoUser(const std::string & configUserFile, unsigned version);

	bool importConfigFromV1toV3();

	bool importConfigFromV2toV3();

	bool importConfigFromV3toV4();

	bool importConfigFromV4toV5();

	bool importContactsFromV1toV3(const std::string & fromDir, UserProfile & userProfile);

	void addContactDetails(Contact & contact, void * structVcard);

	UserProfileHandler & _userProfileHandler;
};

#endif	//OWCONFIGIMPORTER_H
