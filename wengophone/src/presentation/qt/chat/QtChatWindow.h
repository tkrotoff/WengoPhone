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

#ifndef CHATWINDOW_H
#define CHATWINDOW_H

#include <QtGui>
#include <string>
#include <qtutil/WidgetFactory.h>
#include "widgetseeker.h"
#include "QtChatWidget.h"
#include <imwrapper/IMContact.h>
#include <imwrapper/IMChatSession.h>
#include <qtutil/QObjectThreadSafe.h>
#include <control/chat/CChatHandler.h>
// class IMChatSession;
// class IMContact;

class QtChatTabWidget;
//class CChatHandler;
class ChatWindow : public QObjectThreadSafe
{

    Q_OBJECT

public:

    ChatWindow(CChatHandler & cChatHandler, IMChatSession & imChatSession);

	void addChat(IMChatSession * session,const IMContact & from );

	void addChatSession(IMChatSession * imChatSession);

protected:

    QWidget     *   _widget;

    WidgetSeeker    _seeker;

    ChatWidget  *   _chatWidget;

    QtChatTabWidget *   _tabWidget;

    QMenuBar * _menuBar;

    IMChatSession * _imChatSession;

    QWidget			_dialog;

	CChatHandler & _cChatHandler;

public Q_SLOTS:

	void newMessage(IMChatSession* session,const QString & msg);

	void tabSelectionChanged ( int index );

	void show();

protected Q_SLOTS:

	void typingStateChangedThreadSafe(const IMChatSession * sender, const IMContact * imContact,const IMChat::TypingState * state);

Q_SIGNALS:

	void typingStateChangedSignal(const IMChatSession * sender, const IMContact * imContact,const IMChat::TypingState * state);

private:

	void initThreadSafe();

	void messageReceivedEventHandler(IMChatSession & sender, const IMContact & from, const std::string & message);

	void messageReceivedEventHandlerThreadSafe(IMChatSession & sender, const IMContact & from, const std::string  message);

	void typingStateChangedEventHandler(IMChatSession & sender, const IMContact & imContact, IMChat::TypingState state);

};

#endif
