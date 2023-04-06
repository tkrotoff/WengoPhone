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

#include <coipmanager_threaded/smssessionmanager/TSMSSession.h>

#include <coipmanager/smssessionmanager/SMSSession.h>

#include <util/SafeConnect.h>

TSMSSession::TSMSSession(TCoIpManager & tCoIpManager, SMSSession * session)
	: TSession(tCoIpManager, session) {

	SAFE_CONNECT(session,
		SIGNAL(smsStateSignal(std::string, EnumSMSState::SMSState)),
		SIGNAL(smsStateSignal(std::string, EnumSMSState::SMSState)));
}

TSMSSession::~TSMSSession() {
}

SMSSession * TSMSSession::getSMSSession() const {
	return static_cast<SMSSession *>(_module);
}

void TSMSSession::setMessage(const std::string & message) {
	getSMSSession()->setMessage(message);
}

void TSMSSession::addPhoneNumber(const std::string & phoneNumber) {
	getSMSSession()->addPhoneNumber(phoneNumber);
}

void TSMSSession::setPhoneNumberList(const StringList & phoneNumberList) {
	getSMSSession()->setPhoneNumberList(phoneNumberList);
}

StringList TSMSSession::getPhoneNumberList() const {
	return getSMSSession()->getPhoneNumberList();
}
