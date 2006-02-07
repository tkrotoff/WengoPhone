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
#include <model/connect/ConnectHandler.h>

#include <IMContact.h>
#include <Logger.h>

using namespace std;

PresenceHandler::PresenceHandler(ConnectHandler & connectHandler) {
	connectHandler.connectedEvent += 
		boost::bind(&PresenceHandler::connectedEventHandler, this, _1, _2);
	connectHandler.disconnectedEvent += 
		boost::bind(&PresenceHandler::disconnectedEventHandler, this, _1, _2);
}

PresenceHandler::~PresenceHandler() {
	for (PresenceMap::iterator i = _presenceMap.begin() ; i != _presenceMap.end() ; i++) {
		delete (*i).second;
	}
}

void PresenceHandler::subscribeToPresenceOf(const IMContact & imContact) {
	PresenceMap::iterator it = findPresence(_presenceMap, (IMAccount &)imContact.getIMAccount());

	if (it != _presenceMap.end()) {
		LOG_DEBUG("subscribing to Presence of: " + imContact.getContactId());
		(*it).second->subscribeToPresenceOf(imContact.getContactId());
	} else {
		//Presence for 'protocol' has not yet been created. The contactId is pushed in the pending subscription list
		_pendingSubscriptions.insert(pair<const IMAccount *, const IMContact *>(&imContact.getIMAccount(), &imContact));
	}
}

void PresenceHandler::blockContact(const IMContact & imContact) {
	PresenceMap::iterator it = findPresence(_presenceMap, (IMAccount &)imContact.getIMAccount());

	if (it != _presenceMap.end()) {
		LOG_DEBUG("blocking Contact: " + imContact.getContactId() 
			+ " of IMAccount: " + imContact.getIMAccount().getLogin() 
				+ " of protocol " + String::fromNumber(imContact.getIMAccount().getProtocol()));

		(*it).second->blockContact(imContact.getContactId());
	}
}

void PresenceHandler::unblockContact(const IMContact & imContact) {
	PresenceMap::iterator it = findPresence(_presenceMap, (IMAccount &)imContact.getIMAccount());

	if (it != _presenceMap.end()) {
		LOG_DEBUG("unblocking Contact: " + imContact.getContactId() 
			+ " of IMAccount: " + imContact.getIMAccount().getLogin() 
				+ " of protocol " + String::fromNumber(imContact.getIMAccount().getProtocol()));

		(*it).second->unblockContact(imContact.getContactId());
	}
}

void PresenceHandler::connectedEventHandler(ConnectHandler & sender, IMAccount & imAccount) {
	PresenceMap::const_iterator i = _presenceMap.find(&imAccount);
	
	LOG_DEBUG("an account is connected: login: " + imAccount.getLogin() 
		+ "protocol: " + String::fromNumber(imAccount.getProtocol()));
	//Presence for this IMAccount has not been created yet
	if (i == _presenceMap.end()) {
		Presence * presence = new Presence(imAccount);
		_presenceMap[&imAccount] = presence;

		presence->presenceStateChangedEvent += 
			boost::bind(&PresenceHandler::presenceStateChangedEventHandler, this, _1, _2, _3, _4);
		presence->myPresenceStatusEvent += 
			boost::bind(&PresenceHandler::myPresenceStatusEventHandler, this, _1, _2);
		presence->subscribeStatusEvent += 
			boost::bind(&PresenceHandler::subscribeStatusEventHandler, this, _1, _2, _3);

		//Launch all pending subscriptions
		IMContactMultiMap::iterator it = _pendingSubscriptions.find(&imAccount);
		while (it != _pendingSubscriptions.end()) {
			LOG_DEBUG("subscribing to Presence of: " + (*it).second->getContactId());
			presence->subscribeToPresenceOf((*it).second->getContactId());
			//TODO: should we keep the list in case of disconnection?
			_pendingSubscriptions.erase(it);
			it = _pendingSubscriptions.find(&imAccount);
		}

		i = _presenceMap.find(&imAccount);
	}

	//TODO: Presence must be change to Presence set before disconnection
	(*i).second->changeMyPresence(EnumPresenceState::PresenceStateOnline, "");
}

void PresenceHandler::disconnectedEventHandler(ConnectHandler & sender, IMAccount & imAccount) {
	PresenceMap::iterator i = _presenceMap.find(&imAccount);
	
	LOG_DEBUG("an account is disconnected: login: " + imAccount.getLogin() 
		+ ", protocol: " + String::fromNumber(imAccount.getProtocol()));
	if (i != _presenceMap.end()) {
		(*i).second->changeMyPresence(EnumPresenceState::PresenceStateOffline, "");
	}
}

void PresenceHandler::changeMyPresence(EnumPresenceState::PresenceState state,	
	const std::string & note, IMAccount * imAccount) {

	LOG_DEBUG("changing MyPresenceState for "
		+ ((!imAccount) ? "all" : imAccount->getLogin() + ", of protocol " + String::fromNumber(imAccount->getProtocol()))
		+ " with state " + String::fromNumber(state) + " and note " + note);

	if (!imAccount) {
		for (PresenceMap::const_iterator i = _presenceMap.begin() ; i != _presenceMap.end() ; i++) {
			(*i).second->changeMyPresence(state, note);
		}
	} else {
		//Find the desired Protocol
		PresenceMap::iterator it = findPresence(_presenceMap, *imAccount);
		
		if (it != _presenceMap.end()) {
			(*it).second->changeMyPresence(state, note);
		}
	}
}

void PresenceHandler::presenceStateChangedEventHandler(IMPresence & sender, EnumPresenceState::PresenceState state,
	const std::string & note, const std::string & from) {
	
	presenceStateChangedEvent(*this, state, note, sender.getIMAccount(), from);
}

void PresenceHandler::myPresenceStatusEventHandler(IMPresence & sender, EnumPresenceState::MyPresenceStatus status) {
	myPresenceStatusEvent(*this, sender.getIMAccount(), status);
}

void PresenceHandler::subscribeStatusEventHandler(IMPresence & sender, const std::string & contactId, IMPresence::SubscribeStatus status) {
	subscribeStatusEvent(*this, sender.getIMAccount(), contactId, status);
}

PresenceHandler::PresenceMap::iterator PresenceHandler::findPresence(PresenceMap & presenceMap, IMAccount & imAccount) {
	PresenceMap::iterator i;
	for (i = presenceMap.begin() ; i != presenceMap.end() ; i++) {
		if ((*((*i).first)) == imAccount) {
			break;
		} 
	}
	return i;
}
