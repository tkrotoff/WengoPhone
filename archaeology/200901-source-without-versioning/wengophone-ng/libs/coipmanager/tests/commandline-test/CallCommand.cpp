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

#include "CallCommand.h"

#include <coipmanager/CoIpManager.h>
#include <coipmanager_base/EnumPresenceState.h>
#include <coipmanager_base/account/Account.h>
#include <coipmanager/callsessionmanager/CallSession.h>
#include <coipmanager/callsessionmanager/CallSessionManager.h>
#include <coipmanager_base/contact/ContactList.h>
#include <coipmanager_base/imcontact/IMContact.h>
#include <coipmanager_base/userprofile/UserProfile.h>

#include <util/SafeDelete.h>

#include <iostream>

CallCommand::CallCommand(CoIpManager & coIpManager)
	: Command("Call Session management", "list/call/answer/hangup", coIpManager) {

	_menuContent.push_back(std::pair<std::string, boost::function< void () > >("Make a call", boost::bind(&CallCommand::call, this)));
	_menuContent.push_back(std::pair<std::string, boost::function< void () > >("Answer a call", boost::bind(&CallCommand::answer, this)));
	_menuContent.push_back(std::pair<std::string, boost::function< void () > >("Hang up a call", boost::bind(&CallCommand::hangup, this)));
	_menuContent.push_back(std::pair<std::string, boost::function< void () > >("Hold a call", boost::bind(&CallCommand::hold, this)));
	_menuContent.push_back(std::pair<std::string, boost::function< void () > >("Resume a call", boost::bind(&CallCommand::resume, this)));
	_menuContent.push_back(std::pair<std::string, boost::function< void () > >("List calls", boost::bind(&CallCommand::list, this)));
}

CallCommand::~CallCommand() {
}

void CallCommand::list() {
	Menu::listCallSession(_coIpManager);
}

void CallCommand::call() {
	std::cout << "Choose the contact to call";
	Contact * contact = Menu::listContactMenu(_coIpManager);
	CallSession * callSession = _coIpManager.getCallSessionManager().createCallSession();

	callSession->addContact(*contact);
	callSession->start();
}

void CallCommand::answer() {
	CallSession * callSession = Menu::listCallSessionMenu(_coIpManager);
	callSession->start();
}

void CallCommand::hangup() {
	CallSession * callSession = Menu::listCallSessionMenu(_coIpManager);
	callSession->stop();
}

void CallCommand::hold() {
	CallSession * callSession = Menu::listCallSessionMenu(_coIpManager);
	callSession->pause();
}

void CallCommand::resume() {
	CallSession * callSession = Menu::listCallSessionMenu(_coIpManager);
	callSession->resume();
}
