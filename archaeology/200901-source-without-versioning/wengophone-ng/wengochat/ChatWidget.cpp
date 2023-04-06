/*
 * WengoPhone, a voice over Internet phone
 * Copyright (C) 2004-2007  Wengo
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

#include "ChatWidget.h"

#include "WengoChat.h"
#include "Widget.h"

#include "ui_ChatWidget.h"

#include <coipmanager_threaded/chatsessionmanager/TChatSession.h>

#include <qtcoreutil/KeyEventFilter.h>
#include <util/SafeConnect.h>

#include <util/SafeDelete.h>

#include <QtGui/QtGui>

ChatWidget::ChatWidget(WengoChat * wengoChat, TChatSession * session)
	: QWidget(wengoChat) {

	_wengoChat = wengoChat;
	_session = session;

	_ui = new Ui::ChatWidget();
	QWidget * internalWidget = new QWidget();
	_ui->setupUi(internalWidget);

	Widget::createLayout(this);
	layout()->setMargin(10);
	layout()->addWidget(internalWidget);

	_ui->contactList->hide();

	SAFE_CONNECT(_ui->sendButton, SIGNAL(clicked()), SLOT(sendClickedSlot()));

	_keyEventFilter = new KeyPressEventFilter(this, SLOT(keyPressed(QEvent *)));
	_ui->userEntry->installEventFilter(_keyEventFilter);

	SAFE_CONNECT(_session, SIGNAL(messageAddedSignal()),
		SLOT(messageAddedSlot()));

	SAFE_CONNECT(_session, SIGNAL(statusMessageReceivedSignal(EnumChatStatusMessage::ChatStatusMessage, std::string)),
		SLOT(statusMessageReceivedSlot(EnumChatStatusMessage::ChatStatusMessage, std::string)));

	SAFE_CONNECT(_session, SIGNAL(typingStateChangedSignal(Contact, EnumChatTypingState::ChatTypingState)),
		SLOT(typingStateChangedSlot(Contact, EnumChatTypingState::ChatTypingState)));

	messageAddedSlot();

	// We call start for User created Session.
	_session->start();
}

ChatWidget::~ChatWidget() {
	OWSAFE_DELETE(_session);
	OWSAFE_DELETE(_keyEventFilter);
	OWSAFE_DELETE(_ui);
}

void ChatWidget::messageAddedSlot() {
	_ui->history->clear();
	ChatMessageList list = _session->getMessageHistory();
	for (ChatMessageList::const_iterator it = list.begin();
		it != list.end();
		++it) {
		QString message = "<font color='grey'>%1</font> - <font color='red'>%2</font>:<br /> %3";
		message = message.arg(QString::fromStdString((*it).getTime().toString()));
		message = message.arg(QString::fromStdString((*it).getPeer().getPeerId()));
		message = message.arg(QString::fromUtf8((*it).getMessage().c_str()));
		_ui->history->append(message);
	}
}

void ChatWidget::statusMessageReceivedSlot(EnumChatStatusMessage::ChatStatusMessage status, std::string message) {
	if (status == EnumChatStatusMessage::ChatStatusMessageReceived) {
		_wengoChat->showStatusBarMessage("Chat message received: " + message);
	} else if (status == EnumChatStatusMessage::ChatStatusMessageError) {
		_wengoChat->showStatusBarMessage("Chat message sending error: " + message);
	} else if (status == EnumChatStatusMessage::ChatStatusMessageInfo) {
		_wengoChat->showStatusBarMessage("Chat information message: " + message);
	}
}

void ChatWidget::typingStateChangedSlot(Contact contact, EnumChatTypingState::ChatTypingState state) {
	if (state == EnumChatTypingState::ChatTypingStateNotTyping) {
		_wengoChat->showStatusBarMessage(contact.getCompleteName() + "not typing");
	} else if (state == EnumChatTypingState::ChatTypingStateTyping) {
		_wengoChat->showStatusBarMessage(contact.getCompleteName() + "typing");
	} else if (state == EnumChatTypingState::ChatTypingStateStopTyping) {
		_wengoChat->showStatusBarMessage(contact.getCompleteName() + "stopped typing");
	}
}

void ChatWidget::sendClickedSlot() {
	QString message = _ui->userEntry->toPlainText();

	_session->sendMessage(message.toStdString());

	_ui->userEntry->clear();
	_ui->userEntry->ensureCursorVisible();
}

bool ChatWidget::keyPressed(QEvent * event) {
	QKeyEvent * e = static_cast<QKeyEvent *>(event);
	if ((e->key() == Qt::Key_Enter) || (e->key() == Qt::Key_Return)) {
		event->accept();
		sendClickedSlot();
		return true;
	}

	return false;
}
