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

#include "GaimPresenceMngr.h"
#include "GaimAccountMngr.h"

extern "C" {
#include "gaim/privacy.h"
}

#include <util/Logger.h>


/* ***************** GAIM CALLBACK ***************** */
static void C_PermitAddedCbk(GaimAccount *account, const char *name)
{
	GaimPresenceMngr::PermitAddedCbk(account, name);
}

static void C_PermitRemovedCbk(GaimAccount *account, const char *name)
{
	GaimPresenceMngr::PermitRemovedCbk(account, name);
}

static void C_DenyAddedCbk(GaimAccount *account, const char *name)
{
	GaimPresenceMngr::DenyAddedCbk(account, name);
}

static void C_DenyRemovedCbk(GaimAccount *account, const char *name)
{
	GaimPresenceMngr::DenyRemovedCbk(account, name);
}

GaimPrivacyUiOps privacy_wg_ops =	{
										C_PermitAddedCbk,
										C_PermitRemovedCbk,
										C_DenyAddedCbk,
										C_DenyRemovedCbk
									};

/* ************************************************** */


GaimPresenceMngr *GaimPresenceMngr::_staticInstance = NULL;
std::list<GaimIMPresence *> GaimPresenceMngr::_gaimIMPresenceList;

GaimPresenceMngr::GaimPresenceMngr()
{
}

GaimPresenceMngr *GaimPresenceMngr::getInstance()
{
	if (!_staticInstance)
		_staticInstance = new GaimPresenceMngr();

	return _staticInstance;
}


void GaimPresenceMngr::PermitAddedCbk(GaimAccount *account, const char *name)
{
	fprintf(stderr, "GaimPresenceMngr : PermitAddedCbk()\n");
}

void GaimPresenceMngr::PermitRemovedCbk(GaimAccount *account, const char *name)
{
	fprintf(stderr, "GaimPresenceMngr : PermitRemovedCbk()\n");
}

void GaimPresenceMngr::DenyAddedCbk(GaimAccount *account, const char *name)
{
	fprintf(stderr, "GaimPresenceMngr : DenyAddedCbk()\n");
}

void GaimPresenceMngr::DenyRemovedCbk(GaimAccount *account, const char *name)
{
	fprintf(stderr, "GaimPresenceMngr : DenyRemovedCbk()\n");
}


/* **************** MANAGE PRESENCE LIST (Buddy list) ****************** */

GaimIMPresence *GaimPresenceMngr::FindIMPresence(IMAccount &account)
{
	GaimIMPresenceIterator i;
	for (i = _gaimIMPresenceList.begin(); i != _gaimIMPresenceList.end(); i++)
	{
		if ((*i)->equalsTo(account.getLogin(), account.getProtocol()))
		{
			return (*i);
		}
	}

	return NULL;
}

GaimIMPresence *GaimPresenceMngr::AddIMPresence(IMAccount &account)
{
	GaimIMPresence *mIMPresence = FindIMPresence(account);

	if (mIMPresence == NULL)
	{
		mIMPresence = new GaimIMPresence(account);

		_gaimIMPresenceList.push_back(mIMPresence);
	}

	return mIMPresence;
}

void GaimPresenceMngr::RemoveIMPresence(IMAccount &account)
{
	GaimIMPresenceIterator i;
	for (i = _gaimIMPresenceList.begin(); i != _gaimIMPresenceList.end(); i++)
	{
		if ((*i)->equalsTo(account.getLogin(), account.getProtocol()))
		{
			_gaimIMPresenceList.erase(i);
			delete (*i);
			break;
		}
	}
}
/* *********************************************************************** */