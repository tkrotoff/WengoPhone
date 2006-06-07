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

#ifndef QTCHATHANDLER_H
#define QTCHATHANDLER_H

#include <presentation/PChatHandler.h>
#include <qtutil/QObjectThreadSafe.h>

#include "QtChatWidget.h"
#include "QtChatWindow.h"

class CChatHandler;
class IMContactSet;

/**
 *
 * @ingroup presentation
 * @author Tanguy Krotoff
 * @author Philippe Bernery
 */
class QtChatHandler : public QObjectThreadSafe, public PChatHandler {

    Q_OBJECT

public:

	QtChatHandler(CChatHandler & cChatHandler);

	virtual ~QtChatHandler();

	void createSession(IMAccount & imAccount, IMContactSet & imContactSet);

	void newIMChatSessionCreatedEventHandler(IMChatSession & imChatSession);

	void updatePresentation();

public Q_SLOTS:

	void showToaster(IMChatSession * imChatSession);

private:

	void newIMChatSessionCreatedEventHandlerThreadSafe(IMChatSession & imChatSession);

	void updatePresentationThreadSafe();

	void initThreadSafe();

	CChatHandler & _cChatHandler;

	ChatWindow * _qtChatWidget;
};

#endif	//QTCHATHANDLER_H
