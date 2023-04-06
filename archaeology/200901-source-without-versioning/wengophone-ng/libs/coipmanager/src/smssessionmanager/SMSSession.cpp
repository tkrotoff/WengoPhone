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

#include <coipmanager/smssessionmanager/SMSSession.h>

#include <coipmanager/CoIpManager.h>
#include <coipmanager/datamanager/UserProfileManager.h>
#include <coipmanager/smssessionmanager/ISMSSessionPlugin.h>
#include <coipmanager/smssessionmanager/SMSSessionManager.h>

#include <coipmanager_base/account/AccountList.h>

#include <util/Logger.h>
#include <util/SafeConnect.h>
#include <util/SafeDelete.h>

SMSSession::SMSSession(SMSSessionManager & smsSessionManager,
	CoIpManager & coIpManager)
	: Session(coIpManager) {
	_currentISMSSession = NULL;
}

SMSSession::~SMSSession() {
	OWSAFE_DELETE(_currentISMSSession);
}

void SMSSession::start() {
	QMutexLocker lock(_mutex);

	// If a ISMSSession is running, do nothing
	if (_currentISMSSession) {
		LOG_ERROR("SMSSession already running");
		return;
	}

	if (_accountToUse.empty()) {
		// User did not select an account to send the SMS.
		// Select the first available account for sending message.
		ContactList fakeContactList;
		_accountToUse = getFirstUsableAccount(fakeContactList);
	}

	AccountList accountList = _coIpManager.getUserProfileManager().getCopyOfUserProfile().getAccountList();
	Account * account = AccountListHelper::getCopyOfAccount(accountList, _accountToUse);
	if (account) {
		ISMSSessionManagerPlugin *iSMSSessionManager =
			_coIpManager.getSMSSessionManager().getICoIpManagerPlugin(*account, _contactList);
		if (iSMSSessionManager) {
			_currentISMSSession = iSMSSessionManager->createISMSSessionPlugin();
			_currentISMSSession->setPhoneNumberList(_phoneNumberList);
			_currentISMSSession->setMessage(_smsMessage);
			_currentISMSSession->setAccount(*account);
			SAFE_CONNECT(_currentISMSSession,
				SIGNAL(smsStateSignal(std::string, EnumSMSState::SMSState)),
				SLOT(smsStateSlot(std::string, EnumSMSState::SMSState)));
			_currentISMSSession->start();
		}
		OWSAFE_DELETE(account);
	} else {
		LOG_ERROR("couldn't find a common Account");
	}
}

void SMSSession::stop() {
}

void SMSSession::pause() {
}

void SMSSession::resume() {
}

void SMSSession::smsStateSlot(std::string phoneNumber, EnumSMSState::SMSState state) {
	smsStateSignal(phoneNumber, state);
}

EnumSessionError::SessionError SMSSession::addContact(const Contact & contact) {
	return EnumSessionError::SessionErrorInvalidParameter;
}

void SMSSession::removeContact(const std::string & contactId) {
}

bool SMSSession::canCreateISession(const Account & account, const ContactList & contactList) const {
	return (_coIpManager.getSMSSessionManager().getICoIpManagerPlugin(account, contactList) != NULL);
}
