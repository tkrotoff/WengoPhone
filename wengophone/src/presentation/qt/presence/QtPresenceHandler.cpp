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

#include "QtPresenceHandler.h"

#include <control/presence/CPresenceHandler.h>

#include <Logger.h>

QtPresenceHandler::QtPresenceHandler(CPresenceHandler & cPresenceHandler) 
	: _cPresenceHandler(cPresenceHandler) {

	_cPresenceHandler.presenceStateChangedEvent += 
		boost::bind(&QtPresenceHandler::presenceStateChangedEventHandler, this, _1, _2, _3, _4);
	_cPresenceHandler.myPresenceStatusEvent += 
		boost::bind(&QtPresenceHandler::myPresenceStatusEventHandler, this, _1, _2);
	_cPresenceHandler.subscribeStatusEvent += 
		boost::bind(&QtPresenceHandler::subscribeStatusEventHandler, this, _1, _2, _3);
}

void QtPresenceHandler::changeMyPresence(EnumPresenceState::PresenceState state,
	const std::string & note, EnumIMProtocol::IMProtocol protocol) {
	
}

void QtPresenceHandler::subscribeToPresenceOf(EnumIMProtocol::IMProtocol protocol,
	const std::string & contactId) {

}

void QtPresenceHandler::blockContact(EnumIMProtocol::IMProtocol protocol, const std::string & contactId) {

}

void QtPresenceHandler::unblockContact(EnumIMProtocol::IMProtocol protocol, const std::string & contactId) {

}
	
void QtPresenceHandler::presenceStateChangedEventHandler(IMPresence & sender,
	EnumPresenceState::PresenceState state, const std::string & note, const std::string & from) {
	
	LOG_DEBUG("presence of " + from + " change to " + String::fromNumber(state));
}

void QtPresenceHandler::myPresenceStatusEventHandler(IMPresence & sender,
	EnumPresenceState::MyPresenceStatus status) {

	LOG_DEBUG("QtPresenceHandler::myPresenceStatusEventHandler");
}

void QtPresenceHandler::subscribeStatusEventHandler(IMPresence & sender,
	const std::string & contactId, IMPresence::SubscribeStatus status) {

	LOG_DEBUG("QtPresenceHandler::subscribeStatusEventHandler");
}

void QtPresenceHandler::updatePresentation() {
	
}

void QtPresenceHandler::updatePresentationThreadSafe() {

}

void QtPresenceHandler::initThreadSafe() {

}
