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

#ifndef OWTCHATSESSIONMANAGER_H
#define OWTCHATSESSIONMANAGER_H

#include <coipmanager_threaded/ITCoIpSessionManager.h>
#include <coipmanager_threaded/tcoipmanagerdll.h>

class ChatSession;
class ChatSessionManager;
class TChatSession;

/**
 * Threaded Chat Session Manager.
 *
 * @see ChatSessionManager
 * @ingroup TCoIpManager
 * @author Philippe Bernery
 */
class COIPMANAGER_THREADED_API TChatSessionManager : public ITCoIpSessionManager {
	Q_OBJECT
public:

	TChatSessionManager(TCoIpManager & tCoIpManager);

	virtual ~TChatSessionManager();

	/**
	 * @see ChatSessionManager::createChatSession
	 */
	TChatSession * createTChatSession();

Q_SIGNALS:

	/**
	 * @see ChatSessionManager::newChatSessionCreatedSignal()
	 */
	void newChatSessionCreatedSignal(TChatSession * tChatSession);

private Q_SLOTS:

	/**
	 * @see ChatSessionManager::newChatSessionCreatedSignal()
	 */
	void newChatSessionCreatedSlot(ChatSession * chatSession);

private:

	virtual ICoIpSessionManager * getICoIpSessionManager() const;

	/**
	 * Code factorization.
	 */
	ChatSessionManager * getChatSessionManager() const;

};

#endif //OWTCHATSESSIONMANAGER_H
