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

#ifndef OWPHAPICHATSESSIONMANAGER_H
#define OWPHAPICHATSESSIONMANAGER_H

#include <coipmanager/chatsessionmanager/IChatSessionManagerPlugin.h>

#include <coipmanager_base/EnumChatTypingState.h>

#include <QtCore/QMutex>

#include <string>
#include <vector>

class PhApiChatSession;

/**
 * PhApi implementation of Chat Session Manager.
 *
 * @author Philippe Bernery
 * @author Tanguy Krotoff
 * @author Mathieu Stute
 */
class PhApiChatSessionManager : public IChatSessionManagerPlugin {
	Q_OBJECT
	friend class PhApiChatSession;
public:

	PhApiChatSessionManager(CoIpManager & coIpManager);

	virtual ~PhApiChatSessionManager();

	virtual IChatSessionPlugin * createIChatSessionPlugin();

	virtual bool canCreateISession(const Account & account, const ContactList & contactList) const;

	virtual IMContactList getValidIMContacts(const Account & account, const ContactList & contactList) const;

private Q_SLOTS:

	/**
	 * @see SipWrapper::chatMessageReceivedSignal()
	 */
	void chatMessageReceivedSlot(std::string contactIdReceiver,
		std::string contactIdSender, std::string message);

	/**
	 * @see SipWrapper::chatTypingStateChangedSignal()
	 */
	void chatTypingStateChangedSlot(std::string contactIdReceiver,
		std::string contactIdSender, EnumChatTypingState::ChatTypingState state);

private:

	/**
	 * Retrieves a PhApiFileSession with a contactId.
	 *
	 * There is currently only one contact per Session so this is very easy.
	 */
	PhApiChatSession * retrievePhApiChatSession(const std::string & contactId) const;

	/**
	 * Create an IMContact from a contactId. Try to retrieve it in ContactList.
	 */
	IMContact getIMContact(const std::string & contactId) const;

	/**
	 * Registers a PhApiChatSession.
	 *
	 * Adds it to the ChatSessionList.
	 */
	void registerChatSession(PhApiChatSession * session);

	/**
	 * Unregisters a PhApiChatSession.
	 *
	 * Removes it from the ChatSessionList.
	 */
	void unregisterChatSession(PhApiChatSession * session);

	std::vector<PhApiChatSession *> _chatSessionList;

	mutable QMutex * _mutex;
};

#endif	//OWPHAPICHATSESSIONMANAGER_H
