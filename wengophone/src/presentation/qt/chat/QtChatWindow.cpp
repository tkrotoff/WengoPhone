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

#include <model/contactlist/ContactList.h>
#include <model/contactlist/ContactGroup.h>

#include <model/profile/UserProfile.h>
#include <model/config/ConfigManager.h>
#include <model/config/Config.h>

#include <imwrapper/IMChatSession.h>

#include <qtutil/Object.h>
#include <util/Logger.h>

ChatWindow::ChatWindow(CChatHandler & cChatHandler, IMChatSession & imChatSession) : QObjectThreadSafe(),
_cChatHandler(cChatHandler){
    LOG_DEBUG("ChatWindow::ChatWindow(IMChatSession & imChatSession) : QDialog(), _imChatSession(imChatSession)");
	_imChatSession = &imChatSession;

    _imChatSession->messageReceivedEvent +=
		boost::bind(&ChatWindow::messageReceivedEventHandler, this, _1);

    _imChatSession->typingStateChangedEvent +=
		boost::bind(&ChatWindow::typingStateChangedEventHandler, this, _1, _2, _3);

	_imChatSession->contactAddedEvent +=
		boost::bind(&ChatWindow::contactAddedEventHandler, this, _1, _2);

    typedef PostEvent0<void ()> MyPostEvent;
    MyPostEvent * event =
        new MyPostEvent(boost::bind(&ChatWindow::initThreadSafe, this));
	postEvent(event);

	connect (this,SIGNAL(typingStateChangedSignal(const IMChatSession *,const IMContact *,const  IMChat::TypingState *)),
	              SLOT(typingStateChangedThreadSafe(const IMChatSession *,const IMContact *,const IMChat::TypingState *)),
	              Qt::QueuedConnection);

	connect (this,SIGNAL(contactAddedSignal(IMChatSession *,const IMContact *)),
	              SLOT(contactAddedThreadSafe(IMChatSession *,const IMContact *)),
	              Qt::QueuedConnection);


}

void ChatWindow::initThreadSafe(){

	QGridLayout * glayout;
	_chatContactWidgets = new ChatContactWidgets();
	_dialog = new QDialog();
	LOG_DEBUG("************ Creating Chat window ************* ");
	// Create the menu bar
	_menuBar = new QMenuBar(_dialog);
	QMenu * WengoMenu = new QMenu("Wengo");
	WengoMenu->addAction("Exit");
	_menuBar->addMenu(WengoMenu);

	_contactListFrame = new QFrame();

	glayout = new QGridLayout();
	glayout->setMargin(5);
	glayout->setSpacing(0);
	_dialog->setLayout(glayout);

	_tabWidget = new QtChatTabWidget ( _dialog );

	glayout->addWidget(_menuBar);
	glayout->addWidget(_contactListFrame);
	glayout->addWidget(_tabWidget);

	connect ( _tabWidget,SIGNAL(currentChanged (int)), SLOT(tabSelectionChanged(int)) );

	_tabWidget->removeTab(0);
	_dialog->resize(384,464);
	_dialog->setWindowTitle(tr("Wengophone chat window"));
	_dialog->show();

	// Create the contact list scroll area
	QGridLayout * frameLayout = new QGridLayout(_contactListFrame);
	_contactListFrame->setVisible(false);
	_scrollArea = new QScrollArea(_contactListFrame);
	frameLayout->addWidget(_scrollArea);

	_contactViewport = new QWidget(_scrollArea);

	_scrollArea->setWidget(_contactViewport);
	new QHBoxLayout(_contactViewport);
	_contactViewport->layout()->setMargin(0);
	_contactViewport->layout()->setSpacing(5);
	dynamic_cast <QHBoxLayout *> (_contactViewport->layout())->addStretch ( 1 );
	_scrollArea->setWidgetResizable(true);

	QPalette palette = _contactViewport->palette();
	palette.setColor(QPalette::Active,QPalette::Window,Qt::white);
	_contactViewport->setPalette(palette);
	_contactViewport->setAutoFillBackground ( true );

	// *****
	IMContact from = *_imChatSession->getIMContactSet().begin();
	addChat(_imChatSession,from);

	openContactListFrame();
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

void ChatWindow::messageReceivedEventHandler(IMChatSession & sender) {
    typedef PostEvent1<void (IMChatSession & sender), IMChatSession &> MyPostEvent;
	MyPostEvent * event =
		new MyPostEvent(boost::bind(&ChatWindow::messageReceivedEventHandlerThreadSafe, this, _1), sender);
	postEvent(event);
}

void ChatWindow::newMessage(IMChatSession *session,const QString & msg){
	std::string message(msg.toUtf8().constData());
	session->sendMessage(message);
}

void ChatWindow::show(){
	Config & config = ConfigManager::getInstance().getCurrentConfig();
	_dialog->show();
	_dialog->activateWindow();
}

void ChatWindow::messageReceivedEventHandlerThreadSafe(IMChatSession & sender) {
	Config & config = ConfigManager::getInstance().getCurrentConfig();

	IMChatSession::IMChatMessage * imChatMessage = sender.getReceivedMessage();
	if (imChatMessage) {
		const IMContact & from = imChatMessage->getIMContact();
		std::string message = imChatMessage->getMessage();
		delete imChatMessage;

		QString senderName = QString::fromStdString(from.getContactId());

		QString msg = QString::fromUtf8(message.c_str());

		_dialog->show();
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
					_dialog->activateWindow();
				}
				if ( config.getNotificationShowWindowOnTop()){
					_dialog->show();
					_dialog->raise();
					QApplication::setActiveWindow ( _dialog );
				}
				return;
			}
		}
	}
}

void ChatWindow::contactAddedEventHandler(IMChatSession & sender, const IMContact & imContact) {
	// Send message to the GUI thread
	qDebug() << "************** ChatWindow::contactAddedEventHandler *********************";
	contactAddedSignal(&sender, &imContact );
}

void ChatWindow::contactAddedThreadSafe(IMChatSession * session, const IMContact * imContact ){

	int sessionid = session->getId();

	ChatContactWidgets::iterator it;

	it = _chatContactWidgets->find(sessionid);
	if (  it == _chatContactWidgets->end() ){

		// Create the contact widget
		QtChatContactWidget * widget = new QtChatContactWidget(session,_cChatHandler,_contactViewport);
		dynamic_cast <QHBoxLayout *>(_contactViewport->layout())->insertWidget ( _contactViewport->layout()->count()-1,widget);
		_chatContactWidgets->insert(sessionid,widget);
	}
	else
	{
		// Update the Contact widget
		QtChatContactWidget * widget;
		widget = (*it);
		widget->updateDisplay();
	}
}

void ChatWindow::addChatSession(IMChatSession * imChatSession){
	imChatSession->messageReceivedEvent +=
		boost::bind(&ChatWindow::messageReceivedEventHandler, this, _1);

	if ( imChatSession->getIMContactSet().size() != 0 ) {
		IMContact from = *imChatSession->getIMContactSet().begin();
		addChat(imChatSession,from);
		_dialog->show();
	}
	else
		LOG_FATAL("New chat session is empty !!!!!");
}

void ChatWindow::addChat(IMChatSession * session, const IMContact & from) {

	QString nickName = QString().fromStdString(session->getIMChat().getIMAccount().getLogin());
	QString senderName = QString::fromStdString(from.getContactId());
	int tabNumber;
    _chatWidget = new ChatWidget(_cChatHandler,session->getId(), _tabWidget);
	_chatWidget->setIMChatSession(session);
//	connect ( _chatWidget, SIGNAL( newContact(const Contact & ) ), SLOT ( addContactToContactListFrame(const Contact &)));

	if (_tabWidget->count() > 0)
		tabNumber = _tabWidget->insertTab(_tabWidget->count(),_chatWidget,senderName);
	else
		tabNumber = _tabWidget->insertTab(0,_chatWidget,senderName);

    _chatWidget->setNickName(nickName);

	// Adding probably missed message
	IMChatSession::IMChatMessage * imChatMessage = session->getReceivedMessage();
	while (imChatMessage) {
		_chatWidget->addToHistory(QString::fromStdString(imChatMessage->getIMContact().getContactId()),
			QString::fromUtf8(imChatMessage->getMessage().c_str()));

		imChatMessage = session->getReceivedMessage();
	}
	////

    connect (_chatWidget,SIGNAL(newMessage(IMChatSession *,const QString & )),SLOT(newMessage(IMChatSession *,const QString &)));

	_tabWidget->setCurrentIndex(tabNumber);

	Contact * contact = _cChatHandler.getUserProfile().getContactList().findContactThatOwns(from);
	if ( contact ){
		contactAddedSignal(session, &from );
		// addContactToContactListFrame(*contact);
	}else{
		//
		// Contact * contact = _cChatHandler.getUserProfile().getPresenceHandler().getContactIcon(from);
	}
	_dialog->show();
}

void ChatWindow::tabSelectionChanged ( int index ){
	_tabWidget->stopBlinkingTab(index);
}

void ChatWindow::addContactToContactListFrame(const Contact & contact){
/*
	QLabel * contactLabel = new QLabel();
	_contactViewport->layout()->addWidget(contactLabel);
	contactLabel->setMaximumSize(85,85);
	contactLabel->setMinimumSize(85,85);

	Picture picture = contact.getIcon();
	std::string data = picture.getData();
	QPixmap pixmap;
	if ( ! data.empty() ){
		pixmap.loadFromData((uchar *)data.c_str(), data.size());
	}else{
			pixmap.load(":pics/contact_picture.png");
	}
	pixmap = pixmap.scaled(80,80);

	QPixmap border;
	border.load(":pics/contact_border.png");

	QPainter painter(&pixmap);
	painter.drawPixmap(0,0,border);
	painter.end();
	contactLabel->setPixmap(pixmap);
*/
}

void ChatWindow::openContactListFrame(){
	QSize size = _contactListFrame->minimumSize();
	size.setHeight(120);
	_contactListFrame->setMinimumSize(size);
	size = _contactListFrame->maximumSize();
	size.setHeight(120);
	_contactListFrame->setMaximumSize(size);
	_contactListFrame->setVisible(true);
	_contactListFrame->setFrameShape(QFrame::NoFrame);
	_contactListFrame->setFrameShadow(QFrame::Plain);
	_contactListFrame->setAutoFillBackground(true);

	_scrollArea->setFrameShape(QFrame::NoFrame);
	_scrollArea->setFrameShadow(QFrame::Plain);
	// _scrollArea->setHorizontalScrollBarPolicy ( Qt::ScrollBarAlwaysOff );
}

void ChatWindow::closeContactListFrame(){
	_contactListFrame->setVisible(false);
}
