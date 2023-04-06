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

#include <coipmanager/presencemanager/account/AccountPresenceManager.h>

#include <coipmanager/CoIpManager.h>
#include <coipmanager/CoIpManagerPluginLoader.h>
#include <coipmanager/datamanager/UserProfileManager.h>
#include <coipmanager/presencemanager/account/IAccountPresenceManagerPlugin.h>

#include <util/Logger.h>
#include <util/Path.h>
#include <util/SafeDelete.h>
#include <util/SafeConnect.h>

static const std::string ACCOUNTPRESENCEMANAGER_PLUGIN_DIR = "coip-plugins/presencemanager/account/";
static const std::string ACCOUNTPRESENCE_PLUGIN_NAME = "accountpresence";

AccountPresenceManager::AccountPresenceManager(CoIpManager & coIpManager)
	: ICoIpManager(coIpManager) {

	SAFE_CONNECT(&_coIpManager.getUserProfileManager().getAccountManager(),
		SIGNAL(accountUpdatedSignal(std::string, EnumUpdateSection::UpdateSection)),
		SLOT(accountUpdatedSlot(std::string, EnumUpdateSection::UpdateSection)));

	typedef IAccountPresenceManagerPlugin * (*coip_plugin_init_function)(CoIpManager &);

	std::list<void *> pluginList =
		CoIpManagerPluginLoader::loadPlugins(_coIpManager.getCoIpManagerConfig().getCoIpPluginsPath(),
			ACCOUNTPRESENCEMANAGER_PLUGIN_DIR, ACCOUNTPRESENCE_PLUGIN_NAME);

	for (std::list<void *>::const_iterator it = pluginList.begin(); it != pluginList.end(); ++it) {
		coip_plugin_init_function coip_plugin_init = (coip_plugin_init_function) (*it);
		if (coip_plugin_init) {
			IAccountPresenceManagerPlugin * plugin = coip_plugin_init(_coIpManager);
			_iCoIpManagerPluginList.push_back(plugin);
		}
	}
}

AccountPresenceManager::~AccountPresenceManager() {
	uninitialize();
}

void AccountPresenceManager::initialize() {
	ICoIpManager::initialize();
}

void AccountPresenceManager::uninitialize() {
	ICoIpManager::uninitialize();
}

void AccountPresenceManager::accountUpdatedSlot(std::string accountId, EnumUpdateSection::UpdateSection section) {
	AccountList accountList = _coIpManager.getUserProfileManager().getCopyOfUserProfile().getAccountList();
	Account * account = AccountListHelper::getCopyOfAccount(accountList, accountId);
	if (account) {
		for (ICoIpManagerPluginList::const_iterator it = _iCoIpManagerPluginList.begin();
			it != _iCoIpManagerPluginList.end();
			++it) {
			IAccountPresenceManagerPlugin * iAccountPresenceManagerPlugin = static_cast<IAccountPresenceManagerPlugin *>(*it);
			if (iAccountPresenceManagerPlugin->isProtocolSupported(account->getAccountType())) {
				if (section == EnumUpdateSection::UpdateSectionPresenceState) {
					// FIXME: no note given but is it really necessary?
					iAccountPresenceManagerPlugin->setPresenceState(*account, account->getPresenceState());
				} else if (section == EnumUpdateSection::UpdateSectionAlias) {
					iAccountPresenceManagerPlugin->setAlias(*account, account->getAlias());
				} else if (section == EnumUpdateSection::UpdateSectionIcon) {
					iAccountPresenceManagerPlugin->setIcon(*account, account->getIcon());
				}
				break;
			}
		}
		OWSAFE_DELETE(account);
	}
}
