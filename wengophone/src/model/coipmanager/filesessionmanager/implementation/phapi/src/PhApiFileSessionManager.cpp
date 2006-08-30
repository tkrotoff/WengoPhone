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

#include "../include/phapifilesessionmanager/PhApiFileSessionManager.h"
#include "../include/phapifilesessionmanager/PhApiSendFileSession.h"
#include "../include/phapifilesessionmanager/PhApiReceiveFileSession.h"

#include <PhApiSFPEvent.h>


PhApiFileSessionManager::PhApiFileSessionManager(UserProfile & userProfile)
: IFileSessionManager(userProfile) {
	PhApiSFPEvent::newIncomingFileEvent += boost::bind(&PhApiFileSessionManager::newIncomingFileEventHandler, this, _1, _2, _3, _4, _5, _6);
}

PhApiFileSessionManager::~PhApiFileSessionManager() {
}

ISendFileSession * PhApiFileSessionManager::createSendFileSession() {
	return new PhApiSendFileSession(_userProfile);
}

bool PhApiFileSessionManager::isProtocolSupported(EnumIMProtocol::IMProtocol protocol) const {
	// FIXME
	if (protocol == EnumIMProtocol::IMProtocolWengo) {
		return true;
	} else {
		return false;
	}
}

void PhApiFileSessionManager::newIncomingFileEventHandler(PhApiSFPWrapper & sender, int callID, std::string contactID, std::string fileName, std::string fileType, int fileSize){
	PhApiReceiveFileSession * session = new PhApiReceiveFileSession(callID, contactID, fileName, (unsigned int)fileSize);
	newIReceiveFileSessionCreatedEvent(*this, session);
}
