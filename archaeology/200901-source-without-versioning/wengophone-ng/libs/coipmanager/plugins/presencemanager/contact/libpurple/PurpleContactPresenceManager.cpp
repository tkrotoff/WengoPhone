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

#include "PurpleContactPresenceManager.h"

#include <coipmanager/CoIpManager.h>
#include <coipmanager/datamanager/UserProfileManager.h>

#include <purplewrapper/PurpleEnumIMProtocol.h>
#include <purplewrapper/PurpleWrapper.h>

#include <util/SafeDelete.h>

extern "C" {
#include <libpurple/account.h>
#include <libpurple/privacy.h>
}

struct AuthRequest_s {
	PurpleAccountRequestAuthorizationCb accept_cb;
	PurpleAccountRequestAuthorizationCb deny_cb;
	void * user_data;
};

typedef struct misc_s
{
	std::string contact;
	Account * account;
	void * data_ptr;
	int data_int;
	int cbk_id;
}	misc_t;

PurpleAccountUiOps accountUiOps =
{
	NULL, // notify_added
	NULL, // status_changed
	NULL, // request_add
	PurpleContactPresenceManager::authorizationRequestCbk,
	NULL, // close_account_request
	NULL, // purple_reserved1
	NULL, // purple_reserved2
	NULL, // purple_reserved3
	NULL, // purple_reserved4
};

QMutex * PurpleContactPresenceManager::_mutex = NULL;
PurpleContactPresenceManager * PurpleContactPresenceManager::_instance = NULL;
std::map<const std::string, AuthRequest *> PurpleContactPresenceManager::_authRequestMap;


IContactPresenceManagerPlugin * coip_contactpresence_plugin_init(CoIpManager & coIpManager) {
	return new PurpleContactPresenceManager(coIpManager);
}

PurpleContactPresenceManager::PurpleContactPresenceManager(CoIpManager & coIpManager)
	: IContactPresenceManagerPlugin(coIpManager) {

	_instance = this;
	_mutex = new QMutex();
	PurpleWrapper::getInstance().setAccountCallbacks(&accountUiOps);
}

PurpleContactPresenceManager::~PurpleContactPresenceManager() {
	_instance = NULL;
	OWSAFE_DELETE(_mutex);
}

bool PurpleContactPresenceManager::blockContactCbk(void * data) {
	QMutexLocker lock(PurpleContactPresenceManager::_mutex);
	Account * account = ((misc_t *)data)->account;
	const std::string contact = ((misc_t *)data)->contact;

	PurpleAccount * pAccount = purple_accounts_find(PurpleWrapper::purpleLogin(account).c_str(),
		PurpleIMPrcl::getPrclId(account->getAccountType()));

	if (pAccount) {
		purple_privacy_deny_add(pAccount, contact.c_str(), FALSE);
	}
	OWSAFE_DELETE(account);
	
	purple_timeout_remove(((misc_t *)data)->cbk_id);
	free((misc_t *)data);

	return TRUE;
}

void PurpleContactPresenceManager::blockContact(const IMContact & imContact) {
	AccountList accountList = _coIpManager.getUserProfileManager().getCopyOfUserProfile().getAccountList();
	Account * account = AccountListHelper::getCopyOfAccount(accountList, imContact.getAccountId());

	if (account == NULL) {
		return;
	}

	misc_t * data = (misc_t *) malloc(sizeof(misc_t));
	memset(data, 0, sizeof(misc_t));
	data->account = account;
	data->contact = imContact.getContactId();

	QMutexLocker lock(PurpleContactPresenceManager::_mutex);
	data->cbk_id = purple_timeout_add(0, (GSourceFunc) PurpleContactPresenceManager::blockContactCbk, data);
}

bool PurpleContactPresenceManager::unblockContactCbk(void * data) {
	QMutexLocker lock(PurpleContactPresenceManager::_mutex);
	Account * account = ((misc_t *)data)->account;
	const std::string contact = ((misc_t *)data)->contact;

	PurpleAccount * pAccount = purple_accounts_find(PurpleWrapper::purpleLogin(account).c_str(),
		PurpleIMPrcl::getPrclId(account->getAccountType()));

	if (pAccount) {
		purple_privacy_permit_add(pAccount, contact.c_str(), FALSE);
	}
	OWSAFE_DELETE(account);
	
	purple_timeout_remove(((misc_t *)data)->cbk_id);
	free((misc_t *)data);

	return TRUE;
}

void PurpleContactPresenceManager::unblockContact(const IMContact & imContact) {
	AccountList accountList = _coIpManager.getUserProfileManager().getCopyOfUserProfile().getAccountList();
	Account * account = AccountListHelper::getCopyOfAccount(accountList, imContact.getAccountId());

	if (account == NULL) {
		return;
	}

	misc_t * data = (misc_t *) malloc(sizeof(misc_t));
	memset(data, 0, sizeof(misc_t));
	data->account = account;
	data->contact = imContact.getContactId();

	QMutexLocker lock(PurpleContactPresenceManager::_mutex);
	data->cbk_id = purple_timeout_add(0, (GSourceFunc) PurpleContactPresenceManager::unblockContactCbk, data);
}

bool PurpleContactPresenceManager::authorizeContactCbk(void * data) {
	QMutexLocker lock(PurpleContactPresenceManager::_mutex);
	Account * account = ((misc_t *)data)->account;
	const std::string contactId = ((misc_t *)data)->contact;
	int authorized = ((misc_t *)data)->data_int;
	PurpleAccount * pAccount = purple_accounts_find(PurpleWrapper::purpleLogin(account).c_str(),
		PurpleIMPrcl::getPrclId(account->getAccountType()));

	AuthRequest * authReq = PurpleContactPresenceManager::getAuthRequest(contactId);

	if (pAccount && authReq) {
		if (authorized) {
			authReq->accept_cb(authReq->user_data);
		} else {
			authReq->deny_cb(authReq->user_data);
		}

		delete authReq;
	}

	OWSAFE_DELETE(account);

	purple_timeout_remove(((misc_t *)data)->cbk_id);

	free(((misc_t *)data)->data_ptr);
	free((misc_t *)data);

	return TRUE;
}

void PurpleContactPresenceManager::authorizeContact(const IMContact & imContact,
	bool authorized, const std::string & message) {
	AccountList accountList = _coIpManager.getUserProfileManager().getCopyOfUserProfile().getAccountList();
	Account * account = AccountListHelper::getCopyOfAccount(accountList, imContact.getAccountId());

	if (!account) {
		return;
	}

	misc_t * data = (misc_t *) malloc(sizeof(misc_t));
	memset(data, 0, sizeof(misc_t));
	data->account = account;
	data->contact = imContact.getContactId();
	if (message.empty()) {
		data->data_ptr = strdup("");
	} else {
		data->data_ptr = strdup(message.c_str());
	}
	data->data_int = authorized;

	QMutexLocker lock(PurpleContactPresenceManager::_mutex);
	data->cbk_id = purple_timeout_add(0, (GSourceFunc) PurpleContactPresenceManager::authorizeContactCbk, data);
}


void PurpleContactPresenceManager::addAuthRequest(const std::string &contactId,
	PurpleAccountRequestAuthorizationCb acceptCbk, PurpleAccountRequestAuthorizationCb denyCbk, void *userData)
{
	AuthRequest * authReq = new AuthRequest();

	authReq->accept_cb = (PurpleAccountRequestAuthorizationCb) acceptCbk;
	authReq->deny_cb = (PurpleAccountRequestAuthorizationCb) denyCbk;
	authReq->user_data = userData;

	_authRequestMap[contactId] = authReq;
}

AuthRequest * PurpleContactPresenceManager::getAuthRequest(const std::string contactId) {
	AuthRequest * authReq = NULL;

	if (_authRequestMap.find(contactId) != _authRequestMap.end()) {
		authReq = _authRequestMap[contactId];
		_authRequestMap.erase(contactId);
	}

	return authReq;
}

void * PurpleContactPresenceManager::authorizationRequestCbk(PurpleAccount * pAccount, 
	const char * remoteUser, const char * id, const char * alias, const char * message, 
	gboolean onList, PurpleAccountRequestAuthorizationCb authorizeCbk, 
	PurpleAccountRequestAuthorizationCb denyCbk, void * userData) {

	UserProfile userProfile = _instance->_coIpManager.getUserProfileManager().getCopyOfUserProfile();
	ContactList contactList = userProfile.getContactList();
	AccountList accountList = userProfile.getAccountList();
	EnumAccountType::AccountType accountType = PurpleIMPrcl::getEnumIMProtocol(pAccount);
	Account * account = AccountListHelper::getCopyOfAccount(accountList, 
		PurpleWrapper::cleanLogin(pAccount), accountType);
	
	if (!account) {
		return NULL;
	}

	addAuthRequest(std::string(remoteUser), authorizeCbk, denyCbk, userData);
	IMContact imContact(accountType, remoteUser);
	imContact.setAccountId(account->getUUID());
	_instance->emitAuthorizationRequested(imContact, std::string(message != NULL ? message : ""));

	return NULL;
}
