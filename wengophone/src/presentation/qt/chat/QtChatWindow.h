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

#ifndef OWQTCHATWINDOW_H
#define OWQTCHATWINDOW_H

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


class QtChatWindow : public QObject, public Trackable {
	Q_OBJECT

public:

	QtChatWindow(CChatHandler & cChatHandler, IMChatSession & imChatSession);

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

	void statusChangedSignal(IMContact * sender);

public Q_SLOTS:

	void newMessage(IMChatSession* session,const QString & msg);

	void tabSelectionChanged ( int index );

	void show();

	void openContactListFrame();

	void closeContactListFrame();

	void messageReceivedSlot(IMChatSession * sender);

	void typingStateChangedThreadSafe(const IMChatSession * sender, const IMContact * imContact,const IMChat::TypingState * state);

	void inviteContact();

	void callContact();

	void closeTab();

	void ctrlTabPressed();

	void statusChangedSlot(QString contactId);

private:

	typedef QMap <int, QtChatContactWidget *> ChatContactWidgets;

	void flashWindow();

	QString getShortDisplayName(const QString & contactId, const QString & defaultName) const;

	QtChatWidget * _chatWidget;

	QtChatTabWidget * _tabWidget;

	QMenuBar * _menuBar;

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

	void createMenu();

	void showMinimized();

	void showChatWindow();

	QMainWindow * findMainWindow();

	QtWengoPhone * _qtWengoPhone;
};

#endif //OWQTCHATWINDOW_H
