/*
 * WengoPhone, a voice over Internet phone
 * Copyright (C) 2004-2006  Wengo
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

#include "PhApiIMPresence.h"

#include "PhApiWrapper.h"

#include <util/Picture.h>

PhApiIMPresence::PhApiIMPresence(IMAccount & account, PhApiWrapper & phApiWrapper)
	: IMPresence(account),
	_phApiWrapper(phApiWrapper) {

	_phApiWrapper.presenceStateChangedEvent += boost::bind(&PhApiIMPresence::presenceStateChangedEventHandler, this, _1, _2, _3, _4);
	_phApiWrapper.myPresenceStatusEvent += boost::bind(&PhApiIMPresence::myPresenceStatusEventHandler, this, _1, _2, _3);
	_phApiWrapper.subscribeStatusEvent += boost::bind(&PhApiIMPresence::subscribeStatusEventHandler, this, _1, _2, _3);
}

void PhApiIMPresence::changeMyPresence(EnumPresenceState::PresenceState state, const std::string & note) {
	_phApiWrapper.changeMyPresence(state, note);
}

void PhApiIMPresence::changeMyAlias(const std::string & nickname) {
}

void PhApiIMPresence::changeMyIcon(const Picture & picture) {
}

void PhApiIMPresence::subscribeToPresenceOf(const std::string & contactId) {
	_phApiWrapper.subscribeToPresenceOf(contactId);
}

void PhApiIMPresence::blockContact(const std::string & contactId) {
	_phApiWrapper.blockContact(contactId);
}

void PhApiIMPresence::unblockContact(const std::string & contactId) {
	_phApiWrapper.unblockContact(contactId);
}

void PhApiIMPresence::presenceStateChangedEventHandler(PhApiWrapper & sender, EnumPresenceState::PresenceState state, const std::string & note, const std::string & from) {
	presenceStateChangedEvent(*this, state, note, from);
}

void PhApiIMPresence::myPresenceStatusEventHandler(PhApiWrapper & sender, EnumPresenceState::MyPresenceStatus status, const std::string & note) {
	myPresenceStatusEvent(*this, status, note);
}

void PhApiIMPresence::subscribeStatusEventHandler(PhApiWrapper & sender, const std::string & contactId, IMPresence::SubscribeStatus status) {
	subscribeStatusEvent(*this, contactId, status);
}

Picture PhApiIMPresence::getContactIcon(const std::string &) {
	return Picture();
}
