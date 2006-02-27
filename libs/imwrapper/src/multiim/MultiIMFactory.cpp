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

#include "MultiIMFactory.h"

#include <imwrapper/IMAccount.h>
#include <imwrapper/EnumIMProtocol.h>
#include <PhApiFactory.h>
#include <GaimIMFactory.h>

MultiIMFactory::MultiIMFactory(PhApiFactory & phApiFactory, GaimIMFactory & gaimIMFactory)
	: _phApiFactory(phApiFactory), _gaimIMFactory(gaimIMFactory) {
}

IMConnect * MultiIMFactory::createIMConnect(IMAccount & account) {
	if (account.getProtocol() == EnumIMProtocol::IMProtocolSIPSIMPLE) {
		return _phApiFactory.createIMConnect(account);
	} else {
		return _gaimIMFactory.createIMConnect(account);
	}
}

IMChat * MultiIMFactory::createIMChat(IMAccount & account) {
	if (account.getProtocol() == EnumIMProtocol::IMProtocolSIPSIMPLE) {
		return _phApiFactory.createIMChat(account);
	} else {
		return _gaimIMFactory.createIMChat(account);
	}
}

IMPresence * MultiIMFactory::createIMPresence(IMAccount & account) {
	if (account.getProtocol() == EnumIMProtocol::IMProtocolSIPSIMPLE) {
		return _phApiFactory.createIMPresence(account);
	} else {
		return _gaimIMFactory.createIMPresence(account);
	}
}

IMContactList * MultiIMFactory::createIMContactList(IMAccount & account) {
	if (account.getProtocol() == EnumIMProtocol::IMProtocolSIPSIMPLE) {
		return _phApiFactory.createIMContactList(account);
	} else {
		return _gaimIMFactory.createIMContactList(account);
	}
}
