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

#include <sipwrapper/SipWrapper.h>

#include <sipwrapper/SipChatWrapper.h>
#include <sipwrapper/SipContactPresenceWrapper.h>

#include <util/SafeDelete.h>

#include <QtCore/QMetaType>

const std::string SipWrapper::PresenceStateOnline = "online";
const std::string SipWrapper::PresenceStateAway = "away";
const std::string SipWrapper::PresenceStateDoNotDisturb = "do not disturb";
const std::string SipWrapper::PresenceStateBusy = "busy";

static bool qtMetaTypeDeclared = false;
static void declareQtMetaType() {
	if (!qtMetaTypeDeclared) {
		qtMetaTypeDeclared = true;

		//SipWrapper classes
		qRegisterMetaType<EnumPhoneCallState::PhoneCallState>("EnumPhoneCallState::PhoneCallState");
		qRegisterMetaType<EnumPhoneLineState::PhoneLineState>("EnumPhoneLineState::PhoneLineState");
	}
}

SipWrapper::SipWrapper() {
	_sipChatWrapper = NULL;
	_sipContactPresenceWrapper = NULL;

	declareQtMetaType();
}

SipWrapper::~SipWrapper() {
	OWSAFE_DELETE(_sipContactPresenceWrapper);
	OWSAFE_DELETE(_sipChatWrapper);
}

SipChatWrapper * SipWrapper::getSipChatWrapper() const {
	return _sipChatWrapper;
}

SipContactPresenceWrapper * SipWrapper::getSipContactPresenceWrapper() const {
	return _sipContactPresenceWrapper;
}
