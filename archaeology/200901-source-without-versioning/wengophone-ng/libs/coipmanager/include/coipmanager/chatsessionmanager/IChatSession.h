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

#ifndef OWICHATSESSION_H
#define OWICHATSESSION_H

#include <coipmanager_base/EnumChatTypingState.h>

#include <util/Interface.h>

#include <string>

/**
 * Interface for ChatSession.
 *
 * @author Philippe Bernery
 * @author Tanguy Krotoff
 */
class IChatSession : Interface {
public:

	/**
	 * Sends a message to all Contacts in this Session.
	 *
	 * @param message the message to send.
	 */
	virtual void sendMessage(const std::string & message) = 0;

	/**
	 * Sets the typing state.
	 *
	 * @param state the desired typing state
	 */
	virtual void setTypingState(EnumChatTypingState::ChatTypingState state) = 0;

};

#endif	//OWICHATSESSION_H
