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

#include "ui_ChatWidget.h"

#include "emoticons/QtEmoticon.h"

#include <control/chat/CChatHandler.h>

#include <imwrapper/IMContact.h>
#include <imwrapper/IMChatSession.h>
#include <imwrapper/IMChat.h>

#include <util/Trackable.h>

#include <QtGui/QtGui>

class QtChatEditActionBarWidget;
class QtChatEditWidget;
class QtChatHistoryWidget;
class QtChatTabWidget;
class QtChatAvatarFrame;
class QtWengoPhone;
class QtWengoStyleLabel;
class EmoticonsWidget;
class QFont;
class QUrl;
class QTimerEvent;

/**
 *
 * @ingroup presentation
 * @author Mr K.
 * @author Mathieu Stute
 */
class QtChatWidget : public QWidget, public Trackable {
	Q_OBJECT
public:

	QtChatWidget(CChatHandler & cChatHandler, QtWengoPhone * qtWengoPhone,
		int sessionId, QWidget * parent, QtChatTabWidget * qtChatTabWidget);

	virtual ~QtChatWidget();

	void setNickName(const QString & nickname) {_nickName = nickname;}

	const QString & nickName() const {return _nickName;}

	bool canDoMultiChat() {return _imChatSession->canDoMultiChat();}

	void setContactId(QString contactId) {_contactId = contactId;}

	int getSessionId() const {return _sessionId;}

	IMChatSession * getIMChatSession() {return _imChatSession;}

	QString getContactId() const {return _contactId;}

	// called from the model's thread
	void contactAddedEventHandler(IMChatSession & sender, const IMContact & imContact);

	// called from the model's thread
	void contactRemovedEventHandler(IMChatSession & sender, const IMContact & imContact);

	void setIMChatSession(IMChatSession * imChatSession);

	void addToHistory(const QString & senderName, const QString & str);

public Q_SLOTS:

	/**
	 * QtChatEditWidget text has changed
	 */
	void chatEditTextChanged();

	void enterPressed(Qt::KeyboardModifiers modifier = Qt::NoModifier);

	void sendButtonClicked();

	void deletePressed();

	void changeFont();

	void chooseEmoticon();

	void emoticonSelected(QtEmoticon emoticon);

	void showInviteDialog();

	void contactAddedEventSlot(const IMContact & imContact);

	void contactRemovedEventSlot(const IMContact & imContact);

	virtual void setVisible(bool visible);

	void fileDraggedSlot(const QString & filename);

Q_SIGNALS:

	void newMessage(IMChatSession* session,const QString & msg);

	void contactAddedEventSignal(const IMContact & imContact);

	void contactRemovedEventSignal(const IMContact & imContact);

private Q_SLOTS:

	void avatarFrameButtonClicked();

	void changeFontColor();

	void boldClickedSlot();

	void italicClickedSlot();

	void underlineClickedSlot();

private:

	typedef QHash<QString, QString> UserColorHash;
	
	QString getUserColor(const QString & nickName) const {
		return _userColorHash[nickName];
	}
	
	virtual void timerEvent(QTimerEvent * event);

	void sendMessage();

	void updateAvatarFrame();

	void updateUserAvatar();

	bool hasUserColor(const QString & nickname) const;

	/**
	 * @brief stop the timer associated to _notTypingTimerId
	 */
	void stopNotTypingTimer();

	/**
	 * @brief stop the timer associated to _stoppedTypingTimerId
	 */
	void stopStoppedTypingTimer();

	void addAvatarFrame();

	void removeAvatarFrame();

	QString getNewBackgroundColor() const;

	CChatHandler & _cChatHandler;

	QtWengoPhone * _qtWengoPhone;

	IMChatSession * _imChatSession;

	UserColorHash _userColorHash;

	QWidget * _widget;

	QString _contactId;

	int _sessionId;

	int _stoppedTypingTimerId;

	int _notTypingTimerId;

	bool _isTyping;

	bool _isAvatarFrameOpened;

	QString _nickName;

	mutable QColor _lastBackGroundColor;

	mutable QMutex _mutex;

	QtChatEditActionBarWidget * _editActionBar;

	QtChatEditWidget * _chatEdit;

	QtChatHistoryWidget * _chatHistory;

	QtChatAvatarFrame * _avatarFrame;

	EmoticonsWidget * _emoticonsWidget;

	Ui::ChatWidget _ui;

	// font style settings
	QFont _currentFont;
	QColor _currentColor;
	bool _bold;
	bool _italic;
	bool _underline;
	////
};

#endif //OWQTCHATWIDGET_H
