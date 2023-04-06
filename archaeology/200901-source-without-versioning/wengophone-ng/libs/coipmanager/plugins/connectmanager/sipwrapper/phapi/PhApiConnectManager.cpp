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

#include "PhApiConnectManager.h"

#include "PhApiForSipConnect.h"
#include "PhApiForWengoConnect.h"

#include <coipmanager_base/contact/ContactList.h>

#include <util/Logger.h>

#include <coipmanager/coipmanagerplugindll.h>
COIPMANAGER_PLUGIN_API IConnectManagerPlugin * coip_connect_plugin_init(CoIpManager & coIpManager) {
	return new PhApiConnectManager(coIpManager);
}

PhApiConnectManager::PhApiConnectManager(CoIpManager & coIpManager)
	: IConnectManagerPlugin(coIpManager) {

	_supportedAccountType.push_back(EnumAccountType::AccountTypeWengo);
	_supportedAccountType.push_back(EnumAccountType::AccountTypeSIP);
}

PhApiConnectManager::~PhApiConnectManager() {
}

IConnectPlugin * PhApiConnectManager::createIConnectPlugin(const Account & account) {
	IConnectPlugin * result = NULL;

	switch (account.getAccountType()) {
	case EnumAccountType::AccountTypeWengo:
		result = new PhApiForWengoConnect(_coIpManager, account);
		break;
	case EnumAccountType::AccountTypeSIP:
		result = new PhApiForSipConnect(_coIpManager, account);
		break;
	default:
		LOG_FATAL("accountType not supported");
	}

	return result;
}
