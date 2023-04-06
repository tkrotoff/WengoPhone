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

#include "PurpleConnect.h"

#include "PurpleConnectedState.h"
#include "PurpleConnectingState.h"
#include "PurpleDisconnectedState.h"

#include <coipmanager/connectmanager/DisconnectedState.h>

#include <coipmanager_base/EnumConnectionState.h>
#include <coipmanager_base/account/Account.h>

#include <purplewrapper/PurpleEnumIMProtocol.h>
#include <purplewrapper/PurpleWrapper.h>

extern "C" {
#include <libpurple/account.h>
}

typedef struct misc_s
{
	PurpleConnect * instance;
	int cbk_id;
}	misc_t;

QMutex * PurpleConnect::_mutex = NULL;

PurpleConnect::PurpleConnect(CoIpManager & coIpManager, const Account & account)
	: IConnectPlugin(coIpManager, account) {

	_mutex = new QMutex();

	createAccount();

	addState(EnumConnectionState::ConnectionStateDisconnected, new PurpleDisconnectedState(*this));
	addState(EnumConnectionState::ConnectionStateConnecting, new PurpleConnectingState(*this));
	addState(EnumConnectionState::ConnectionStateConnected, new PurpleConnectedState(*this));

	setState(EnumConnectionState::ConnectionStateDisconnected);
}

PurpleConnect::~PurpleConnect() {
	OWSAFE_DELETE(_mutex);
}

bool PurpleConnect::checkValidity() {
	return true;
}

void PurpleConnect::emitAccountReadySignal() {
	accountReadySignal();
}

bool PurpleConnect::createAccountCbk(void * data) {
	QMutexLocker lock(_mutex);
	PurpleConnect * instance = ((misc_t *)data)->instance;
	std::string login = PurpleWrapper::purpleLogin(instance->_account);
	PurpleAccount * pAccount = purple_accounts_find(login.c_str(),
		PurpleIMPrcl::getPrclId(instance->_account.getAccountType()));

	if (!pAccount) {
		char *PrclId = const_cast<char *>(PurpleIMPrcl::getPrclId(instance->_account.getAccountType()));
		PurpleAccount * pAccount = purple_account_new(login.c_str(), PrclId);

		if (pAccount) {
			purple_account_set_password(pAccount, instance->_account.getPassword().c_str());
			purple_accounts_add(pAccount);
		}
	}

	purple_timeout_remove(((misc_t *)data)->cbk_id);
	free((misc_t *)data);

	instance->emitAccountReadySignal();
	
	return TRUE;
}

void PurpleConnect::createAccount() {
	misc_t * data = (misc_t *) malloc(sizeof(misc_t));
	memset(data, 0, sizeof(misc_t));
	data->instance = this;

	QMutexLocker lock(_mutex);
	data->cbk_id = purple_timeout_add(0, (GSourceFunc) PurpleConnect::createAccountCbk, data);
}

