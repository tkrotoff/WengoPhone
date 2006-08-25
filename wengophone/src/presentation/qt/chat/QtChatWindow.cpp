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

#include "QtChatWindow.h"
#include "QtChatWidget.h"
#include "QtChatTabWidget.h"
#include "emoticons/QtEmoticonsManager.h"

#include <presentation/qt/QtWengoPhone.h>
#include <presentation/qt/contactlist/QtContactList.h>
#include <presentation/qt/contactlist/QtContactListManager.h>
#include <presentation/qt/toaster/QtChatToaster.h>

#include <control/CWengoPhone.h>
#include <control/profile/CUserProfile.h>

#include <model/contactlist/ContactProfile.h>
#include <model/config/ConfigManager.h>
#include <model/config/Config.h>

#include <imwrapper/IMChatSession.h>
#include <imwrapper/IMAccount.h>

#include <qtutil/WengoStyleLabel.h>
#include <qtutil/Object.h>

#include <util/Logger.h>

#include <cutil/global.h>

#if defined(OS_WINDOWS)
	#include <windows.h>
#elif defined(OS_MACOSX)
/*
#include <Carbon/Carbon.h>
#include <Cocoa/Cocoa.h>
// Picture defined in Carbon conflicts with the one we define in our libs.
#ifdef Picture
#undef Picture
#endif //Picture
*/
#endif //!OS_MACOSX

QtChatWindow::QtChatWindow(CChatHandler & cChatHandler, IMChatSession & imChatSession)
	: QObject(NULL),
	_cChatHandler(cChatHandler) {

	_imChatSession = &imChatSession;
	_window = NULL;
	_flashTimerId = -1;
	_flashStat=false;
	_flashCount=0;

	_imChatSession->messageReceivedEvent +=
		boost::bind(&QtChatWindow::messageReceivedEventHandler, this, _1);

	_imChatSession->typingStateChangedEvent +=
		boost::bind(&QtChatWindow::typingStateChangedEventHandler, this, _1, _2, _3);

	QGridLayout * glayout;
	//_chatContactWidgets = new ChatContactWidgets();

	_window = new QWidget(NULL, Qt::Window | Qt::WindowMinMaxButtonsHint);

	_qtWengoPhone = dynamic_cast<QtWengoPhone *> (_cChatHandler.getCUserProfile().getCWengoPhone().getPresentation());
	_qtWengoPhone->setChatWindow(_window);

	// Create the menu bar
	_menuBar = new QMenuBar(_window);
	createMenu();

	QtEmoticonsManager::getInstance();

	_contactListFrame = new QFrame();

	glayout = new QGridLayout();
	glayout->setMargin(0);
	glayout->setSpacing(0);
	_window->setLayout(glayout);

	_tabWidget = new QtChatTabWidget(_window);
	connect(_tabWidget,SIGNAL(ctrlTabPressed()),SLOT(ctrlTabPressed()));

	QPushButton * closeTabButton = new QPushButton(_tabWidget);
	QIcon closeIcon(QPixmap(":/pics/close_normal.png"));
	closeIcon.addPixmap(QPixmap(":/pics/close_on.png"),QIcon::Normal,QIcon::On);
	closeTabButton->setIcon(closeIcon);
	closeTabButton->setMaximumSize(QSize(16,16));
	closeTabButton->setMinimumSize(QSize(16,16));
	closeTabButton->resize(QSize(16,16));
	closeTabButton->setToolTip(tr("close chat"));
	closeTabButton->setFlat(true );

	_tabWidget->setCornerWidget(closeTabButton,Qt::TopRightCorner);

	connect(closeTabButton, SIGNAL(clicked() ), SLOT(closeTab()));

	glayout->addWidget(_menuBar);
	glayout->addWidget(_contactListFrame);
	glayout->addWidget(_tabWidget);

	connect(_tabWidget,SIGNAL(currentChanged(int)), SLOT(tabSelectionChanged(int)));

	_tabWidget->removeTab(0);
	_window->resize(490,470);
	_window->setWindowTitle("WengoChat");

	if (imChatSession.isUserCreated()) {
		_window->show();
	} else {
		if (!_window->isVisible()) {
			showChatWindow();
		}

		QCoreApplication::processEvents();
		flashWindow();
    }
	// *****
	IMContact from = *_imChatSession->getIMContactSet().begin();
	addChat(_imChatSession,from);

	openContactListFrame();

	connect(this,SIGNAL(typingStateChangedSignal(const IMChatSession *,const IMContact *,const  IMChat::TypingState *)),
		SLOT(typingStateChangedThreadSafe(const IMChatSession *,const IMContact *,const IMChat::TypingState *)),Qt::QueuedConnection);

	connect(this, SIGNAL(messageReceivedSignal(IMChatSession *)),
		SLOT(messageReceivedSlot(IMChatSession *)),Qt::QueuedConnection);
}

QtChatWindow::~QtChatWindow() {
	QtEmoticonsManager * qtEmoticonsManager = QtEmoticonsManager::getInstance();
	delete qtEmoticonsManager;
}

void QtChatWindow::enableChatButton() {
	_qtWengoPhone->setChatWindow(_window );
}

void QtChatWindow::callContact(){
	QtChatWidget * widget = dynamic_cast<QtChatWidget *>(_tabWidget->widget(_tabWidget->currentIndex()));
	QString contactId;
	QtContactList * qtContactList;
	ContactProfile contactProfile;
	QtContactListManager * ul ;
	if (widget) {
		contactId = widget->getContactId();
		qtContactList = _qtWengoPhone->getContactList();
		contactProfile = qtContactList->getCContactList().getContactProfile(contactId.toStdString());
		ul = QtContactListManager::getInstance();
		ul->startCall(contactId);
		if (_qtWengoPhone->getWidget()->isMinimized()) {
			_qtWengoPhone->getWidget()->showNormal();
		}
#if defined(OS_WINDOWS)
		SetForegroundWindow(_qtWengoPhone->getWidget()->winId());
#endif
	}
}

void QtChatWindow::closeTab() {
	QtChatWidget * widget = dynamic_cast<QtChatWidget *>(_tabWidget->widget(_tabWidget->currentIndex()));
	delete widget;
	if (_tabWidget->count() == 0 ) {
		_window->hide();
		_qtWengoPhone->setChatWindow(NULL);
	}
}

void QtChatWindow::typingStateChangedEventHandler(IMChatSession & sender, const IMContact & imContact, IMChat::TypingState state) {
	IMChat::TypingState * tmpState = new IMChat::TypingState;
	*tmpState = state;
	typingStateChangedSignal(&sender,&imContact,tmpState);
}

void QtChatWindow::typingStateChangedThreadSafe(const IMChatSession * sender, const IMContact *,const IMChat::TypingState * state) {
	int tabs=_tabWidget->count();
	for (int i=0; i<tabs;i++) {
		QtChatWidget * widget = dynamic_cast<QtChatWidget *>(_tabWidget->widget(i) );
		if (widget->getSessionId() == sender->getId()) {
			widget->setRemoteTypingState(*sender,*state);
		}
	}
	delete state;
}

void QtChatWindow::show() {
	if (_window->isMinimized()) {
		_window->hide();
	}
	_window->showNormal();
	_window->activateWindow();
	_window->raise();
}

void QtChatWindow::messageReceivedEventHandler(IMChatSession & sender) {

	messageReceivedSignal(&sender);
}

void QtChatWindow::messageReceivedSlot(IMChatSession * sender) {
	showToaster(sender);
	IMChatSession::IMChatMessage * imChatMessage = sender->getReceivedMessage();
	if (imChatMessage) {
		const IMContact & from = imChatMessage->getIMContact();
		std::string message = imChatMessage->getMessage();
		delete imChatMessage;
		QString senderName = QString::fromStdString(from.getContactId());

		QtContactList * qtContactList = _qtWengoPhone->getContactList();
		CContactList & cContactList = qtContactList->getCContactList();
		QString contactId = QString::fromStdString(cContactList.findContactThatOwns(from));
		QString senderDisplayName = getShortDisplayName(contactId,QString::fromStdString(from.getContactId()));
		QString msg = QString::fromUtf8(message.c_str());

		if (!_window->isVisible()) {
			showChatWindow();
			flashWindow();
		} else {
			flashWindow();
		}
		int tabs = _tabWidget->count();
		for (int i = 0; i < tabs; i++) {
			QtChatWidget * widget = dynamic_cast<QtChatWidget *>(_tabWidget->widget(i));
			if (widget->getSessionId() == sender->getId()) {
				_chatWidget = qobject_cast<QtChatWidget *>(_tabWidget->widget(i));
				_chatWidget->addToHistory(senderDisplayName, msg);
				if (_tabWidget->currentWidget() != _chatWidget) {
					if (_window->isMinimized())
						_tabWidget->setCurrentIndex(i);
					else
						_tabWidget->setBlinkingTab(i);
				}
				if (_window->isMinimized()) {
					flashWindow();
				}
				return;
			}
		}
	}
}

void QtChatWindow::addChatSession(IMChatSession * imChatSession) {
	// If this chat session already exists, display the tab
	int tabs = _tabWidget->count();
	for (int i = 0; i < tabs; i++) {
		QtChatWidget * widget = dynamic_cast<QtChatWidget *>(_tabWidget->widget(i));
		if (widget->getSessionId() == imChatSession->getId()) {
			_tabWidget->setCurrentIndex(i);
			if (imChatSession->isUserCreated()) {
				show();
				return;
			} else {
				if (!_window->isVisible()) {
					showChatWindow();
					flashWindow();
					return;
				}
				if (_window->isMinimized()) {
						flashWindow();
						return;
				}
			}
			return;
		}
	}
	imChatSession->messageReceivedEvent +=
		boost::bind(&QtChatWindow::messageReceivedEventHandler, this, _1);

	imChatSession->typingStateChangedEvent +=
		boost::bind(&QtChatWindow::typingStateChangedEventHandler, this, _1, _2, _3);

	if (imChatSession->getIMContactSet().size() != 0 ) {
		IMContact from = *imChatSession->getIMContactSet().begin();
		addChat(imChatSession,from);
		if (imChatSession->isUserCreated()) {
			show();
		} else {
			if (!_window->isVisible()) {
				showChatWindow();
			}
			flashWindow();
		}
	} else {
		LOG_FATAL("New chat session is empty !!!!!");
	}
	enableChatButton();
}

QString QtChatWindow::getShortDisplayName(const QString & contactId, const QString & defaultName) const {
	QtContactList * qtContactList = _qtWengoPhone->getContactList();
	CContactList & cContactList = qtContactList->getCContactList();
	std::string tmpSendername = cContactList.getContactProfile(contactId.toStdString()).getFirstName();
	if (tmpSendername.empty()) {
		tmpSendername = cContactList.getContactProfile(contactId.toStdString()).getLastName();
	}
	if (tmpSendername.empty()) {
		tmpSendername = defaultName.toStdString();
	}
	return QString::fromUtf8(tmpSendername.c_str());
}

void QtChatWindow::addChat(IMChatSession * session, const IMContact & from) {

	QtContactList * qtContactList = _qtWengoPhone->getContactList();
	CContactList & cContactList = qtContactList->getCContactList();

	QString contactId = QString::fromStdString(cContactList.findContactThatOwns(from));
	std::string tmpNickName = session->getIMChat().getIMAccount().getLogin();
	QString nickName = QString::fromUtf8(tmpNickName.c_str());

	QString senderName = getShortDisplayName(contactId,QString::fromStdString(from.getContactId()));
	int tabNumber;

	_chatWidget = new QtChatWidget(_cChatHandler,_qtWengoPhone, session->getId(), _tabWidget, _tabWidget);
	_chatWidget->setIMChatSession(session);
	_chatWidget->setContactId(QString::fromStdString(qtContactList->getCContactList().findContactThatOwns(from)));

	connect(qtContactList,SIGNAL(contactChangedEventSignal(QString )), SLOT(statusChangedSlot(QString)));
	if (_tabWidget->count() > 0) {
		tabNumber = _tabWidget->insertTab(_tabWidget->count(),_chatWidget,senderName);
	} else {
		tabNumber = _tabWidget->insertTab(0,_chatWidget,senderName);
	}
	if (session->isUserCreated()) {
		_tabWidget->setCurrentIndex(tabNumber);
	}
	statusChangedSlot(QString::fromStdString(cContactList.findContactThatOwns(from)));

	_window->setWindowTitle(_tabWidget->tabText(tabNumber));
	_chatWidget->setNickName(nickName);
	// Adding probably missed message
	IMChatSession::IMChatMessage * imChatMessage = session->getReceivedMessage();
	bool remain = false;
	while (imChatMessage) {
		_chatWidget->addToHistory(getShortDisplayName(contactId,QString::fromStdString(from.getContactId())),
			QString::fromUtf8(imChatMessage->getMessage().c_str()));
		remain = true;
		imChatMessage = session->getReceivedMessage();
	}
	////
	if (remain) {
		showToaster(session);
	}
	if (!_window->isVisible() || _window->isMinimized()) {
		_tabWidget->setCurrentIndex(tabNumber);
	}
	std::string contact = _cChatHandler.getCUserProfile().getCContactList().findContactThatOwns(from);
}

void QtChatWindow::tabSelectionChanged(int index) {
	_tabWidget->stopBlinkingTab(index);
	_window->setWindowTitle(_tabWidget->tabText(_tabWidget->currentIndex()));
}

void QtChatWindow::openContactListFrame() {
	_contactListFrame->setVisible(false);
}

void QtChatWindow::closeContactListFrame() {
	_contactListFrame->setVisible(false);
}

void QtChatWindow::createMenu() {
	//FIXME!
	//Create a new class to manage menu for the main window and the chat window
	QtWengoPhone * mainWindow = dynamic_cast<QtWengoPhone *>(_cChatHandler.getCUserProfile().getCWengoPhone().getPresentation());
	QAction * action;

	QMenu * WengoMenu = new QMenu("Wengo");

	action = WengoMenu->addAction(tr("View my &Wengo Account"));
	connect(action, SIGNAL(triggered(bool)), mainWindow, SLOT(showWengoAccount()));

	action = WengoMenu->addAction(tr("Edit my &Profile"));
	connect(action, SIGNAL(triggered(bool)), mainWindow, SLOT(editMyProfile()));

	WengoMenu->addSeparator();

	action = WengoMenu->addAction(tr("&Learn more about Wengo Services"));
	connect(action, SIGNAL(triggered(bool)), mainWindow, SLOT(showCallOut()));

	WengoMenu->addSeparator();

	action = WengoMenu->addAction(tr("&Open Another Wengo Account"));
	connect(action, SIGNAL(triggered(bool)), mainWindow, SLOT(openWengoAccount()));

	WengoMenu->addSeparator();
	action = WengoMenu->addAction(tr("&Close"));
	connect(action, SIGNAL(triggered(bool)), mainWindow, SLOT(hideMainWindow()));

	_menuBar->addMenu(WengoMenu);

	QMenu * ContactsMenu = new QMenu(tr("&Contacts"));

	action = ContactsMenu->addAction(tr("Add contact"));
	connect(action, SIGNAL(triggered(bool)), mainWindow, SLOT(addContact()));

	action = ContactsMenu->addAction(tr("Search for Wengo users"));
	connect(action, SIGNAL(triggered(bool)), mainWindow, SLOT(showSearchContactWindows()));

	ContactsMenu->addSeparator();

	ContactsMenu->addAction(tr("Show contact groups"));

	action = ContactsMenu->addAction(tr("Show / hide contacts offline"));
	connect(action, SIGNAL(triggered(bool)), mainWindow, SLOT(showHideOffLineContacts()));

	_menuBar->addMenu(ContactsMenu);

	QMenu * Actions = new QMenu(tr("&Actions"));

	action = Actions->addAction(tr("Send a short text message (SMS)"));
	connect(action, SIGNAL(triggered(bool)), mainWindow, SLOT(sendSms()));

	action = Actions->addAction(tr("Create a conference call"));
	connect(action, SIGNAL(triggered()), mainWindow, SLOT(showCreateConferenceCall()));

	Actions->addSeparator();
	_menuBar->addMenu(Actions);
	action = Actions->addAction(tr("Accept"));
	connect(action, SIGNAL(triggered(bool)), mainWindow, SLOT(acceptCall()));

	Actions->addAction(tr("Hangup"));
	connect(action, SIGNAL(triggered(bool)), mainWindow, SLOT(hangupCall()));

	Actions->addAction(tr("Hold / Resume"));
	connect(action, SIGNAL(triggered(bool)), mainWindow, SLOT(resumeCall()));

	QMenu * ToolsMenu = new QMenu(tr("&Tools"));

	action = ToolsMenu->addAction(tr("Instant Messaging Accounts"));
	connect(action, SIGNAL(triggered(bool)), mainWindow, SLOT(showAccountSettings()));

	ToolsMenu->addSeparator();

	action = ToolsMenu->addAction(tr("Configuration"));
	connect(action, SIGNAL(triggered(bool)), mainWindow, SLOT(showConfig()));

	QMenu * HistoryMenu = new QMenu(tr("Clear History"));

	action = HistoryMenu->addAction(tr("Outgoing Calls"));
	connect(action, SIGNAL(triggered(bool)), mainWindow, SLOT(eraseHistoryOutgoingCalls()));

	action = HistoryMenu->addAction(tr("Incoming Calls"));
	connect(action, SIGNAL(triggered(bool)), mainWindow, SLOT(eraseHistoryIncomingCalls()));

	action = HistoryMenu->addAction(tr("Missed Calls"));
	connect(action, SIGNAL(triggered(bool)), mainWindow, SLOT(eraseHistoryMissedCalls()));

	action = HistoryMenu->addAction(tr("Chat sessions"));
	connect(action, SIGNAL(triggered(bool)), mainWindow, SLOT(eraseHistoryChatSessions()));

	action = HistoryMenu->addAction(tr("Short text message (SMS)"));
	connect(action, SIGNAL(triggered(bool)), mainWindow, SLOT(eraseHistorySms()));

	HistoryMenu->addAction(tr("All"));
	connect(action, SIGNAL(triggered(bool)), mainWindow, SLOT(eraseHistory()));

	ToolsMenu->addMenu(HistoryMenu);

	_menuBar->addMenu(ToolsMenu);

	QMenu * HelpMenu = new QMenu("&Help");

	action = HelpMenu->addAction(tr("&Forum"));
	connect(action, SIGNAL(triggered(bool)), mainWindow, SLOT(showForum()));

	HelpMenu->addAction(tr("&Wiki / Faq"));
	connect(action, SIGNAL(triggered(bool)), mainWindow, SLOT(showFaq()));

	HelpMenu->addSeparator();

	HelpMenu->addAction(tr("&About"));
	connect(action, SIGNAL(triggered(bool)), mainWindow, SLOT(showAbout()));

	_menuBar->addMenu(HelpMenu);
}

QMainWindow * QtChatWindow::findMainWindow() {
	QWidgetList widgetList = qApp->allWidgets();
	QWidgetList::iterator it;
	for (it=widgetList.begin(); it!=widgetList.end(); it++) {
		if ((*it)->objectName() == QString("WengoPhoneWindow")) {
			return (dynamic_cast<QMainWindow *>((*it)));
		}
	}
	return NULL;
}

void QtChatWindow::flashWindow() {
#if defined(OS_WINDOWS)
	FLASHWINFO flashInfo;
	if (_qtWengoPhone->getWidget()->winId() == GetForegroundWindow()) {
		flashInfo.cbSize = sizeof(FLASHWINFO);
		flashInfo.hwnd = _window->winId();
		flashInfo.dwFlags = FLASHW_TRAY;
		flashInfo.uCount = 5;
		flashInfo.dwTimeout = 500;
		FlashWindowEx(&flashInfo);
		//FlashWindow(_window->winId(), true);
	} else {
		flashInfo.cbSize = sizeof(FLASHWINFO);
		flashInfo.hwnd = _window->winId();
		flashInfo.dwFlags = FLASHW_TRAY|FLASHW_TIMERNOFG;
		flashInfo.uCount = 5;
		flashInfo.dwTimeout = 500;
		FlashWindowEx(&flashInfo);
	}
/*#elif defined(OS_MACOSX)
	WindowPtr windowPtr = (WindowPtr) _qtWengoPhone->getWidget()->winId();
	if (IsValidWindowPtr(windowPtr)) {
		if (!IsWindowActive(windowPtr)) {
			// The chat window is not the frontmost window.
			// Notifying user by making the dock icon bounce.
			int request = [NSApp requestUserAttention:NSInformationalRequest];
			[NSApp cancelUserAttentionRequest:request];
		}
	}*/
#else
	_window->activateWindow();
#endif
}

bool QtChatWindow::chatIsVisible(){
	if (_window->isVisible() && (!_window->isMinimized())) {
		return true;
	}
	return false;
}

void QtChatWindow::imContactChangedEventHandler(IMContact & sender) {
	statusChangedSignal(&sender);
}

void QtChatWindow::statusChangedSlot(QString contactId) {
	QtContactList * qtContactList = _qtWengoPhone->getContactList();
	std::string sdname = qtContactList->getCContactList().getContactProfile(contactId.toStdString()).getDisplayName();
	EnumPresenceState::PresenceState pstate = qtContactList->getCContactList().getContactProfile(contactId.toStdString()).getPresenceState();
	QString displayName = QString::fromStdString(sdname);
	// search for the tab that contain sender
	for (int i = 0; i < _tabWidget->count(); i++) {
		QtChatWidget * widget = dynamic_cast<QtChatWidget *>(_tabWidget->widget(i));
		if (widget) {
			if (widget->getContactId() == contactId){
				switch(pstate) {
					case EnumPresenceState::PresenceStateOnline:
						_tabWidget->setTabIcon(i,QIcon(QPixmap(":/pics/status/online.png")));
						break;
					case EnumPresenceState::PresenceStateOffline:
						_tabWidget->setTabIcon(i,QIcon(QPixmap(":/pics/status/offline.png")));
						break;
					case EnumPresenceState::PresenceStateDoNotDisturb:
						_tabWidget->setTabIcon(i,QIcon(QPixmap(":/pics/status/donotdisturb.png")));
						break;
					case EnumPresenceState::PresenceStateAway:
						_tabWidget->setTabIcon(i,QIcon(QPixmap(":/pics/status/away.png")));
						break;
					default:
						_tabWidget->setTabIcon(i,QIcon(QPixmap(":/pics/contact/chat.png")));
						break;
				}
				break;
			}
		}
	}
}

void QtChatWindow::showToaster(IMChatSession * imChatSession) {

	QtContactList * qtContactList = _qtWengoPhone->getContactList();
	CContactList & cContactList = qtContactList->getCContactList();

	if (chatIsVisible()) {
		return;
	}
	if (imChatSession->isUserCreated()) {
		return;
	}

	QPixmap result;
	QPixmap background = QPixmap(":/pics/fond_avatar.png");
	QtChatToaster * toaster = new QtChatToaster();

	if (imChatSession->getIMContactSet().size() > 0) {
		QString message;
		for (IMContactSet::const_iterator it = imChatSession->getIMContactSet().begin();
			it != imChatSession->getIMContactSet().end();
			++it) {
			if (it != imChatSession->getIMContactSet().begin()) {
				message += ", ";
			}
			QString contactId = QString::fromStdString(cContactList.findContactThatOwns((*it)));
			message+=getShortDisplayName(contactId,
				QString::fromStdString((*it).getContactId()));

			std::string contact =  _cChatHandler.getCUserProfile().getCContactList().findContactThatOwns((*it));
			if (!contact.empty()) {
				ContactProfile contactProfile = _cChatHandler.getCUserProfile().getCContactList().getContactProfile(contact);
				Picture picture = contactProfile.getIcon();
				std::string data = picture.getData();
				if (!data.empty()) {
					result.loadFromData((uchar *) data.c_str(), data.size());
				}
			}
		}
		toaster->setMessage(message);
	}
	if (!result.isNull()) {
		QPainter pixpainter(& background);
		pixpainter.drawPixmap(5, 5, result.scaled(60, 60));
		pixpainter.end();
	} else {
		result = QPixmap(":pics/toaster/default-avatar.png");
		QPainter pixpainter(& background);
		pixpainter.drawPixmap(5, 5, result.scaled(60, 60));
		pixpainter.end();
	}
	toaster->setPixmap(background);
	connect(toaster, SIGNAL(chatButtonClicked()), SLOT(show()));
	toaster->show();
}

void QtChatWindow::showMinimized(){
#ifdef OS_WINDOWS
	HWND topWindow = GetForegroundWindow();
#endif
	_window->showMinimized();
#ifdef OS_WINDOWS
	SetForegroundWindow(topWindow);
#endif
}

void QtChatWindow::showChatWindow() {
#if !defined(OS_MACOSX)
	showMinimized();
#else
	_window->showNormal();
#endif
}

void QtChatWindow::ctrlTabPressed() {
	if (_tabWidget->count() <= 1) {
		return;
	}

	int i=_tabWidget->currentIndex();
	i++;
	if (i ==_tabWidget->count()) {
		i=0;
	}
	_tabWidget->setCurrentIndex(i);
}
