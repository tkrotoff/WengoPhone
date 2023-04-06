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

#include "SipWrapperBasedAccountPresenceManager.h"

#include <coipmanager_base/account/Account.h>
#include <coipmanager_base/account/SipAccount.h>

#include <sipwrapper/SipWrapper.h>

#include <util/Logger.h>

SipWrapperBasedAccountPresenceManager::SipWrapperBasedAccountPresenceManager(CoIpManager & coIpManager)
	: IAccountPresenceManagerPlugin(coIpManager) {

	_sipWrapper = NULL;
}

void SipWrapperBasedAccountPresenceManager::setPresenceState(const Account & account,
	EnumPresenceState::PresenceState state, const std::string & note) {

	SipAccount * sipAccount = static_cast<SipAccount *>(account.getPrivateAccount());

	switch(state) {
	case EnumPresenceState::PresenceStateOnline:
		_sipWrapper->publishPresence(sipAccount->getVirtualLineId(), true, SipWrapper::PresenceStateOnline);
		break;

	case EnumPresenceState::PresenceStateAway:
		_sipWrapper->publishPresence(sipAccount->getVirtualLineId(), true, SipWrapper::PresenceStateAway);
		break;

	case EnumPresenceState::PresenceStateDoNotDisturb:
		_sipWrapper->publishPresence(sipAccount->getVirtualLineId(), true, SipWrapper::PresenceStateDoNotDisturb);
		break;

	case EnumPresenceState::PresenceStateUserDefined:
		_sipWrapper->publishPresence(sipAccount->getVirtualLineId(), true, note);
		break;

	case EnumPresenceState::PresenceStateInvisible:
	case EnumPresenceState::PresenceStateOffline:
		_sipWrapper->publishPresence(sipAccount->getVirtualLineId(), false, note);
		break;

	case EnumPresenceState::PresenceStateUnknown:
		//No Presence to Publish
		break;

	default:
		LOG_FATAL("unknown presence state=" + String::fromNumber(state));
	}
}

void SipWrapperBasedAccountPresenceManager::setAlias(const Account & account, const std::string & alias) {
	setPresenceState(account, EnumPresenceState::PresenceStateUserDefined, alias);
}

void SipWrapperBasedAccountPresenceManager::setIcon(const Account & account, const OWPicture & picture) {
	//SipAccount * sipAccount = static_cast<SipAccount *>(account.getPrivateAccount());
}
