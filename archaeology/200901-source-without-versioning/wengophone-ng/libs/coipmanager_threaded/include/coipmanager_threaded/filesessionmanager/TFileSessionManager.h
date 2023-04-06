/*
 * WengoPhone, a voice over Internet phone
 * Copyright (C) 2004-2007  Wengo
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

#ifndef OWTFILESESSIONMANAGER_H
#define OWTFILESESSIONMANAGER_H

#include <coipmanager_threaded/ITCoIpSessionManager.h>
#include <coipmanager_threaded/tcoipmanagerdll.h>

class FileSessionManager;
class ReceiveFileSession;
class TReceiveFileSession;
class TSendFileSession;

/**
 * Threaded FileSessionManager.
 *
 * @see FileSessionManager
 * @author Philippe Bernery
 */
class COIPMANAGER_THREADED_API TFileSessionManager : public ITCoIpSessionManager {
	Q_OBJECT
public:

	TFileSessionManager(TCoIpManager & tCoIpManager);

	virtual ~TFileSessionManager();

	/**
	 * @see FileSessionManager::createSendFileSession
	 */
	TSendFileSession * createTSendFileSession();

Q_SIGNALS:

	/**
	 * @see FileSessionManager::newReceiveFileSessionCreatedSignal
	 */
	void newTReceiveFileSessionCreatedSignal(TReceiveFileSession * fileSession);

	/**
	 * @see IFileSessionManagerPlugin::needsUpgradeEvent
	 */
	void needsUpgradeSignal();

	/**
	 * @see IFileSessionManagerPlugin::peerNeedsUpgradeEvent
	 */
	void peerNeedsUpgradeSignal(Contact contact);

private Q_SLOTS:

	/**
	 * @see FileSessionManager::newReceiveFileSessionCreatedSignal
	 */
	void newReceiveFileSessionCreatedSlot(ReceiveFileSession * fileSession);

private:

	virtual ICoIpSessionManager * getICoIpSessionManager() const;

	FileSessionManager * getFileSessionManager() const;
};

#endif //OWTFILESESSIONMANAGER_H
