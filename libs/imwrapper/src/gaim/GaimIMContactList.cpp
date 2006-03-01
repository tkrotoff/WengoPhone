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

#include "GaimIMContactList.h"
#include "GaimEnumIMProtocol.h"

extern "C" {
#include "gaim/blist.h"
}

#include <Logger.h>



GaimIMContactList::GaimIMContactList(IMAccount & account)
	: IMContactList(account) 
{
}

bool GaimIMContactList::equalsTo(std::string login, EnumIMProtocol::IMProtocol protocol)
{
	IMAccount imAccount(login, "", protocol);

	if (_imAccount == imAccount)
		return true;
	else
		return false;
}

void GaimIMContactList::addContact(const std::string & groupName, const std::string & contactId)
{
	GaimGroup *gGroup;
	GaimBuddy *gBuddy;
	GaimAccount *gAccount;

	if (contactId.empty())
		return;

	gAccount = gaim_accounts_find(_imAccount.getLogin().c_str(), 
								GaimIMPrcl::GetPrclId(_imAccount.getProtocol()));
	if (gAccount)
	{
		if ((gGroup = gaim_find_group(groupName.c_str())) == NULL)
		{
			gGroup = gaim_group_new(groupName.c_str());
			gaim_blist_add_group(gGroup, NULL);
		}

		gBuddy = gaim_buddy_new(gAccount, contactId.c_str(), contactId.c_str());
		gaim_blist_add_buddy(gBuddy, NULL, gGroup, NULL);
		gaim_account_add_buddy(gAccount, gBuddy);
	}
}

void GaimIMContactList::removeContact(const std::string & groupName, const std::string & contactId)
{
	GaimGroup *gGroup = NULL;
	GaimBuddy *gBuddy = NULL;
	GaimAccount *gAccount = NULL;

	gAccount = gaim_accounts_find(_imAccount.getLogin().c_str(), 
								GaimIMPrcl::GetPrclId(_imAccount.getProtocol()));

	if (contactId.empty())
		return;

	if (!groupName.empty())
	{
		gGroup = gaim_find_group(groupName.c_str());
		gBuddy = gaim_find_buddy_in_group(gAccount, contactId.c_str(), gGroup);
	}
	else
	{
		gBuddy = gaim_find_buddy(gAccount, contactId.c_str());
	}

	gaim_account_remove_buddy(gAccount, gBuddy, gGroup);
	gaim_blist_remove_buddy(gBuddy);
}

void GaimIMContactList::addGroup(const std::string & groupName)
{
	if (groupName.empty())
		return;

	GaimGroup *gGroup = gaim_find_group(groupName.c_str());
		
	if (!gGroup)
	{
		gGroup = gaim_group_new(groupName.c_str());
		gaim_blist_add_group(gGroup, NULL);
	}
}

void GaimIMContactList::removeGroup(const std::string & groupName)
{
	if (groupName.empty())
		return;

	GaimGroup *gGroup = gaim_find_group(groupName.c_str());

	if (gGroup)
	{
		gaim_blist_remove_group(gGroup);
	}
}

void GaimIMContactList::changeGroupName(const std::string & oldGroupName, const std::string & newGroupName)
{
	GaimGroup *gGroup = gaim_find_group(oldGroupName.c_str());
	
	if (!gGroup)
		return;

	gaim_blist_rename_group(gGroup, newGroupName.c_str());
}

void GaimIMContactList::moveContactToGroup(const std::string & newGroupName,
											const std::string & oldGroupName, 
											const std::string & contactId)
{
	GaimAccount *gAccount = NULL;
	GaimBuddy *gBuddy = NULL;
	GaimGroup *gOldGroup, *gNewGroup;
	
	gAccount = gaim_accounts_find(_imAccount.getLogin().c_str(),
								GaimIMPrcl::GetPrclId(_imAccount.getProtocol()));
	if (!gAccount)
		return;

	gOldGroup = gaim_find_group(oldGroupName.c_str());
	if (!gOldGroup)
		return;

	gBuddy = gaim_find_buddy_in_group(gAccount, contactId.c_str(), gOldGroup);
	if (!gBuddy)
		return;

	gNewGroup = gaim_find_group(newGroupName.c_str());
	if (!gNewGroup)
		gNewGroup = gaim_group_new(newGroupName.c_str());
	
	gaim_blist_add_buddy(gBuddy, NULL, gNewGroup, NULL);
}

