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

#ifndef OWICHATSESSIONPLUGIN_H
#define OWICHATSESSIONPLUGIN_H

#include <coipmanager/chatsessionmanager/EnumChatStatusMessage.h>
#include <coipmanager/chatsessionmanager/IChatSession.h>
#include <coipmanager/session/ISession.h>

#include <coipmanager_base/EnumChatTypingState.h>

/**
 * Interface for ChatSession implementation.
 *
 * @author Philippe Bernery
 * @author Tanguy Krotoff
 */
class COIPMANAGER_API IChatSessionPlugin : public ISession, public IChatSession {
	Q_OBJECT
public:

	IChatSessionPlugin(CoIpManager & coIpManager);

	virtual ~IChatSessionPlugin();

Q_SIGNALS:

	/**
	 * Emitted when a message has been received.
	 *
	 * @param imContact the sender of the message
	 * @param message the received message
	 */
	void messageReceivedSignal(IMContact imContact, std::string message);

	/**
	 * Chat typing state event.
	 *
	 * @param contactId contact who typing state has changed
	 * @param state @see EnumChatTypingState
	 */
	void typingStateChangedSignal(IMContact imContact, EnumChatTypingState::ChatTypingState state);

		/**
	 * Emitted when a status message has been received. (e.g "Joe is connected")
	 *
	 * @param status the type of status message
	 * @param message a message describing the status
	 */
	void statusMessageReceivedSignal(EnumChatStatusMessage::ChatStatusMessage status, std::string message);

};

#endif	//OWICHATSESSIONPLUGIN_H
