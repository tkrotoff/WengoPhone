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

#ifndef OWCHATSESSIONMANAGER_H
#define OWCHATSESSIONMANAGER_H

#include "IChatSessionManagerPlugin.h"

#include <coipmanager/ICoIpSessionManager.h>

class Account;
class ChatSession;
class IChatSessionPlugin;
class IChatSessionManagerPlugin;

/**
 * Chat Session Manager.
 *
 * Used to manage classes related to Chat.
 *
 * @ingroup CoIpManager
 * @author Philippe Bernery
 * @author Tanguy Krotoff
 */
class COIPMANAGER_API ChatSessionManager : public ICoIpSessionManager {
	Q_OBJECT
	friend class ChatSession;
public:

	ChatSessionManager(CoIpManager & coIpManager);

	virtual ~ChatSessionManager();

	/**
	 * Creates a ChatSession.
	 *
	 * Caller is responsible for deleting the Session.
	 */
	ChatSession * createChatSession();

	virtual IChatSessionManagerPlugin * getICoIpManagerPlugin(const Account & account, const ContactList & contactList) const;

Q_SIGNALS:

	/**
	 * Emitted when a new ChatSession is created from outside.
	 *
	 * @param chatSession pointer to the newly created ChatSession
	 *        This ChatSession must be deleted by the Event receiver
	 */
	void newChatSessionCreatedSignal(ChatSession * chatSession);

private Q_SLOTS:

	/**
	 * @see IChatSessionManagerPlugin::newIChatSessionCreatedSignal()
	 */
	void newIChatSessionPluginCreatedSlot(IChatSessionPlugin * iChatSession);

private:

	virtual void initialize();

	virtual void uninitialize();

	/**
	 * Closes all ChatSession.
	 */
	void closeAllChatSession();

};

#endif	//OWCHATSESSIONMANAGER_H
