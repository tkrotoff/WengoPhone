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

#ifndef OWQTCHATWIDGET_H
#define OWQTCHATWIDGET_H

#include "QtEmoticonsWidget.h"
#include "QtEmoticon.h"
#include "QtChatContactInfo.h"
#include "QtChatRoomInviteDlg.h"

#include <control/chat/CChatHandler.h>

#include <imwrapper/IMContact.h>
#include <imwrapper/IMChatSession.h>
#include <imwrapper/IMChat.h>

#include <util/Trackable.h>

#include "ui_ChatRoomWidget.h"

#include <QObject>
#include <QWidget>
#include <QString>
#include <QHash>
#include <QColor>

class QFont;
class QUrl;
class QTimerEvent;
class QtWengoStyleLabel;
class QtEmoticonsManager;
class QtWengoPhone;

class QtChatWidget : public QWidget, public Trackable {
	Q_OBJECT
public:

	QtChatWidget(CChatHandler & cChatHandler, QtWengoPhone * qtWengoPhone, int sessionId,QWidget * parent =0, Qt::WFlags f = 0);

	virtual ~QtChatWidget();

	void setNickName(const QString & nickname);

	void setIMChatSession(IMChatSession * imChatSession);

	int getSessionId() const { return _sessionId;};

	IMChatSession * getIMChatSession() {return _imChatSession;};

	const QString & nickName() const;

	void addToHistory(const QString & senderName,const QString & str);

	void setRemoteTypingState(const IMChatSession & sender,const IMChat::TypingState state);

	bool canDoMultiChat();

	// called from the model's thread
	void contactAddedEventHandler(IMChatSession & sender, const IMContact & imContact);

	// called from the model's thread
	void contactRemovedEventHandler(IMChatSession & sender, const IMContact & imContact);

	void setContactId(QString contactId) { _contactId = contactId;}

	QString getContactId() const { return _contactId;}

public Q_SLOTS:

	void enterPressed(Qt::KeyboardModifiers modifier = Qt::NoModifier);

	void sendButtonClicked();

	void deletePressed();

	void chooseFont();

	void chooseEmoticon();

	void emoticonSelected(QtEmoticon emoticon);

	void urlClicked(const QUrl & link);

	void inviteContact();

	void chatEditChanged();

	void contactAddedEventSlot();

	void contactRemovedEventSlot();

	virtual void setVisible ( bool visible );

Q_SIGNALS:

	void newMessage(IMChatSession* session,const QString & msg);

	void newContact(const Contact & contact);

	void contactAddedEventSignal();

	void contactRemovedEventSignal();

	void contactAdded();

private:

	typedef QHash<QString, QtChatContactInfo> ContactInfoHash;

	const QString replaceUrls(const QString & str, const QString & htmlstr);

	const QString text2Emoticon(const QString & htmlstr);

	const QString emoticon2Text(const QString & htmlstr);

	virtual void timerEvent(QTimerEvent * event);

	void createActionFrame();

	void setupSendButton();

	void updateContactListLabel();

	QtChatContactInfo getQtChatContactInfo(const QString & nickName) const;

	bool hasQtChatContactInfo(const QString & nickname) const;

	QString getNewBackgroundColor() const;

	QString getProtocol() const;

	/**
	 * Prepares a message to be sent to the network.
	 *
	 * @param message the message to prepare
	 * @return the prepared message
	 */
	QString prepareMessageForSending(const QString & message);

	/**
	 * Inserts a font tag based on _currentFont.
	 *
	 * @param message the message to change
	 * @return the changed message
	 */
	QString insertFontTag(const QString & message);

	ContactInfoHash _qtContactInfo;

	QtEmoticonsManager * _qtEmoticonManager;

	Ui::ChatRoomWidget _ui;

	QWidget * _widget;

	CChatHandler & _cChatHandler;

	QtWengoPhone * _qtWengoPhone;

	QString _contactId;

	int _sessionId;

	int _stoppedTypingTimerId;

	int _notTypingTimerId;

	bool _isTyping;

	QtWengoStyleLabel *_emoticonsLabel;

	QtWengoStyleLabel *_fontLabel;

	IMChatSession * _imChatSession;

	QFont _currentFont;

	QString _nickName;

	mutable QColor _lastBackGroundColor;

	EmoticonsWidget *_emoticonsWidget;

	mutable QMutex _mutex;
};

#endif //OWQTCHATWIDGET_H
