/*
 * WengoPhone, a voice over Internet phone
 * Copyright (C) 2004-2007 Wengo
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#include "WengoSMSSessionManager.h"

#include "WengoSMSSession.h"

#include <coipmanager/CoIpManager.h>

ISMSSessionManagerPlugin * coip_smssession_plugin_init(CoIpManager & coIpManager) {
	return new WengoSMSSessionManager(coIpManager);
}

WengoSMSSessionManager::WengoSMSSessionManager(CoIpManager & coIpManager)
	: ISMSSessionManagerPlugin(coIpManager) {
}

WengoSMSSessionManager::~WengoSMSSessionManager() {
}

ISMSSessionPlugin * WengoSMSSessionManager::createISMSSessionPlugin() {
	NetworkProxy networkProxy;
	networkProxy.setServer(_coIpManager.getCoIpManagerConfig().getHttpProxyServer());
	networkProxy.setServerPort(_coIpManager.getCoIpManagerConfig().getHttpProxyServerPort());
	networkProxy.setLogin(_coIpManager.getCoIpManagerConfig().getHttpProxyLogin());
	networkProxy.setPassword(_coIpManager.getCoIpManagerConfig().getHttpProxyPassword());

	return new WengoSMSSession(_coIpManager, networkProxy);
}

bool WengoSMSSessionManager::canCreateISession(const Account & account,
	const ContactList & contactList) const {

	return account.getAccountType() == EnumAccountType::AccountTypeWengo;
}

IMContactList WengoSMSSessionManager::getValidIMContacts(const Account & account,
	const ContactList & contactList) const {
	return IMContactList();
}
