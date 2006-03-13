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

#include "Presence.h"

#include <model/WengoPhone.h>
#include <model/contactlist/ContactList.h>
#include <model/presence/MyPresenceState.h>
#include <model/presence/MyPresenceStateOffline.h>

#include <imwrapper/IMWrapperFactory.h>

#include <Logger.h>

Presence::Presence(IMAccount & imAccount)
	: _imAccount(imAccount) {

	_imPresence = IMWrapperFactory::getFactory().createIMPresence(_imAccount);

	_imPresence->presenceStateChangedEvent += presenceStateChangedEvent;
	_imPresence->myPresenceStatusEvent += myPresenceStatusEvent;
	_imPresence->subscribeStatusEvent += subscribeStatusEvent;

	_state = MyPresenceStateOffline::getInstance();
}

Presence::~Presence() {

}

void Presence::changeMyPresence(EnumPresenceState::PresenceState state, const std::string & note) {
	setState(state);

	_imPresence->changeMyPresence(state, note);
}

void Presence::subscribeToPresenceOf(const std::string & contactId) {
	_imPresence->subscribeToPresenceOf(contactId);
}

void Presence::blockContact(const std::string & contactId) {
	_imPresence->blockContact(contactId);
}

void Presence::unblockContact(const std::string & contactId) {
	_imPresence->unblockContact(contactId);
}

void Presence::setState(EnumPresenceState::PresenceState state) {
	switch(state) {
	case EnumPresenceState::PresenceStateOnline:
		_state = _state->online();
		break;
	case EnumPresenceState::PresenceStateOffline:
		_state = _state->offline();
		break;
	case EnumPresenceState::PresenceStateAway:
		_state = _state->away();
		break;
	case EnumPresenceState::PresenceStateDoNotDisturb:
		_state = _state->doNotDisturb();
		break;
	case EnumPresenceState::PresenceStateUserDefined:
		_state = _state->userDefined();
		break;
	/*
	case EnumPresenceState::PresenceStateUnknown:
		_state = _state->unknown();
		break;
	*/
	default:
		LOG_FATAL("EnumPresenceState::PresenceState unknown state");
	}
}
