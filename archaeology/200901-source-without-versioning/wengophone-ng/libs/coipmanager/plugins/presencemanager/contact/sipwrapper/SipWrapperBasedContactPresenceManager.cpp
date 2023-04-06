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

#include "SipWrapperBasedContactPresenceManager.h"

#include <coipmanager/CoIpManager.h>
#include <coipmanager/connectmanager/ConnectManager.h>
#include <coipmanager/datamanager/UserProfileManager.h>

#include <coipmanager_base/account/Account.h>
#include <coipmanager_base/account/SipAccount.h>
#include <coipmanager_base/imcontact/SipIMContact.h>
#include <coipmanager_base/imcontact/WengoIMContact.h>

#include <sipwrapper/SipContactPresenceWrapper.h>
#include <sipwrapper/SipWrapper.h>

#include <util/Logger.h>
#include <util/SafeConnect.h>
#include <util/SafeDelete.h>
#include <util/String.h>

SipWrapperBasedContactPresenceManager::SipWrapperBasedContactPresenceManager(CoIpManager & coIpManager)
	: IContactPresenceManagerPlugin(coIpManager), _sipWrapper(NULL) {

	SAFE_CONNECT(&_coIpManager.getConnectManager(),
		SIGNAL(accountConnectionStateSignal(std::string, EnumConnectionState::ConnectionState)),
		SLOT(accountConnectionStateSlot(std::string, EnumConnectionState::ConnectionState)));
}

SipWrapperBasedContactPresenceManager::~SipWrapperBasedContactPresenceManager() {
}

void SipWrapperBasedContactPresenceManager::setSipWrapper(SipWrapper *sipWrapper) {
	_sipWrapper = sipWrapper;

	SAFE_CONNECT(_sipWrapper->getSipContactPresenceWrapper(),
		SIGNAL(presenceStateChangedSignal(EnumPresenceState::PresenceState, const std::string &, const std::string &)),
		SLOT(presenceStateChangedSlot(EnumPresenceState::PresenceState, const std::string &, const std::string &)));
}

void SipWrapperBasedContactPresenceManager::blockContact(const IMContact &imContact) {
	std::string accountId = imContact.getAccountId();
	AccountList accountList = _coIpManager.getUserProfileManager().getCopyOfUserProfile().getAccountList();
	Account *account = AccountListHelper::getCopyOfAccount(accountList, accountId);
	if (account) {
		SipAccount *sipAccount = static_cast<SipAccount *>(account->getPrivateAccount());
		SipIMContact *sipIMContact = static_cast<SipIMContact *>(imContact.getPrivateIMContact());
		_sipWrapper->getSipContactPresenceWrapper()->blockContact(sipAccount->getVirtualLineId(), sipIMContact->getFullIdentity());
		OWSAFE_DELETE(account);
	}
}

void SipWrapperBasedContactPresenceManager::unblockContact(const IMContact &imContact) {
	std::string accountId = imContact.getAccountId();
	AccountList accountList = _coIpManager.getUserProfileManager().getCopyOfUserProfile().getAccountList();
	Account *account = AccountListHelper::getCopyOfAccount(accountList, accountId);
	if (account) {
		SipAccount *sipAccount = static_cast<SipAccount *>(account->getPrivateAccount());
		SipIMContact * sipIMContact = static_cast<SipIMContact *>(imContact.getPrivateIMContact());
		_sipWrapper->getSipContactPresenceWrapper()->unblockContact(sipAccount->getVirtualLineId(), sipIMContact->getFullIdentity());
		OWSAFE_DELETE(account);
	}
}

void SipWrapperBasedContactPresenceManager::authorizeContact(const IMContact &imContact,
	bool authorized, const std::string &message) {

	std::string accountId = imContact.getAccountId();
	AccountList accountList = _coIpManager.getUserProfileManager().getCopyOfUserProfile().getAccountList();
	Account *account = AccountListHelper::getCopyOfAccount(accountList, accountId);
	if (account) {
		SipAccount *sipAccount = static_cast<SipAccount *>(account->getPrivateAccount());
		SipIMContact * sipIMContact = static_cast<SipIMContact *>(imContact.getPrivateIMContact());
		_sipWrapper->getSipContactPresenceWrapper()->authorizeContact(sipAccount->getVirtualLineId(), 
			sipIMContact->getFullIdentity(), authorized, message);
		OWSAFE_DELETE(account);
	}
}

void SipWrapperBasedContactPresenceManager::imContactAdded(const IMContact & imContact) {
	std::string accountId = imContact.getAccountId();
	AccountList accountList = _coIpManager.getUserProfileManager().getCopyOfUserProfile().getAccountList();
	Account *account = AccountListHelper::getCopyOfAccount(accountList, accountId);
	if (account) {
		if (account->getConnectionState() == EnumConnectionState::ConnectionStateConnected) {
			SipAccount *sipAccount = static_cast<SipAccount *>(account->getPrivateAccount());
			SipIMContact * sipIMContact = static_cast<SipIMContact *>(imContact.getPrivateIMContact());
			_sipWrapper->getSipContactPresenceWrapper()->subscribeToPresenceOf(sipAccount->getVirtualLineId(),
				sipIMContact->getFullIdentity());
		}
		OWSAFE_DELETE(account);
	}
}

void SipWrapperBasedContactPresenceManager::imContactRemoved(const IMContact & imContact) {
	std::string accountId = imContact.getAccountId();
	AccountList accountList = _coIpManager.getUserProfileManager().getCopyOfUserProfile().getAccountList();
	Account *account = AccountListHelper::getCopyOfAccount(accountList, accountId);
	if (account) {
		SipAccount *sipAccount = static_cast<SipAccount *>(account->getPrivateAccount());
		SipIMContact * sipIMContact = static_cast<SipIMContact *>(imContact.getPrivateIMContact());
		_sipWrapper->getSipContactPresenceWrapper()->unsubscribeToPresenceOf(sipAccount->getVirtualLineId(),
			sipIMContact->getFullIdentity());
		OWSAFE_DELETE(account);
	}
}

void SipWrapperBasedContactPresenceManager::accountConnectionStateSlot(std::string accountId,
	EnumConnectionState::ConnectionState state) {

	// Subscribe to presence of all Sip/Wengo contacts if connected.
	// Unsubscribe in case of disconnection.
	UserProfile userProfile = _coIpManager.getUserProfileManager().getCopyOfUserProfile();
	AccountList accountList = userProfile.getAccountList();
	Account *account = AccountListHelper::getCopyOfAccount(accountList, accountId);
	if (account) {
		if ((account->getAccountType() == EnumAccountType::AccountTypeWengo)
		 	|| (account->getAccountType() == EnumAccountType::AccountTypeSIP)) {
			switch (state) {
			case EnumConnectionState::ConnectionStateConnected: {
				ContactList contactList = userProfile.getContactList();
				for (ContactList::const_iterator cIt = contactList.begin();
					cIt != contactList.end();
					cIt++) {
					IMContactList imContactList = IMContactListHelper::getCopyOfAssociatedIMContacts((*cIt).getIMContactList(), accountId);
					for (IMContactList::const_iterator imIt = imContactList.begin();
						imIt != imContactList.end();
						++imIt) {
						SipIMContact *sipIMContact = static_cast<SipIMContact *>((*imIt).getPrivateIMContact());
						SipAccount *sipAccount = static_cast<SipAccount *>(account->getPrivateAccount());
						_sipWrapper->getSipContactPresenceWrapper()->subscribeToPresenceOf(sipAccount->getVirtualLineId(), sipIMContact->getFullIdentity());
					}
				}
				break;
			}
			case EnumConnectionState::ConnectionStateDisconnecting: {
				ContactList contactList = userProfile.getContactList();
				for (ContactList::const_iterator cIt = contactList.begin();
					cIt != contactList.end();
					cIt++) {
					IMContactList imContactList = IMContactListHelper::getCopyOfAssociatedIMContacts((*cIt).getIMContactList(), accountId);
					for (IMContactList::const_iterator imIt = imContactList.begin();
						imIt != imContactList.end();
						++imIt) {
						SipIMContact *sipIMContact = static_cast<SipIMContact *>((*imIt).getPrivateIMContact());
						SipAccount *sipAccount = static_cast<SipAccount *>(account->getPrivateAccount());
						_sipWrapper->getSipContactPresenceWrapper()->unsubscribeToPresenceOf(sipAccount->getVirtualLineId(), sipIMContact->getFullIdentity());
					}
				}
				break;
			}
			default:
				break;
			}
		}
		OWSAFE_DELETE(account);
	}
	////
}

void SipWrapperBasedContactPresenceManager::presenceStateChangedSlot(EnumPresenceState::PresenceState state,
	const std::string & alias, const std::string & from) {

	IIMContact *myIMContact = NULL;
	SipPeer sipPeer = SipPeer::fromFullIdentity(from);
	if (sipPeer.getRealm() == WengoIMContact::WENGO_REALM) {
		myIMContact = new WengoIMContact(sipPeer);
	} else {
		myIMContact = new SipIMContact(sipPeer);
	}
	IMContact imContact(myIMContact);
	OWSAFE_DELETE(myIMContact);

	imContact.setPresenceState(state);
	imContact.setAlias(String::null);
	if (state == EnumPresenceState::PresenceStateUserDefined) {
		imContact.setPresenceState(EnumPresenceState::PresenceStateOnline);
		imContact.setAlias(alias);
	}

	imContactUpdated(imContact);
}
