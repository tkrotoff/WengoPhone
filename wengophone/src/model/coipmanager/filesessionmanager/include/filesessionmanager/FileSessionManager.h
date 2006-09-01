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

#ifndef OWFILESESSIONMANAGER_H
#define OWFILESESSIONMANAGER_H

#include <util/Event.h>

#include <vector>

class Account;
class IFileSessionManager;
class IReceiveFileSession;
class ISendFileSession;
class ReceiveFileSession;
class SendFileSession;
class UserProfile;

/**
 * File Session Manager.
 *
 * Used to manage classes related to File Transfer
 *
 * @author Philippe Bernery
 */
class FileSessionManager {
	friend class SendFileSession;
public:

	/**
	 * Emitted when a new FileSession is created.
	 * Can be emitted after a call to createFileSession or 
	 * because subsystem created it (surely because someone
	 * is trying to send us a file).
	 *
	 * @param sender this class
	 * @param fileSession pointer to the newly created FileSession.
	 * Here the FileSession is a pointer because the received of the event is
	 * responsible of the deleting of this object.
	 */
	Event< void (FileSessionManager & sender, ReceiveFileSession fileSession)> newReceiveFileSessionCreatedEvent;

	FileSessionManager(UserProfile & userProfile);

	virtual ~FileSessionManager();

	/**
	 * Creates a SendFileSession. Caller is responsible for deleting the
	 * Session.
	 */
	SendFileSession * createSendFileSession();

private:

	/**
	 * @see IFileSessionManager::newIReceiveFileSessionCreatedEvent
	 */
	void newIReceiveFileSessionCreatedEventHandler(IFileSessionManager & sender, 
		IReceiveFileSession * iReceiveFileSession);

	/**
	 * Create a ISendFileSession suitable for the given Account.
	 *
	 * @return a pointer to the new FileSession.
	 * Must be destroyed by the caller.
	 */
	ISendFileSession * createFileSessionForAccount(const Account & account);

	/** Vector of available FileSessionManager. */
	std::vector<IFileSessionManager *> _fileSessionManagerVector;

	/** Link to UserProfile. */
	UserProfile & _userProfile;

};

#endif //OWFILESESSIONMANAGER_H
