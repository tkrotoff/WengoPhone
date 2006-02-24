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

#include <Logger.h>

#include "GaimConnectMngr.h"
#include "GaimEnumIMProtocol.h"


/* ***************** GAIM CALLBACK ***************** */
static void C_ConnProgressCbk(GaimConnection *gc, const char *text, 
							  size_t step, size_t step_count)
{
	GaimConnectMngr::ConnProgressCbk(gc, text, step, step_count);
}

static void C_ConnConnectedCbk(GaimConnection *gc)
{
	GaimConnectMngr::ConnConnectedCbk(gc);
}

static void C_ConnDisconnectedCbk(GaimConnection *gc)
{
	GaimConnectMngr::ConnDisconnectedCbk(gc);
}

static void C_ConnNoticeCbk(GaimConnection *gc, const char *text)
{
	GaimConnectMngr::ConnNoticeCbk(gc, text);
}

static void C_ConnReportDisconnectCbk(GaimConnection *gc, const char *text)
{
	GaimConnectMngr::ConnReportDisconnectCbk(gc, text);
}

GaimConnectionUiOps conn_wg_ops =	
{	
	C_ConnProgressCbk,
	C_ConnConnectedCbk,
	C_ConnDisconnectedCbk,
	C_ConnNoticeCbk,
	C_ConnReportDisconnectCbk
};

/* ************************************************** */

GaimConnectMngr *GaimConnectMngr::_staticInstance = NULL;
std::list<GaimIMConnect *> GaimConnectMngr::_gaimIMConnectList;

GaimConnectMngr::GaimConnectMngr()
{
	_accountMngr = GaimAccountMngr::getInstance();
}

GaimConnectMngr *GaimConnectMngr::getInstance() 
{
	if (!_staticInstance) 
	{
		_staticInstance = new GaimConnectMngr();
	}
	return _staticInstance;
}

GaimIMConnect *FindIMConnnectByGaimConnection(GaimConnection *gc)
{
	GaimAccount *Gaccount = NULL;
	IMAccount	*account = NULL;
	EnumIMProtocol::IMProtocol protocol = EnumIMProtocol::IMProtocolAll;

	Gaccount = gaim_connection_get_account(gc);

	protocol = GaimEnumIMProtocol::GetEnumIMProtocol(Gaccount->protocol_id);
	account = GaimAccountMngr::FindIMAccount(Gaccount->username, protocol);
	
	return GaimConnectMngr::FindIMConnect(*account);
}

void GaimConnectMngr::ConnProgressCbk(GaimConnection *gc, const char *text, 
									  size_t step, size_t step_count)
{
  fprintf(stderr, "GaimConnectMngr : ConnProgressCbk()\n");
}

void GaimConnectMngr::ConnConnectedCbk(GaimConnection *gc)
{
	GaimIMConnect *GIMConnect = NULL;

	fprintf(stderr, "GaimConnectMngr : ConnConnectedCbk()\n");

	GIMConnect = FindIMConnnectByGaimConnection(gc);
	if (GIMConnect != NULL)
		GIMConnect->loginStatusEvent(*GIMConnect, GIMConnect->LoginStatusConnected);
}

void GaimConnectMngr::ConnDisconnectedCbk(GaimConnection *gc)
{
	GaimIMConnect *GIMConnect = NULL;

	fprintf(stderr, "GaimConnectMngr : ConnDisconnectedCbk()\n");

	GIMConnect = FindIMConnnectByGaimConnection(gc);
	if (GIMConnect != NULL)
		GIMConnect->loginStatusEvent(*GIMConnect, GIMConnect->LoginStatusDisconnected);
}

void GaimConnectMngr::ConnNoticeCbk(GaimConnection *gc, const char *text)
{
  	fprintf(stderr, "GaimConnectMngr : ConnNoticeCbk()\n");
}

void GaimConnectMngr::ConnReportDisconnectCbk(GaimConnection *gc, const char *text)
{
	GaimIMConnect *GIMConnect = NULL;

  	fprintf(stderr, "GaimConnectMngr : ConnReportDisconnectCbk()\n");

	GIMConnect = FindIMConnnectByGaimConnection(gc);
	if (GIMConnect != NULL)
		GIMConnect->loginStatusEvent(*GIMConnect, GIMConnect->LoginStatusDisconnected);
}

/* **************** MANAGE CONNECT_LIST ****************** */
GaimIMConnect *GaimConnectMngr::FindIMConnect(IMAccount &account)
{
	GaimIMConnectIterator i;
	for (i = _gaimIMConnectList.begin(); i != _gaimIMConnectList.end(); i++)
	{
		if ((*i)->equalsTo(account.getLogin(), account.getProtocol()))
		{
			return (*i);
		}
	}

	return NULL;
}

GaimIMConnect *GaimConnectMngr::AddIMConnect(IMAccount &account)
{
	GaimIMConnect *mIMConnect = FindIMConnect(account);

	if (mIMConnect == NULL)
	{
		mIMConnect = new GaimIMConnect(account);
		_gaimIMConnectList.push_back(mIMConnect);

		_accountMngr->AddIMAccount(account);
	}
	
	return mIMConnect;
}

void GaimConnectMngr::RemoveIMConnect(IMAccount &account)
{
	GaimIMConnectIterator i;
	for (i = _gaimIMConnectList.begin(); i != _gaimIMConnectList.end(); i++)
	{
		if ((*i)->equalsTo(account.getLogin(), account.getProtocol()))
		{
			_gaimIMConnectList.erase(i);
			delete (*i);
			break;
		}
	}
}
/* ******************************************************** */

