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

#include "GaimAccountMngr.h"

#include <Logger.h>

GaimAccountMngr *GaimAccountMngr::StaticInstance = NULL;
std::list<IMAccount *> GaimAccountMngr::_GaimIMAccountList;

/* ***************** GAIM CALLBACK ***************** */
static void C_NotifyAddedCbk(GaimAccount *account, const char *remote_user,
							const char *id, const char *alias,
							const char *message)
{
	GaimAccountMngr::NotifyAddedCbk(account, remote_user, id, alias, message);
}

static void C_RequestAddCbk(GaimAccount *account, const char *remote_user,
							const char *id, const char *alias,
							const char *message)
{
	GaimAccountMngr::RequestAddCbk(account, remote_user, id, alias, message);
}


GaimAccountUiOps acc_wg_ops = 
{
	C_NotifyAddedCbk,
	NULL,
	C_RequestAddCbk,
};

/* ************************************************* */


GaimAccountMngr::GaimAccountMngr()
{

}

GaimAccountMngr *GaimAccountMngr::getInstance()
{
	if (!StaticInstance)
		StaticInstance = new GaimAccountMngr();

	return StaticInstance;
}

void GaimAccountMngr::NotifyAddedCbk(GaimAccount *account, const char *remote_user,
										const char *id, const char *alias,
										const char *message)
{
	fprintf(stderr, "GaimAccountMngr : NotifyAddedCbk()\n");
}

void GaimAccountMngr::RequestAddCbk(GaimAccount *account, const char *remote_user,
									const char *id, const char *alias,
									const char *message)
{
	fprintf(stderr, "GaimAccountMngr : RequestAddCbk()\n");
}

IMAccount *GaimAccountMngr::FindIMAccount(const char *login, EnumIMProtocol::IMProtocol protocol)
{
	GaimIMAccountIterator i;
	for (i = _GaimIMAccountList.begin(); i != _GaimIMAccountList.end(); i++)
	{
		if ((*i)->getLogin().compare(login) == 0 
			&& (*i)->getProtocol() == protocol)
		{
			return (*i);
		}
	}

	return NULL;
}

void GaimAccountMngr::AddIMAccount(IMAccount &account)
{
	IMAccount *mIMAccount = FindIMAccount(account.getLogin().c_str(), account.getProtocol());

	if (mIMAccount == NULL)
	{
		_GaimIMAccountList.push_back(&account);
	}
}

void GaimAccountMngr::RemoveIMAccount(IMAccount &account)
{
	GaimIMAccountIterator i;
	for (i = _GaimIMAccountList.begin(); i != _GaimIMAccountList.end(); i++)
	{
		if ((*i)->getLogin().compare(account.getLogin()) == 0 
			&& (*i)->getProtocol() == account.getProtocol())
		{
			_GaimIMAccountList.erase(i);
			break;
		}
	}
}