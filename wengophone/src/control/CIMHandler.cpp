/*
 * WengoPhone, a voice over Internet phone
 * Copyright (C) 2004-2005  Wengo
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

#include "CIMHandler.h"

#include <model/IMHandler.h>
#include <control/chat/CChat.h>
#include <control/presence/CPresence.h>
#include <presentation/PFactory.h>

#include <Logger.h>

CIMHandler::CIMHandler(IMHandler & imHandler, CWengoPhone & cWengoPhone)
	: _imHandler(imHandler),
	_cWengoPhone(cWengoPhone) {

	_pIMHandler = PFactory::getFactory().createPresentationIMHandler(*this);

	_imHandler.chatCreatedEvent += boost::bind(&CIMHandler::chatCreatedEventHandler, this, _1, _2);
	_imHandler.presenceCreatedEvent += boost::bind(&CIMHandler::presenceCreatedEventHandler, this, _1, _2);
}

void CIMHandler::chatCreatedEventHandler(IMHandler & sender, IMChat & chat) {
	CChat * cChat = new CChat(chat, *this);

	LOG_DEBUG("CChat created");
}

void CIMHandler::presenceCreatedEventHandler(IMHandler & sender, Presence & presence) {
	CPresence * cPresence = new CPresence(presence, *this);

	LOG_DEBUG("CPresence created");
}
