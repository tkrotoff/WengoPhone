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
#include "../QtWengoPhone.h"

#include <model/contactlist/ContactList.h>
#include <model/contactlist/ContactGroup.h>

#include <model/profile/UserProfile.h>
#include <model/config/ConfigManager.h>
#include <model/config/Config.h>

#include <control/CWengoPhone.h>

#include <imwrapper/IMChatSession.h>

#include <qtutil/QtWengoStyleLabel.h>

#include <qtutil/Object.h>
#include <util/Logger.h>

ChatWindow::ChatWindow(CChatHandler & cChatHandler, IMChatSession & imChatSession) : QObjectThreadSafe(NULL),
_cChatHandler(cChatHandler){
    // LOG_DEBUG("ChatWindow::ChatWindow(IMChatSession & imChatSession) : QDialog(), _imChatSession(imChatSession)");
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

	_dialog = new QDialog(findMainWindow());

	LOG_DEBUG("************ Creating Chat window ************* ");
	// Create the menu bar
	_menuBar = new QMenuBar(_dialog);

    createMenu();

    _contactListFrame = new QFrame();

	glayout = new QGridLayout();
	glayout->setMargin(0);
	glayout->setSpacing(0);
	_dialog->setLayout(glayout);

	_tabWidget = new QtChatTabWidget ( _dialog );
	QPushButton * closeTabButton = new QPushButton(_tabWidget);
	QIcon closeIcon(QPixmap(":/pics/close_normal.png"));
	closeIcon.addPixmap(QPixmap(":/pics/close_on.png"),QIcon::Normal,QIcon::On);
	closeTabButton->setIcon(closeIcon);
	closeTabButton->setMaximumSize(QSize(16,16));
	closeTabButton->setMinimumSize(QSize(16,16));
	closeTabButton->resize(QSize(16,16));
	closeTabButton->setToolTip(tr("close chat"));
	closeTabButton->setFlat ( true );

	_tabWidget->setCornerWidget ( closeTabButton,Qt::TopRightCorner);

	connect ( closeTabButton, SIGNAL(clicked() ), SLOT(closeTab()));

/*
	QPalette dialogPalette = _dialog->palette();
	dialogPalette.setColor(QPalette::Window, QColor(60,60,60));
	_dialog->setPalette(dialogPalette);
*/
	_inviteFrame = new QFrame(_dialog);
	createInviteFrame();

	glayout->addWidget(_menuBar);
	glayout->addWidget(_inviteFrame);
	glayout->addWidget(_contactListFrame);
	glayout->addWidget(_tabWidget);

	connect ( _tabWidget,SIGNAL(currentChanged (int)), SLOT(tabSelectionChanged(int)) );

	_tabWidget->removeTab(0);
	_dialog->resize(300,464);
	_dialog->setWindowTitle(tr("WengoPhone chat window"));
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
void ChatWindow::createInviteFrame(){

    _inviteFrame->setMaximumSize(QSize(10000,80));
    _inviteFrame->setMinimumSize(QSize(16,80));
	QGridLayout * layout = new QGridLayout(_inviteFrame);
	_callLabel = new QtWengoStyleLabel(_inviteFrame);
	_inviteLabel = new QtWengoStyleLabel(_inviteFrame);
	_callLabel->setPixmaps(
						QPixmap(":/pics/chat/chat_call_bar_button.png"),
						QPixmap(),
						QPixmap(), // Fill

						QPixmap(":/pics/chat/chat_call_bar_button_on.png"),
						QPixmap(":/pics/profilebar/bar_separator.png"),
						QPixmap()  // Fill
	                      );
	_callLabel->setMaximumSize(QSize(46,65));
	_callLabel->setMinimumSize(QSize(46,46));

	_inviteLabel->setPixmaps(
						QPixmap(),
						QPixmap(":/pics/profilebar/bar_end.png"), //
						QPixmap(":/pics/profilebar/bar_fill.png"), // Fill
						QPixmap(),
						QPixmap(":/pics/profilebar/bar_on_end.png"),
						QPixmap(":/pics/profilebar/bar_on_fill.png") // Fill
	                      );
	_inviteLabel->setMinimumSize(QSize(120,65));
	_inviteLabel->setText(tr("   invite"));
	_inviteLabel->setTextColor(Qt::white);

	_inviteLabel->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    connect(_inviteLabel,SIGNAL(clicked()), SLOT(inviteContact()));

	layout->addWidget(_callLabel,0,0);
	layout->addWidget(_inviteLabel,0,1);

	layout->setMargin(0);
	layout->setSpacing(0);

}

void ChatWindow::inviteContact(){

    ChatWidget * widget = dynamic_cast<ChatWidget *> ( _tabWidget->widget(_tabWidget->currentIndex() ) );
    if ( widget){
        if (widget->canDoMultiChat()){
            widget->inviteContact();
        }
    }
}

void ChatWindow::closeTab(){
    ChatWidget * widget = dynamic_cast<ChatWidget *> ( _tabWidget->widget(_tabWidget->currentIndex() ) );

    if ( widget){
        if (widget->canDoMultiChat()){
            widget->getIMChatSession()->close();
        }
    }
    delete widget;
    if ( _tabWidget->count() == 0 )
        _dialog->hide();
    //_tabWidget->removeTab ( _tabWidget->currentIndex() );
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

				_dialog->activateWindow();
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

	imChatSession->typingStateChangedEvent +=
		boost::bind(&ChatWindow::typingStateChangedEventHandler, this, _1, _2, _3);

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

	_contactListFrame->setVisible(false);
	/*
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
	tbfoftjs
	*/
}

void ChatWindow::closeContactListFrame(){
	_contactListFrame->setVisible(false);
}

void ChatWindow::createMenu(){

    QtWengoPhone * mainWindow = dynamic_cast<QtWengoPhone *> (_cChatHandler.getCWengoPhone().getPresentation());
    QAction * action;

   	QMenu * WengoMenu = new QMenu("Wengo");

   	action = WengoMenu->addAction(tr("View my &Wengo Account"));
   	connect(action,SIGNAL(triggered(bool)),mainWindow,SLOT(showWengoAccount()));

   	action = WengoMenu->addAction(tr("Edit my &Profile"));
   	connect(action,SIGNAL(triggered(bool)),mainWindow,SLOT(editMyProfile()));

   	WengoMenu->addSeparator ();
   	action = WengoMenu->addAction(tr("&Call-out service"));
   	connect(action,SIGNAL(triggered(bool)),mainWindow,SLOT(showCallOut()));

   	action = WengoMenu->addAction(tr("&Short text messages (SMS) "));
   	connect(action,SIGNAL(triggered(bool)),mainWindow,SLOT(showSms()));

   	WengoMenu->addAction(tr("&Voicemail"));
   	connect(action,SIGNAL(triggered(bool)),mainWindow,SLOT(showVoiceMail()));

   	WengoMenu->addSeparator ();

   	action = WengoMenu->addAction(tr("&Open Another Wengo Account"));
   	connect(action,SIGNAL(triggered(bool)),mainWindow,SLOT(openWengoAccount()));

	WengoMenu->addAction(tr("&Log off"));


   	WengoMenu->addSeparator ();
	action = WengoMenu->addAction(tr("&Close"));
	connect (action,SIGNAL(triggered(bool)),mainWindow,SLOT(exitApplication()));
	_menuBar->addMenu(WengoMenu);

	QMenu * ContactsMenu = new QMenu(tr("&Contact"));

    action = ContactsMenu->addAction(tr("Add a contact"));
    connect (action,SIGNAL(triggered(bool)),mainWindow,SLOT(addContact()));

    ContactsMenu->addAction(tr("Search for contact"));

    ContactsMenu->addAction(tr("&Manage blocked contacts"));
    ContactsMenu->addSeparator ();
    ContactsMenu->addAction(tr("Add a group"));
    ContactsMenu->addAction(tr("Show contact groups"));
    ContactsMenu->addSeparator ();

    action = ContactsMenu->addAction(tr("Show / hide contacts offline"));
    connect (action,SIGNAL(triggered(bool)),mainWindow,SLOT(showHideOffLineContacts()));

    QMenu * sortMenu = new QMenu(tr("Sort contacts"));
    sortMenu->addAction(tr("alphabetically"));
    sortMenu->addAction(tr("by presence"));
    sortMenu->addAction(tr("by media"));
    sortMenu->addAction(tr("by protocol"));
    ContactsMenu->addMenu(sortMenu);

    _menuBar->addMenu(ContactsMenu);

    QMenu * Actions = new QMenu(tr("&Actions"));

    action = Actions->addAction(tr("Send a short text message (SMS)"));
    connect (action,SIGNAL(triggered(bool)),mainWindow,SLOT(sendSms()));

    Actions->addAction(tr("Forward incoming calls"));
    Actions->addSeparator ();
    Actions->addAction(tr("Create a conference call"));
    Actions->addAction(tr("Create a conference chat"));
    _menuBar->addMenu(Actions);

    QMenu * ToolsMenu = new QMenu(tr("&Tools"));

    action = ToolsMenu->addAction(tr("Instant Messaging settings"));
    connect (action,SIGNAL(triggered(bool)),mainWindow,SLOT(showAccountSettings()));

    ToolsMenu->addSeparator ();
    action = ToolsMenu->addAction(tr("Configuration"));
    connect (action,SIGNAL(triggered(bool)),mainWindow,SLOT(showConfig()));

    action = ToolsMenu->addAction(tr("Advanced configuration"));
    connect (action,SIGNAL(triggered(bool)),mainWindow,SLOT(showAdvancedConfig()));

    ToolsMenu->addSeparator ();
    ToolsMenu->addAction(tr("View toolbar"));
    ToolsMenu->addAction(tr("View adressbar"));
    ToolsMenu->addAction(tr("View dialpad"));
    ToolsMenu->addAction(tr("View avatars in chat window"));
    ToolsMenu->addSeparator ();

    QMenu * HistoryMenu = new QMenu(tr("Clear History"));

    action = HistoryMenu->addAction(tr("Outgoing Calls"));
    connect (action,SIGNAL(triggered(bool)),mainWindow,SLOT(eraseHistoryOutgoingCalls()));

    action = HistoryMenu->addAction(tr("Incoming Calls"));
    connect (action,SIGNAL(triggered(bool)),mainWindow,SLOT(eraseHistoryIncomingCalls()));

    action = HistoryMenu->addAction(tr("Missed Calls"));
    connect (action,SIGNAL(triggered(bool)),mainWindow,SLOT(eraseHistoryMissedCalls()));

    action = HistoryMenu->addAction(tr("Chat sessions"));
    connect (action,SIGNAL(triggered(bool)),mainWindow,SLOT(eraseHistoryChatSessions()));

    action = HistoryMenu->addAction(tr("Short text message (SMS)"));
    connect (action,SIGNAL(triggered(bool)),mainWindow,SLOT(eraseHistorySms()));

    HistoryMenu->addAction(tr("All"));
    connect (action,SIGNAL(triggered(bool)),mainWindow,SLOT(eraseHistory()));

    ToolsMenu->addMenu(HistoryMenu);

    _menuBar->addMenu(ToolsMenu);

    QMenu * HelpMenu = new QMenu ("&Help");

    action = HelpMenu->addAction(tr("&Forum"));
    connect (action,SIGNAL(triggered(bool)),mainWindow,SLOT(showForum()));

    HelpMenu->addAction(tr("&Wiki / Faq"));
    connect (action,SIGNAL(triggered(bool)),mainWindow,SLOT(showFaq()));

    HelpMenu->addSeparator ();

    HelpMenu->addAction(tr("&About"));
    connect (action,SIGNAL(triggered(bool)),mainWindow,SLOT(showAbout()));

    _menuBar->addMenu(HelpMenu);

}

QMainWindow * ChatWindow::findMainWindow(){
    QMainWindow * tmp;
    QWidgetList widgetList = qApp->allWidgets();
    QWidgetList::iterator iter;

    for (iter = widgetList.begin(); iter != widgetList.end(); iter++){
        if ( (*iter)->objectName() == QString("WengoPhoneWindow") ){
            return ( dynamic_cast<QMainWindow *> ((*iter)) );
        }
    }
    return NULL;
}
