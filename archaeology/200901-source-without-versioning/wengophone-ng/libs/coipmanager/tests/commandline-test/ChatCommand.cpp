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

#include "ChatCommand.h"

#include <coipmanager/CoIpManager.h>
#include <coipmanager/chatsessionmanager/ChatSession.h>
#include <coipmanager/chatsessionmanager/ChatSessionManager.h>

#include <util/SafeDelete.h>
#include <event/Event2.h>

#include <iostream>

ChatCommand::ChatCommand(CoIpManager & coIpManager)
	: Command("Chat Session management", "start a chat", coIpManager) {

	_menuContent.push_back(std::pair<std::string, boost::function< void () > >("Start a chat", boost::bind(&ChatCommand::startChat, this)));

	_commandList.push_back("/help");
	_commandList.push_back("/quit");
}

ChatCommand::~ChatCommand() {
}

void ChatCommand::startChat() {
	// Getting data
	std::cout << "Choose the contact to chat with:" << std::endl;
	Contact * contact = Menu::listContactMenu(_coIpManager);
	////

	// Setting ChatSession
	ChatSession *chatSession = _coIpManager.getChatSessionManager().createChatSession();
	chatSession->addContact(*contact);
	OWSAFE_DELETE(contact);
	////

	// Starting chat and waiting for user entry
	chatSession->start();
	manageChatSession(chatSession);
	////
}

void ChatCommand::displayHelp() {
	std::cout << "During a ChatSession, you can enter commands or send message." << std::endl;
	std::cout << "To send message, simply type message and press enter." << std::endl;
	std::cout << "To enter command, type '/command_name'." << std::endl;
	std::cout << "Available commands are: " << std::endl;
	for (std::vector<std::string>::const_iterator it = _commandList.begin();
		it != _commandList.end();
		++it) {
		std::cout << "\t" << *it << std::endl;
	}
	std::cout << std::endl;
}

void ChatCommand::manageChatSession(ChatSession *chatSession) {
	displayHelp();

	std::string userEntry = waitForUserEntry();

	while (userEntry != "/quit") {
		if (userEntry[0] == '/') {
			// Command
		} else {
			// Send message
			chatSession->sendMessage(userEntry);
		}

		userEntry = waitForUserEntry();
	}

	OWSAFE_DELETE(chatSession);
}
