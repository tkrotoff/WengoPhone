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
#include "GaimEnumIMProtocol.h"
#include "GaimEnumPresenceState.h"

#include <Logger.h>

/* ***************** GAIM CALLBACK ***************** */
static void C_NewListCbk(GaimBuddyList *blist)
{
	GaimPresenceMngr::NewListCbk(blist);
}

static void C_NewNodeCbk(GaimBlistNode *node)
{
	GaimPresenceMngr::NewNodeCbk(node);
}

static void C_ShowCbk(GaimBuddyList *list)
{
	GaimPresenceMngr::ShowCbk(list);
}

static void C_UpdateCbk(GaimBuddyList *list, GaimBlistNode *node)
{
	GaimPresenceMngr::UpdateCbk(list, node);
}

static void C_RemoveCbk(GaimBuddyList *list, GaimBlistNode *node)
{
	GaimPresenceMngr::RemoveCbk(list, node);
}

static void C_DestroyCbk(GaimBuddyList *list)
{
	GaimPresenceMngr::DestroyCbk(list);
}

static void C_SetVisibleCbk(GaimBuddyList *list, gboolean show)
{
	GaimPresenceMngr::SetVisibleCbk(list, show);
}

static void C_RequestAddBuddyCbk(GaimAccount *account, const char *username,
								const char *group, const char *alias)
{
	GaimPresenceMngr::RequestAddBuddyCbk(account, username, group, alias);
}

static void C_RequestAddChatCbk(GaimAccount *account, GaimGroup *group,
								const char *alias, const char *name)
{
	GaimPresenceMngr::RequestAddChatCbk(account, group, alias, name);
}

static void C_RequestAddGroupCbk(void)
{
	GaimPresenceMngr::RequestAddGroupCbk();
}

GaimBlistUiOps blist_wg_ops = {
										C_NewListCbk,
										C_NewNodeCbk,
										C_ShowCbk,
										C_UpdateCbk,
										C_RemoveCbk,
										C_DestroyCbk,
										C_SetVisibleCbk,
										C_RequestAddBuddyCbk,
										C_RequestAddChatCbk,
										C_RequestAddGroupCbk
									};
/* ************************************************** */

GaimPresenceMngr *GaimPresenceMngr::StaticInstance = NULL;
std::list<GaimIMPresence *> GaimPresenceMngr::_GaimIMPresenceList;

GaimPresenceMngr::GaimPresenceMngr()
{
	gaim_set_blist(gaim_blist_new());
	gaim_blist_load();
}

GaimPresenceMngr *GaimPresenceMngr::getInstance()
{
	if (!StaticInstance)
		StaticInstance = new GaimPresenceMngr();

	return StaticInstance;
}

void GaimPresenceMngr::NewListCbk(GaimBuddyList *blist)
{
	fprintf(stderr, "GaimPresenceMngr : NewListCbk()\n");
}

void GaimPresenceMngr::NewNodeCbk(GaimBlistNode *node)
{
	fprintf(stderr, "GaimPresenceMngr : NewNodeCbk()\n");
}


void GaimPresenceMngr::ShowCbk(GaimBuddyList *list)
{
	fprintf(stderr, "GaimPresenceMngr : ShowCbk()\n");
}

void GaimPresenceMngr::UpdateBuddy(GaimBuddyList *list, GaimBlistNode *node)
{
	GaimIMPresence *GIMpresence = NULL;
	GaimAccount	*Gaccount = NULL;
	GaimPresence *presence;
	GaimStatus *status;
	GaimStatusType *status_type;
	GaimStatusPrimitive primitive;
	IMAccount *account = NULL;
	const char *GPrclId;
	const char *GPresenceId;

	presence = gaim_buddy_get_presence((GaimBuddy *)node);
	status = gaim_presence_get_active_status(presence);
	status_type = gaim_status_get_type(status);
	primitive = gaim_status_type_get_primitive(status_type);
	GPresenceId = gaim_primitive_get_id_from_type(primitive);
	Gaccount = gaim_buddy_get_account((GaimBuddy *)node);
	GPrclId = gaim_account_get_protocol_id(Gaccount);
	account = GaimAccountMngr::FindIMAccount(gaim_account_get_username(Gaccount),
											GaimEnumIMProtocol::GetEnumIMProtocol(GPrclId));
	if (account)
	{
		GIMpresence = FindIMPresence(*account);

		if (GIMpresence == NULL)
			GIMpresence = AddIMPresence(*account);

		GIMpresence->presenceStateChangedEvent(*GIMpresence, 
												GaimEnumPresenceState::GetPresenceState(GPresenceId),
												"",
												std::string(gaim_buddy_get_name((GaimBuddy *)node))
												);
	}											
}


void GaimPresenceMngr::UpdateCbk(GaimBuddyList *list, GaimBlistNode *node)
{
	fprintf(stderr, "GaimPresenceMngr : UpdateCbk()\n");
	
	switch (node->type)
	{
		case GAIM_BLIST_BUDDY_NODE:
			UpdateBuddy(list, node);
			break;

		default:
			return;
	}
}

void GaimPresenceMngr::RemoveCbk(GaimBuddyList *list, GaimBlistNode *node)
{
	fprintf(stderr, "GaimPresenceMngr : RemoveCbk()\n");
}

void GaimPresenceMngr::DestroyCbk(GaimBuddyList *list)
{
	fprintf(stderr, "GaimPresenceMngr : DestroyCbk()");
}

void GaimPresenceMngr::SetVisibleCbk(GaimBuddyList *list, gboolean show)
{
	fprintf(stderr, "GaimPresenceMngr : SetVisibleCbk()\n");
}


void GaimPresenceMngr::RequestAddBuddyCbk(GaimAccount *account, const char *username,
												const char *group, const char *alias)
{
	fprintf(stderr, "GaimPresenceMngr : RequestAddBuddyCbk()\n");
}


void GaimPresenceMngr::RequestAddChatCbk(GaimAccount *account, GaimGroup *group,
												const char *alias, const char *name)
{
	fprintf(stderr, "GaimPresenceMngr : RequestAddChatCbk()\n");
}

void GaimPresenceMngr::RequestAddGroupCbk(void)
{
	fprintf(stderr, "GaimPresenceMngr : RequestAddGroupCbk()\n");
}

/* **************** MANAGE PRESENCE LIST (Buddy list) ****************** */

GaimIMPresence *GaimPresenceMngr::FindIMPresence(IMAccount &account)
{
	GaimIMPresenceIterator i;
	for (i = _GaimIMPresenceList.begin(); i != _GaimIMPresenceList.end(); i++)
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

		_GaimIMPresenceList.push_back(mIMPresence);
	}
	
	return mIMPresence;
}

void GaimPresenceMngr::RemoveIMPresence(IMAccount &account)
{
	GaimIMPresenceIterator i;
	for (i = _GaimIMPresenceList.begin(); i != _GaimIMPresenceList.end(); i++)
	{
		if ((*i)->equalsTo(account.getLogin(), account.getProtocol()))
		{
			_GaimIMPresenceList.erase(i);
			delete (*i);
			break;
		}
	}
}
/* *********************************************************************** */