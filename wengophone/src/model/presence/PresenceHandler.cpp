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
 
#include "PresenceHandler.h"

#include <model/presence/Presence.h>
#include <model/contactlist/ContactList.h>

#include <Logger.h>

using namespace std;

PresenceHandler::PresenceHandler() {
}

PresenceHandler::~PresenceHandler() {
	for (PresenceMap::iterator i = _presenceMap.begin() ; i != _presenceMap.end() ; i++) {
		delete (*i).second;
	}
}

void PresenceHandler::subscribeToPresenceOf(EnumIMProtocol::IMProtocol protocol, const std::string & contactId) {
	PresenceMap::iterator it = findPresence(_presenceMap, protocol);

	if (it != _presenceMap.end()) {
		LOG_DEBUG("subscribing to Presence of: " + contactId);
		(*it).second->subscribeToPresenceOf(contactId);
	} else {
		//Presence for 'protocol' has not yet been created. The contactId is pushed in the pending subscription list
		_pendingSubscriptions.insert(pair<EnumIMProtocol::IMProtocol, const std::string>(protocol, contactId));
	}
}

void PresenceHandler::blockContact(EnumIMProtocol::IMProtocol protocol, const std::string & contactId) {
	PresenceMap::iterator it = findPresence(_presenceMap, protocol);

	if (it != _presenceMap.end()) {
		LOG_DEBUG("blocking Contact: " + contactId + " of Protocol: " + String::fromNumber(protocol));
		(*it).second->blockContact(contactId);
	}
}

void PresenceHandler::unblockContact(EnumIMProtocol::IMProtocol protocol, const std::string & contactId) {
	PresenceMap::iterator it = findPresence(_presenceMap, protocol);

	if (it != _presenceMap.end()) {
		LOG_DEBUG("unblocking Contact: " + contactId + " of Protocol: " + String::fromNumber(protocol));
		(*it).second->unblockContact(contactId);
	}
}

void PresenceHandler::connected(IMAccount & account) {
	PresenceMap::iterator i = _presenceMap.find(&account);
	
	LOG_DEBUG("an account is connected: login: " + account.getLogin() 
		+ "protocol: " + String::fromNumber(account.getProtocol()));
	//Presence for this IMAccount has not been created yet
	if (i == _presenceMap.end()) {
		Presence * presence = new Presence(account);
		_presenceMap[&account] = presence;

		presence->presenceStateChangedEvent += presenceStateChangedEvent;
		presence->myPresenceStatusEvent += myPresenceStatusEvent;
		presence->subscribeStatusEvent += subscribeStatusEvent;

		//Launch all pending subscriptions
		ContactIDMultiMap::iterator it = _pendingSubscriptions.find(account.getProtocol());
		while (it != _pendingSubscriptions.end()) {
			LOG_DEBUG("subscribing to Presence of: " + (*it).second);
			presence->subscribeToPresenceOf((*it).second);
			//TODO: should we keep the list in case of disconnection?
			_pendingSubscriptions.erase(it);
			it = _pendingSubscriptions.find(account.getProtocol());
		}

		i = _presenceMap.find(&account);
	}

	//TODO: Presence must be change to Presence set before disconnection
	(*i).second->changeMyPresence(EnumPresenceState::PresenceStateOnline, "");
}

void PresenceHandler::disconnected(IMAccount & account) {
	PresenceMap::iterator i = _presenceMap.find(&account);
	
	LOG_DEBUG("an account is disconnected: login: " + account.getLogin() 
		+ ", protocol: " + String::fromNumber(account.getProtocol()));
	if (i != _presenceMap.end()) {
		(*i).second->changeMyPresence(EnumPresenceState::PresenceStateOffline, "");
	}
}

void PresenceHandler::changeMyPresence(EnumPresenceState::PresenceState state,	
	const std::string & note, EnumIMProtocol::IMProtocol protocol) {

	LOG_DEBUG("changing MyPresenceState for "
		+ ((protocol == EnumIMProtocol::IMProtocolAll) ? "all" : String::fromNumber(protocol))
		+ " protocol(s) with state " + String::fromNumber(state) + " and note " + note);

	if (protocol == EnumIMProtocol::IMProtocolAll) {
		for (PresenceMap::const_iterator i = _presenceMap.begin() ; i != _presenceMap.end() ; i++) {
			(*i).second->changeMyPresence(state, note);
		}
	} else {
		//Find the desired Protocol
		PresenceMap::iterator it = findPresence(_presenceMap, protocol);
		
		if (it != _presenceMap.end()) {
			(*it).second->changeMyPresence(state, note);
		}
	}
}

PresenceHandler::PresenceMap::iterator PresenceHandler::findPresence(PresenceMap & presenceMap, EnumIMProtocol::IMProtocol protocol) {
	PresenceMap::iterator i;
	for (i = presenceMap.begin() ; i != presenceMap.end() ; i++) {
		if ((*i).first->getProtocol() == protocol) {
			break;
		} 
	}
	return i;
}
