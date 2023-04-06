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

#include "PurpleAccountPresenceManager.h"

#include <coipmanager/CoIpManager.h>
#include <coipmanager/datamanager/UserProfileManager.h>

#include <purplewrapper/PurpleEnumIMProtocol.h>
#include <purplewrapper/PurpleEnumPresenceState.h>
#include <purplewrapper/PurpleWrapper.h>

#include <util/File.h>
#include <util/SafeDelete.h>

extern "C" {
#include <libpurple/account.h>
}

typedef struct misc_s
{
	const Account * account;
	void * data_ptr;
	int data_int;
	int cbk_id;
}	misc_t;

QMutex * PurpleAccountPresenceManager::_mutex = NULL;

IAccountPresenceManagerPlugin * coip_accountpresence_plugin_init(CoIpManager & coIpManager) {
	return new PurpleAccountPresenceManager(coIpManager);
}

PurpleAccountPresenceManager::PurpleAccountPresenceManager(CoIpManager & coIpManager)
	: IAccountPresenceManagerPlugin(coIpManager) {
	_mutex = new QMutex();
}

PurpleAccountPresenceManager::~PurpleAccountPresenceManager() {
	OWSAFE_DELETE(_mutex);
}

bool PurpleAccountPresenceManager::setPresenceStateCbk(void * data) {
	QMutexLocker lock(_mutex);
	
	const Account * account = ((misc_t *)data)->account;
	EnumPresenceState::PresenceState state = 
		(EnumPresenceState::PresenceState)((misc_t *)data)->data_int;
	const char * note = (const char *)((misc_t *)data)->data_ptr;
	PurpleAccount * pAccount = purple_accounts_find(PurpleWrapper::purpleLogin(account).c_str(),
		PurpleIMPrcl::getPrclId(account->getAccountType()));

	if (pAccount && !purple_account_is_connecting(pAccount))
	{
		if (pAccount && purple_account_is_connected(pAccount)) {
			if (strlen(note) == 0) {
				purple_account_set_status(pAccount,
					PurplePreState::getStatusId(state, account->getAccountType()),
					TRUE, NULL);
			} else {
				purple_account_set_status(pAccount,
					PurplePreState::getStatusId(state, account->getAccountType()),
					TRUE, "message", note, NULL);
			}
		}
	}

	purple_timeout_remove(((misc_t *)data)->cbk_id);

	free(((misc_t *)data)->data_ptr);
	free((misc_t *)data);
	
	return TRUE;
}

void PurpleAccountPresenceManager::setPresenceState(const Account & account,
	EnumPresenceState::PresenceState state, const std::string & note) {
	
	misc_t * data = (misc_t *) malloc(sizeof(misc_t));
	memset(data, 0, sizeof(misc_t));
	data->account = &account;
	data->data_int = state;
	if (note.empty()) {
		data->data_ptr = strdup("");
	} else {
		data->data_ptr = strdup(note.c_str());
	}

	QMutexLocker lock(_mutex);
	data->cbk_id = purple_timeout_add(0, (GSourceFunc) PurpleAccountPresenceManager::setPresenceStateCbk, data);
}

bool PurpleAccountPresenceManager::setAliasCbk(void * data) {
	QMutexLocker lock(_mutex);
	
	const Account * account = ((misc_t *)data)->account;
	const char * alias = (const char *)((misc_t *)data)->data_ptr;
	PurpleAccount * pAccount = purple_accounts_find(PurpleWrapper::purpleLogin(account).c_str(),
		PurpleIMPrcl::getPrclId(account->getAccountType()));

	if (pAccount && purple_account_is_connected(pAccount)) {
		purple_account_set_alias(pAccount, alias, TRUE);
	}

	purple_timeout_remove(((misc_t *)data)->cbk_id);

	free(((misc_t *)data)->data_ptr);
	free((misc_t *)data);
	
	return TRUE;
}

void PurpleAccountPresenceManager::setAlias(const Account & account, const std::string & alias) {
	misc_t * data = (misc_t *) malloc(sizeof(misc_t));
	memset(data, 0, sizeof(misc_t));
	data->account = &account;
	if (alias.empty()) {
		data->data_ptr = strdup("");
	} else {
		data->data_ptr = strdup(alias.c_str());
	}

	QMutexLocker lock(_mutex);
	data->cbk_id = purple_timeout_add(0, (GSourceFunc) PurpleAccountPresenceManager::setAliasCbk, data);
}


bool PurpleAccountPresenceManager::setIconCbk(void * data) {
	QMutexLocker lock(_mutex);

	const Account * account = ((misc_t *)data)->account;
	const char * iconPath = (const char *)((misc_t *)data)->data_ptr;
	PurpleAccount * pAccount = purple_accounts_find(PurpleWrapper::purpleLogin(account).c_str(),
		PurpleIMPrcl::getPrclId(account->getAccountType()));

	purple_account_set_buddy_icon_path(pAccount, iconPath);
	purple_timeout_remove(((misc_t *)data)->cbk_id);

	free(((misc_t *)data)->data_ptr);
	free((misc_t *)data);

	return TRUE;
	
}

void PurpleAccountPresenceManager::setIcon(const Account & account, const OWPicture & picture) {
	PurpleAccount * pAccount = purple_accounts_find(PurpleWrapper::purpleLogin(account).c_str(),
		PurpleIMPrcl::getPrclId(account.getAccountType()));

	if (pAccount && purple_account_is_connected(pAccount)) {
		FileWriter file(File::createTemporaryFile());
		std::string pictData = std::string(picture.getData().constData(), picture.getData().length());
		file.write(pictData);
		file.close();

		misc_t * data = (misc_t *) malloc(sizeof(misc_t));
		memset(data, 0, sizeof(misc_t));

		data->account = &account;
		data->data_ptr = strdup(file.getFullPath().c_str());

		QMutexLocker lock(_mutex);
		data->cbk_id = purple_timeout_add(0, (GSourceFunc) PurpleAccountPresenceManager::setIconCbk, data);
	}
}
