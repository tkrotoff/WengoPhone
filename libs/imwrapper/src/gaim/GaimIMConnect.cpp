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
	: IMConnect(account), _account(account)
{
}

bool GaimIMConnect::equalsTo(std::string login, EnumIMProtocol::IMProtocol protocol)
{
	if (login.compare(_account.getLogin()) == 0 
		&& _account.getProtocol() == protocol)
		return true;
	else
		return false;
}

void *GaimIMConnect::CreateAccount()
{
	GaimAccount	*GAccount;
	char *PrclId = (char *)GaimEnumIMProtocol::GetPrclId(_account.getProtocol());
	
	GAccount = gaim_account_new(_account.getLogin().c_str(), PrclId);
	gaim_account_set_password(GAccount, _account.getPassword().c_str());
	gaim_account_set_bool(GAccount, "http_method", TRUE);

	gaim_accounts_add(GAccount);

	return GAccount;
}

void GaimIMConnect::connect() 
{	
	GaimAccount	*GAccount;

	GAccount = gaim_accounts_find(_account.getLogin().c_str(), 
								GaimEnumIMProtocol::GetPrclId(_account.getProtocol()));
	if (GAccount == NULL)
	{
		GAccount = (GaimAccount *) CreateAccount();
	}

	gaim_account_set_password(GAccount, _account.getPassword().c_str());
	if (!gaim_account_get_enabled(GAccount, gaim_core_get_ui()))
		gaim_account_set_enabled(GAccount, gaim_core_get_ui(), TRUE);

	gaim_account_connect(GAccount);
}

void GaimIMConnect::disconnect() 
{
	GaimAccount	*GAccount;

	GAccount = gaim_accounts_find(_account.getLogin().c_str(), 
								GaimEnumIMProtocol::GetPrclId(_account.getProtocol()));
	if (GAccount)
		gaim_account_disconnect(GAccount);
}
