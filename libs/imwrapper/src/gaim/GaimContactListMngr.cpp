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

#include "GaimContactListMngr.h"
#include "GaimEnumIMProtocol.h"
#include "GaimEnumPresenceState.h"

extern "C" {
#include "gaim/blist.h"
#include "gaim/status.h"
}

#include <Logger.h>

#define DEFAULT_GROUP_NAME	"Default"

/* ***************** GAIM CALLBACK ***************** */
static void C_NewListCbk(GaimBuddyList *blist)
{
	GaimContactListMngr::NewListCbk(blist);
}

static void C_NewNodeCbk(GaimBlistNode *node)
{
	GaimContactListMngr::NewNodeCbk(node);
}

static void C_ShowCbk(GaimBuddyList *list)
{
	GaimContactListMngr::ShowCbk(list);
}

static void C_UpdateCbk(GaimBuddyList *list, GaimBlistNode *node)
{
	GaimContactListMngr::UpdateCbk(list, node);
}

static void C_RemoveCbk(GaimBuddyList *list, GaimBlistNode *node)
{
	GaimContactListMngr::RemoveCbk(list, node);
}

static void C_DestroyCbk(GaimBuddyList *list)
{
	GaimContactListMngr::DestroyCbk(list);
}

static void C_SetVisibleCbk(GaimBuddyList *list, gboolean show)
{
	GaimContactListMngr::SetVisibleCbk(list, show);
}

static void C_RequestAddBuddyCbk(GaimAccount *account, const char *username,
								const char *group, const char *alias)
{
	GaimContactListMngr::RequestAddBuddyCbk(account, username, group, alias);
}

static void C_RequestAddChatCbk(GaimAccount *account, GaimGroup *group,
								const char *alias, const char *name)
{
	GaimContactListMngr::RequestAddChatCbk(account, group, alias, name);
}

static void C_RequestAddGroupCbk(void)
{
	GaimContactListMngr::RequestAddGroupCbk();
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

GaimContactListMngr *GaimContactListMngr::_staticInstance = NULL;
GaimPresenceMngr *GaimContactListMngr::_presenceMngr = NULL;
GaimAccountMngr *GaimContactListMngr::_accountMngr = NULL;
std::list<GaimIMContactList *> GaimContactListMngr::_gaimIMContactListList;

GaimContactListMngr::GaimContactListMngr()
{
	_presenceMngr = GaimPresenceMngr::getInstance();
	_accountMngr = GaimAccountMngr::getInstance();
	gaim_set_blist(gaim_blist_new());
	gaim_blist_load();
}

GaimContactListMngr *GaimContactListMngr::getInstance()
{
	if (!_staticInstance)
		_staticInstance = new GaimContactListMngr();

	return _staticInstance;
}


void GaimContactListMngr::NewListCbk(GaimBuddyList *blist)
{
	fprintf(stderr, "GaimContactListMngr : NewListCbk()\n");
}

const char *GaimContactListMngr::FindBuddyGroup(GaimBuddy *gBuddy)
{
	GaimGroup *gGroup = gaim_buddy_get_group(gBuddy);
	
	if (gGroup)
		return gGroup->name;
	else
		return DEFAULT_GROUP_NAME;
}

void GaimContactListMngr::NewBuddyAdded(GaimBuddy *gBuddy)
{
	GaimAccount *gAccount = gaim_buddy_get_account(gBuddy);
	const char *gPrclId = gaim_account_get_protocol_id(gAccount);
	IMAccount *account = _accountMngr->FindIMAccount(gaim_account_get_username(gAccount),
												GaimIMPrcl::GetEnumIMProtocol(gPrclId));
	
	GaimIMContactList *mIMContactList = FindIMContactList(*account);
	if (mIMContactList)
	{
		const char *groupName = FindBuddyGroup(gBuddy);

		mIMContactList->newContactAddedEvent(*mIMContactList,
											gaim_buddy_get_name(gBuddy),
											groupName);
	}
}

void GaimContactListMngr::NewGroupAdded(GaimGroup *gGroup)
{
	// GAIM CONTACT LIST GROUPS ARE NOT ASSOCIATED WITH ANY ACCOUNTS
	// THAT'S WHY WE TAKE THE FIRST FOUND IM_CONTACTLIST 
	// TO SEND NEW_GROUP_ADDDED EVENT

	IMAccount *account = _accountMngr->GetFirstIMAccount();

	if (!account)
		return;

	GaimIMContactList *gIMContactList = FindIMContactList(*account);
	if (gIMContactList)
	{
		const char *groupName = (gGroup->name ? gGroup->name : DEFAULT_GROUP_NAME);

		gIMContactList->newContactGroupAddedEvent(*gIMContactList, groupName);
	}
}

void GaimContactListMngr::NewNodeCbk(GaimBlistNode *node)
{
	fprintf(stderr, "GaimContactListMngr : NewNodeCbk()\n");
	
	switch (node->type)
	{
		case GAIM_BLIST_BUDDY_NODE:
			NewBuddyAdded((GaimBuddy *) node);
			break;

		case GAIM_BLIST_GROUP_NODE:
			NewGroupAdded((GaimGroup *) node);
			break;

		case GAIM_BLIST_CONTACT_NODE:
			break;

		case GAIM_BLIST_CHAT_NODE:
			break;

		default:
			break;
	}
}


void GaimContactListMngr::ShowCbk(GaimBuddyList *list)
{
	fprintf(stderr, "GaimContactListMngr : ShowCbk()\n");
}

void GaimContactListMngr::UpdateBuddy(GaimBuddyList *list, GaimBuddy *gBuddy)
{
	GaimIMPresence *mIMpresence = NULL;
	GaimAccount	*gAccount = NULL;
	GaimPresence *gPresence;
	GaimStatus *gStatus;
	GaimStatusType *gStatusType;
	GaimStatusPrimitive gStatusPrim;
	IMAccount *account = NULL;
	const char *gPrclId;
	const char *gPresenceId;

	GaimGroup *gGroup = gaim_buddy_get_group(gBuddy);

	gPresence = gaim_buddy_get_presence(gBuddy);
	gStatus = gaim_presence_get_active_status(gPresence);
	gStatusType = gaim_status_get_type(gStatus);
	gStatusPrim = gaim_status_type_get_primitive(gStatusType);
	gPresenceId = gaim_primitive_get_id_from_type(gStatusPrim);
	gAccount = gaim_buddy_get_account(gBuddy);
	gPrclId = gaim_account_get_protocol_id(gAccount);
	account = _accountMngr->FindIMAccount(gaim_account_get_username(gAccount),
											GaimIMPrcl::GetEnumIMProtocol(gPrclId));
	if (account)
	{
		mIMpresence = _presenceMngr->FindIMPresence(*account);

		if (mIMpresence == NULL)
			return;

		mIMpresence->presenceStateChangedEvent(*mIMpresence, 
												GaimPreState::GetPresenceState(gPresenceId),
												"",
												std::string(gaim_buddy_get_name(gBuddy))
												);
	}											
}

void GaimContactListMngr::UpdateCbk(GaimBuddyList *list, GaimBlistNode *node)
{
	fprintf(stderr, "GaimContactListMngr : UpdateCbk()\n");
	
	switch (node->type)
	{
		case GAIM_BLIST_BUDDY_NODE:
			UpdateBuddy(list, (GaimBuddy *)node);
			break;

		default:
			return;
	}
}

void GaimContactListMngr::RemoveCbk(GaimBuddyList *list, GaimBlistNode *node)
{
	fprintf(stderr, "GaimContactListMngr : RemoveCbk()\n");
}

void GaimContactListMngr::DestroyCbk(GaimBuddyList *list)
{
	fprintf(stderr, "GaimContactListMngr : DestroyCbk()");
}

void GaimContactListMngr::SetVisibleCbk(GaimBuddyList *list, gboolean show)
{
	fprintf(stderr, "GaimContactListMngr : SetVisibleCbk()\n");
}

void GaimContactListMngr::RequestAddBuddyCbk(GaimAccount *account, const char *username,
												const char *group, const char *alias)
{
	fprintf(stderr, "GaimContactListMngr : RequestAddBuddyCbk()\n");
}

void GaimContactListMngr::RequestAddChatCbk(GaimAccount *account, GaimGroup *group,
												const char *alias, const char *name)
{
	fprintf(stderr, "GaimContactListMngr : RequestAddChatCbk()\n");
}

void GaimContactListMngr::RequestAddGroupCbk(void)
{
	fprintf(stderr, "GaimContactListMngr : RequestAddGroupCbk()\n");
}

/* **************** MANAGE CONTACT LIST (Buddy list) ****************** */

GaimIMContactList *GaimContactListMngr::FindIMContactList(IMAccount &account)
{
	GaimIMContactListIterator i;
	for (i = _gaimIMContactListList.begin(); i != _gaimIMContactListList.end(); i++)
	{
		if ((*i)->equalsTo(account.getLogin(), account.getProtocol()))
		{
			return (*i);
		}
	}

	return NULL;
}

GaimIMContactList *GaimContactListMngr::AddIMContactList(IMAccount &account)
{
	GaimIMContactList *mIMContactList = FindIMContactList(account);

	if (mIMContactList == NULL)
	{
		mIMContactList = new GaimIMContactList(account);

		_gaimIMContactListList.push_back(mIMContactList);
	}

	return mIMContactList;
}

void GaimContactListMngr::RemoveIMContactList(IMAccount &account)
{
	GaimIMContactListIterator i;
	for (i = _gaimIMContactListList.begin(); i != _gaimIMContactListList.end(); i++)
	{
		if ((*i)->equalsTo(account.getLogin(), account.getProtocol()))
		{
			_gaimIMContactListList.erase(i);
			delete (*i);
			break;
		}
	}
}