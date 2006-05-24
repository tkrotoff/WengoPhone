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
#include <cutil/global.h>
#include "QtChatWindow.h"
#include "QtChatWidget.h"
#include "QtChatTabWidget.h"
#include "../QtWengoPhone.h"
#include "../contactlist/QtContactList.h"
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

#ifdef OS_WINDOWS
#include <windows.h>
#endif

ChatWindow::ChatWindow(CChatHandler & cChatHandler, IMChatSession & imChatSession)
: QObject(NULL), _cChatHandler(cChatHandler){
    // LOG_DEBUG("ChatWindow::ChatWindow(IMChatSession & imChatSession) : QDialog(), _imChatSession(imChatSession)");
	_imChatSession = &imChatSession;
	_dialog = NULL;

	_imChatSession->messageReceivedEvent +=
		boost::bind(&ChatWindow::messageReceivedEventHandler, this, _1);

	_imChatSession->typingStateChangedEvent +=
		boost::bind(&ChatWindow::typingStateChangedEventHandler, this, _1, _2, _3);

	_imChatSession->contactAddedEvent +=
		boost::bind(&ChatWindow::contactAddedEventHandler, this, _1, _2);

	QGridLayout * glayout;
	_chatContactWidgets = new ChatContactWidgets();

	// _dialog = new QDialog(findMainWindow());
	_dialog = new QDialog(NULL, Qt::Window | Qt::WindowMinMaxButtonsHint);

	QtWengoPhone * qtWengoPhone = dynamic_cast<QtWengoPhone *> (_cChatHandler.getCWengoPhone().getPresentation());
	qtWengoPhone->setChatWindow( _dialog );

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

	_inviteFrame = new QFrame(_dialog);
	createInviteFrame();

	glayout->addWidget(_menuBar);
	glayout->addWidget(_inviteFrame);
	glayout->addWidget(_contactListFrame);
	glayout->addWidget(_tabWidget);

	connect ( _tabWidget,SIGNAL(currentChanged (int)), SLOT(tabSelectionChanged(int)) );

	_tabWidget->removeTab(0);
	_dialog->resize(300,464);
	_dialog->setWindowTitle(tr("WengoChat"));

	if ( imChatSession.isUserCreated() ){
        _dialog->show();
	}
    else{
        if ( !_dialog->isVisible())
            _dialog->showMinimized ();
            flashWindow();
    }

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

	connect (this,SIGNAL(typingStateChangedSignal(const IMChatSession *,const IMContact *,const  IMChat::TypingState *)),
		SLOT(typingStateChangedThreadSafe(const IMChatSession *,const IMContact *,const IMChat::TypingState *)));

	connect (this,SIGNAL(contactAddedSignal(IMChatSession *,const IMContact *)),
		SLOT(contactAddedThreadSafe(IMChatSession *,const IMContact *)));

	connect(this, SIGNAL(messageReceivedSignal(IMChatSession *)),
		SLOT(messageReceivedSlot(IMChatSession *)));
}

void ChatWindow::enableChatButton(){
   QtWengoPhone * qtWengoPhone = dynamic_cast<QtWengoPhone *> (_cChatHandler.getCWengoPhone().getPresentation());
   qtWengoPhone->setChatWindow( _dialog );
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

    delete widget;

    if ( _tabWidget->count() == 0 )
    {
        _dialog->hide();
        QtWengoPhone * qtWengoPhone = dynamic_cast<QtWengoPhone *> (_cChatHandler.getCWengoPhone().getPresentation());
        qtWengoPhone->setChatWindow( NULL );
    }
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

void ChatWindow::newMessage(IMChatSession *session,const QString & msg){
	std::string message(msg.toUtf8().constData());
	session->sendMessage(message);
}

void ChatWindow::show(){
    if ( _dialog->isMinimized())
        _dialog->hide();
	_dialog->showNormal();

	//flashWindow();
}

void ChatWindow::messageReceivedEventHandler(IMChatSession & sender) {
	messageReceivedSignal(&sender);
}

void ChatWindow::messageReceivedSlot(IMChatSession * sender) {
	Config & config = ConfigManager::getInstance().getCurrentConfig();

	IMChatSession::IMChatMessage * imChatMessage = sender->getReceivedMessage();
	if (imChatMessage) {
		const IMContact & from = imChatMessage->getIMContact();
		std::string message = imChatMessage->getMessage();
		delete imChatMessage;

		QString senderName = QString::fromStdString(from.getContactId());

		QString msg = QString::fromUtf8(message.c_str());

        if (!_dialog->isVisible())
        {
            _dialog->showMinimized ();
            flashWindow();
        }
		int tabs = _tabWidget->count();
		for (int i = 0; i < tabs; i++)
		{
			ChatWidget * widget = dynamic_cast<ChatWidget *> ( _tabWidget->widget(i) );
			if (widget->getSessionId() == sender->getId())
			{
				_chatWidget = qobject_cast<ChatWidget *>(_tabWidget->widget(i));
				_chatWidget->addToHistory(senderName, msg);
				if ( _tabWidget->currentWidget() != _chatWidget ) {
					if (_dialog->isMinimized())
                        _tabWidget->setCurrentIndex(i);
                    else
                        _tabWidget->setBlinkingTab(i);
				}
				if (_dialog->isMinimized())
                    flashWindow();
				return;
			}
		}
	}
}

void ChatWindow::contactAddedEventHandler(IMChatSession & sender, const IMContact & imContact) {
	// Send message to the GUI thread
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

	// If this chat session already exists, display the right tab
	int tabs = _tabWidget->count();
	for (int i = 0; i < tabs; i++) {
		ChatWidget * widget = dynamic_cast<ChatWidget *> ( _tabWidget->widget(i) );
		if (widget->getSessionId() == imChatSession->getId()) {
			_tabWidget->setCurrentIndex(i);
			if (imChatSession->isUserCreated()){
                show();
                return;
			}
			if ( ! _dialog->isVisible())
			{
                _dialog->showMinimized ();
                flashWindow();
            }
            else
                if (_dialog->isMinimized())
                    flashWindow();
			return;
		}
	}

	imChatSession->messageReceivedEvent +=
		boost::bind(&ChatWindow::messageReceivedEventHandler, this, _1);

	imChatSession->typingStateChangedEvent +=
		boost::bind(&ChatWindow::typingStateChangedEventHandler, this, _1, _2, _3);

	if ( imChatSession->getIMContactSet().size() != 0 ) {
		IMContact from = *imChatSession->getIMContactSet().begin();
		addChat(imChatSession,from);
		if (imChatSession->isUserCreated())
            _dialog->show();
        else{
            if ( !_dialog->isVisible())
                _dialog->showMinimized ();
                flashWindow();
        }
	} else {
		LOG_FATAL("New chat session is empty !!!!!");
	}

	enableChatButton();
}

void ChatWindow::addChat(IMChatSession * session, const IMContact & from) {

	QString nickName = QString().fromStdString(session->getIMChat().getIMAccount().getLogin());
	QString senderName = QString::fromStdString(from.getContactId());
	int tabNumber;

    QtWengoPhone * qtWengoPhone = dynamic_cast<QtWengoPhone *> (_cChatHandler.getCWengoPhone().getPresentation());
    QtContactList * qtContactList = qtWengoPhone->getContactList();

    _chatWidget = new ChatWidget(_cChatHandler,session->getId(), _tabWidget);
	_chatWidget->setIMChatSession(session);
	_chatWidget->setContactId(QString::fromStdString(qtContactList->getCContactList().findContactThatOwns(from)));

	connect(qtContactList,SIGNAL(contactChangedEventSignal(QString )), SLOT(statusChangedSlot(QString)));

//	connect ( _chatWidget, SIGNAL( newContact(const Contact & ) ), SLOT ( addContactToContactListFrame(const Contact &)));

	if (_tabWidget->count() > 0)
		tabNumber = _tabWidget->insertTab(_tabWidget->count(),_chatWidget,senderName);
	else
		tabNumber = _tabWidget->insertTab(0,_chatWidget,senderName);


    statusChangedSlot(QString::fromStdString(qtContactList->getCContactList().findContactThatOwns(from)));

    _dialog->setWindowTitle(_tabWidget->tabText(tabNumber));
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

	if ( !_dialog->isVisible() || _dialog->isMinimized())
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
	_dialog->setWindowTitle(_tabWidget->tabText(_tabWidget->currentIndex()));
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

	action = WengoMenu->addAction(tr("&Learn more about Wengo Services"));
	connect(action,SIGNAL(triggered(bool)),mainWindow,SLOT(showCallOut()));

//	action = WengoMenu->addAction(tr("&Short text messages (SMS) "));
//	connect(action,SIGNAL(triggered(bool)),mainWindow,SLOT(showSms()));

//	WengoMenu->addAction(tr("&Voicemail"));
//	connect(action,SIGNAL(triggered(bool)),mainWindow,SLOT(showVoiceMail()));

	WengoMenu->addSeparator ();

	action = WengoMenu->addAction(tr("&Open Another Wengo Account"));
	connect(action,SIGNAL(triggered(bool)),mainWindow,SLOT(openWengoAccount()));

	WengoMenu->addSeparator ();
	action = WengoMenu->addAction(tr("&Close"));
	connect (action,SIGNAL(triggered(bool)),mainWindow,SLOT(hideMainWindow()));

//	action = WengoMenu->addAction(tr("Log off"));
//	connect (action,SIGNAL(triggered(bool)),mainWindow,SLOT(logOff()));

	_menuBar->addMenu(WengoMenu);

	QMenu * ContactsMenu = new QMenu(tr("&Contacts"));

	action = ContactsMenu->addAction(tr("Add contact"));
	connect (action,SIGNAL(triggered(bool)),mainWindow,SLOT(addContact()));

	action = ContactsMenu->addAction(tr("Search for Wengo users"));
	connect(action,SIGNAL(triggered(bool)),mainWindow,SLOT(showSearchContactWindows()));

	ContactsMenu->addSeparator ();


//	ContactsMenu->addAction(tr("&Manage blocked contacts"));
//	ContactsMenu->addSeparator ();

//	ContactsMenu->addAction(tr("Add a group"));
	ContactsMenu->addAction(tr("Show contact groups"));
//	ContactsMenu->addSeparator ();

	action = ContactsMenu->addAction(tr("Show / hide contacts offline"));
	connect (action,SIGNAL(triggered(bool)),mainWindow,SLOT(showHideOffLineContacts()));

	_menuBar->addMenu(ContactsMenu);

	QMenu * Actions = new QMenu(tr("&Actions"));

	action = Actions->addAction(tr("Send a short text message (SMS)"));
	connect (action,SIGNAL(triggered(bool)),mainWindow,SLOT(sendSms()));

//	Actions->addAction(tr("Forward incoming calls"));
//	Actions->addSeparator ();
	action = Actions->addAction(tr("Create a conference call"));
	connect(action, SIGNAL(triggered()),mainWindow,SLOT(showCreateConferenceCall()));

//	Actions->addAction(tr("Create a conference chat"));
	Actions->addSeparator ();
	_menuBar->addMenu(Actions);
	action = Actions->addAction(tr("Accept"));
	connect (action,SIGNAL(triggered(bool)),mainWindow,SLOT(acceptCall()));

	Actions->addAction(tr("Hangup"));
	connect (action,SIGNAL(triggered(bool)),mainWindow,SLOT(hangupCall()));

	Actions->addAction(tr("Hold / Resume"));
	connect (action,SIGNAL(triggered(bool)),mainWindow,SLOT(resumeCall()));

	QMenu * ToolsMenu = new QMenu(tr("&Tools"));

	action = ToolsMenu->addAction(tr("Instant Messaging Accounts"));
	connect (action,SIGNAL(triggered(bool)),mainWindow,SLOT(showAccountSettings()));

	ToolsMenu->addSeparator ();

	action = ToolsMenu->addAction(tr("Configuration"));
	connect (action,SIGNAL(triggered(bool)),mainWindow,SLOT(showConfig()));

//	action = ToolsMenu->addAction(tr("Advanced configuration"));
//	connect (action,SIGNAL(triggered(bool)),mainWindow,SLOT(showAdvancedConfig()));

//	ToolsMenu->addSeparator ();
//	ToolsMenu->addAction(tr("View toolbar"));
//	ToolsMenu->addAction(tr("View adressbar"));
//	ToolsMenu->addAction(tr("View dialpad"));
//	ToolsMenu->addAction(tr("View avatars in chat window"));
//	ToolsMenu->addSeparator ();

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
void ChatWindow::flashWindow() {
#ifdef OS_WINDOWS
	FLASHWINFO flashInfo;
	flashInfo.cbSize = sizeof(FLASHWINFO);
	flashInfo.hwnd = _dialog->winId();
	flashInfo.dwFlags = FLASHW_TRAY; //FLASHW_TRAY | FLASHW_TIMERNOFG;
	flashInfo.uCount = 5;
	flashInfo.dwTimeout = 500;
	FlashWindowEx(&flashInfo);
#endif
}

bool ChatWindow::isVisible(){
    if (_dialog->isVisible() && (!_dialog->isMinimized()))
        return true;
    return false;
}

void ChatWindow::imContactChangedEventHandler(IMContact & sender) {
    statusChangedSignal(&sender);
}

void ChatWindow::statusChangedSlot(QString contactId) {

    QtWengoPhone * qtWengoPhone = dynamic_cast<QtWengoPhone *> (_cChatHandler.getCWengoPhone().getPresentation());
    QtContactList * qtContactList = qtWengoPhone->getContactList();


    std::string sdname = qtContactList->getCContactList().getContactProfile(contactId.toStdString()).getDisplayName();

    EnumPresenceState::PresenceState pstate = qtContactList->getCContactList().getContactProfile(contactId.toStdString()).getPresenceState();

    QString displayName = QString::fromStdString(sdname);

    // search for the tab that contain sender
    for (int i = 0; i < _tabWidget->count(); i++) {
        ChatWidget * widget = dynamic_cast<ChatWidget *> (_tabWidget->widget(i));
        if (widget) {
            if (widget->getContactId() == contactId){
                switch(pstate) {
                    case EnumPresenceState::PresenceStateOnline:
                        // _tabWidget->setTabText(i,displayName + " (Online)");
                        _tabWidget->setTabIcon(i,QIcon(QPixmap(":/pics/status/online.png")));
                        break;
                    case EnumPresenceState::PresenceStateOffline:
                        // _tabWidget->setTabText(i,displayName + " (Offline)");
                        _tabWidget->setTabIcon(i,QIcon(QPixmap(":/pics/status/offline.png")));
                        break;
                    case EnumPresenceState::PresenceStateDoNotDisturb:
                        // _tabWidget->setTabText(i,displayName + " (DND)");
                        _tabWidget->setTabIcon(i,QIcon(QPixmap(":/pics/status/donotdisturb.png")));
                        break;
                    case EnumPresenceState::PresenceStateAway:
                        // _tabWidget->setTabText(i,displayName + " (Away)");
                        _tabWidget->setTabIcon(i,QIcon(QPixmap(":/pics/status/away.png")));
                        break;
                    default:
                        break;
                }
                break;
            }
        }
    }
}
