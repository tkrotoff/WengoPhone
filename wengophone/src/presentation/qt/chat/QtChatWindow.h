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

#ifndef OWCHATWINDOW_H
#define OWCHATWINDOW_H

#include <control/chat/CChatHandler.h>

#include <qtutil/WidgetFactory.h>

#include <imwrapper/IMContact.h>
#include <imwrapper/IMChatSession.h>

#include <util/Trackable.h>

#include "QtChatWidget.h"
#include "QtChatContactWidget.h"

#include <QtGui>

#include <string>

class QtChatTabWidget;
class QtWengoPhone;


class ChatWindow : public QObject, public Trackable {
	Q_OBJECT

public:

	ChatWindow(CChatHandler & cChatHandler, IMChatSession & imChatSession);

	void addChat(IMChatSession * session,const IMContact & from );

	void addChatSession(IMChatSession * imChatSession);

	QWidget * getWidget() { return _dialog; }

	void enableChatButton();

	bool chatIsVisible();

	void imContactChangedEventHandler(IMContact & sender);

	void showToaster(IMChatSession * imChatSession);

Q_SIGNALS:

	void messageReceivedSignal(IMChatSession * sender);

	void typingStateChangedSignal(const IMChatSession * sender, const IMContact * imContact,const IMChat::TypingState * state);

	void contactAddedSignal(IMChatSession * session, const IMContact * imContact );

	void statusChangedSignal(IMContact * sender);

public Q_SLOTS:

	void addContactToContactListFrame(const Contact & contact);

	void newMessage(IMChatSession* session,const QString & msg);

	void tabSelectionChanged ( int index );

	void show();

	void openContactListFrame();

	void closeContactListFrame();

protected Q_SLOTS:

	void messageReceivedSlot(IMChatSession * sender);

	void typingStateChangedThreadSafe(const IMChatSession * sender, const IMContact * imContact,const IMChat::TypingState * state);

	void contactAddedThreadSafe(IMChatSession * session, const IMContact * imContact );

	void inviteContact();

	void callContact();

	void closeTab();

	void statusChangedSlot(QString contactId);

protected:

	typedef QMap <int, QtChatContactWidget *> ChatContactWidgets;

	void flashWindow();

	ChatWidget * _chatWidget;

	QtChatTabWidget * _tabWidget;

	QMenuBar * _menuBar;

	QScrollArea * _scrollArea;

	QWidget * _contactViewport;

	QFrame * _contactListFrame;

	QFrame * _inviteFrame;

	QtWengoStyleLabel * _callLabel;

	QtWengoStyleLabel * _inviteLabel;

	IMChatSession * _imChatSession;

	QWidget	* _dialog;

	CChatHandler & _cChatHandler;

	ChatContactWidgets * _chatContactWidgets;

	int _flashTimerId;

	bool _flashStat;

	int _flashCount;

private:

	void createInviteFrame();

	void messageReceivedEventHandler(IMChatSession & sender);

	void typingStateChangedEventHandler(IMChatSession & sender, const IMContact & imContact, IMChat::TypingState state);

	void contactAddedEventHandler(IMChatSession & sender, const IMContact & imContact);

	void createMenu();

	void showMinimized();

	void showChatWindow();

	QMainWindow * findMainWindow();

	QtWengoPhone * _qtWengoPhone;
};

#endif //OWCHATWINDOW_H
