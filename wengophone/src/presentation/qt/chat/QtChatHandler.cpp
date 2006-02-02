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

#include "QtChatWidget.h"

#include <control/chat/CChatHandler.h>

#include <Logger.h>

#include <QWidget>

QtChatHandler::QtChatHandler(CChatHandler & cChatHandler) 
	: _cChatHandler(cChatHandler) {

	_cChatHandler.newChatSessionCreatedEvent +=
		boost::bind(&QtChatHandler::newChatSessionCreatedEventHandler, this, _1, _2);
}

QtChatHandler::~QtChatHandler() {

}

void QtChatHandler::newChatSessionCreatedEventHandler(ChatHandler & sender, IMChatSession & imChatSession) {
	LOG_DEBUG("new ChatSession created");
	QtChatWidget * qtChatWidget = new QtChatWidget(imChatSession);
	qtChatWidget->getWidget()->show();
}
