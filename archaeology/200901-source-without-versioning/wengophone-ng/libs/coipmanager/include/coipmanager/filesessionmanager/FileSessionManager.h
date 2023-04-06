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

#ifndef OWFILESESSIONMANAGER_H
#define OWFILESESSIONMANAGER_H

#include <coipmanager/ICoIpSessionManager.h>

#include <coipmanager/filesessionmanager/IFileSessionManagerPlugin.h>
#include <coipmanager/filesessionmanager/ReceiveFileSession.h>

class Account;
class CoIpModule;
class IFileSessionManagerPlugin;
class IReceiveFileSessionPlugin;
class ISendFileSessionPlugin;
class SendFileSession;

/**
 * File Session Manager.
 *
 * Used to manage classes related to File Transfer.
 *
 * @ingroup CoIpManager
 * @author Philippe Bernery
 */
class COIPMANAGER_API FileSessionManager : public ICoIpSessionManager {
	Q_OBJECT
	friend class SendFileSession;
public:

	FileSessionManager(CoIpManager & coIpManager);

	virtual ~FileSessionManager();

	/**
	 * Creates a SendFileSession. Caller is responsible for deleting the
	 * Session.
	 */
	SendFileSession * createSendFileSession();

	virtual IFileSessionManagerPlugin * getICoIpManagerPlugin(const Account & account, const ContactList & contactList) const;

Q_SIGNALS:
	/**
	 * Emitted when a new ReceiveFileSession
	 *
	 * @param sender this class
	 * @param fileSession pointer to the newly created FileSession. The receiver of the signal is responsible to delete the Session.
	 */
	void newReceiveFileSessionCreatedSignal(ReceiveFileSession * fileSession);

	/**
	 * TODO: to be changed: if a new implementation of File transfer is created,
	 * these events will not be valid anymore. There should in a class like Session
	 *
	 * @see IFileSessionManagerPlugin::needsUpgradeEvent
	 */
	void needsUpgradeSignal();

	/**
	 * @see IFileSessionManagerPlugin::peerNeedsUpgradeEvent
	 */
	void peerNeedsUpgradeSignal(Contact contact);

private Q_SLOTS:

	/**
	 * @see IFileSessionManagerPlugin::newIReceiveFileSessionCreatedEvent
	 */
	void newIReceiveFileSessionPluginCreatedSlot(IReceiveFileSessionPlugin * iReceiveFileSession);

	/**
	 * @see IFileSessionManagerPlugin::needsUpgradeEventHandler
	 */
	void needsUpgradeSlot();

	/**
	 * @see IFileSessionManagerPlugin::peerNeedsUpgradeEventHandler
	 */
	void peerNeedsUpgradeSlot(IMContact imContact);

private:

	virtual void initialize();

	virtual void uninitialize();
};

#endif	//OWFILESESSIONMANAGER_H
