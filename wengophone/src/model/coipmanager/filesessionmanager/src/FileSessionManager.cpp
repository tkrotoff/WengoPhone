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

#include <filesessionmanager/FileSessionManager.h>

#include <filesessionmanager/ReceiveFileSession.h>
#include <filesessionmanager/SendFileSession.h>

#include "../implementation/test/include/testfilesessionmanager/TestFileSessionManager.h"
#include "../implementation/phapi/include/phapifilesessionmanager/PhApiFileSessionManager.h"

#include <imwrapper/Account.h>

#include <util/Logger.h>

FileSessionManager::FileSessionManager(UserProfile & userProfile)
	: _userProfile(userProfile) {

	_fileSessionManagerVector.push_back(new TestFileSessionManager(_userProfile));
	PhApiFileSessionManager * phapiManager = new PhApiFileSessionManager(_userProfile);
	phapiManager->newIReceiveFileSessionCreatedEvent += boost::bind(&FileSessionManager::newIReceiveFileSessionCreatedEventHandler, this, _1, _2);
	_fileSessionManagerVector.push_back(phapiManager);
}

FileSessionManager::~FileSessionManager() {
	for (std::vector<IFileSessionManager *>::const_iterator it = _fileSessionManagerVector.begin();
		it != _fileSessionManagerVector.end();
		++it) {
		delete (*it);
	}
}

SendFileSession * FileSessionManager::createSendFileSession() {
	SendFileSession * newFileSession = new SendFileSession(*this, _userProfile);
	return newFileSession;
}

ISendFileSession * FileSessionManager::createFileSessionForAccount(const Account & account) {
	ISendFileSession * result = NULL;

	for (std::vector<IFileSessionManager *>::const_iterator it = _fileSessionManagerVector.begin();
		it != _fileSessionManagerVector.end();
		++it) {
		if ((*it)->isProtocolSupported(account.getProtocol())) {
			result = (*it)->createSendFileSession();
		}
	}

	if (!result) {
		LOG_ERROR("no FileSession implementation available for protocol " + 
			EnumIMProtocol::toString(account.getProtocol()));
	}

	return result;
}

void FileSessionManager::newIReceiveFileSessionCreatedEventHandler(IFileSessionManager & sender, IReceiveFileSession * iReceiveFileSession) {
	LOG_DEBUG("new IReceiveFileSession created event");

	ReceiveFileSession * newFileSession = new ReceiveFileSession(_userProfile, iReceiveFileSession);

	newReceiveFileSessionCreatedEvent(*this, newFileSession);
}
