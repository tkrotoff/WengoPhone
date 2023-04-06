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

#include <coipmanager_threaded/datamanager/TAccountManager.h>

#include <coipmanager_threaded/TCoIpManager.h>

#include <coipmanager/CoIpManager.h>
#include <coipmanager/datamanager/UserProfileManager.h>

#include <util/SafeConnect.h>

TAccountManager::TAccountManager(TCoIpManager & tCoIpManager)
	: _tCoIpManager(tCoIpManager) {

	AccountManager * accountManager = getAccountManager();

	SAFE_CONNECT(accountManager, SIGNAL(accountAddedSignal(std::string)),
		SIGNAL(accountAddedSignal(std::string)));
	SAFE_CONNECT(accountManager, SIGNAL(accountRemovedSignal(std::string)),
		SIGNAL(accountRemovedSignal(std::string)));
	SAFE_CONNECT(accountManager, SIGNAL(accountUpdatedSignal(std::string, EnumUpdateSection::UpdateSection)),
		SIGNAL(accountUpdatedSignal(std::string, EnumUpdateSection::UpdateSection)));
}

TAccountManager::~TAccountManager() {
}

AccountManager * TAccountManager::getAccountManager() {
	return &_tCoIpManager.getCoIpManager().getUserProfileManager().getAccountManager();
}

bool TAccountManager::add(const Account & account) {
	return getAccountManager()->add(account);
}

bool TAccountManager::remove(const std::string & accountId) {
	return getAccountManager()->remove(accountId);
}

bool TAccountManager::update(const Account & account, EnumUpdateSection::UpdateSection section) {
	return getAccountManager()->update(account, section);
}
