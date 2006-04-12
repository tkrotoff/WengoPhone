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
#include "QtChatTabWidget.h"


#include <model/config/ConfigManager.h>
#include <model/config/Config.h>

#include <imwrapper/IMChatSession.h>

#include <qtutil/Object.h>
#include <util/Logger.h>

ChatWindow::ChatWindow(CChatHandler & cChatHandler, IMChatSession & imChatSession) : QObjectThreadSafe(),
_cChatHandler(cChatHandler)
{
    LOG_DEBUG("ChatWindow::ChatWindow(IMChatSession & imChatSession) : QDialog(), _imChatSession(imChatSession)");
	_imChatSession = &imChatSession;

    _imChatSession->messageReceivedEvent +=
		boost::bind(&ChatWindow::messageReceivedEventHandler, this, _1, _2, _3);

    _imChatSession->typingStateChangedEvent +=
		boost::bind(&ChatWindow::typingStateChangedEventHandler, this, _1, _2, _3);

    typedef PostEvent0<void ()> MyPostEvent;
    MyPostEvent * event =
        new MyPostEvent(boost::bind(&ChatWindow::initThreadSafe, this));
	postEvent(event);

	connect (this,SIGNAL(typingStateChangedSignal(const IMChatSession *,const IMContact *,const  IMChat::TypingState *)),
	              SLOT(typingStateChangedThreadSafe(const IMChatSession *,const IMContact *,const IMChat::TypingState *)),
	              Qt::QueuedConnection);
}
void ChatWindow::typingStateChangedEventHandler(IMChatSession & sender, const IMContact & imContact, IMChat::TypingState state){

	IMChat::TypingState * tmpState = new IMChat::TypingState;
	*tmpState = state;
	typingStateChangedSignal(&sender,&imContact,tmpState);
}

void ChatWindow::typingStateChangedThreadSafe(const IMChatSession * sender, const IMContact * imContact,const IMChat::TypingState * state){
	IMChat::TypingState	tmpState = *state;

	int tabs=_tabWidget->count();
	for (int i=0; i<tabs;i++){
		ChatWidget * widget = dynamic_cast<ChatWidget *> ( _tabWidget->widget(i) );
		if ( widget->getSessionId() == sender->getId() )
		{
			widget->setRemoteTypingState(*sender,*state);
		}
	}
	delete state;
}

void ChatWindow::messageReceivedEventHandler(IMChatSession & sender, const IMContact & from, const std::string & message) {
    typedef PostEvent3<void (IMChatSession & sender, const IMContact & from, const std::string message),
        IMChatSession &, const IMContact&, const std::string> MyPostEvent;
	MyPostEvent * event =
		new MyPostEvent(boost::bind(&ChatWindow::messageReceivedEventHandlerThreadSafe, this, _1, _2, _3), sender, from, message);
	postEvent(event);
}

void ChatWindow::newMessage(IMChatSession *session,const QString & msg)
{
	std::string message(msg.toUtf8().constData());
	session->sendMessage(message);
}

void ChatWindow::show()
{
	Config & config = ConfigManager::getInstance().getCurrentConfig();
	_dialog.show();
	_dialog.activateWindow();
}

void ChatWindow::messageReceivedEventHandlerThreadSafe(IMChatSession & sender, const IMContact & from, const std::string message)
{
	Config & config = ConfigManager::getInstance().getCurrentConfig();

    QString senderName = QString::fromStdString(from.getContactId());

	QString msg = QString::fromUtf8(message.c_str());

    _dialog.show();
	int tabs=_tabWidget->count();

	for (int i=0; i<tabs;i++)
	{
		ChatWidget * widget = dynamic_cast<ChatWidget *> ( _tabWidget->widget(i) );
		if ( widget->getSessionId() == sender.getId() )
		{
			_chatWidget = qobject_cast<ChatWidget *>(_tabWidget->widget(i));
			_chatWidget->addToHistory(senderName,msg);
			if ( _tabWidget->currentWidget() != _chatWidget )
				_tabWidget->setBlinkingTab(i);

			if ( config.getNotificationShowBlinkingWindow()){
				_dialog.activateWindow();
			}
			if ( config.getNotificationShowWindowOnTop()){
				_dialog.show();
				_dialog.raise();
				QApplication::setActiveWindow ( &_dialog );
			}
			return;
		}
	}
}

void ChatWindow::addChatSession(IMChatSession * imChatSession)
{
	imChatSession->messageReceivedEvent +=
		boost::bind(&ChatWindow::messageReceivedEventHandler, this, _1, _2, _3);

	if ( imChatSession->getIMContactSet().size() != 0 ) {
		IMContact from = *imChatSession->getIMContactSet().begin();
		addChat(imChatSession,from);
		_dialog.show();
	}
	else
		LOG_FATAL("New chat session is empty !!!!!");
}

void ChatWindow::initThreadSafe() {
	_widget = WidgetFactory::create(":/forms/chat/chat.ui", &_dialog);
	QGridLayout * layout = new QGridLayout();
	layout->addWidget(_widget);
	layout->setMargin(0);
	_dialog.setLayout(layout);


	new QGridLayout(_widget);
	_tabWidget = new QtChatTabWidget ( _widget );
	_widget->layout()->addWidget(_tabWidget);

	connect ( _tabWidget,SIGNAL(currentChanged (int)), SLOT(tabSelectionChanged(int)));

	// _tabWidget = _seeker.getTabWidget(_widget,"tabWidget");
	_tabWidget->removeTab(0);
	_dialog.resize(384,464);
	_dialog.setWindowTitle(tr("Wengophone Chat"));
	_dialog.show();
	IMContact from = *_imChatSession->getIMContactSet().begin();


	addChat(_imChatSession,from);
}

void ChatWindow::addChat(IMChatSession * session, const IMContact & from) {
	QString nickName = QString().fromStdString(session->getIMChat().getIMAccount().getLogin());
	QString senderName = QString::fromStdString(from.getContactId());
	int tabNumber;
    _chatWidget = new ChatWidget(_cChatHandler,session->getId(), _tabWidget);
	_chatWidget->setIMChatSession(session);

	if (_tabWidget->count() > 0)
		tabNumber = _tabWidget->insertTab(_tabWidget->count(),_chatWidget,senderName);
	else
		tabNumber = _tabWidget->insertTab(0,_chatWidget,senderName);

    _chatWidget->setNickName(nickName);

	// Adding probably missed message
	for (IMChatSession::IMChatMessageList::const_iterator it = session->getReceivedIMChatMessageList().begin();
		it != session->getReceivedIMChatMessageList().end();
		++it) {
		_chatWidget->addToHistory(QString::fromStdString((*it).getIMContact().getContactId()),
			QString::fromUtf8((*it).getMessage().c_str()));
	}


    connect (_chatWidget,SIGNAL(newMessage(IMChatSession *,const QString & )),SLOT(newMessage(IMChatSession *,const QString &)));
	_tabWidget->setCurrentIndex(tabNumber);
	_dialog.show();
}
void ChatWindow::tabSelectionChanged ( int index ){
	_tabWidget->stopBlinkingTab(index);
}
