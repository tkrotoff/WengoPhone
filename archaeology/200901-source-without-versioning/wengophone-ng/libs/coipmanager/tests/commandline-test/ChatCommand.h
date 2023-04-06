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

#ifndef OWCHATCOMMAND_H
#define OWCHATCOMMAND_H

#include "Command.h"

#include <string>
#include <vector>

class ChatSession;

/**
 * Command for Chat management.
 *
 * @author Philippe Bernery
 */
class ChatCommand : public Command {
public:

	ChatCommand(CoIpManager & coIpManager);

	virtual ~ChatCommand();

private:

	/**
	 * @name Menu entry
	 * @{
	 */

	/**
	 * Starts a chat.
	 */
	void startChat();

	/** @} */

	/**
	 * Manages user entry and message display during a ChatSession.
	 */
	void manageChatSession(ChatSession *chatSession);

	/**
	 * Displays existing commands during a ChatSession.
	 */
	void displayHelp();

	std::vector<std::string> _commandList;
};

#endif //OWCHATCOMMAND_H
