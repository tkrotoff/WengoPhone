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

#include "CoIpManagerTest.h"

#include "Command.h"

#include "AccountCommand.h"
#include "CallCommand.h"
#include "ChatCommand.h"
#include "ConnectCommand.h"
#include "ContactListCommand.h"
#include "UserProfileCommand.h"

#include <coipmanager/CoIpManager.h>
#include <coipmanager/datamanager/UserProfileManager.h>
#include <coipmanager_base/storage/UserProfileFileStorage.h>

#include <util/Logger.h>
#include <util/Path.h>
#include <util/SafeDelete.h>

#include <boost/bind.hpp>

#include <iostream>

CoIpManagerTest::CoIpManagerTest()
	: Menu("CoIpManagerTest") {

	_quit = false;
	_coIpManager = NULL;

	initCoIpManager();
	initCommands();
	initMenu();
}

CoIpManagerTest::~CoIpManagerTest() {
	for (std::vector<Command *>::iterator it = _commandList.begin();
		it != _commandList.end();
		++it) {
		OWSAFE_DELETE(*it);
	}

	OWSAFE_DELETE(_coIpManager);
}

void CoIpManagerTest::initCoIpManager() {
	// Creating CoIpManager
	CoIpManagerConfig coIpManagerConfig;
	std::string endOfPath = "WengoPhone-coip/";

#if defined(OS_LINUX)
	endOfPath = ".wengophone-coip/";
#endif

	coIpManagerConfig.set(CoIpManagerConfig::CONFIG_PATH_KEY, Path::getConfigurationDirPath() + endOfPath);

	_coIpManager = new CoIpManager(coIpManagerConfig);
	_coIpManager->initialize();
	////
}

void CoIpManagerTest::initCommands() {
	_commandList.push_back(new UserProfileCommand(*_coIpManager));
	_commandList.push_back(new AccountCommand(*_coIpManager));
	_commandList.push_back(new CallCommand(*_coIpManager));
	_commandList.push_back(new ChatCommand(*_coIpManager));
	_commandList.push_back(new ConnectCommand(*_coIpManager));
	_commandList.push_back(new ContactListCommand(*_coIpManager));
}

void CoIpManagerTest::initMenu() {
	for (std::vector<Command *>::const_iterator it = _commandList.begin();
		it != _commandList.end();
		++it) {
		std::string menuName = (*it)->commandName() + ": " + (*it)->commandDesc();
		_menuContent.push_back(std::pair<std::string, boost::function< void () > >(menuName, boost::bind(&Command::exec, (*it))));
	}

	_menuContent.push_back(std::pair<std::string, boost::function< void () > >("Quit", boost::bind(&CoIpManagerTest::quit, this)));
}

void CoIpManagerTest::exec() {
	while (!_quit) {
		executeMenu();
	}
}

void CoIpManagerTest::quit() {
	_quit = true;
}
