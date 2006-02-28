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

#include "QtChatWindow.h"
#include "QtChatWidget.h"

#include <Object.h>
#include <Logger.h>

ChatWindow::ChatWindow(IMChatSession & imChatSession) : QObjectThreadSafe(), _imChatSession(imChatSession)
{
    LOG_DEBUG("ChatWindow::ChatWindow(IMChatSession & imChatSession) : QDialog(), _imChatSession(imChatSession)");
    _imChatSession.messageReceivedEvent +=
		boost::bind(&ChatWindow::messageReceivedEventHandler, this, _1, _2, _3);

    typedef PostEvent0<void ()> MyPostEvent;
    MyPostEvent * event = 
        new MyPostEvent(boost::bind(&ChatWindow::initThreadSafe, this));
	postEvent(event);
}

void ChatWindow::messageReceivedEventHandler(IMChatSession & sender, const IMContact & from, const std::string & message) {
    typedef PostEvent3<void (IMChatSession & sender, const IMContact & from, const std::string message),
        IMChatSession &, const IMContact&, const std::string> MyPostEvent;
	MyPostEvent * event = 
		new MyPostEvent(boost::bind(&ChatWindow::messageReceivedEventHandlerThreadSafe, this, _1, _2, _3), sender, from, message);
	postEvent(event);
}

void ChatWindow::newMessage(const QString & msg)
{
    LOG_DEBUG("ChatWindow::newMessage : sending message");
    _imChatSession.sendMessage(msg.toStdString());
}

void ChatWindow::messageReceivedEventHandlerThreadSafe(IMChatSession & sender, const IMContact & from, const std::string message)
{
	LOG_DEBUG("message received: " + message);
    QString senderName = QString::fromStdString(from.getContactId());
    QString msg = QString::fromStdString(message);
    _dialog.show();
    _chatWidget->addToHistory(senderName,msg);
}

void ChatWindow::initThreadSafe() {
	
    _widget = WidgetFactory::create(":/forms/chat.ui", &_dialog);
    QGridLayout * layout = new QGridLayout();
    layout->addWidget(_widget);
    layout->setMargin(0);
    _dialog.setLayout(layout);
    
    _tabWidget = _seeker.getTabWidget(_widget,"tabWidget");
    _tabWidget->removeTab(0);
    
    _chatWidget = new ChatWidget(_tabWidget);
    _tabWidget->insertTab(0,_chatWidget,"Chat");
    _chatWidget->setNickName(QString().fromStdString(_imChatSession.getIMChat().getIMAccount().getLogin()));
    connect (_chatWidget,SIGNAL(newMessage(const QString & )),SLOT(newMessage(const QString &)));
    
    LOG_DEBUG("ChatWindow init ok");

}
