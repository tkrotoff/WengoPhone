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
class Thread;
class UserProfile;
class UserProfileHandler;

/**
 * Import configuration from WengoPhone classic.
 *
 * @ingroup model
 * @author Julien Bossart
 * @author Philippe Bernery
 */
class ConfigImporter {
public:

	ConfigImporter(UserProfileHandler & userProfileHandler, Thread & modelThread);

	/**
	 * Imports the WengoPhone Classic Config only if no WengoPhone NG
	 * configuration exists in the given folder.
	 *
	 * @return true if configuration has been imported, false if no configuration
	 * found or the configuration has already been imported.
	 */
	bool importConfig(const std::string & str);

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

	static std::string classicVCardToString(void *structVcard);

	static void * getLastWengoUser(const std::string & configUserFile, unsigned version);

	/**
	 * Imports a config from a particular version to another one.
	 *
	 * @param from version from which we want to import
	 * @param to version we want to have
	 */
	void makeImportConfig(unsigned from, unsigned to);

	bool importConfigFromV1toV3();

	bool importConfigFromV2toV3();

	bool importConfigFromV3toV4();

	bool importContactsFromV1toV3(const std::string & fromDir, UserProfile & userProfile);

	void addContactDetails(Contact & contact, void * structVcard);

	UserProfileHandler & _userProfileHandler;

	Thread & _modelThread;
};

#endif	//OWCONFIGIMPORTER_H