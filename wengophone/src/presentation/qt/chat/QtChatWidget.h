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

#ifndef CHATWIDGET_H
#define CHATWIDGET_H

#include <QtGui>

#include "QtEmoticonsWidget.h"
#include "widgetseeker.h"
#include "QtEmoticon.h"


#include <imwrapper/IMContact.h>
#include <imwrapper/IMChatSession.h>
#include <imwrapper/IMChat.h>

#include <control/chat/CChatHandler.h>

#include "QtChatRoomInviteDlg.h"

class QtWengoStyleLabel;

class ChatWidget : public QWidget //, Ui::ChatWidget
{
    Q_OBJECT

public:

    ChatWidget(CChatHandler & cChatHandler, int sessionId,QWidget * parent =0, Qt::WFlags f = 0);

    void setNickBgColor(const QString &color);

    void setNickTextColor(const QString &color);

    void setNickFont(QFont &font);

    void setNickName(const QString & nickname);

	void setIMChatSession(IMChatSession * imChatSession);

	int getSessionId() const { return _sessionId;};

	IMChatSession * getIMChatSession() {return _imChatSession;};

    const QString & nickName();

    const QFont&    nickFont();

    const QString&   nickBgColor();

    const QString&   nickTextColor();

    void  addToHistory(const QString & senderName,const QString & str);

    void setStoppedTypingDelay(int sec) { _stoppedTypingDelay = sec*1000; };

    void setNotTypingDelay(int sec) { _notTypingDelay = sec * 1000;};

    void setRemoteTypingState(const IMChatSession & sender,const IMChat::TypingState state);

    void openContactListFrame();

    bool canDoMultiChat();

protected:

    QWidget *       _widget;

	CChatHandler & _cChatHandler;

	int _sessionId;

	int _stoppedTypingTimerId;

	int _notTypingTimerId;

	int _stoppedTypingDelay;

	int _notTypingDelay;

	bool _isTyping;

	WidgetSeeker    _seeker;

    QFrame * _actionFrame;

   	QtWengoStyleLabel *_emoticonsLabel;

	QtWengoStyleLabel *_fontLabel;

	QTextBrowser *   _chatHistory;

	QTextEdit    *   _chatEdit;

	QPushButton  *   _sendButton;

	QScrollArea  *  _scrollArea;

	QWidget * _contactViewport;

	QFrame * _contactListFrame;

	QLabel * _typingStateLabel;

	IMChatSession * _imChatSession;

	QFont _currentFont;

	QFont _nickFont;

	QString _nickBgColor;

	QString _nickBgColorAlt;

    QString _nickTextColor;

    QString _nickName;

    EmoticonsWidget *_emoticonsWidget;

    const QString replaceUrls(const QString & str, const QString & htmlstr);

	const QString text2Emoticon(const QString &htmlstr);

	const QString Emoticon2Text(const QString &htmlstr);

	virtual void timerEvent ( QTimerEvent * event );

	void addContactToContactListFrame(const Contact & contact);

    void createActionFrame();

    QString generateHtmlHeader(const QString & bgColor,
                                       const QString & barColor,
                                       const QString & stringColor,
                                       const QString & nickName);

	/**
	 * Prepares a message to be sent to the network.
	 *
	 * @param message the message to prepare
	 * @return the prepared message
	 */
	QString prepareMessageForSending(const QString & message);

	/**
	 * Replaces text URLs with HTML URLs.
	 *
	 * @param message the message to change
	 * @return the changed message
	 */
	QString replaceTextURLs(const QString & message);

	/**
	 * Inserts a font tag based on _currentFont.
	 *
	 * @param message the message to change
	 * @return the changed message
	 */
	QString insertFontTag(const QString & message);

public Q_SLOTS:

    void enterPressed();

    void chooseFont();

    void chooseEmoticon();

    void emoticonSelected(QtEmoticon emoticon);

    void urlClicked(const QUrl & link);

    void inviteContact();

    void chatEditChanged();

Q_SIGNALS:

    void newMessage(IMChatSession* session,const QString & msg);

    void newContact(const Contact & contact);

    void contactAdded();
};

#endif
