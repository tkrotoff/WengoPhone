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
#include "GaimEnumPresenceState.h"

extern "C" {
#include "gaim/account.h"
#include "gaim/privacy.h"
#include "gaim/buddyicon.h"
}

#include <util/File.h>
#include <util/Logger.h>
#include <util/OWPicture.h>

GaimIMPresence::GaimIMPresence(IMAccount & account)
	: IMPresence(account)
{
}

bool GaimIMPresence::equalsTo(std::string login, EnumIMProtocol::IMProtocol protocol)
{
	IMAccount imAccount(login, String::null, protocol);

	if (_imAccount == imAccount) {
		return true;
	} else {
		return false;
	}
}

void GaimIMPresence::changeMyPresence(EnumPresenceState::PresenceState state,
									  const std::string & note)
{
	GaimAccount *gAccount = gaim_accounts_find(_imAccount.getLogin().c_str(),
											GaimIMPrcl::GetPrclId(_imAccount.getProtocol()));

	if (gAccount)
	{
		if (note.length() == 0)
			gaim_account_set_status(gAccount, GaimPreState::GetStatusId(state, _imAccount.getProtocol()),
									TRUE, NULL);
		else
			gaim_account_set_status(gAccount, GaimPreState::GetStatusId(state, _imAccount.getProtocol()),
									TRUE, "message", note.c_str(), NULL);
	}

	IMPresence::changeMyPresence(state, note);
}

void GaimIMPresence::changeMyAlias(const std::string & nickname)
{
	GaimAccount *gAccount = gaim_accounts_find(_imAccount.getLogin().c_str(),
											GaimIMPrcl::GetPrclId(_imAccount.getProtocol()));

	if (gAccount && gaim_account_is_connected(gAccount) && !nickname.empty())
		gaim_account_set_alias(gAccount, nickname.c_str(), TRUE);
}

void GaimIMPresence::changeMyIcon(const OWPicture & picture)
{
	GaimAccount *gAccount = gaim_accounts_find(_imAccount.getLogin().c_str(),
											GaimIMPrcl::GetPrclId(_imAccount.getProtocol()));

	FileWriter file(File::createTemporaryFile());
	file.write(picture.getData());
	file.close();

	gaim_account_set_buddy_icon(gAccount, file.getFullPath().c_str());
}

void GaimIMPresence::subscribeToPresenceOf(const std::string & contactId)
{
}

void GaimIMPresence::blockContact(const std::string & contactId)
{
	GaimAccount *gAccount = gaim_accounts_find(_imAccount.getLogin().c_str(),
								GaimIMPrcl::GetPrclId(_imAccount.getProtocol()));

	if (gAccount)
	{
		gaim_privacy_deny_add(gAccount, contactId.c_str(), FALSE);
	}
}

void GaimIMPresence::unblockContact(const std::string & contactId)
{
	GaimAccount *gAccount = gaim_accounts_find(_imAccount.getLogin().c_str(),
								GaimIMPrcl::GetPrclId(_imAccount.getProtocol()));

	if (gAccount)
	{
		gaim_privacy_permit_add(gAccount, contactId.c_str(), FALSE);
	}
}

void GaimIMPresence::authorizeContact(const std::string & contactId, bool authorized,
									  const std::string message)
{
	GaimAccount *gAccount = gaim_accounts_find(_imAccount.getLogin().c_str(),
								GaimIMPrcl::GetPrclId(_imAccount.getProtocol()));

	if (!gAccount)
		return;

	if (authorized)
	{
		gaim_account_accept_auth_request(gAccount, contactId.c_str(), NULL, message.c_str());
	}
	else
	{
		gaim_account_deny_auth_request(gAccount, contactId.c_str(), NULL, message.c_str());
	}
}
