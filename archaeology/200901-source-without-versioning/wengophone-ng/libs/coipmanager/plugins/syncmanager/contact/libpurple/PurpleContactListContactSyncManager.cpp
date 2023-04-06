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

#include "PurpleContactListContactSyncManager.h"

#include <coipmanager/CoIpManager.h>
#include <coipmanager/datamanager/UserProfileManager.h>

#include <purplewrapper/PurpleEnumIMProtocol.h>
#include <purplewrapper/PurpleEnumPresenceState.h>
#include <purplewrapper/PurpleWrapper.h>

#include <util/Logger.h>
#include <util/SafeDelete.h>

IIMContactSyncManagerPlugin * coip_imcontactsync_plugin_init(CoIpManager & coIpManager) {
	return new PurpleContactListContactSyncManager(coIpManager);
}

static PurpleBlistUiOps blistUiOps = {
	PurpleContactListContactSyncManager::newListCbk,
	PurpleContactListContactSyncManager::newNodeCbk,
	PurpleContactListContactSyncManager::showCbk,
	PurpleContactListContactSyncManager::updateCbk,
	PurpleContactListContactSyncManager::removeCbk,
	PurpleContactListContactSyncManager::destroyCbk,
	PurpleContactListContactSyncManager::setVisibleCbk,
	PurpleContactListContactSyncManager::requestAddBuddyCbk,
	PurpleContactListContactSyncManager::requestAddChatCbk,
	PurpleContactListContactSyncManager::requestAddGroupCbk,
	NULL, // purple_reserved1
	NULL, // purple_reserved2
	NULL, // purple_reserved3
	NULL, // purple_reserved4
};

PurpleContactListContactSyncManager * PurpleContactListContactSyncManager::_instance = NULL;

PurpleContactListContactSyncManager::PurpleContactListContactSyncManager(CoIpManager & coIpManager)
	: IIMContactSyncManagerPlugin(coIpManager) {

	_instance = this;

	PurpleWrapper::getInstance().setBuddyListCallbacks(&blistUiOps);
}

PurpleContactListContactSyncManager::~PurpleContactListContactSyncManager() {
	_instance = NULL;
}

void PurpleContactListContactSyncManager::initialize() {
	purple_set_blist(purple_blist_new());
	purple_blist_init();
	purple_blist_load();

	initContactEvent();
}

void PurpleContactListContactSyncManager::initContactEvent() {
	void * handle = PurpleWrapper::getInstance().getHandle();

	purple_signal_connect(purple_blist_get_handle(), "buddy-icon-changed",
		handle, PURPLE_CALLBACK(PurpleContactListContactSyncManager::updateBuddyIcon), NULL);
}

void PurpleContactListContactSyncManager::updateBuddyIcon(PurpleBuddy * buddy) {
	LOG_DEBUG("PurpleContactListContactSyncManager: updateBuddyIcon()");

	size_t size = 0;
	const char * data = (const char *) purple_buddy_icon_get_data(buddy->icon, &size);

	if (data && size > 0) {
		OWPicture picture = OWPicture::pictureFromData(std::string(data, size));

		PurpleAccount *pAccount = purple_buddy_get_account(buddy);
		EnumAccountType::AccountType accountType = PurpleIMPrcl::getEnumIMProtocol(pAccount);

		IMContact imContact(accountType, purple_buddy_get_name(buddy));
		UserProfile userProfile = _instance->_coIpManager.getUserProfileManager().getCopyOfUserProfile();
		ContactList contactList = userProfile.getContactList();
		if (!ContactListHelper::getIMContact(contactList, imContact)) {
			AccountList accountList = userProfile.getAccountList();
			Account * account = AccountListHelper::getCopyOfAccount(accountList, 
				PurpleWrapper::cleanLogin(pAccount), accountType);
			if (account) {
				imContact.setAccountId(account->getUUID());
				OWSAFE_DELETE(account);
			} else {
				LOG_ERROR("cannot find associated Account");
				return;
			}
		}
		imContact.setIcon(picture);

		_instance->emitIMContactUpdatedSignal(imContact);
	}
}

void PurpleContactListContactSyncManager::newListCbk(PurpleBuddyList * blist) {
	LOG_DEBUG("PurpleContactListContactSyncManager: newListCbk()");
}

void PurpleContactListContactSyncManager::newBuddyAdded(PurpleBuddy * buddy) {
	LOG_DEBUG("PurpleContactListContactSyncManager: newBuddyAdded()");

	PurpleAccount * pAccount = purple_buddy_get_account(buddy);
	EnumAccountType::AccountType accountType = PurpleIMPrcl::getEnumIMProtocol(pAccount);

	IMContact imContact(accountType, purple_buddy_get_name(buddy));
	UserProfile userProfile = _instance->_coIpManager.getUserProfileManager().getCopyOfUserProfile();
	ContactList contactList = userProfile.getContactList();
	if (!ContactListHelper::getIMContact(contactList, imContact)) {
		AccountList accountList = userProfile.getAccountList();
		Account * account = AccountListHelper::getCopyOfAccount(accountList, 
			PurpleWrapper::cleanLogin(pAccount), accountType);
		if (account) {
			imContact.setAccountId(account->getUUID());
			OWSAFE_DELETE(account);

			/*/const char *groupName = FindBuddyGroup(buddy);
			if (groupName) {

			}*/
		} else {
			LOG_ERROR("cannot find associated Account");
			return;
		}
	}

	_instance->emitIMContactAddedSignal(imContact);
}

void PurpleContactListContactSyncManager::newGroupAdded(PurpleGroup * pGroup) {
	LOG_DEBUG("PurpleContactListContactSyncManager: newGroupAdded()");

	//TODO: groupAddedEvent(_instance, pGroup->name);
}

void PurpleContactListContactSyncManager::newNodeCbk(PurpleBlistNode * node) {
	LOG_DEBUG("PurpleContactListContactSyncManager: newNodeCbk()");

	switch (node->type) {
	case PURPLE_BLIST_BUDDY_NODE:
		newBuddyAdded((PurpleBuddy *) node);
		break;

	case PURPLE_BLIST_GROUP_NODE:
		newGroupAdded((PurpleGroup *) node);
		break;

	case PURPLE_BLIST_CONTACT_NODE:
		break;

	case PURPLE_BLIST_CHAT_NODE:
		break;

	default:
		break;
	}
}

void PurpleContactListContactSyncManager::showCbk(PurpleBuddyList * list) {
	LOG_DEBUG("PurpleContactListContactSyncManager : showCbk()");
}

void PurpleContactListContactSyncManager::updateBuddy(PurpleBuddyList * list, PurpleBuddy * buddy) {
	LOG_DEBUG("PurpleContactListContactSyncManager : updateBuddy()");

	PurpleAccount * pAccount = purple_buddy_get_account(buddy);
	EnumAccountType::AccountType accountType = PurpleIMPrcl::getEnumIMProtocol(pAccount);

	IMContact imContact(accountType, purple_buddy_get_name(buddy));
	UserProfile userProfile = _instance->_coIpManager.getUserProfileManager().getCopyOfUserProfile();
	ContactList contactList = userProfile.getContactList();
	if (!ContactListHelper::getIMContact(contactList, imContact)) {
		AccountList accountList = userProfile.getAccountList();
		Account * account = AccountListHelper::getCopyOfAccount(accountList, 
			PurpleWrapper::cleanLogin(pAccount), accountType);
		if (account) {
			imContact.setAccountId(account->getUUID());
			OWSAFE_DELETE(account);
		} else {
			LOG_ERROR("cannot find associated Account");
			return;
		}
	}

	// Setting group
	PurpleGroup * pGroup = purple_buddy_get_group(buddy);
	if (pGroup) {
		const char * groupName = pGroup->name;
		if (groupName) {
			//TODO:
			//imContact.setGroup(groupName);
		}
	}
	////

	// Setting PresenceState
	PurplePresence * gPresence = purple_buddy_get_presence(buddy);
	PurpleStatus * gStatus = purple_presence_get_active_status(gPresence);
	PurpleStatusType * gStatusType = purple_status_get_type(gStatus);
	PurpleStatusPrimitive gStatusPrim = purple_status_type_get_primitive(gStatusType);
	const char * gPresenceId = purple_primitive_get_id_from_type(gStatusPrim);

	imContact.setPresenceState(PurplePreState::getPresenceState(gPresenceId));
	////

	// Setting Alias
	const char * buddy_alias =
		(buddy->server_alias && *buddy->server_alias) ? buddy->server_alias : buddy->alias;
	if (buddy_alias) {
		imContact.setAlias(buddy_alias);
	}
	////

	_instance->emitIMContactUpdatedSignal(imContact);
}

void PurpleContactListContactSyncManager::updateCbk(PurpleBuddyList * list, PurpleBlistNode * node) {
	LOG_DEBUG("PurpleContactListContactSyncManager : updateCbk()");

	switch (node->type) {
	case PURPLE_BLIST_BUDDY_NODE:
		updateBuddy(list, (PurpleBuddy *) node);
		break;

	default:
		break;
	}
}

void PurpleContactListContactSyncManager::removeCbk(PurpleBuddyList * list, PurpleBlistNode * node) {
	LOG_DEBUG("PurpleContactListMngr : removeCbk()");
}

void PurpleContactListContactSyncManager::destroyCbk(PurpleBuddyList * list) {
	LOG_DEBUG("PurpleContactListMngr : destroyCbk()");
}

void PurpleContactListContactSyncManager::setVisibleCbk(PurpleBuddyList * list, gboolean show) {
	LOG_DEBUG("PurpleContactListMngr : setVisibleCbk()");
}

void PurpleContactListContactSyncManager::requestAddBuddyCbk(PurpleAccount * account, const char * username,
	const char * group, const char * alias) {
	LOG_DEBUG("PurpleContactListMngr : requestAddBuddyCbk()");
}

void PurpleContactListContactSyncManager::requestAddChatCbk(PurpleAccount * account, PurpleGroup * group,
	const char * alias, const char * name) {
	LOG_DEBUG("PurpleContactListMngr : requestAddChatCbk()");
}

void PurpleContactListContactSyncManager::requestAddGroupCbk(void) {
	LOG_DEBUG("PurpleContactListMngr : requestAddGroupCbk()");
}

void PurpleContactListContactSyncManager::add(const IMContact & imContact) {
}

void PurpleContactListContactSyncManager::remove(const IMContact & imContact) {
}

void PurpleContactListContactSyncManager::update(const IMContact & imContact) {
}
