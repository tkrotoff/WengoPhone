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

#ifndef OWPURPLEPARAMSETTER_H
#define OWPURPLEPARAMSETTER_H

#include <coipmanager_base/EnumAccountType.h>

extern "C" {
#include <libpurple/account.h>
}

class Account;
class NetworkProxy;

/**
 * Base class to set parameters on LibPurple account.
 *
 * @author Philippe Bernery
 * @author Julien Bossart
 */
class PurpleParamSetter {
public:

	/**
	 * Sets account parameters.
	 * account can be changed according to alternateConnectionWay.
	 * if pAccount is NULL, only account will be modified.
	 *
	 * @param networkProxy NetworkProxy information
	 * @param account Account to get information from
	 * @param pAccount PurpleAccount to fill
	 * @param alternateConnectionWay if true, will set parameters to use the alternate connection way 
	 * (e.g: HTTP connection with MSN, etc.)
	 */
	static void setAccountParams(const NetworkProxy &networkProxy, Account & account,
		PurpleAccount * pAccount, bool alternateConnectionWay);

	/**
	 * This methods checks if Account is configured to use the alternate
	 * connection.
	 */
	static bool isAlternateConnectionUsed(const Account & account);

private:
	
	static void setGTalkParams(Account & account, PurpleAccount *pAccount, bool alternateConnectionWay);

	static void setJabberParams(Account & account, PurpleAccount *pAccount, bool alternateConnectionWay);

	static void setMSNParams(Account & account, PurpleAccount *pAccount, bool alternateConnectionWay);

	static void setOscarParams(Account & account, PurpleAccount *pAccount, bool alternateConnectionWay);

	static void setYahooParams(Account & account, PurpleAccount *pAccount, bool alternateConnectionWay);

};

#endif	//OWPURPLEPARAMSETTER_H
