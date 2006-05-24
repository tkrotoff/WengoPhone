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

#ifndef CUSERPROFILE_H
#define CUSERPROFILE_H

#include <control/contactlist/CContactList.h>

#include <imwrapper/EnumIMProtocol.h>

#include <set>

class IMAccount;
class Thread;
class UserProfile;
class WengoAccount;

class CUserProfile {
public:

	CUserProfile(UserProfile & userProfile, Thread & modelThread);

	/**
	 * @see UserProfile::disconnect
	 */
	void disconnect();

	/**
	 * @see UserProfile::makeCall(Contact & contact, bool enableVideo)
	 */
	void makeContactCall(const std::string & contactId);

	/**
	 * Video will be enable from if set in Config.
	 * @see UserProfile::makeCall(const std::string & phoneNumber, bool enableVideo)
	 */
	void makeCall(const std::string & phoneNumber);

	/**
	 * @see UserProfile::makeCall(const std::string & phoneNumber, bool enableVideo)
	 */
	void makeCall(const std::string & phoneNumber, bool enableVideo);

	/**
	 * @see UserProfile::startIM
	 */
	void startIM(const std::string & contactId);

	/**
	 * @see UserProfile::setWengoAccount
	 */
	void setWengoAccount(const WengoAccount & wengoAccount);

	/**
	 * Gets IMAccounts of a desired protocol.
	 */
	std::set<IMAccount *> getIMAccountsOfProtocol(EnumIMProtocol::IMProtocol protocol) const;

	/**
	 * Gets the CContactList.
	 *
	 * @return the CContactList
	 */
	CContactList & getCContactList() {
		return _cContactList;
	}

	/**
	 * Gets the UserProfile.
	 *
	 * TODO: This method should exist (because it allows the GUI to access
	 * the model directly) and so should be removed ASAP.
	 */
	UserProfile & getUserProfile() {
		return _userProfile;
	}

private:

	/**
	 * @see disconnect
	 */
	void disconnectThreadSafe();

	/**
	 * @see :makeCall(Contact & contact, bool enableVideo)
	 */
	void makeContactCallThreadSafe(std::string contactId);

	/**
	 * @see makeCall(const std::string & phoneNumber, bool enableVideo)
	 */
	void makeCallThreadSafe(std::string phoneNumber);

	/**
	 * @see makeCall(const std::string & phoneNumber, bool enableVideo)
	 */
	void makeCallThreadSafe(std::string phoneNumber, bool enableVideo);

	/**
	 * @see startIM
	 */
	void startIMThreadSafe(std::string contactId);

	/**
	 * @see setWengoAccount
	 */
	void setWengoAccountThreadSafe(WengoAccount wengoAccount);

	UserProfile & _userProfile;

	CContactList _cContactList;

	Thread & _modelThread;
};

#endif	//CUSERPROFILE_H
