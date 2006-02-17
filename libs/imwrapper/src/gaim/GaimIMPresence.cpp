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

#include "GaimIMPresence.h"
#include "GaimEnumIMProtocol.h"
#include "GaimEnumPresenceState.h"

extern "C" {
#include "gaim/account.h"
#include "gaim/privacy.h"
}

#include <Logger.h>

GaimIMPresence::GaimIMPresence(IMAccount & account)
	: IMPresence(account), _account(account) 
{
}

bool GaimIMPresence::equalsTo(std::string login, EnumIMProtocol::IMProtocol protocol)
{
	if (login.compare(_account.getLogin()) == 0 
		&& _account.getProtocol() == protocol)
		return true;
	else
		return false;
}

void GaimIMPresence::changeMyPresence(EnumPresenceState::PresenceState state, 
									  const std::string & note) 
{
	GaimAccount *Gaccount = gaim_accounts_find(_account.getLogin().c_str(),
											GaimEnumIMProtocol::GetPrclId(_account.getProtocol()));
	
	if (Gaccount)
	{
		if (note.length() == 0)
			gaim_account_set_status(Gaccount, GaimEnumPresenceState::GetStatusId(state), 
									TRUE, NULL);
		else
			gaim_account_set_status(Gaccount, GaimEnumPresenceState::GetStatusId(state),
									TRUE, "message", note.c_str(), NULL);
	}
}

void GaimIMPresence::subscribeToPresenceOf(const std::string & contactId)
{
	GaimGroup *group;
	GaimBuddy *buddy;
	GaimAccount *Gaccount;

	const char *grp = "default";

	if ((group = gaim_find_group(grp)) == NULL)
		{
			group = gaim_group_new(grp);
			gaim_blist_add_group(group, NULL);
		}

	Gaccount = gaim_accounts_find(_account.getLogin().c_str(), 
								GaimEnumIMProtocol::GetPrclId(_account.getProtocol()));

	if (Gaccount)
	{
		buddy = gaim_buddy_new(Gaccount, contactId.c_str(), contactId.c_str());
		gaim_blist_add_buddy(buddy, NULL, group, NULL);
		gaim_account_add_buddy(Gaccount, buddy);
	}
}

void GaimIMPresence::blockContact(const std::string & contactId)
{
	GaimAccount *Gaccount;

	Gaccount = gaim_accounts_find(_account.getLogin().c_str(), 
								GaimEnumIMProtocol::GetPrclId(_account.getProtocol()));

	if (Gaccount)
	{
		gaim_privacy_deny_add(Gaccount, contactId.c_str(), FALSE);
	}
}

void GaimIMPresence::unblockContact(const std::string & contactId)
{
	GaimAccount *Gaccount;

	Gaccount = gaim_accounts_find(_account.getLogin().c_str(), 
								GaimEnumIMProtocol::GetPrclId(_account.getProtocol()));

	if (Gaccount)
	{
		gaim_privacy_permit_add(Gaccount, contactId.c_str(), FALSE);
	}
}
