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

extern "C" {
#include "gaim/account.h"
#include "gaim/connection.h"
#include "gaim/core.h"
}

#include "GaimIMConnect.h"
#include "GaimEnumIMProtocol.h"
#include <imwrapper/IMAccount.h>

#include <Logger.h>


int GetTabSize(char **tab)
{
	int i;

	if (!tab)
		return 0;

	for (i = 0; tab[i]; i++);

	return i;
}

GaimIMConnect::GaimIMConnect(IMAccount & account)
	: IMConnect(account)
{
}

bool GaimIMConnect::equalsTo(std::string login, EnumIMProtocol::IMProtocol protocol)
{
	IMAccount imAccount(login, "", protocol);

	if (_imAccount == imAccount)
		return true;
	else
		return false;
}

void *GaimIMConnect::CreateAccount()
{
	GaimAccount	*gAccount = NULL;
	char *PrclId = (char *)GaimIMPrcl::GetPrclId(_imAccount.getProtocol());
	
	gAccount = gaim_account_new(_imAccount.getLogin().c_str(), PrclId);
	
	if (gAccount)
	{
		gaim_account_set_password(gAccount, _imAccount.getPassword().c_str());
		gaim_account_set_bool(gAccount, "http_method", TRUE);

		gaim_accounts_add(gAccount);
	}

	return gAccount;
}

void GaimIMConnect::connect() 
{	
	GaimAccount	*gAccount;

	gAccount = gaim_accounts_find(_imAccount.getLogin().c_str(), 
								GaimIMPrcl::GetPrclId(_imAccount.getProtocol()));
	if (gAccount == NULL)
	{
		if (!(gAccount = (GaimAccount *) CreateAccount()))
			return;
	}

	gaim_account_set_password(gAccount, _imAccount.getPassword().c_str());
	if (!gaim_account_get_enabled(gAccount, gaim_core_get_ui()))
		gaim_account_set_enabled(gAccount, gaim_core_get_ui(), TRUE);

	gaim_account_connect(gAccount);
}

void GaimIMConnect::disconnect() 
{
	GaimAccount	*gAccount;

	gAccount = gaim_accounts_find(_imAccount.getLogin().c_str(), 
								GaimIMPrcl::GetPrclId(_imAccount.getProtocol()));
	if (gAccount)
		gaim_account_disconnect(gAccount);
}
