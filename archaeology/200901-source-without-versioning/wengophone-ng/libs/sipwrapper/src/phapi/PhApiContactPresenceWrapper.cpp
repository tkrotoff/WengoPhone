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

#include "PhApiContactPresenceWrapper.h"

#include <phapi.h>

PhApiContactPresenceWrapper::PhApiContactPresenceWrapper()
	: SipContactPresenceWrapper() {
}

PhApiContactPresenceWrapper::~PhApiContactPresenceWrapper() {
}

void PhApiContactPresenceWrapper::blockContact(int lineId, const std::string & contactId) {
}

void PhApiContactPresenceWrapper::unblockContact(int lineId, const std::string & contactId) {
}

void PhApiContactPresenceWrapper::authorizeContact(int lineId, const std::string & contactId,
	bool auhorized, const std::string message) {
}

void PhApiContactPresenceWrapper::subscribeToPresenceOf(int lineId, const std::string & contactId) {
	OWPL_SUB hSub;
	owplPresenceSubscribe(lineId, contactId.c_str(), 0, &hSub);
}

void PhApiContactPresenceWrapper::unsubscribeToPresenceOf(int lineId, const std::string & contactId) {
	owplPresenceUnsubscribeFromUri(lineId, contactId.c_str());
}
