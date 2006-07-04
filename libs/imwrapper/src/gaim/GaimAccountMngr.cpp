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
#include "GaimPresenceMngr.h"
#include "GaimChatMngr.h"
#include "GaimConnectMngr.h"
#include "GaimContactListMngr.h"

#include <util/Logger.h>

void *gaim_wg_get_handle()
{
	static int handle;

	return &handle;
}

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

static void C_AuthRequestCbk(GaimAccount *account, const char *remote_user,
							const char *id, const char *alias,
							const char *message, gboolean response)
{
	GaimAccountMngr::AuthRequestCbk(account, remote_user, id, alias, message, response);
}

GaimAccountUiOps acc_wg_ops =
{
	C_NotifyAddedCbk,
	NULL,
	C_RequestAddCbk,
	C_AuthRequestCbk,
};

/* ************************************************* */
GaimAccountMngr *GaimAccountMngr::_staticInstance = NULL;
std::list<IMAccount *> GaimAccountMngr::_gaimIMAccountList;
GaimPresenceMngr *GaimAccountMngr::_presenceMngr = NULL;
GaimChatMngr *GaimAccountMngr::_chatMngr = NULL;
GaimConnectMngr *GaimAccountMngr::_connectMngr = NULL;
GaimContactListMngr *GaimAccountMngr::_clistMngr = NULL;

GaimAccountMngr::GaimAccountMngr()
{
}

GaimAccountMngr *GaimAccountMngr::getInstance()
{
	if (!_staticInstance)
		_staticInstance = new GaimAccountMngr();

	return _staticInstance;
}

void GaimAccountMngr::Init()
{
	_presenceMngr = GaimPresenceMngr::getInstance();
	_chatMngr = GaimChatMngr::getInstance();
	_connectMngr = GaimConnectMngr::getInstance();
	_clistMngr = GaimContactListMngr::getInstance();
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

void GaimAccountMngr::AuthRequestCbk(GaimAccount *account, const char *remote_user,
									const char *id, const char *alias,
									const char *message, gboolean response)
{
	const char *gPrclId = gaim_account_get_protocol_id(account);
	IMAccount *mAccount = FindIMAccount(gaim_account_get_username(account),
									GaimIMPrcl::GetEnumIMProtocol(gPrclId));
	GaimIMPresence *mPresence = NULL;
	
	if (!mAccount)
		return;

	mPresence = _presenceMngr->FindIMPresence(*mAccount);
	mPresence->authorizationRequestEvent(*mPresence, remote_user, message != NULL ? message : "");
}

/* **************** MANAGE ACCOUNT LIST (Buddy list) ****************** */

IMAccount *GaimAccountMngr::GetFirstIMAccount()
{
	GaimIMAccountIterator i = _gaimIMAccountList.begin();

	if (i == _gaimIMAccountList.end())
		return NULL;
	else
		return (*i);
}

IMAccount *GaimAccountMngr::FindIMAccount(const char *login, EnumIMProtocol::IMProtocol protocol)
{
	GaimIMAccountIterator i;
	for (i = _gaimIMAccountList.begin(); i != _gaimIMAccountList.end(); i++)
	{
		if (((*i)->getLogin().compare(login) == 0)
			&& ((*i)->getProtocol() == protocol))
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
		account.imAccountWillDieEvent +=
			boost::bind(&GaimAccountMngr::imAccountWillDieEventHandler, this, _1);
		_gaimIMAccountList.push_back(&account);
	}
}

void GaimAccountMngr::RemoveIMAccount(IMAccount &account)
{
	GaimIMAccountIterator i;
	for (i = _gaimIMAccountList.begin(); i != _gaimIMAccountList.end(); i++)
	{
		if ((*i)->getLogin().compare(account.getLogin()) == 0
			&& (*i)->getProtocol() == account.getProtocol())
		{
			_gaimIMAccountList.erase(i);
			break;
		}
	}
}

void GaimAccountMngr::imAccountWillDieEventHandler(IMAccount & imAccount) 
{
	GaimAccount	*gAccount;
	GaimIMConnect *mConnect = NULL;

	gAccount = gaim_accounts_find(imAccount.getLogin().c_str(),
								GaimIMPrcl::GetPrclId(imAccount.getProtocol()));
	if (!gAccount)
		return;

	mConnect = _connectMngr->FindIMConnect(imAccount);
	mConnect->disconnect();
	gaim_accounts_delete(gAccount);

	_presenceMngr->RemoveIMPresence(imAccount);
	_chatMngr->RemoveIMChat(imAccount);
	_clistMngr->RemoveIMContactList(imAccount);
	_connectMngr->RemoveIMConnect(imAccount);

	RemoveIMAccount(imAccount);
}

/* ******************************************************************** */
