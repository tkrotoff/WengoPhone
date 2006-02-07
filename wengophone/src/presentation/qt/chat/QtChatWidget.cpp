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

#include "QtChatWidget.h"

#include <control/chat/CChatHandler.h>

#include <QLineEdit>
#include <QListWidget>
#include <QPushButton>

#include <IMContact.h>
#include <IMChatSession.h>
#include <WidgetFactory.h>
#include <Object.h>
#include <Logger.h>

QtChatWidget::QtChatWidget(IMChatSession & imChatSession) 
	: QObject(), _imChatSession(imChatSession) {

	_chatWidget = WidgetFactory::create(":/forms/chat/ChatWidget.ui", NULL);

	_lineEdit = Object::findChild<QLineEdit *>(_chatWidget, "lineEdit");
	_listWidget = Object::findChild<QListWidget *>(_chatWidget, "textEdit");
	_sendButton = Object::findChild<QPushButton *>(_chatWidget, "sendButton");

	connect(_lineEdit, SIGNAL(returnPressed()), SLOT(sendMessage()));
	connect(_sendButton, SIGNAL(clicked()), SLOT(sendMessage()));

	_imChatSession.messageReceivedEvent +=
		boost::bind(&QtChatWidget::messageReceivedEventHandler, this, _1, _2, _3);
}

QtChatWidget::~QtChatWidget() {
	LOG_DEBUG("chat window closed");
	_imChatSession.sendMessage("I closed the conversation window");
}

void QtChatWidget::sendMessage() {
	LOG_DEBUG("sending message: " + _lineEdit->text().toStdString());
	_imChatSession.sendMessage(_lineEdit->text().toStdString());
}

void QtChatWidget::messageReceivedEventHandler(IMChatSession & sender, const IMContact & from, const std::string & message) {
	LOG_DEBUG("message received: " + message);
	
	_listWidget->addItem(QString::fromStdString(from.getContactId() + ": " + message));
}
