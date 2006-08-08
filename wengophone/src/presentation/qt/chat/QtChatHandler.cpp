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

#include "QtChatHandler.h"

#include "QtChatWindow.h"

#include <control/chat/CChatHandler.h>

#include <util/Logger.h>

QtChatHandler::QtChatHandler(CChatHandler & cChatHandler)
	: QObject(NULL),
	_cChatHandler(cChatHandler) {

	_qtChatWindow = NULL;
}

QtChatHandler::~QtChatHandler() {
}

void QtChatHandler::newIMChatSessionCreatedEvent(IMChatSession & imChatSession) {
	if (!_qtChatWindow) {
		_qtChatWindow =  new QtChatWindow(_cChatHandler, imChatSession);
		_qtChatWindow->showToaster(&imChatSession);
	} else {
		_qtChatWindow->addChatSession(&imChatSession);
	}
}

void QtChatHandler::createSession(IMAccount & imAccount, IMContactSet & imContactSet) {
	_cChatHandler.createSession(imAccount, imContactSet);
}

void QtChatHandler::updatePresentation() {
}
