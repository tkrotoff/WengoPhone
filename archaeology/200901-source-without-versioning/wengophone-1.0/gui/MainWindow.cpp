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

#include "MainWindow.h"

#include "MyWidgetFactory.h"
#include "PhonePageWidget.h"
#include "callhistory/CallHistoryWidget.h"
#include "contact/Contact.h"
#include "contact/ContactId.h"
#include "contact/ContactList.h"
#include "contact/ContactListWidget.h"
#include "connect/Connect.h"
#include "contact/ContactWindow.h"
#include "StatusBar.h"
#include "systray/Systray.h"
#include "login/LoginWindow.h"
#include "login/User.h"
#include "HomePageWidget.h"
#include "AboutWindow.h"
#include "AudioCallManager.h"
#include "AudioCall.h"
#include "SessionWindow.h"
#include "Softphone.h"
#include "config/ConfigWindow.h"
#include "config/VideoWidget.h"
#include "config/Audio.h"
#include "sip/SipAddress.h"
#include "util/WidgetStackControl.h"
#include "util/PhoneLineEdit.h"
#include "util/EventFilter.h"
#include "presence/PresencePopupMenu.h"
#include "presence/PresenceStatus.h"
#include "config/Config.h"
#include "config/AdvancedConfig.h"
#include "search/SearchPageWidget.h"
#include "config/Video.h"

#include "softphone-config.h"

#include <WebBrowser.h>

#include <qapplication.h>
#include <qwidgetlist.h>
#include <qtooltip.h>
#include <qpixmap.h>
#include <qdialog.h>
#include <qaction.h>
#include <qobject.h>
#include <qlineedit.h>
#include <qmainwindow.h>
#include <qwidgetstack.h>
#include <qwidgetfactory.h>
#include <qcombobox.h>
#include <qpushbutton.h>
#include <qslider.h>
#include <qvalidator.h>
#include <qregexp.h>
#include <qlabel.h>
#include <qbuttongroup.h>
#include <qdesktopwidget.h>
#include <qmenubar.h>
#include <qcheckbox.h>

#include <iostream>
using namespace std;
#include <cassert>

const QString MainWindow::ONNETCALL_TAG = "1";
const QString MainWindow::CELLPHONECALL_TAG = "2";
const QString MainWindow::HOMECALL_TAG = "3";
const QString MainWindow::WORKCALL_TAG = "4";
const QString MainWindow::CHAT_TAG = "5";
const QString MainWindow::SMS_TAG = "6";

MainWindow::MainWindow() {

	_mainWindow = (QMainWindow *) MyWidgetFactory::create("MainWindowForm.ui");
	_mainWindow->setCaption("WengoPhone");

	_menuBar = _mainWindow->menuBar();
	_actionMenu = new QPopupMenu(_menuBar);
	connect(_actionMenu, SIGNAL(aboutToShow()), this, SLOT(buildActionMenu()));
	_menuBar->insertItem(tr("Wengo"), _actionMenu, 0, 0);

	Softphone::getInstance().setMainWindow(*this);
	Systray * systray = new Systray(*this);
	Softphone::getInstance().setSystray(*systray);

	//CloseEvent filter
	CloseEventFilter * closeEventFilter = new CloseEventFilter(this, SLOT(close()));
	_mainWindow->installEventFilter(closeEventFilter);

	//PhoneComboBox
	_phoneComboBox = new PhoneComboBox(_mainWindow);

	//CallButton
	_callButton = (QPushButton *) _mainWindow->child("callButton", "QPushButton");
	//_callButton->setEnabled(false);
	connect(_callButton, SIGNAL(clicked()),
		_phoneComboBox, SLOT(callButtonClicked()));
	
	//Stack widget with CallHistoryWidget and ContactListWidget
	QAction * contactListAction = (QAction *) _mainWindow->child("contactListAction", "QAction");
	connect(contactListAction, SIGNAL(activated()), this, SLOT(contactListActivated()));

	QAction * callHistoryAction = (QAction *) _mainWindow->child("callHistoryAction", "QAction");
	connect(callHistoryAction, SIGNAL(activated()), this, SLOT(callHistoryActivated()));

	QAction * homePageAction = (QAction *) _mainWindow->child("homePageAction", "QAction");
	connect(homePageAction, SIGNAL(activated()), this, SLOT(homePageActivated()));

	//Search Widget
#ifdef WIN32
	_searchPageWidget = new SearchPageWidget(0);
#endif
	QAction * _smartDirAction = (QAction *) _mainWindow->child("findContactAction", "QAction");
	connect(_smartDirAction, SIGNAL(activated()) , this, SLOT(showSearchContactDialog()));
	
	//ContactList
	_contactListWidget = new ContactListWidget(ContactList::getInstance(),
						(QMainWindow *) _mainWindow,
						(QMainWindow *) _mainWindow);
	
	//CallHistory
	_callHistoryWidget = new CallHistoryWidget(CallHistory::getInstance(), _mainWindow);
	
	//HomePage
	_homePageWidget = new HomePageWidget(_mainWindow);
	
	//Stack
	_stack = (QWidgetStack *) _mainWindow->child("widgetStack", "QWidgetStack");
	_stackControl = new WidgetStackControl(_stack);
	_stackControl->addToStack(homePageAction, _homePageWidget->getWidget());
	_stackControl->addToStack(contactListAction, _contactListWidget->getWidget());
	_stackControl->addToStack(callHistoryAction, _callHistoryWidget->getWidget());
	
	//StatusBar
	_statusBar = new StatusBar((QMainWindow *) _mainWindow);
	
	//Volume sliders
	_outputSoundSlider = (QSlider *) _mainWindow->child("outputSoundSlider");
	_inputSoundSlider = (QSlider *) _mainWindow->child("inputSoundSlider");
	_videoSettingsLabel = (QLabel *) _mainWindow->child("VideoSettingsLabel");
	_audioSettingsLabel = (QLabel *) _mainWindow->child("AudioSettingsLabel");
	LeftMouseClickEventFilter * leftMouseClickEventFilter2 = new LeftMouseClickEventFilter(this, SLOT(goToAudioConfig()));
	_audioSettingsLabel->installEventFilter(leftMouseClickEventFilter2);
	LeftMouseClickEventFilter * leftMouseClickEventFilter3 = new LeftMouseClickEventFilter(this, SLOT(goToVideoConfig()));
	_videoSettingsLabel->installEventFilter(leftMouseClickEventFilter3);
	_enableVideoCheckBox = (QCheckBox*)_mainWindow->child("EnableVideoCheckBox", "QCheckBox");
	
	//Configuration Window
	_configWindow = new ConfigWindow(_mainWindow, *this);
	QAction * configAction = (QAction *) _mainWindow->child("configAction", "QAction");
	connect(configAction, SIGNAL(activated()), this, SLOT(showConfigurationWindow()));

	//Dialpad Window
	_dialpadAction = (QAction *) _mainWindow->child("dialpadAction", "QAction");
	connect(_dialpadAction, SIGNAL(activated()),
		this, SLOT(showDialpad()));


	//About Window
	_aboutWindow = new AboutWindow(_mainWindow);
	QAction * aboutAction = (QAction *) _mainWindow->child("aboutAction", "QAction");
	connect(aboutAction, SIGNAL(activated()),
		this, SLOT(showAbout()));


	//viewProfileAction
	QAction * viewProfileAction = (QAction *) _mainWindow->child("viewProfileAction", "QAction");
	connect(viewProfileAction, SIGNAL(activated()),
		this, SLOT(showMyWengoAccount()));


	//helpAction
	QAction * helpAction = (QAction *) _mainWindow->child("helpAction", "QAction");
	connect(helpAction, SIGNAL(activated()),
		this, SLOT(showHelp()));


	//forumAccessAction
	QAction * forumAccessAction = (QAction *) _mainWindow->child("forumAccessAction", "QAction");
	connect(forumAccessAction, SIGNAL(activated()),
		this, SLOT(showForum()));

	//credit account action
	QAction * creditAccountAction = (QAction *) _mainWindow->child("creditAccountAction", "QAction");
	connect(creditAccountAction, SIGNAL(activated()),
			this, SLOT(showCreditAccount()));

	//account creation action
	QAction * newAccountAction = (QAction *) _mainWindow->child("newAccountAction", "QAction");
	connect(newAccountAction, SIGNAL(activated()),
			this, SLOT(showAccountCreation()));

	//PresencePopupMenu
	_presencePopupMenu = new PresencePopupMenu(_mainWindow);

	//myPresenceStatusButton
	_myPresenceStatusButton = (QPushButton *) _mainWindow->child("myPresenceStatusButton", "QPushButton");
	_myPresenceStatusButton->setPopup(_presencePopupMenu);
	connect(_presencePopupMenu, SIGNAL(presenceChanged(const PresenceStatus &)),
		this, SLOT(presenceStatusChanged(const PresenceStatus &)));

	//Publish my status online
	_presencePopupMenu->defaultStatus();

	//Quit action
	QAction * quitAction = (QAction *) _mainWindow->child("quitAction", "QAction");
	connect(quitAction, SIGNAL(activated()),
		this, SLOT(exit()));

	//sendSMSAction
	QAction * sendSMSAction = (QAction *) _mainWindow->child("sendSMSAction", "QAction");
	connect(sendSMSAction, SIGNAL(activated()),
		this, SLOT(showSMSSessionWindow()));

	//audio configuration widget stack
	QWidgetStack * audioStack = (QWidgetStack *) getWidget()->child("audioWidgetStack", "QWidgetStack");
	QWidget * audioPage = (QWidget *) getWidget()->child("WStackPage", "QWidget");
	audioStack->raiseWidget(audioPage);
	_audioPanel = (QWidgetStack *) _mainWindow->child("audioWidgetStack", "QWidgetStack");
	LeftMouseClickEventFilter * leftMouseClickEventFilter = new LeftMouseClickEventFilter(this, SLOT(switchAudioPanel()));
	_audioSwitchLabel = (QLabel *) _mainWindow->child("audioSwitchLabel", "QLabel");
	_audioSwitchLabel->installEventFilter(leftMouseClickEventFilter);
	QLabel * audioSwitchLabel2 = (QLabel *) _mainWindow->child("audioTextLabel", "QLabel");
	audioSwitchLabel2->installEventFilter(leftMouseClickEventFilter);
	_showAudioPanel = false;
	switchAudioPanel();

#ifdef ENABLE_VIDEO
	//enable video checkbox sync with video configuration panel
	VideoWidget & videoWidget = _configWindow->getVideoWidget();
	_enableVideoCheckBox->setChecked(videoWidget.getEnableVideoCheckBox()->isChecked());
	connect(_enableVideoCheckBox, SIGNAL(toggled(bool)), videoWidget.getEnableVideoCheckBox(), SLOT(setChecked(bool)));
#endif
	
#ifndef WIN32
	_mainWindow->setIcon(QPixmap::fromMimeSource("logo-spiral.png"));
#endif
	
	// test if the window position can be set according to the screen resolution
	int desk_w = qApp->desktop()->width();
	int desk_h = qApp->desktop()->height();
	AdvancedConfig & adv_conf = AdvancedConfig::getInstance();
	if((adv_conf.getWindowX() >= 0) && (adv_conf.getWindowX() <= desk_w) &&
		   (adv_conf.getWindowY() >= 0) && (adv_conf.getWindowY() <= desk_h)) {
		_mainWindow->move(adv_conf.getWindowX(), adv_conf.getWindowY());
	}
	_mainWindow->resize(adv_conf.getWindowWidth(), adv_conf.getWindowHeight());
}

MainWindow::~MainWindow() {
	AdvancedConfig & adv_conf = AdvancedConfig::getInstance();
	adv_conf.setWindowX(_mainWindow->x());
	adv_conf.setWindowY(_mainWindow->y());
	adv_conf.setWindowWidth(_mainWindow->width());
	if(_showAudioPanel) {
		adv_conf.setWindowHeight(_mainWindow->height());
	} else {
		int diff = ((QWidgetStack *) _mainWindow->child("mainAudioWidgetStack", "QWidgetStack"))->height();
		adv_conf.setWindowHeight(_mainWindow->height() - diff);
	}
	Config & conf = Config::getInstance();
	conf.saveAsXml();

	//delete _presencePopupMenu;
	delete _configWindow;
	delete _statusBar;
	//FIXME crashes
	//delete _stack;
	delete _stackControl;
	delete _callHistoryWidget;
	delete _contactListWidget;
#ifdef WIN32
	delete _searchPageWidget;
#endif
	delete _mainWindow;
}

void MainWindow::showDialpad() {
	AudioCall * audioCall = AudioCallManager::getInstance().createAudioCall();
	audioCall->getSessionWindow().showEmptyDefaultPage();
}

void MainWindow::showMyWengoAccount() {
	acessAuthWebSite("homepage");
}

void MainWindow::showCreditAccount() {
	acessAuthWebSite("reload");
}

void MainWindow::showAccountCreation() {
	acessWebSite(Softphone::URL_CREATE_WENGO_ACCOUNT);
}

void MainWindow::showHelp() {
	acessWebSite(Softphone::URL_HELP_WENGO);
}

void MainWindow::showForum() {
	acessWebSite(Softphone::URL_FORUM_WENGO);
}

void MainWindow::showAbout() {
	_aboutWindow->getWidget()->show();
}

void MainWindow::PCtoPCmode() {
	Connect::getInstance().disconnect();

	//Does not change the SIP status so that
	//we can make on-net calls using IP addresses
	//like sip:192.168.0.1
	_statusBar->setSipStatus(true);

	Connect::getInstance().setPCtoPCmode(true);
	Connect::getInstance().connect();
}

void MainWindow::unRegister() {
	Connect::getInstance().disconnect();
}

void MainWindow::close() {
	//Reduces the memory consumption
	_mainWindow->showMinimized();
}

void MainWindow::exit() {
	_presencePopupMenu->publishOfflineStatus();

	_statusBar->setMessage(tr("Unregistering, please wait..."));
	//Unregister + phTerminate
	Softphone::getInstance().exit();
}

CallHistoryWidget & MainWindow::getCallHistoryWidget() const {
	assert(_callHistoryWidget && "_callHIstoryWidget cannot be NULL");
	return *_callHistoryWidget;
}

ConfigWindow & MainWindow::getConfigWindow() const {
	assert(_configWindow && "_configWindow cannot be NULL");
	return *_configWindow;
}

void MainWindow::showAsActiveWindow() {
	QAction * contactListAction = (QAction *) _mainWindow->child("contactListAction", "QAction");
	contactListAction->activate();

	_mainWindow->showNormal();
	_mainWindow->setActiveWindow();
}

void MainWindow::presenceStatusChanged(const PresenceStatus & presenceStatus) {
	static const unsigned int MAX_SIZE = 15;
	_myPresenceStatusButton->setIconSet(presenceStatus.getStatusIcon());

	QString tmp(presenceStatus.getStatusTextTranslated());
	if (tmp.length() > MAX_SIZE) {
		tmp.truncate(MAX_SIZE);
		tmp += "...";
	}

	_myPresenceStatusButton->setText(tmp);

	//No tooltip
	QToolTip::add(_myPresenceStatusButton, presenceStatus.getStatusText());
}

void MainWindow::switchAudioPanel() {
	int margin = ((QWidgetStack *) _mainWindow->child("mainAudioWidgetStack", "QWidgetStack"))->margin();
	if (!_showAudioPanel) {
		_audioPanel->hide();
		_audioSwitchLabel->setPixmap(QPixmap::fromMimeSource("arrow_right.png"));
		_mainWindow->resize(_mainWindow->width(), _mainWindow->height() - _audioPanel->height() - margin);
	} else {
		_audioPanel->show();
		_audioSwitchLabel->setPixmap(QPixmap::fromMimeSource("arrow_down.png"));
		_mainWindow->resize(_mainWindow->width(), _mainWindow->height() + _audioPanel->height() + margin);
	}
	_showAudioPanel= !_showAudioPanel;
}

void MainWindow::raiseHomePage(){
	homePageActivated();
	_stack->raiseWidget(_homePageWidget->getWidget());
}

void MainWindow::raiseContactList(){
	contactListActivated();
	_stack->raiseWidget(_contactListWidget->getWidget());
}

void MainWindow::raiseHistory(){
	callHistoryActivated();
	_stack->raiseWidget(_callHistoryWidget->getWidget());
}

void MainWindow::showSearchContactDialog() {
#ifdef WIN32
	_searchPageWidget->getWidget()->show();
#else
	Config & conf = Config::getInstance();
	const QString langCode = conf.getLanguageCode();
	WebBrowser::openUrl(Softphone::URL_SMART_DIRECTORY + "&lang=" + langCode);
#endif
}

void MainWindow::showSMSSessionWindow() {
	AudioCall * audioCall = AudioCallManager::getInstance().createAudioCall();
	audioCall->getSessionWindow().showSmsPage();
}

void MainWindow::showAddContact() {
	ContactListWidget * lw = getContactListWidget();
	ContactWindow * cw = lw->getContactWindow();
	cw->execAddContact();
}

void MainWindow::showConfigurationWindow() {
	_configWindow->getWidget()->show();
}

void MainWindow::acessWebSite(QString url) {
	Config & conf = Config::getInstance();
	const QString langCode = conf.getLanguageCode();
	WebBrowser::openUrl(url + "&lang=" + langCode + "&wl=" + QString(WL_TAG));
}

void MainWindow::acessAuthWebSite(QString page) {
	Softphone & softphone = Softphone::getInstance();
	const User & user = softphone.getUser();
	Config & conf = Config::getInstance();
	const QString langCode = conf.getLanguageCode();
	WebBrowser::openUrl(Softphone::URL_WENGO_ACCOUNT_AUTH + "?login=" + user.getLogin() + "&password=" + user.getPassword() + "&lang=" + langCode + "&wl=" + QString(WL_TAG) + "&page=" + page);
}

void MainWindow::buildActionMenu() {
	
	if(_actionMenu) {
		bool contacts_landline = false;
		bool contacts_online = false;
		bool contacts_mobile = false;
		
		//the action menu
		_actionMenu->clear();
		
		//call menu
		QPopupMenu * callMenu = new QPopupMenu(_actionMenu);
		
		QPopupMenu * onNetCallMenu = new QPopupMenu(callMenu);
		QPopupMenu * fixCallMenu = new QPopupMenu(callMenu);
		QPopupMenu * mobileCallMenu = new QPopupMenu(callMenu);
		
		callMenu->insertItem(tr("Compose"), this, SLOT(showDialpad()));
		
		callMenu->insertItem(tr("Online Buddies"), onNetCallMenu);
		callMenu->insertItem(tr("Land-Line"), fixCallMenu);
		callMenu->insertItem(tr("Cell-Phone"), mobileCallMenu);
	
		//chat menu
		QPopupMenu * chatMenu = new QPopupMenu(_actionMenu);
	
		//sms menu
		QPopupMenu * smsMenu = new QPopupMenu(_actionMenu);
		QAction * sendSMSAction = (QAction *) _mainWindow->child("sendSMSAction", "QAction");
		sendSMSAction->addTo(smsMenu);
	
		//iterate over all contacts
		ContactList & contactList = ContactList::getInstance();
		for (unsigned int i = 0; i < contactList.size(); i++) {
			//current contact
			Contact & contact = contactList[i];
	
			//if the contact at at least one phone number, we add an entry
			if( contact.getPhoneList().size() > 0 ) {
				
				QString id;
				
				// if the contact has a wengo name & is not offline
				// we add an entry in on net call menu & in start chat menu
				if( (contact.getWengoPhone() != QString::null) && contact.getPresenceStatus().getStatus() != PresenceStatus::Offline) {
	
					//choose the status pixmap
					QPixmap pix;
					switch(contact.getPresenceStatus().getStatus()) {
						case PresenceStatus::Online:
							pix = QPixmap::fromMimeSource("online.png");
							break;
						case PresenceStatus::Away:
							pix = QPixmap::fromMimeSource("away.png");
							break;
						case PresenceStatus::DoNotDisturb:
							pix = QPixmap::fromMimeSource("dnd.png");
							break;
						case PresenceStatus::UserDefine:
							pix = QPixmap::fromMimeSource("user.png");
							break;
					}
	
					id = ONNETCALL_TAG + contact.getId().toString();
					onNetCallMenu->insertItem(
							pix,
							contact.toString() + ": " + contact.getWengoPhone(),
							this,
							SLOT(actionMenuSlot(int)),
							0,
							id.toInt());
	
					id = CHAT_TAG + contact.getId().toString();
					chatMenu->insertItem(
							pix,
							contact.toString() + ": " + contact.getWengoPhone(),
							this,
							SLOT(actionMenuSlot(int)),
							0,
							id.toInt());
					contacts_online = true;
				}
				
				// if the contact has a mobile phone
				// we add an entry in mobile call menu & in the SMS menu
				if( (contact.getMobilePhone() != QString::null) && (contact.getMobilePhone() != "") ) {
					
					id = CELLPHONECALL_TAG + contact.getId().toString();
					mobileCallMenu->insertItem(
							QPixmap::fromMimeSource("contact_phone_mobile.png"),
							contact.toString() + ": " + contact.getMobilePhone(),
							this,
							SLOT(actionMenuSlot(int)),
							0,
							id.toInt());
					
					id = SMS_TAG + contact.getId().toString();
					smsMenu->insertItem(
							QPixmap::fromMimeSource("contact_phone_mobile.png"),
							contact.toString() + ": " + contact.getMobilePhone(),
							this,
							SLOT(actionMenuSlot(int)),
							0,
							id.toInt());
					contacts_mobile = true;
				}
				
				// if the contact has a land line phone (home & work)
				// we add an entry in phone call menu
				id = HOMECALL_TAG + contact.getId().toString();
				if( (contact.getHomePhone() != QString::null) && (contact.getHomePhone() != "") ) {
					
					fixCallMenu->insertItem(
							QPixmap::fromMimeSource("contact_phone_home.png"),
							contact.toString() + ": " + contact.getHomePhone(),
							this,
							SLOT(actionMenuSlot(int)),
							0,
							id.toInt());
					contacts_landline = true;
				}
	
				id = WORKCALL_TAG + contact.getId().toString();
				if( (contact.getWorkPhone() != QString::null) && (contact.getWorkPhone() != "") ) {
					
					fixCallMenu->insertItem(
							QPixmap::fromMimeSource("contact_phone_work.png"),
							contact.toString() + ": " + contact.getWorkPhone(),
							this,
							SLOT(actionMenuSlot(int)),
							0,
							id.toInt());
					contacts_landline = true;
				}
			}
		}
		
		if(!contacts_online) {
			onNetCallMenu->insertItem(tr("No contact online"));
			chatMenu->insertItem(tr("No contact online"));
		}
		if(!contacts_mobile) {
			mobileCallMenu->insertItem(tr("No contact match"));
		}
		if(!contacts_landline) {
			fixCallMenu->insertItem(tr("No contact match"));
		}
		
		_actionMenu->insertItem(tr("Make Call"), callMenu);
		_actionMenu->insertItem(tr("Send SMS"), smsMenu);
		_actionMenu->insertItem(tr("Start Chat"), chatMenu);
	
		//separator
		_actionMenu->insertSeparator();
	
		//add contact
		QAction * addContactActionNoIcon = (QAction *) _mainWindow->child("addContactActionNoIcon", "QAction");
		addContactActionNoIcon->addTo(_actionMenu);
	
		//search buddies
		_actionMenu->insertItem(tr("Search Buddies..."), this, SLOT(showSearchContactDialog()));
	 
		//show configuration panel
		QAction * configAction = (QAction *) _mainWindow->child("configAction", "QAction");
		configAction->addTo(_actionMenu);
	
		//separator
		_actionMenu->insertSeparator();
	
		//quit
		QAction * quitAction = (QAction *) _mainWindow->child("quitAction", "QAction");
		quitAction->addTo(_actionMenu);
	}
}

void MainWindow::actionMenuSlot(int id) {
	QString tmp = QString::number(id);
	ContactList & contactList = ContactList::getInstance();
	Contact & contact = contactList.getContact(ContactId(tmp.right(tmp.length() - 1)));

	if(tmp.startsWith(ONNETCALL_TAG)) {
		onNetCall(contact);
	} else if(tmp.startsWith(CELLPHONECALL_TAG)) {
		cellPhoneCall(contact);
	} else if(tmp.startsWith(HOMECALL_TAG)) {
		homePhoneCall(contact);
	} else if(tmp.startsWith(WORKCALL_TAG)) {
		workPhoneCall(contact);
	} else if(tmp.startsWith(CHAT_TAG)) {
		startChat(contact);
	} else if(tmp.startsWith(SMS_TAG)) {
		sendSms(contact);
	}
}

void MainWindow::sendSms(Contact & contact) {
	AudioCallManager::getInstance().sendSms(contact.getMobilePhone(), "", &contact);
}

void MainWindow::startChat(Contact & contact) {
	AudioCallManager::getInstance().startChat(contact);
}

void MainWindow::cellPhoneCall(Contact & contact) {
	QString number = contact.getMobilePhone();
	if (!number.isEmpty()) {
		AudioCallManager::getInstance().createAudioCall(
			SipAddress::fromPhoneNumber(number), &contact);
	}
}

void MainWindow::homePhoneCall(Contact & contact) {
	QString number = contact.getHomePhone();
	if (!number.isEmpty()) {
		AudioCallManager::getInstance().createAudioCall(
			SipAddress::fromPhoneNumber(number), &contact);
	}
}

void MainWindow::workPhoneCall(Contact & contact) {
	QString number = contact.getWorkPhone();
	if (!number.isEmpty()) {
		AudioCallManager::getInstance().createAudioCall(
			SipAddress::fromPhoneNumber(number), &contact);
	}
}

void MainWindow::onNetCall(Contact & contact) {
	QString number = contact.getWengoPhone();
	if (!number.isEmpty()) {
#ifdef ENABLE_VIDEO
		if (Video::getInstance().getEnable()) {
			AudioCallManager::getInstance().createAudioCall(
				SipAddress::fromPhoneNumber(number), &contact, true);
		} else
#endif
	{
		AudioCallManager::getInstance().createAudioCall(
			SipAddress::fromPhoneNumber(number), &contact, false);
	}
	}
}

void MainWindow::contactListActivated() {
	QAction * contactListAction = (QAction *) _mainWindow->child("contactListAction", "QAction");
	QAction * callHistoryAction = (QAction *) _mainWindow->child("callHistoryAction", "QAction");
	QAction * homePageAction = (QAction *) _mainWindow->child("homePageAction", "QAction");

	contactListAction->setIconSet(QPixmap::fromMimeSource("contact_list_on.png"));
	callHistoryAction->setIconSet(QPixmap::fromMimeSource("call_history.png"));
	homePageAction->setIconSet(QPixmap::fromMimeSource("home.png"));
}

void MainWindow::homePageActivated() {
	QAction * contactListAction = (QAction *) _mainWindow->child("contactListAction", "QAction");
	QAction * callHistoryAction = (QAction *) _mainWindow->child("callHistoryAction", "QAction");
	QAction * homePageAction = (QAction *) _mainWindow->child("homePageAction", "QAction");

	contactListAction->setIconSet(QPixmap::fromMimeSource("contact_list.png"));
	callHistoryAction->setIconSet(QPixmap::fromMimeSource("call_history.png"));
	homePageAction->setIconSet(QPixmap::fromMimeSource("home_on.png"));
}

void MainWindow::callHistoryActivated() {
	QAction * contactListAction = (QAction *) _mainWindow->child("contactListAction", "QAction");
	QAction * callHistoryAction = (QAction *) _mainWindow->child("callHistoryAction", "QAction");
	QAction * homePageAction = (QAction *) _mainWindow->child("homePageAction", "QAction");

	contactListAction->setIconSet(QPixmap::fromMimeSource("contact_list.png"));
	callHistoryAction->setIconSet(QPixmap::fromMimeSource("call_history_on.png"));
	homePageAction->setIconSet(QPixmap::fromMimeSource("home.png"));
}

void MainWindow::goToAudioConfig() {
	_configWindow->raiseAudioConfig();
}

void MainWindow::goToVideoConfig() {
	_configWindow->raiseVideoConfig();
}

