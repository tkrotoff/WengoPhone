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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  US
 */

#include "IAXClientCallSession.h"

#include <sipwrapper/SipWrapper.h>

#include <util/Logger.h>

IAXClientCallSession::IAXClientCallSession(CoIpManager & coIpManager, SipWrapper * iaxClientWrapper,
	int callId, EnumPhoneCallState::PhoneCallState state, const std::string & from)
	: SipWrapperCallSession(coIpManager, iaxClientWrapper, callId, state, from) {

	//TODO get the contact from somewhere else
	IMContact contact(EnumAccountType::AccountTypeIAX, from);
	addIMContact(contact);
}

IAXClientCallSession::IAXClientCallSession(CoIpManager & coIpManager, SipWrapper * iaxClientWrapper)
	: SipWrapperCallSession(coIpManager, iaxClientWrapper) {
}

IAXClientCallSession::~IAXClientCallSession() {
}

void IAXClientCallSession::start() {
	if (_isIncoming) {
		if (_state != EnumPhoneCallState::PhoneCallStateRinging) {
			LOG_DEBUG("Call allready answered -> ignore");
			return;
		}
		_sipWrapper->acceptCall(_callId, false);
	}

	//Outgoing
	else {
		/*IAXAccount * iaxAccount = NULL;
		Account account = getAccount();
		if (account.getAccountType() == EnumAccountType::AccountTypeIAX) {
			iaxAccount = static_cast<IAXAccount *>(account.getPrivateAccount());
		} else {
			LOG_FATAL("Account is not a IAXAccount");
		}*/
		//TODO what if multiple contacts??
		_callId = _sipWrapper->makeCall(0, _imContactList.front().getContactId(), false);
	}
}
