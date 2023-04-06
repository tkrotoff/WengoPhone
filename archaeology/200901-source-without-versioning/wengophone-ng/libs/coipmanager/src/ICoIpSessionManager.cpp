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

#include <coipmanager/ICoIpSessionManager.h>

#include <coipmanager/CoIpManager.h>
#include <coipmanager/ICoIpSessionManagerPlugin.h>
#include <coipmanager/datamanager/UserProfileManager.h>

ICoIpSessionManager::ICoIpSessionManager(CoIpManager & coIpManager)
	: ICoIpManager(coIpManager) {
}

ICoIpSessionManager::~ICoIpSessionManager() {
}

bool ICoIpSessionManager::canCreateSession(const ContactList & contactList) const {
	bool result = false;

	AccountList accountList = _coIpManager.getUserProfileManager().getCopyOfUserProfile().getAccountList();
	for (AccountList::const_iterator it = accountList.begin();
		it != accountList.end();
		it++) {
		result = canCreateSession(*it, contactList);
		if (result) {
			break;
		}
	}

	return result;
}

bool ICoIpSessionManager::canCreateSession(const Account & account, const ContactList & contactList) const {
	return getICoIpManagerPlugin(account, contactList) != NULL;
}

ICoIpSessionManagerPlugin * ICoIpSessionManager::getICoIpManagerPlugin(const Account & account, const ContactList & contactList) const {
	ICoIpSessionManagerPlugin *result = NULL;

	for (ICoIpManagerPluginList::const_iterator it = _iCoIpManagerPluginList.begin();
		it != _iCoIpManagerPluginList.end();
		++it) {
		ICoIpSessionManagerPlugin * plugin = static_cast<ICoIpSessionManagerPlugin *>(*it);
		if (plugin->canCreateISession(account, contactList)) {
			result = plugin;
			break;
		}
	}

	return result;
}
