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

#include <control/chat/CChatHandler.h>
#include <util/Logger.h>
#include <QWidget>

QtChatHandler::QtChatHandler(CChatHandler & cChatHandler)
	: QObjectThreadSafe(), _cChatHandler(cChatHandler) {

	_qtChatWidget = NULL;

	_cChatHandler.newIMChatSessionCreatedEvent +=
		boost::bind(&QtChatHandler::newIMChatSessionCreatedEventHandler, this, _1, _2);
}

QtChatHandler::~QtChatHandler() {

}

void QtChatHandler::newIMChatSessionCreatedEventHandler(ChatHandler & sender, IMChatSession & imChatSession) {


	typedef PostEvent2<void (ChatHandler & sender, IMChatSession & imChatSession), ChatHandler &, IMChatSession &> MyPostEvent;
	MyPostEvent * event =
		new MyPostEvent(boost::bind(&QtChatHandler::newIMChatSessionCreatedEventHandlerThreadSafe, this, _1, _2), sender, imChatSession);
	postEvent(event);
}

void QtChatHandler::newIMChatSessionCreatedEventHandlerThreadSafe(ChatHandler & sender, IMChatSession & imChatSession) {
	if (!_qtChatWidget)
	{

		_qtChatWidget =  new ChatWindow(imChatSession);
	}
	else
	{

		_qtChatWidget->addChatSession(&imChatSession);
	}
}

void QtChatHandler::createSession(const IMAccount & imAccount, IMContactSet & imContactSet) {
	_cChatHandler.createSession(imAccount, imContactSet);
}

void QtChatHandler::updatePresentation() {

}

void QtChatHandler::updatePresentationThreadSafe() {

}

void QtChatHandler::initThreadSafe() {
	_qtChatWidget = NULL;
}
