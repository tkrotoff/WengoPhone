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

#include "ConnectCommand.h"

#include <coipmanager/CoIpManager.h>
#include <coipmanager_base/account/Account.h>
#include <coipmanager/connectmanager/ConnectManager.h>

#include <util/SafeDelete.h>
#include <event/Event2.h>

ConnectCommand::ConnectCommand(CoIpManager & coIpManager)
	: Command("Connect management", "Connect/Disconnect account", coIpManager) {

	_menuContent.push_back(std::pair<std::string, boost::function< void () > >("Connect an account", boost::bind(&ConnectCommand::connect, this)));
	_menuContent.push_back(std::pair<std::string, boost::function< void () > >("Disconnect an account", boost::bind(&ConnectCommand::disconnect, this)));
}

ConnectCommand::~ConnectCommand() {
}

void ConnectCommand::connect() {
	Account * account = Menu::listAccountMenu(_coIpManager);
	_coIpManager.getConnectManager().connect(account->getUUID());
	OWSAFE_DELETE(account);
}

void ConnectCommand::disconnect() {
	Account * account = Menu::listAccountMenu(_coIpManager);
	_coIpManager.getConnectManager().disconnect(account->getUUID());
	OWSAFE_DELETE(account);
}

