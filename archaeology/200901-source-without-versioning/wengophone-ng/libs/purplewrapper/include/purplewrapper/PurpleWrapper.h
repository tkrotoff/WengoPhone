/*
 * WengoPhone, a voice over Internet phone
 * Copyright (C) 2004-2007  Wengo
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

#ifndef OWPURPLEWRAPPER_H
#define OWPURPLEWRAPPER_H

#include <purplewrapper/purplewrapperdll.h>

extern "C" {
#include <glib.h>
#include <libpurple/connection.h> //For PurpleConnectionUiOps
#include <libpurple/conversation.h> //For PurpleConversationUiOps
}

#include <string>

class Account;

/**
 * Purple Wrapper.
 *
 * @author Philippe Bernery
 * @author Julien Bossart
 */
class PurpleWrapper {
public:

	PURPLEWRAPPER_API static PurpleWrapper & getInstance();

	/**
	 * Initializes LibPurple.
	 */
	PURPLEWRAPPER_API void initialize();

	PURPLEWRAPPER_API void uninitialize();

	/** 
	 * Sets configPath for LibPurple. Must be called before initialize.
	 * @param configPath set the path to LibPurple configuration.
	 */
	PURPLEWRAPPER_API void setConfigPath(const std::string & configPath);

	/**
	 * Sets callbacks for connection.
	 */
	PURPLEWRAPPER_API void setConnectionCallbacks(PurpleConnectionUiOps * connUiOps);

	/**
	 * Sets callbacks for conversation.
	 */
	PURPLEWRAPPER_API void setConversationCallbacks(PurpleConversationUiOps * convUiOps);

	/**
	 * Sets callbacks for budy list.
	 */
	PURPLEWRAPPER_API void setBuddyListCallbacks(PurpleBlistUiOps * blistUiOps);

	/**
	 * Sets callbacks for budy list.
	 */
	PURPLEWRAPPER_API void setAccountCallbacks(PurpleAccountUiOps * accountUiOps);

	/**
	 * Used for signal connection.
	 */
	PURPLEWRAPPER_API void * getHandle();

	/**
	 * Utility function to get the real login of the given account.
	 * LibPurple sometimes adds more information in username field.
	 * (e.g: for Jabber, LibPurple adds the resource in the username field, "login/resource")
	 */
	PURPLEWRAPPER_API static std::string cleanLogin(PurpleAccount *gAccount);

	/**
	 * Utility function to get the login of the given account, compatible.
	 * with ones used by Purple.
	 * (e.g: for Jabber, LibPurple needs the resource in the username field, "login/resource")
	 */
	PURPLEWRAPPER_API static std::string purpleLogin(const Account & account);

private:

	PurpleWrapper();

	~PurpleWrapper();

	void setCallbacks();

	static PurpleWrapper * _instance;

	std::string _configPath;

	/** Connection callbacks. */
	PurpleConnectionUiOps * _connUiOps;

	/** Conversation callbacks. */
	PurpleConversationUiOps * _convUiOps;

	/** Buddy list callbacks. */
	PurpleBlistUiOps * _blistUiOps;

	/** Accounts callbacks. */
	PurpleAccountUiOps * _accountUiOps;

	bool _isInitialized;
};

#endif	//OWPURPLEWRAPPER_H
