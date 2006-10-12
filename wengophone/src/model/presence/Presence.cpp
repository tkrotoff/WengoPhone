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

#include <util/Logger.h>
#include <util/SafeDelete.h>	

Presence::Presence(IMAccount & imAccount)
	: _imAccount(imAccount) {

	_imPresence = IMWrapperFactory::getFactory().createIMPresence(_imAccount);

	_imPresence->presenceStateChangedEvent += presenceStateChangedEvent;

	//For debug reason
	_imPresence->presenceStateChangedEvent += boost::bind(&Presence::presenceStateChangedEventHandler, this, _1, _2, _3, _4);

	_imPresence->myPresenceStatusEvent += myPresenceStatusEvent;
	_imPresence->subscribeStatusEvent += subscribeStatusEvent;
	_imPresence->authorizationRequestEvent += authorizationRequestEvent;
	_imPresence->contactIconChangedEvent += contactIconChangedEvent;

	_state = MyPresenceStateOffline::getInstance();
}

Presence::~Presence() {
	OWSAFE_DELETE(_imPresence);
}

void Presence::changeMyPresence(EnumPresenceState::PresenceState state, const std::string & note) {
	setState(state);

	_imPresence->changeMyPresence(state, note);
}

void Presence::changeMyAlias(const std::string & alias) {
	_imPresence->changeMyAlias(alias);
}

void Presence::changeMyIcon(const OWPicture & picture) {
	_imPresence->changeMyIcon(picture);
}

void Presence::subscribeToPresenceOf(const std::string & contactId) {
	_imPresence->subscribeToPresenceOf(contactId);
}

void Presence::unsubscribeToPresenceOf(const std::string & contactId) {
	_imPresence->unsubscribeToPresenceOf(contactId);
}

void Presence::blockContact(const std::string & contactId) {
	_imPresence->blockContact(contactId);
}

void Presence::unblockContact(const std::string & contactId) {
	_imPresence->unblockContact(contactId);
}

void Presence::authorizeContact(const std::string & contactId, bool authorized, const std::string message) {
	_imPresence->authorizeContact(contactId, authorized, message);
}

void Presence::setState(EnumPresenceState::PresenceState state) {
	switch(state) {
	case EnumPresenceState::PresenceStateUnknown:
		//Do nothing
		break;
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
	case EnumPresenceState::PresenceStateInvisible:
		_state = _state->invisible();
		break;
	default:
		LOG_FATAL("unknown presence state=" + String::fromNumber(state));
	}
}

void Presence::presenceStateChangedEventHandler(IMPresence & sender, EnumPresenceState::PresenceState state,
		const std::string & alias, const std::string & from) {

	LOG_DEBUG("presence of=" + from + " changed=" + EnumPresenceState::toString(state));
}
