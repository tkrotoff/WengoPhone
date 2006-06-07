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
#include <model/profile/UserProfile.h>

#include <imwrapper/IMContact.h>

#include <util/Logger.h>
#include <util/Picture.h>

using namespace std;

PresenceHandler::PresenceHandler(UserProfile & userProfile)
: _userProfile(userProfile) {
	_userProfile.newIMAccountAddedEvent +=
		boost::bind(&PresenceHandler::newIMAccountAddedEventHandler, this, _1, _2);
	_userProfile.getConnectHandler().connectedEvent +=
		boost::bind(&PresenceHandler::connectedEventHandler, this, _1, _2);
	_userProfile.getConnectHandler().disconnectedEvent +=
		boost::bind(&PresenceHandler::disconnectedEventHandler, this, _1, _2);
}

PresenceHandler::~PresenceHandler() {
	_userProfile.newIMAccountAddedEvent -=
		boost::bind(&PresenceHandler::newIMAccountAddedEventHandler, this, _1, _2);
	_userProfile.getConnectHandler().connectedEvent -=
		boost::bind(&PresenceHandler::connectedEventHandler, this, _1, _2);
	_userProfile.getConnectHandler().disconnectedEvent -=
		boost::bind(&PresenceHandler::disconnectedEventHandler, this, _1, _2);
}

void PresenceHandler::subscribeToPresenceOf(const IMContact & imContact) {
	PresenceMap::iterator it = findPresence(_presenceMap, (IMAccount *)imContact.getIMAccount());

	if (it != _presenceMap.end()) {
		LOG_DEBUG("subscribing to Presence of: " + imContact.getContactId());
		(*it).second->subscribeToPresenceOf(imContact.getContactId());
	} else {
		//Presence for 'protocol' has not yet been created. The contactId is pushed in the pending subscription list
		_pendingSubscriptions.insert(pair<IMAccount *, const IMContact *>((IMAccount *)imContact.getIMAccount(), &imContact));
	}
}

void PresenceHandler::blockContact(const IMContact & imContact) {
	PresenceMap::iterator it = findPresence(_presenceMap, (IMAccount *)imContact.getIMAccount());

	if (it != _presenceMap.end()) {
		LOG_DEBUG("blocking Contact: " + imContact.getContactId()
			+ " of IMAccount: " + imContact.getIMAccount()->getLogin()
				+ " of protocol " + String::fromNumber(imContact.getIMAccount()->getProtocol()));

		(*it).second->blockContact(imContact.getContactId());
	} else {
		LOG_FATAL("the given IMAccount has not been added yet");
	}
}

void PresenceHandler::unblockContact(const IMContact & imContact) {
	PresenceMap::iterator it = findPresence(_presenceMap, (IMAccount *)imContact.getIMAccount());

	if (it != _presenceMap.end()) {
		LOG_DEBUG("unblocking Contact: " + imContact.getContactId()
			+ " of IMAccount: " + imContact.getIMAccount()->getLogin()
				+ " of protocol " + String::fromNumber(imContact.getIMAccount()->getProtocol()));

		(*it).second->unblockContact(imContact.getContactId());
	} else {
		LOG_FATAL("the given IMAccount has not been added yet");
	}
}

void PresenceHandler::connectedEventHandler(ConnectHandler & sender, IMAccount & imAccount) {
	PresenceMap::const_iterator it = _presenceMap.find(&imAccount);

	if (it != _presenceMap.end()) {
		LOG_DEBUG("an account is connected: login: " + imAccount.getLogin()
			+ " protocol: " + String::fromNumber(imAccount.getProtocol()));

		//TODO: Presence must be change to Presence set before disconnection
		EnumPresenceState::PresenceState presenceState = (*it).first->getPresenceState();
		if (presenceState == EnumPresenceState::PresenceStateOffline) {
			presenceState =  EnumPresenceState::PresenceStateOnline;
		}

		(*it).second->changeMyPresence(presenceState, String::null);

		//Launch all pending subscriptions
		IMContactMultiMap::iterator pendIt = _pendingSubscriptions.find(&imAccount);
		while (pendIt != _pendingSubscriptions.end()) {
			LOG_DEBUG("subscribing to Presence of: " + (*pendIt).second->getContactId());
			(*it).second->subscribeToPresenceOf((*pendIt).second->getContactId());
			//TODO: should we keep the list in case of disconnection?
			_pendingSubscriptions.erase(pendIt);
			pendIt = _pendingSubscriptions.find(&imAccount);
		}
	} else {
		LOG_FATAL("the given IMAccount has not been added yet");
	}
}

void PresenceHandler::disconnectedEventHandler(ConnectHandler & sender, IMAccount & imAccount) {
	PresenceMap::iterator it = _presenceMap.find(&imAccount);

	LOG_DEBUG("an account is disconnected, login=" + imAccount.getLogin()
		+ ", protocol=" + String::fromNumber(imAccount.getProtocol()));
	if (it != _presenceMap.end()) {
		// The presence state is now used to save the last presence state
		// used when connected. @see IMAccount::_presenceState
		// (*it).second->changeMyPresence(EnumPresenceState::PresenceStateOffline, String::null);
	} else {
		LOG_ERROR("this IMAccount has already been added=" + imAccount.getLogin());
	}
}

void PresenceHandler::changeMyPresenceState(EnumPresenceState::PresenceState state,
	const string & note, IMAccount * imAccount) {

	LOG_DEBUG("changing MyPresenceState for "
		+ ((!imAccount) ? "all" : imAccount->getLogin() + ", of protocol " + String::fromNumber(imAccount->getProtocol()))
		+ " with state " + String::fromNumber(state) + " and note " + note);

	if (!imAccount) {
		for (PresenceMap::const_iterator it = _presenceMap.begin(); it != _presenceMap.end(); it++) {
			(*it).second->changeMyPresence(state, note);
		}
	} else {
		//Find the desired Protocol
		PresenceMap::iterator it = findPresence(_presenceMap, imAccount);

		if (it != _presenceMap.end()) {
			(*it).second->changeMyPresence(state, note);
		}
	}
}

void PresenceHandler::changeMyAlias(const string & alias, IMAccount * imAccount) {
	LOG_DEBUG("changing alias for "
		+ ((!imAccount) ? "all" : imAccount->getLogin() + ", of protocol " + String::fromNumber(imAccount->getProtocol()))
		+ " with alias " + alias);

	if (!imAccount) {
		for (PresenceMap::const_iterator it = _presenceMap.begin(); it != _presenceMap.end(); it++) {
			(*it).second->changeMyAlias(alias);
		}
	} else {
		//Find the desired Protocol
		PresenceMap::iterator it = findPresence(_presenceMap, imAccount);

		if (it != _presenceMap.end()) {
			(*it).second->changeMyAlias(alias);
		}
	}
}

void PresenceHandler::changeMyIcon(const Picture & picture, IMAccount * imAccount) {
	LOG_DEBUG("changing icon for "
		+ ((!imAccount) ? "all" : imAccount->getLogin() + ", of protocol " + String::fromNumber(imAccount->getProtocol())));

	if (!imAccount) {
		for (PresenceMap::const_iterator it = _presenceMap.begin(); it != _presenceMap.end(); it++) {
			(*it).second->changeMyIcon(picture);
		}
	} else {
		//Find the desired Protocol
		PresenceMap::iterator it = findPresence(_presenceMap, imAccount);

		if (it != _presenceMap.end()) {
			(*it).second->changeMyIcon(picture);
		}
	}
}

void PresenceHandler::presenceStateChangedEventHandler(IMPresence & sender, EnumPresenceState::PresenceState state,
	const std::string & alias, const std::string & from) {

	//Plays a sound

	presenceStateChangedEvent(*this, state, alias, IMContact(sender.getIMAccount(), from));
}

void PresenceHandler::myPresenceStatusEventHandler(IMPresence & sender, EnumPresenceState::MyPresenceStatus status) {
	myPresenceStatusEvent(*this, sender.getIMAccount(), status);
}

void PresenceHandler::authorizationRequestEventHandler(IMPresence & sender, const std::string & contactId, const std::string & message) {
	authorizationRequestEvent(*this, IMContact(sender.getIMAccount(), contactId), message);
}

void PresenceHandler::subscribeStatusEventHandler(IMPresence & sender, const std::string & contactId, IMPresence::SubscribeStatus status) {
	subscribeStatusEvent(*this, IMContact(sender.getIMAccount(), contactId), status);
}

PresenceHandler::PresenceMap::iterator PresenceHandler::findPresence(PresenceMap & presenceMap, IMAccount * imAccount) {
	PresenceMap::iterator it;
	for (it = presenceMap.begin(); it != presenceMap.end(); it++) {
		if ((*it).first == imAccount) {
			break;
		}
	}
	return it;
}

void PresenceHandler::newIMAccountAddedEventHandler(UserProfile & sender, IMAccount & imAccount) {
	PresenceMap::const_iterator i = _presenceMap.find(&imAccount);

	//Presence for this IMAccount has not yet been created
	if (i == _presenceMap.end()) {
		Presence * presence = new Presence(imAccount);
		_presenceMap[&imAccount] = presence;

		presence->presenceStateChangedEvent +=
			boost::bind(&PresenceHandler::presenceStateChangedEventHandler, this, _1, _2, _3, _4);
		presence->myPresenceStatusEvent +=
			boost::bind(&PresenceHandler::myPresenceStatusEventHandler, this, _1, _2);
		presence->subscribeStatusEvent +=
			boost::bind(&PresenceHandler::subscribeStatusEventHandler, this, _1, _2, _3);
		presence->authorizationRequestEvent +=
			boost::bind(&PresenceHandler::authorizationRequestEventHandler, this, _1, _2, _3);
		presence->contactIconChangedEvent +=
			boost::bind(&PresenceHandler::contactIconChangedEventHandler, this, _1, _2, _3);

		imAccount.imAccountDeadEvent +=
			boost::bind(&PresenceHandler::imAccountDeadEventHandler, this, _1);

	} else {
		LOG_ERROR("this IMAccount has already been added=" + imAccount.getLogin());
	}
}

void PresenceHandler::imAccountDeadEventHandler(IMAccount & sender) {
	PresenceMap::iterator it = _presenceMap.find(&sender);

	if (it != _presenceMap.end()) {
		delete (*it).second;
		_presenceMap.erase(it);
	} else {
		LOG_ERROR("this IMAccount has not been added=" + sender.getLogin());
	}
}

Picture PresenceHandler::getContactIcon(const IMContact & imContact) {
	//LOG_DEBUG("Getting icon of " + imContact.getContactId());

	//PresenceMap::iterator it = findPresence(_presenceMap, (IMAccount *)imContact.getIMAccount());

	//if (it != _presenceMap.end()) {
	//	return (*it).second->getContactIcon(imContact.getContactId());
	//} else {
	//	LOG_FATAL("Unknown IMAccount");
	//	return Picture();
	//}
	return Picture();
}

void PresenceHandler::authorizeContact(const IMContact & imContact, bool authorized, const std::string & message) {
	PresenceMap::iterator it = findPresence(_presenceMap, (IMAccount *) imContact.getIMAccount());

	if (it != _presenceMap.end()) {
		return (*it).second->authorizeContact(imContact.getContactId(), authorized, message);
	} else {
		LOG_FATAL("unknown IMAccount");
	}
}

void PresenceHandler::contactIconChangedEventHandler(IMPresence & sender,
	const std::string & contactId, Picture icon) {

	contactIconChangedEvent(*this, IMContact(sender.getIMAccount(), contactId), icon);
}
