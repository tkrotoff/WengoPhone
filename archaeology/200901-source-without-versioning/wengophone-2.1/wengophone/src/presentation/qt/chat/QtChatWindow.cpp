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

#include "QtChatWindow.h"
#include "QtChatWidget.h"
#include "QtChatTabWidget.h"
#include "ui_ChatMainWindow.h"
#include "emoticons/QtEmoticonsManager.h"

#include <model/config/Config.h>
#include <model/config/ConfigManager.h>
#include <model/contactlist/ContactProfile.h>
#include <model/profile/AvatarList.h>
#include <model/profile/UserProfile.h>

#include <control/CWengoPhone.h>
#include <control/profile/CUserProfile.h>
#include <control/profile/CUserProfileHandler.h>

#include <presentation/qt/QtWengoPhone.h>
#include <presentation/qt/QtToolBar.h>
#include <presentation/qt/contactlist/QtContactList.h>
#include <presentation/qt/contactlist/QtContactListManager.h>
#include <presentation/qt/toaster/QtChatToaster.h>
#include <presentation/qt/profile/QtUserProfileHandler.h>
#include <presentation/qt/webservices/sms/QtSms.h>
#include <presentation/qt/profile/QtProfileDetails.h>
#include <presentation/qt/filetransfer/QtFileTransfer.h>

#include <imwrapper/IMChatSession.h>
#include <imwrapper/IMAccount.h>
#include <imwrapper/IMContact.h>
#include <imwrapper/EnumIMProtocol.h>
#include <imwrapper/IMContactSet.h>

#include <cutil/global.h>
#include <qtutil/Object.h>
#include <qtutil/SafeConnect.h>
#include <qtutil/WidgetUtils.h>
#include <util/Logger.h>
#include <util/SafeDelete.h>

#include <sound/Sound.h>

#include <QtGui/QtGui>

#if defined(OS_WINDOWS)
	#include <windows.h>
#elif defined (OS_MACOSX)
	#include "QtCloseChatTabWidget.h"
#endif

QtChatWindow::QtChatWindow(QWidget * parent, CChatHandler & cChatHandler, IMChatSession & imChatSession, QtWengoPhone & qtWengoPhone)
	: QMainWindow(parent),
	_cChatHandler(cChatHandler),
	_qtWengoPhone(qtWengoPhone) {

	_imChatSession = &imChatSession;

	_ui = new Ui::ChatMainWindow();
	_ui->setupUi(this);
	setupMenuBarActions();
	setupToolBarActions();

	// bind to IMChatSession events
	_imChatSession->messageReceivedEvent +=
		boost::bind(&QtChatWindow::messageReceivedEventHandler, this, _1);
	_imChatSession->imChatSessionWillDieEvent +=
		boost::bind(&QtChatWindow::imChatSessionWillDieEventHandler, this, _1);
	_imChatSession->typingStateChangedEvent +=
		boost::bind(&QtChatWindow::typingStateChangedEventHandler, this, _1, _2, _3);
	////

	// create the tab widget and connect to its signals
	_tabWidget = new QtChatTabWidget(this);
	_tabWidget->removeTab(0);
	setCentralWidget(_tabWidget);
	SAFE_CONNECT(_tabWidget, SIGNAL(closeButtonClicked()), SLOT(closeActiveTab()));
	SAFE_CONNECT(_tabWidget, SIGNAL(currentChanged(int)), SLOT(activeTabChanged(int)));
	////

	// create instance of QtEmoticonsManager
	QtEmoticonsManager::getInstance();

	// thread safe code
	SAFE_CONNECT_TYPE(this, SIGNAL(typingStateChangedSignal(const IMChatSession *, const IMContact *, const  IMChat::TypingState *)),
		SLOT(typingStateChangedThreadSafe(const IMChatSession *, const IMContact *, const IMChat::TypingState *)), Qt::QueuedConnection);
	SAFE_CONNECT_TYPE(this, SIGNAL(messageReceivedSignal(IMChatSession *)),
		SLOT(messageReceivedSlot(IMChatSession *)), Qt::QueuedConnection);
	////

	_qtWengoPhone.setChatWindow(this);
	resize(490, 470);
	setWindowTitle("WengoChat");

#ifdef OS_LINUX
	std::string data = AvatarList::getInstance().getDefaultAvatarPicture().getData();
	QPixmap defaultAvatar;
	defaultAvatar.loadFromData((uchar*)data.c_str(), data.size());
	setWindowIcon(QIcon(defaultAvatar));
#endif

	// add a tab widget
	IMContact from = *_imChatSession->getIMContactSet().begin();
	addChatWidgetTab(_imChatSession, from);
	////
}

QtChatWindow::~QtChatWindow() {

	//destroyed tabWidget so that chat are saved
	while(_tabWidget->count()>0) {
		closeActiveTab();
	}
	////
	QtEmoticonsManager * m = QtEmoticonsManager::getInstance();
	OWSAFE_DELETE(m);

	OWSAFE_DELETE(_ui);
}

QtChatWidget * QtChatWindow::getActiveTabWidget() {
	return dynamic_cast<QtChatWidget *>(_tabWidget->widget(_tabWidget->currentIndex()));
}

void QtChatWindow::activeTabChanged(int index) {
	_tabWidget->stopBlinkingTab(index);
	setWindowTitle(_tabWidget->tabText(_tabWidget->currentIndex()));
	updateToolBarActions();
	statusBar()->showMessage(QString::null);
}

const QString QtChatWindow::getActiveTabContactId() {
	QString contactId;
	QtChatWidget * widget = getActiveTabWidget();
	if (widget) {
		contactId = widget->getContactId();
	}
	return contactId;
}

ContactProfile QtChatWindow::getContactProfileFromContactId(const QString & contactId) {
	return _qtWengoPhone.getQtContactList()->getCContactList().getContactProfile(contactId.toStdString());
}

void QtChatWindow::closeActiveTab() {
	QtChatWidget * widget = getActiveTabWidget();
	_tabWidget->removeTab(_tabWidget->indexOf(widget));
	OWSAFE_DELETE(widget);
	if (_tabWidget->count() == 0) {
		close();
		_qtWengoPhone.setChatWindow(NULL);
	}
	activeTabChanged(_tabWidget->currentIndex());
}

void QtChatWindow::showMinimized() {
#if !defined(OS_MACOSX)

#ifdef OS_WINDOWS
	HWND topWindow = GetForegroundWindow();
#endif
	QMainWindow::showMinimized();
#ifdef OS_WINDOWS
	SetForegroundWindow(topWindow);
#endif

#else
	showNormal();
#endif
}

void QtChatWindow::show() {
	showNormal();
	activateWindow();
	raise();
}

void QtChatWindow::createChatConference() {
	QtChatWidget * widget = getActiveTabWidget();
	if (widget) {
		widget->showInviteDialog();
	}
}

void QtChatWindow::typingStateChangedEventHandler(IMChatSession & sender, const IMContact & imContact, IMChat::TypingState state) {
	IMChat::TypingState * tmpState = new IMChat::TypingState;
	*tmpState = state;
	typingStateChangedSignal(&sender,&imContact,tmpState);
}

void QtChatWindow::typingStateChangedThreadSafe(const IMChatSession * sender, const IMContact * from,const IMChat::TypingState * state) {
	int tabs = _tabWidget->count();
	for (int i = 0; i < tabs; i++) {
		QtChatWidget * widget = (QtChatWidget *) _tabWidget->widget(i);
		if (widget->getSessionId() == sender->getId()) {
		
			//only display mssage if IMChatSessio is active
			if (!widget->isVisible()) {
				return;
			}
		
			QString remoteName = QString::fromUtf8(from->getDisplayContactId().c_str());
			switch (*state) {
			case IMChat::TypingStateNotTyping:
				statusBar()->showMessage(QString::null);
				break;
			case IMChat::TypingStateTyping:
				statusBar()->showMessage(remoteName + tr(" is typing"));
				break;
			case IMChat::TypingStateStopTyping:
				statusBar()->showMessage(QString::null);
				break;
			default:
				break;
			}
		}
	}
	OWSAFE_DELETE(state);
}

void QtChatWindow::messageReceivedEventHandler(IMChatSession & sender) {
	messageReceivedSignal(&sender);
}

QString QtChatWindow::getShortDisplayName(const QString & contactId, const QString & defaultName) const {
	QtContactList * qtContactList = _qtWengoPhone.getQtContactList();
	CContactList & cContactList = qtContactList->getCContactList();
	std::string tmpSendername = cContactList.getContactProfile(contactId.toStdString()).getShortDisplayName();
	if (tmpSendername.empty()) {
		tmpSendername = defaultName.toStdString();
	}

	return QString::fromUtf8(tmpSendername.c_str());
}

void QtChatWindow::statusChangedSlot(QString contactId) {
	QtContactList * qtContactList = _qtWengoPhone.getQtContactList();
	std::string sdname = qtContactList->getCContactList().getContactProfile(contactId.toStdString()).getDisplayName();
	EnumPresenceState::PresenceState pstate = qtContactList->getCContactList().getContactProfile(contactId.toStdString()).getPresenceState();
	QString displayName = QString::fromStdString(sdname);

	// search for the tab that contain sender
	// FIXME: the contact could be in several tabs: for instance when
	// we do a single-chat and a multi-chat with this contact
	for (int i = 0; i < _tabWidget->count(); i++) {
		QtChatWidget * widget = (QtChatWidget *) _tabWidget->widget(i);
		if (widget) {
			if (widget->getContactId() == contactId) {
				switch(pstate) {
				case EnumPresenceState::PresenceStateOnline:
					_tabWidget->setTabIcon(i, QIcon(QPixmap(":/pics/status/online.png")));
					widget->setContactConnected(true);
					break;
				case EnumPresenceState::PresenceStateOffline:
					_tabWidget->setTabIcon(i, QIcon(QPixmap(":/pics/status/offline.png")));
					widget->setContactConnected(false);
					break;
				case EnumPresenceState::PresenceStateDoNotDisturb:
					_tabWidget->setTabIcon(i, QIcon(QPixmap(":/pics/status/donotdisturb.png")));
					widget->setContactConnected(true);
					break;
				case EnumPresenceState::PresenceStateAway:
					_tabWidget->setTabIcon(i, QIcon(QPixmap(":/pics/status/away.png")));
					widget->setContactConnected(true);
					break;
				default:
					_tabWidget->setTabIcon(i, QIcon(QPixmap(":/pics/contact/chat.png")));
					if (!displayName.isEmpty()) {
						// If Display name is empty, it means that the Contact is not in the ContactList.
						// Thus we should not say that the contact is offline as we don't really know its
						// status.
						widget->setContactConnected(false);
					}
					break;
				}
				break;
			}
		}
	}

	updateToolBarActions();
}

void QtChatWindow::copyQAction(QObject * actionParent, QAction * action) {
	QAction * tmp = actionParent->findChild<QAction *>(action->objectName());
	if (!tmp) {
		LOG_FATAL("QAction is null, cannot copy it");
	}
	action->setIcon(tmp->icon());
	action->setIconText(tmp->iconText());
	action->setText(tmp->text());
	action->setToolTip(tmp->toolTip());
	action->setShortcut(tmp->shortcut());
}

void QtChatWindow::setupMenuBarActions() {
	QtToolBar * qtToolBar = &_qtWengoPhone.getQtToolBar();
	QWidget * toolBar = qtToolBar->getWidget();

	// setup "Wengo" menu
	copyQAction(toolBar, _ui->actionShowWengoAccount);
	SAFE_CONNECT_RECEIVER(_ui->actionShowWengoAccount, SIGNAL(triggered()), qtToolBar, SLOT(showWengoAccount()));
	copyQAction(toolBar, _ui->actionEditMyProfile);
	SAFE_CONNECT_RECEIVER(_ui->actionEditMyProfile, SIGNAL(triggered()), qtToolBar, SLOT(editMyProfile()));
	copyQAction(toolBar, _ui->actionWengoServices);
	SAFE_CONNECT_RECEIVER(_ui->actionWengoServices, SIGNAL(triggered()), qtToolBar, SLOT(showWengoServices()));
	copyQAction(toolBar, _ui->actionClose);
	SAFE_CONNECT(_ui->actionClose, SIGNAL(triggered()), SLOT(close()));
	////

	// setup "contact" menubar
	copyQAction(toolBar, _ui->actionAddContact);
	SAFE_CONNECT_RECEIVER(_ui->actionAddContact, SIGNAL(triggered()), qtToolBar, SLOT(addContact()));
	copyQAction(toolBar, _ui->actionSearchWengoContact);
	SAFE_CONNECT_RECEIVER(_ui->actionSearchWengoContact, SIGNAL(triggered()), qtToolBar, SLOT(searchWengoContact()));
	////

	// setup "actions" menu
	copyQAction(toolBar, _ui->actionCreateConferenceCall);
	SAFE_CONNECT_RECEIVER(_ui->actionCreateConferenceCall, SIGNAL(triggered()), qtToolBar, SLOT(createConferenceCall()));
	copyQAction(toolBar, _ui->actionSendSms);
	SAFE_CONNECT_RECEIVER(_ui->actionSendSms, SIGNAL(triggered()), qtToolBar, SLOT(sendSms()));
	SAFE_CONNECT(_ui->actionSaveHistoryAs, SIGNAL(triggered()), SLOT(saveActiveTabChatHistory()));
	////

	// setup "tools" menu
	copyQAction(toolBar, _ui->actionShowFileTransfer);
	SAFE_CONNECT_RECEIVER(_ui->actionShowFileTransfer, SIGNAL(triggered()), qtToolBar, SLOT(showFileTransferWindow()));
	////

	// setup "help" menu
	copyQAction(toolBar, _ui->actionShowWengoForum);
	SAFE_CONNECT_RECEIVER(_ui->actionShowWengoForum, SIGNAL(triggered()), qtToolBar, SLOT(showWengoForum()));
	copyQAction(toolBar, _ui->actionWengoFAQ);
	SAFE_CONNECT_RECEIVER(_ui->actionWengoFAQ, SIGNAL(triggered()), qtToolBar, SLOT(showWengoFAQ()));
	////
}

void QtChatWindow::setupToolBarActions() {
	SAFE_CONNECT(_ui->actionCallContact, SIGNAL(triggered()), SLOT(callActiveTabContact()));
	SAFE_CONNECT(_ui->actionSendSms, SIGNAL(triggered()), SLOT(sendSmsToActiveTabContact()));
	SAFE_CONNECT(_ui->actionSendFile, SIGNAL(triggered()), SLOT(sendFileToActiveTabContact()));
	SAFE_CONNECT(_ui->actionCreateChatConf, SIGNAL(triggered()), SLOT(createChatConference()));
	SAFE_CONNECT(_ui->actionContactInfo, SIGNAL(triggered()), SLOT(showActiveTabContactInfo()));
	SAFE_CONNECT(_ui->actionBlockContact, SIGNAL(triggered()), SLOT(blockActiveTabContact()));

#if defined(OS_MACOSX)
	// Add the close tab widget
	QtCloseChatTabWidget * closeTabWidget = new QtCloseChatTabWidget(_ui->toolBar);
	_ui->toolBar->addWidget(closeTabWidget);
	SAFE_CONNECT(closeTabWidget, SIGNAL(clicked()), SLOT(closeActiveTab()));
	////
#endif
}

void QtChatWindow::updateToolBarActions() {
	//FIXME: this code assume that there is only one contact in the session

	QtChatWidget * widget = getActiveTabWidget();
	QString contactId;
	QtContactList * qtContactList;
	ContactProfile contactProfile;

	if (widget) {
		contactId = widget->getContactId();
		qtContactList = _qtWengoPhone.getQtContactList();
		contactProfile = qtContactList->getCContactList().getContactProfile(contactId.toStdString());

		_ui->actionCallContact->setEnabled(contactProfile.hasCall()
			&& contactProfile.isAvailable());
		_ui->actionSendSms->setEnabled(!contactProfile.getMobilePhone().empty());
		_ui->actionSendFile->setEnabled(widget->canDoFileTransfer());

		_ui->actionCreateChatConf->setEnabled(widget->canDoMultiChat());
		_ui->actionContactInfo->setEnabled(true);
		//TODO: uncomment when block a contact will be implemented
		//_ui->actionBlockContact->setEnabled(!contactProfile.isBlocked());
		_ui->actionBlockContact->setEnabled(false);
		////
	}
}

void QtChatWindow::messageReceivedSlot(IMChatSession * sender) {
	if (!isActiveWindow()) {
		if (!isVisible()) {
			// Make sure the window appears on the taskbar,
			// otherwise it won't flash...
			showMinimized();
		}
		WidgetUtils::flashWindow(this);
	}

	// new way to get messages
	IMChatSession::IMChatMessageList imChatMessageList = sender->getReceivedMessage(_lastReceivedMessageIndex[sender->getId()] + 1);
	if (imChatMessageList.size() > 0) {
		_lastReceivedMessageIndex[sender->getId()] += imChatMessageList.size();
		IMChatSession::IMChatMessageList::iterator imChatMessageListIterator = imChatMessageList.begin();
		while (imChatMessageListIterator < imChatMessageList.end()) {
			IMChatSession::IMChatMessage * imChatMessage = * imChatMessageListIterator;
			const IMContact & from = imChatMessage->getIMContact();
			std::string message = imChatMessage->getMessage();
			QString senderName = QString::fromStdString(from.getContactId());

			QtContactList * qtContactList = _qtWengoPhone.getQtContactList();
			CContactList & cContactList = qtContactList->getCContactList();
			QString contactId = QString::fromStdString(cContactList.findContactThatOwns(from));
			QString senderDisplayName = getShortDisplayName(contactId, QString::fromStdString(from.getDisplayContactId()));
			QString msg = QString::fromUtf8(message.c_str());

			int tabs = _tabWidget->count();
			for (int i = 0; i < tabs; i++) {
				QtChatWidget * widget = dynamic_cast<QtChatWidget *>(_tabWidget->widget(i));
				if (widget->getSessionId() == sender->getId()) {
					_chatWidget = qobject_cast<QtChatWidget *>(_tabWidget->widget(i));
					_chatWidget->addToHistory(senderDisplayName, msg);
					if (_tabWidget->currentWidget() != _chatWidget) {
						if (isMinimized()) {
							_tabWidget->setCurrentIndex(i);
						} else {
							_tabWidget->setBlinkingTab(i);
						}
					}
					break;
				}
			}
			imChatMessageListIterator++;
		}
	}
	////
}

void QtChatWindow::imChatSessionCreatedHandler(IMChatSession * imChatSession) {
	// If this chat session already exists, display the tab
	int tabs = _tabWidget->count();
	for (int i = 0; i < tabs; i++) {
		QtChatWidget * widget = dynamic_cast<QtChatWidget *>(_tabWidget->widget(i));
		if (widget->getSessionId() == imChatSession->getId()) {
			_tabWidget->setCurrentIndex(i);
			show();
			return;
		}
	}

	// bind to IMChatSession events
	imChatSession->messageReceivedEvent +=
		boost::bind(&QtChatWindow::messageReceivedEventHandler, this, _1);
	imChatSession->imChatSessionWillDieEvent +=
		boost::bind(&QtChatWindow::imChatSessionWillDieEventHandler, this, _1);
	imChatSession->typingStateChangedEvent +=
		boost::bind(&QtChatWindow::typingStateChangedEventHandler, this, _1, _2, _3);
	////

	if (imChatSession->getIMContactSet().size() != 0 ) {
		IMContact from = *imChatSession->getIMContactSet().begin();
		addChatWidgetTab(imChatSession, from);
	} else {
		LOG_FATAL("New chat session is empty!");
	}
	_qtWengoPhone.setChatWindow(this);
}

void QtChatWindow::addChatWidgetTab(IMChatSession * imChatSession, const IMContact & from) {

	QtContactList * qtContactList = _qtWengoPhone.getQtContactList();
	if (qtContactList) {
		CContactList & cContactList = qtContactList->getCContactList();

		QString contactId = QString::fromStdString(cContactList.findContactThatOwns(from));
		std::string tmpNickName;
		IMAccount * imAccount =
			_cChatHandler.getCUserProfile().getUserProfile().getIMAccountManager().getIMAccount(imChatSession->getIMChat().getIMAccountId());
		if (imAccount) {
			tmpNickName = imAccount->getLogin();
			OWSAFE_DELETE(imAccount);
		} else {
			LOG_ERROR("cannot get associated IMAccount");
		}
		
		QString nickName = QString::fromUtf8(tmpNickName.c_str());

		QString senderName = getShortDisplayName(contactId, QString::fromStdString(from.getDisplayContactId()));

		_chatWidget = new QtChatWidget(_cChatHandler, &_qtWengoPhone, imChatSession->getId(), 0 /*parent*/);
		SAFE_CONNECT_RECEIVER(_chatWidget, SIGNAL(ctrlTabPressed()), _tabWidget, SLOT(goToNextTab()));
		_chatWidget->setIMChatSession(imChatSession);
		_chatWidget->setContactId(QString::fromStdString(qtContactList->getCContactList().findContactThatOwns(from)));

		SAFE_CONNECT(qtContactList, SIGNAL(contactChangedEventSignal(QString )), SLOT(statusChangedSlot(QString)));

		int tabNumber;
		if (_tabWidget->count() > 0) {
			tabNumber = _tabWidget->insertTab(_tabWidget->count(), _chatWidget, senderName);
		} else {
			tabNumber = _tabWidget->insertTab(0, _chatWidget, senderName);
		}

		if (imChatSession->isUserCreated()) {
			_tabWidget->setCurrentIndex(tabNumber);
		}

		statusChangedSlot(QString::fromStdString(cContactList.findContactThatOwns(from)));
		
		activeTabChanged(_tabWidget->currentIndex());

		_chatWidget->setNickName(nickName);

		// Adding probably missed message
		_lastReceivedMessageIndex[imChatSession->getId()] = -1;
		IMChatSession::IMChatMessageList imChatMessageList = 
			imChatSession->getReceivedMessage(_lastReceivedMessageIndex[imChatSession->getId()] + 1);
		if (imChatMessageList.size() > 0) {
			_lastReceivedMessageIndex[imChatSession->getId()] += imChatMessageList.size();
			IMChatSession::IMChatMessageList::iterator imChatMessageListIterator = imChatMessageList.begin();
			while (imChatMessageListIterator != imChatMessageList.end()){
				IMChatSession::IMChatMessage * imChatMessage = * imChatMessageListIterator;

				//history chat message are reconstructed with no protocol
				if(imChatMessage->getIMContact().getProtocol() != EnumIMProtocol::IMProtocolUnknown) {
					_chatWidget->addToHistory(getShortDisplayName(contactId,QString::fromStdString(from.getDisplayContactId())),
						QString::fromUtf8(imChatMessage->getMessage().c_str()));
				} else {
					_chatWidget->addToHistory(QString::fromStdString(imChatMessage->getIMContact().cleanContactId()),
						QString::fromUtf8(imChatMessage->getMessage().c_str()),
						QString::fromStdString(imChatMessage->getDate().toString() + " " + imChatMessage->getTime().toString()));
				}

				imChatMessageListIterator++;
			}
		}
		////

		if (imChatSession->isUserCreated()) {
			show();
		} else {
			if (!isVisible() || isMinimized()) {
				_tabWidget->setCurrentIndex(tabNumber);
				showMinimized();
				showToaster(imChatSession);
				QCoreApplication::processEvents();
				WidgetUtils::flashWindow(this);
			}
		}

		updateToolBarActions();
	}
}

void QtChatWindow::showToaster(IMChatSession * imChatSession) {
	QtContactList * qtContactList = _qtWengoPhone.getQtContactList();
	if (qtContactList) {
		CContactList & cContactList = qtContactList->getCContactList();
		QPixmap avatar;
		QtChatToaster * toaster = new QtChatToaster();

		if (imChatSession->getIMContactSet().size() > 0) {
			QString message;
			for (IMContactSet::const_iterator it = imChatSession->getIMContactSet().begin();
				it != imChatSession->getIMContactSet().end(); ++it) {

				if (it != imChatSession->getIMContactSet().begin()) {
					message += ", ";
				}
				QString contactId = QString::fromStdString(cContactList.findContactThatOwns((*it)));
				message += getShortDisplayName(contactId, QString::fromStdString((*it).getDisplayContactId()));

				std::string contact =  _cChatHandler.getCUserProfile().getCContactList().findContactThatOwns((*it));
				if (!contact.empty()) {
					ContactProfile contactProfile = _cChatHandler.getCUserProfile().getCContactList().getContactProfile(contact);
					OWPicture picture = contactProfile.getIcon();
					std::string data = picture.getData();
					if (!data.empty()) {
						avatar.loadFromData((uchar *) data.c_str(), data.size());
					}
				}
			}
			toaster->setMessage(message);
		}

		if (avatar.isNull()) {
			std::string data = AvatarList::getInstance().getDefaultAvatarPicture().getData();
			avatar.loadFromData((uchar*)data.c_str(), data.size());
		}

		toaster->setPixmap(avatar);

		Config & config = ConfigManager::getInstance().getCurrentConfig();
		Sound::play(config.getAudioIncomingChatFile());
		SAFE_CONNECT(toaster, SIGNAL(chatButtonClicked()), SLOT(show()));
		toaster->show();
	}
}

void QtChatWindow::imChatSessionWillDieEventHandler(IMChatSession & sender) {
	_lastReceivedMessageIndex.erase(sender.getId());
}

void QtChatWindow::sendSmsToActiveTabContact() {
	// retrieve mobile phone number.
	QString phoneNumber;
	const QString contactId = getActiveTabContactId();
	if (!contactId.isEmpty()) {
		ContactProfile contactProfile =  getContactProfileFromContactId(contactId);
		phoneNumber = QString::fromStdString(contactProfile.getMobilePhone());
	}
	////

	// configure & show the sms widget
	QtSms * sms = _qtWengoPhone.getQtSms();
	if (sms) {
		sms->setPhoneNumber(phoneNumber);
		sms->getWidget()->show();
	}
	////
}

void QtChatWindow::sendFileToActiveTabContact() {

	Config & config = ConfigManager::getInstance().getCurrentConfig();
	QtFileTransfer * qtFileTransfer = _qtWengoPhone.getFileTransfer();
	if (!qtFileTransfer) {
		return;
	}

	QString filename = qtFileTransfer->getChosenFile();
	if (!filename.isEmpty()) {
		QFileInfo fileInfo(filename);
		config.set(Config::FILETRANSFER_LASTUPLOADEDFILE_FOLDER_KEY, fileInfo.dir().absolutePath().toStdString());
	}

	if (!filename.isEmpty()) {
		QtChatWidget * widget = getActiveTabWidget();
		if (widget) {
			widget->sendFileToSession(filename);
		}
	}
}

void QtChatWindow::callActiveTabContact() {
	const QString contactId = getActiveTabContactId();
	if (!contactId.isEmpty()) {
		QtContactListManager::getInstance()->startCall(contactId);
		if (_qtWengoPhone.getWidget()->isMinimized()) {
			_qtWengoPhone.getWidget()->showNormal();
		}
#if defined(OS_WINDOWS)
		SetForegroundWindow(_qtWengoPhone.getWidget()->winId());
#endif
	}
}

void QtChatWindow::showActiveTabContactInfo() {
	const QString contactId = getActiveTabContactId();
	if (!contactId.isEmpty()) {
		ContactProfile contactProfile =  getContactProfileFromContactId(contactId);
		QtProfileDetails qtProfileDetails(
				*_qtWengoPhone.getCWengoPhone().getCUserProfileHandler().getCUserProfile(),
		contactProfile, this, tr("Edit Contact"));
		if (qtProfileDetails.show()) {
			_qtWengoPhone.getCWengoPhone().getCUserProfileHandler().getCUserProfile()->getCContactList().updateContact(contactProfile);
		}
	}
}

void QtChatWindow::saveActiveTabChatHistory() {
	QtChatWidget * chatWidget = getActiveTabWidget();
	if (chatWidget) {
		chatWidget->saveHistoryAsHtml();
	}
}

void QtChatWindow::closeEvent(QCloseEvent *event) {
	//LOG_DEBUG(" femeture des sessions ");
	//while(_tabWidget->count()>0) {
	//	closeActiveTab();
	//}
}
