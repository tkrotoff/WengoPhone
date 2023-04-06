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

#include "PurpleConnectingState.h"

#include "PurpleParamSetter.h"

#include <coipmanager/AccountMutator.h>
#include <coipmanager/CoIpManager.h>
#include <coipmanager/CoIpManagerConfig.h>
#include <coipmanager/connectmanager/IConnectPlugin.h>

#include <purplewrapper/PurpleEnumIMProtocol.h>
#include <purplewrapper/PurpleWrapper.h>

#include <networkdiscovery/NetworkProxyDiscovery.h>

#include <util/Logger.h>
#include <util/SafeDelete.h>

#include <QtCore/QTimer>

extern "C" {
#include <libpurple/account.h>
#include <libpurple/core.h>
}

typedef struct misc_s
{
	Account * account;
	PurpleAccount * pAccount;
	int cbk_id;
}	misc_t;

QMutex * PurpleConnectingState::_mutex = NULL;

PurpleConnectingState::PurpleConnectingState(IConnectPlugin & context)
	: IConnectState(context) {
	_nbConnectionTry = 0;
	_mutex = new QMutex();
}

PurpleConnectingState::~PurpleConnectingState() {
	OWSAFE_DELETE(_mutex);
}

void PurpleConnectingState::connect() {
	//Nothing to do in this state.
}

void PurpleConnectingState::disconnect() {
	//Nothing to do in this state.
}

bool PurpleConnectingState::executeCbk(void * data)
{
	QMutexLocker lock(_mutex);
	Account * account = ((misc_t *)data)->account;
	PurpleAccount * pAccount = ((misc_t *)data)->pAccount;

	purple_account_set_password(pAccount, account->getPassword().c_str());
	if (!purple_account_get_enabled(pAccount, purple_core_get_ui())) {
		purple_account_set_enabled(pAccount, purple_core_get_ui(), TRUE);
	} else {
		purple_account_connect(pAccount);	
	}

	purple_timeout_remove(((misc_t *)data)->cbk_id);
	free((misc_t *)data);

	return TRUE;
}

void PurpleConnectingState::execute() {
	if (purple_get_core() == NULL) {
		LOG_FATAL("LibPurple not initialized");
	}

	IConnectPlugin &iConnect = static_cast<IConnectPlugin &>(_stateContext);
	Account &account = iConnect.getAccount();
	EnumAccountType::AccountType accountType = account.getAccountType();

	if (accountType == EnumAccountType::AccountTypeGTalk) {
		accountType = EnumAccountType::AccountTypeJabber;
	}

	PurpleAccount *pAccount = purple_accounts_find(PurpleWrapper::purpleLogin(account).c_str(),
		PurpleIMPrcl::getPrclId(accountType));

	if (!pAccount) {
		LOG_FATAL("cannot find Account in LibPurple database. Sync problems.");
		return;
	}

	NetworkProxy networkProxy;
	networkProxy.setServer(iConnect.getCoIpManager().getCoIpManagerConfig().getHttpProxyServer());
	networkProxy.setServerPort(iConnect.getCoIpManager().getCoIpManagerConfig().getHttpProxyServerPort());
	networkProxy.setLogin(iConnect.getCoIpManager().getCoIpManagerConfig().getHttpProxyLogin());
	networkProxy.setPassword(iConnect.getCoIpManager().getCoIpManagerConfig().getHttpProxyPassword());

	checkConnectionConfiguration();

	if (_nbConnectionTry == 0) {
		PurpleParamSetter::setAccountParams(networkProxy, account, pAccount, false);
		AccountMutator::emitAccountUpdatedSignal(account, EnumUpdateSection::UpdateSectionUnknown);
	} else if (_nbConnectionTry == 1) {
		PurpleParamSetter::setAccountParams(networkProxy, account, pAccount, true);
		AccountMutator::emitAccountUpdatedSignal(account, EnumUpdateSection::UpdateSectionUnknown);
	} else {
		// _nbConnectionTry > 1
		iConnect.setState(EnumConnectionState::ConnectionStateDisconnected);
		_nbConnectionTry = 0;
		iConnect.execute();
		return;
	}
	_nbConnectionTry++;

	misc_t * data = (misc_t *) malloc(sizeof(misc_t));
	memset(data, 0, sizeof(misc_t));
	data->account = &account;
	data->pAccount = pAccount;

	QMutexLocker lock(_mutex);
	data->cbk_id = purple_timeout_add(0, (GSourceFunc) PurpleConnectingState::executeCbk, data);
}

void PurpleConnectingState::checkConnectionConfiguration() {
	IConnectPlugin &iConnect = static_cast<IConnectPlugin &>(_stateContext);
	Account &account = iConnect.getAccount();

	if (PurpleParamSetter::isAlternateConnectionUsed(account)) {
		_nbConnectionTry = 1;
	}
}
