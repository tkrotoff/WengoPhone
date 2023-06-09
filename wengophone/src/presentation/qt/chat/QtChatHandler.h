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

#ifndef OWQTCHATHANDLER_H
#define OWQTCHATHANDLER_H

#include <presentation/PChatHandler.h>

#include <QtCore/QObject>

class CChatHandler;
class IMAccount;
class IMContactSet;
class QtChatWindow;
class QtWengoPhone;

/**
 *
 * @ingroup presentation
 * @author Tanguy Krotoff
 * @author Philippe Bernery
 */
class QtChatHandler : public QObject, public PChatHandler {
	Q_OBJECT
public:

	QtChatHandler(CChatHandler & cChatHandler, QtWengoPhone & qtWengoPhone);

	~QtChatHandler();

	void createSession(IMAccount & imAccount, IMContactSet & imContactSet);

	void newIMChatSessionCreatedEvent(IMChatSession & imChatSession);

	void updatePresentation();

	void deletePresentation();

private:

	CChatHandler & _cChatHandler;

	QtChatWindow * _qtChatWindow;

	QtWengoPhone & _qtWengoPhone;
};

#endif	//OWQTCHATHANDLER_H
