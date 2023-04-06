/*
 * WengoPhone, a voice over Internet phone
 * Copyright (C) 2004-2007  Wengo
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

#include "PurpleConnectedState.h"

#include <coipmanager/connectmanager/IConnectPlugin.h>

#include <purplewrapper/PurpleEnumIMProtocol.h>
#include <purplewrapper/PurpleWrapper.h>

#include <util/Logger.h>
#include <util/SafeDelete.h>

extern "C" {
#include <libpurple/account.h>
#include <libpurple/core.h>
}

typedef struct misc_s
{
	Account * account;
	int cbk_id;
}	misc_t;

QMutex * PurpleConnectedState::_mutex = NULL;

PurpleConnectedState::PurpleConnectedState(IConnectPlugin & context)
	: IConnectState(context) {
	_mutex = new QMutex();
}

PurpleConnectedState::~PurpleConnectedState() {
	OWSAFE_DELETE(_mutex);
}

void PurpleConnectedState::connect() {
	// Nothing to do in this state
}

bool PurpleConnectedState::disconnectCbk(void * data) {
	QMutexLocker lock(_mutex);

	const Account * account = ((misc_t *)data)->account;
	PurpleAccount *pAccount = purple_accounts_find(PurpleWrapper::purpleLogin(account).c_str(),
		PurpleIMPrcl::getPrclId(account->getAccountType()));

	if (!pAccount) {
		LOG_ERROR("Account not in LibPurple database");
		return FALSE;
	}

	if (purple_account_get_enabled(pAccount, purple_core_get_ui())) {
		purple_account_set_enabled(pAccount, purple_core_get_ui(), FALSE);
	}

	purple_account_disconnect(pAccount);

	purple_timeout_remove(((misc_t *)data)->cbk_id);
	free((misc_t *)data);

	return TRUE;
}

void PurpleConnectedState::disconnect() {
	IConnectPlugin &iConnect = static_cast<IConnectPlugin &>(_stateContext);
	Account &account = iConnect.getAccount();

	misc_t * data = (misc_t *) malloc(sizeof(misc_t));
	memset(data, 0, sizeof(misc_t));
	data->account = &account;

	QMutexLocker lock(_mutex);
	data->cbk_id = purple_timeout_add(0, (GSourceFunc) PurpleConnectedState::disconnectCbk, data);
}

void PurpleConnectedState::execute() {
	// Nothing to do in this state
}
