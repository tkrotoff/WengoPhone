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

#include "QtWengoPhone.h"

#include <model/account/SipAccount.h>
#include <model/account/wengo/WengoAccount.h>
#include <model/connect/ConnectHandler.h>
#include <model/phonecall/PhoneCall.h>
#include <model/profile/UserProfile.h>

#include <control/CWengoPhone.h>

#include <imwrapper/EnumIMProtocol.h>

#include "phoneline/QtPhoneLine.h"
#include "phonecall/QtPhoneCall.h"
#include "QtLogger.h"
#include "login/QtLogin.h"
#include "login/QtSetLogin.h"
#include "login/QtEditMyProfile.h"
#include "login/QtAccountManager.h"
#include "contactlist/QtContactList.h"
#include "contactlist/QtAddContact.h"
#include "QtDialpad.h"
#include "QtAbout.h"
#include "sms/QtSms.h"
#include "QtHttpProxyLogin.h"
#include "config/QtAdvancedConfig.h"
#include "config/QtWengoConfigDialog.h"
#include "statusbar/QtStatusBar.h"
#include "history/QtHistoryWidget.h"

#include <qtutil/WidgetFactory.h>
#include <qtutil/Object.h>
#include <thread/Thread.h>
#include <util/Logger.h>
#include <QtBrowser.h>

#include <cutil/global.h>

#include <QtGui>
#include <trayicon.h>

using namespace std;

const std::string QtWengoPhone::ANCHOR_CONTACTLIST = "openwengo_phonebook";
const std::string QtWengoPhone::ANCHOR_HISTORY = "openwengo_log";
const std::string QtWengoPhone::ANCHOR_CONFIGURATION = "openwengo_configuration";
const std::string QtWengoPhone::ANCHOR_DIALPAD = "openwengo_dial";
const std::string QtWengoPhone::ANCHOR_ADDCONTACT = "openwengo_addcontact";
const std::string QtWengoPhone::ANCHOR_SELFCARE = "openwengo_selfcare";
const std::string QtWengoPhone::ANCHOR_FORUM = "openwengo_forum";
const std::string QtWengoPhone::URL_WENGO_MINI_HOME = "https://www.wengo.fr/auth/auth.php";
const std::string QtWengoPhone::LOCAL_WEB_DIR = "webpages/windows";


QtWengoPhone::QtWengoPhone(CWengoPhone & cWengoPhone)
	: QObjectThreadSafe(),
	_cWengoPhone(cWengoPhone) {

	_qtSms = NULL;

	_cWengoPhone.loginStateChangedEvent +=
		boost::bind(&QtWengoPhone::loginStateChangedEventHandler, this, _1, _2);
	_cWengoPhone.networkDiscoveryStateChangedEvent +=
		boost::bind(&QtWengoPhone::networkDiscoveryStateChangedEventHandler, this, _1, _2);
	_cWengoPhone.noAccountAvailableEvent +=
		boost::bind(&QtWengoPhone::noAccountAvailableEventHandler, this, _1);
	_cWengoPhone.proxyNeedsAuthenticationEvent +=
		boost::bind(&QtWengoPhone::proxyNeedsAuthenticationEventHandler, this, _1, _2, _3);
	_cWengoPhone.wrongProxyAuthenticationEvent +=
		boost::bind(&QtWengoPhone::wrongProxyAuthenticationEventHandler, this, _1, _2, _3, _4, _5);

	typedef PostEvent0<void ()> MyPostEvent;
	MyPostEvent * event = new MyPostEvent(boost::bind(&QtWengoPhone::initThreadSafe, this));
	postEvent(event);
}

void QtWengoPhone::initThreadSafe() {
	_wengoPhoneWindow = qobject_cast<QMainWindow *>(WidgetFactory::create(":/forms/WengoPhoneWindow.ui", NULL));

	//callButton
	_callButton = Object::findChild<QPushButton *>(_wengoPhoneWindow, "callButton");

	//phoneComboBox
	_phoneComboBox = Object::findChild<QComboBox *>(_wengoPhoneWindow, "phoneComboBox");

	//Button initialization
	initButtons();

	//tabWidget
	_tabWidget = Object::findChild<QTabWidget *>(_wengoPhoneWindow, "tabWidget");

	//Dialpad
	QtDialpad * qtDialpad = new QtDialpad(this);
	QWidget * tabDialpad = Object::findChild<QWidget *>(_tabWidget, "tabDialpad");
	createLayout(tabDialpad)->addWidget(qtDialpad->getWidget());

	//Systray
	_trayMenu = NULL;
	_trayIcon = new TrayIcon(QPixmap(":pics/systray_icon.png"),QString("Wengophone"), _trayMenu, _wengoPhoneWindow);
	setTrayMenu();
	_trayIcon->show();

	//logger
	//FIXME no more logger tab widget
	/*QtLogger * qtLogger = new QtLogger(_wengoPhoneWindow);
	QWidget * tabLogger = Object::findChild<QWidget *>(_tabWidget, "tabLogger");
	createLayout(tabLogger)->addWidget(qtLogger->getWidget());*/

	//actionShowWengoAccount
	QAction * actionShowWengoAccount = Object::findChild<QAction *>(_wengoPhoneWindow, "actionShowWengoAccount");
	connect(actionShowWengoAccount, SIGNAL(triggered()), SLOT(showWengoAccount()));

	//actionOpenWengoAccount
	QAction * actionOpenWengoAccount = Object::findChild<QAction *>(_wengoPhoneWindow, "actionOpenWengoAccount");
	connect(actionOpenWengoAccount, SIGNAL(triggered()), SLOT(openWengoAccount()));

	//actionEditMyProfile
	QAction * actionEditMyProfile = Object::findChild<QAction *>(_wengoPhoneWindow, "actionEditMyProfile");
	connect(actionEditMyProfile, SIGNAL(triggered()), SLOT(editMyProfile()));

	//actionExit
	QAction * actionExit = Object::findChild<QAction *>(_wengoPhoneWindow, "actionExit");
	connect(actionExit, SIGNAL(triggered()), SLOT(exitApplication()));

	//actionAddContact
	QAction * actionAddContact = Object::findChild<QAction *>(_wengoPhoneWindow, "actionAddContact");
	connect(actionAddContact, SIGNAL(triggered()), SLOT(addContact()));

	//actionSetLogin
	QAction * actionSetLogin = Object::findChild<QAction *>(_wengoPhoneWindow, "actionSetLogin");
	connect(actionSetLogin, SIGNAL(triggered()), SLOT(actionSetLogin()));

	//actionConfiguration
	QAction * actionConfiguration = Object::findChild<QAction *>(_wengoPhoneWindow, "actionConfiguration");
	connect(actionConfiguration, SIGNAL(triggered()), SLOT(showConfig()));

	//actionShowForum
	QAction * actionShowForum = Object::findChild<QAction *>(_wengoPhoneWindow, "actionShowForum");
	connect(actionShowForum, SIGNAL(triggered()), SLOT(showForum()));

	//actionShowHelp
	QAction * actionShowHelp = Object::findChild<QAction *>(_wengoPhoneWindow, "actionShowHelp");
	connect(actionShowHelp, SIGNAL(triggered()), SLOT(showHelp()));

	//actionShowAbout
	QAction * actionShowAbout = Object::findChild<QAction *>(_wengoPhoneWindow, "actionShowAbout");
	connect(actionShowAbout, SIGNAL(triggered()), SLOT(showAbout()));

	//actionSendSms
	QAction * actionSendSms = Object::findChild<QAction *>(_wengoPhoneWindow, "actionSendSms");
	connect(actionSendSms, SIGNAL(triggered()), SLOT(sendSms()));

	//actionAdvancedConfiguration
	QAction * actionAdvancedConfiguration = Object::findChild<QAction *>(_wengoPhoneWindow, "actionAdvancedConfiguration");
	connect(actionAdvancedConfiguration, SIGNAL(triggered()), SLOT(showAdvancedConfig()));

	//actionFaq
	QAction * actionFaq = Object::findChild<QAction *>(_wengoPhoneWindow, "actionFaq");
	connect (actionFaq, SIGNAL (triggered()), SLOT(showFaq()));

	//actionBuyCallOutCredits
	QAction * actionBuyCallOutCredits = Object::findChild<QAction *>(_wengoPhoneWindow, "actionBuyCallOutCredits");
	connect(actionBuyCallOutCredits, SIGNAL(triggered()), SLOT(showByOut()));

	//actionCallOutService
	QAction * actionCallOutService = Object::findChild<QAction *>(_wengoPhoneWindow, "actionCallOutService");
	connect(actionCallOutService, SIGNAL(triggered()), SLOT (showCallOut()));

	//actionSms
	QAction * actionSms = Object::findChild<QAction *>(_wengoPhoneWindow, "actionSms");
	connect(actionSms, SIGNAL(triggered()), SLOT (showSms()));

	//actionVoiceMail
	QAction * actionVoiceMail = Object::findChild<QAction *>(_wengoPhoneWindow, "actionVoiceMail");
	connect(actionVoiceMail, SIGNAL(triggered()), SLOT (showVoiceMail()));

	// actionIM_Account_Settings
	QAction * actionIM_Account_Settings = Object::findChild<QAction *>(_wengoPhoneWindow,"actionIM_Account_Settings");
	connect (actionIM_Account_Settings,SIGNAL(triggered()), SLOT(showAccountSettings()));

	//actionCreateConferenceCall
	QAction * actionCreateConferenceCall = Object::findChild<QAction *>(_wengoPhoneWindow, "actionCreateConferenceCall");
	connect(actionCreateConferenceCall, SIGNAL(triggered()), SLOT(showCreateConferenceCall()));

	//Embedded Browser
	_browser = new QtBrowser(NULL);
	_browser->urlClickedEvent += boost::bind(&QtWengoPhone::urlClickedEventHandler, this, _1);
	QWidget * tabHome = Object::findChild<QWidget *>(_tabWidget, "tabHome");
	createLayout(tabHome)->addWidget((QWidget*) _browser->getWidget());
#ifdef OS_WINDOWS
	_browser->setUrl(qApp->applicationDirPath().toStdString() + "/" + LOCAL_WEB_DIR + "/connecting_fr.htm");
#endif

	//Add the profile bar
	QFrame * profileBar = Object::findChild<QFrame *>(_wengoPhoneWindow, "profileBar");
	QWidget * centralWidget = Object::findChild<QWidget *>(_wengoPhoneWindow, "centralWidget");

	QGridLayout * gridlayout;
	gridlayout = (QGridLayout *) centralWidget->layout();
	gridlayout->removeWidget(profileBar);
	gridlayout->addWidget(new QtStatusBar(centralWidget), 1, 0);

	//Status bar
	QStatusBar * statusBar = Object::findChild<QStatusBar *>(_wengoPhoneWindow, "statusBar");

	_internetConnectionStateLabel = new QLabel(statusBar);
	_internetConnectionStateLabel->setPixmap(QPixmap(":/pics/statusbar_connect_error.png"));
	_internetConnectionStateLabel->setToolTip(tr("Not connected"));
	statusBar->addPermanentWidget(_internetConnectionStateLabel);

	_phoneLineStateLabel = new QLabel(statusBar);
	_phoneLineStateLabel->setPixmap(QPixmap(":/pics/statusbar_sip_error.png"));
	_phoneLineStateLabel->setToolTip(tr("Not connected"));
	statusBar->addPermanentWidget(_phoneLineStateLabel);

	_wengoPhoneWindow->show();
}

void QtWengoPhone::initButtons() {
	//callButton
	_callButton->disconnect();
	connect(_callButton, SIGNAL(clicked()), SLOT(callButtonClicked()));
	enableCallButton();

	//phoneComboBox
	_phoneComboBox->disconnect();
	connect(_phoneComboBox, SIGNAL(activated(int)), SLOT(callButtonClicked()));
	connect(_phoneComboBox, SIGNAL(editTextChanged(const QString &)), SLOT(enableCallButton()));
}

void QtWengoPhone::enableCallButton() {
	std::string phoneNumber = _phoneComboBox->currentText().toStdString();
	_callButton->setEnabled(!phoneNumber.empty());
}

void QtWengoPhone::addPhoneCall(QtPhoneCall * qtPhoneCall) {
	static QWidget * tabPhoneCall = Object::findChild<QWidget *>(_tabWidget, "tabPhoneCall");
	static QGridLayout * layout = new QGridLayout(tabPhoneCall);

	layout->addWidget(qtPhoneCall->getWidget());
}

void QtWengoPhone::showLoginWindow() {
	static QtLogin * login = new QtLogin(_wengoPhoneWindow);

	int ret = login->exec();

	if (ret == QDialog::Accepted) {
		_cWengoPhone.addWengoAccount(login->getLogin(), login->getPassword(), login->hasAutoLogin());
	}
}

QLayout * QtWengoPhone::createLayout(QWidget * parent) {
	QGridLayout * layout = new QGridLayout(parent);
	layout->setSpacing(0);
	layout->setMargin(0);
	return layout;
}

void QtWengoPhone::setContactList(QtContactList * qtContactList) {
	QWidget * tabContactList = Object::findChild<QWidget *>(_tabWidget, "tabContactList");
	createLayout(tabContactList)->addWidget(qtContactList->getWidget());
	LOG_DEBUG("QtContactList added");
}

void QtWengoPhone::setHistory(QtHistoryWidget * qtHistoryWidget) {
	QWidget * tabHistory = Object::findChild<QWidget *>(_tabWidget,"tabHistory");
	//QtHistoryWidget * qtHistoryWidget = new QtHistoryWidget(qtHistory->getWidget());
	createLayout(tabHistory)->addWidget(qtHistoryWidget);
}

void QtWengoPhone::setSms(QtSms * qtSms) {
	_qtSms = qtSms;
}

void QtWengoPhone::updatePresentation() {
	typedef PostEvent0<void ()> MyPostEvent;
	MyPostEvent * event = new MyPostEvent(boost::bind(&QtWengoPhone::updatePresentationThreadSafe, this));
	postEvent(event);
}

void QtWengoPhone::updatePresentationThreadSafe() {
}

void QtWengoPhone::loginStateChangedEventHandler(SipAccount & sender, SipAccount::LoginState state) {
	typedef PostEvent2<void (SipAccount & sender, SipAccount::LoginState), SipAccount &, SipAccount::LoginState> MyPostEvent;
	MyPostEvent * event = new MyPostEvent(boost::bind(&QtWengoPhone::loginStateChangedEventHandlerThreadSafe, this, _1, _2), sender, state);
	postEvent(event);
}

void QtWengoPhone::loginStateChangedEventHandlerThreadSafe(SipAccount & sender, SipAccount::LoginState state) {
	const WengoAccount * wengoAccount = dynamic_cast<const WengoAccount *>(&sender);

	switch (state) {
	case SipAccount::LoginStateReady:
#ifdef OS_WINDOWS
		if( wengoAccount ) {
			std::string data = "?login=" + wengoAccount->getWengoLogin() + "&password=" + wengoAccount->getWengoPassword()
				+ "&lang=" + "fr" + "&wl=" + "wengo" + "&page=softphone-web";
			_browser->setUrl(URL_WENGO_MINI_HOME + data);
		}
#endif
		break;

	case SipAccount::LoginStateConnected:
		break;

	case SipAccount::LoginStateDisconnected:
#ifdef OS_WINDOWS
		if( wengoAccount ) {
			_browser->setUrl(qApp->applicationDirPath().toStdString() + "/" + LOCAL_WEB_DIR + "/connecting_fr.htm");
		}
#endif
		break;

	case SipAccount::LoginStatePasswordError:
		break;

	case SipAccount::LoginStateNetworkError:
		break;

	default:
		LOG_FATAL("Unknown state");
	};
}

void QtWengoPhone::noAccountAvailableEventHandler(UserProfile & sender) {
	typedef PostEvent1<void (UserProfile & sender), UserProfile &> MyPostEvent;
	MyPostEvent * event = new MyPostEvent(boost::bind(&QtWengoPhone::noAccountAvailableEventHandlerThreadSafe, this, _1), sender);
	postEvent(event);
}

void QtWengoPhone::noAccountAvailableEventHandlerThreadSafe(UserProfile & sender) {
	showLoginWindow();
}

void QtWengoPhone::networkDiscoveryStateChangedEventHandler(SipAccount & sender, SipAccount::NetworkDiscoveryState state) {
	typedef PostEvent2<void (SipAccount &, SipAccount::NetworkDiscoveryState), SipAccount &, SipAccount::NetworkDiscoveryState> MyPostEvent;
	MyPostEvent * event = new MyPostEvent(boost::bind(&QtWengoPhone::networkDiscoveryStateChangedEventHandlerThreadSafe, this, _1, _2), sender, state);
	postEvent(event);
}

void QtWengoPhone::networkDiscoveryStateChangedEventHandlerThreadSafe(SipAccount & sender, SipAccount::NetworkDiscoveryState state) {
	QString tooltip;
	QString pixmap;

	switch (state) {
	case SipAccount::NetworkDiscoveryStateOk:
		tooltip = tr("Internet connection OK");
		pixmap = ":/pics/statusbar_connect.png";
		break;

	case SipAccount::NetworkDiscoveryStateHTTPError:
		tooltip = tr("Internet connection error");
		pixmap = ":/pics/statusbar_connect_error.png";
		break;

	case SipAccount::NetworkDiscoveryStateSIPError:
		tooltip = tr("Internet connection error");
		pixmap = ":/pics/statusbar_connect_error.png";
		break;

	case SipAccount::NetworkDiscoveryStateProxyNeedsAuthentication:
		tooltip = tr("Internet connection error");
		pixmap = ":/pics/statusbar_connect_error.png";
		break;

	case SipAccount::NetworkDiscoveryStateError:
		tooltip = tr("Internet connection error");
		pixmap = ":/pics/statusbar_connect_error.png";
		break;

	default:
		LOG_FATAL("unknown state=" + String::fromNumber(state));
	};

	_internetConnectionStateLabel->setPixmap(pixmap);
	_internetConnectionStateLabel->setToolTip(tooltip);
}

void QtWengoPhone::dialpad(const std::string & tone, const std::string & soundFile) {
	PhoneCall * phoneCall = _cWengoPhone.getActivePhoneCall();
	if (phoneCall) {
		if (soundFile.empty()) {
			if (tone == "0") { phoneCall->playTone(EnumTone::Tone0); }
			if (tone == "1") { phoneCall->playTone(EnumTone::Tone1); }
			if (tone == "2") { phoneCall->playTone(EnumTone::Tone2); }
			if (tone == "3") { phoneCall->playTone(EnumTone::Tone3); }
			if (tone == "4") { phoneCall->playTone(EnumTone::Tone4); }
			if (tone == "5") { phoneCall->playTone(EnumTone::Tone5); }
			if (tone == "6") { phoneCall->playTone(EnumTone::Tone6); }
			if (tone == "7") { phoneCall->playTone(EnumTone::Tone7); }
			if (tone == "8") { phoneCall->playTone(EnumTone::Tone8); }
			if (tone == "9") { phoneCall->playTone(EnumTone::Tone9); }
			if (tone == "*") { phoneCall->playTone(EnumTone::ToneStar); }
			if (tone == "#") { phoneCall->playTone(EnumTone::TonePound); }
		} else {
			phoneCall->playSoundFile(soundFile);
		}
	} else {
		_phoneComboBox->setEditText(_phoneComboBox->currentText() + QString::fromStdString(tone));
	}
}

void QtWengoPhone::callButtonClicked() {
	std::string phoneNumber = _phoneComboBox->currentText().toStdString();
	if (!phoneNumber.empty()) {
		_cWengoPhone.makeCall(phoneNumber);
	}
}

void QtWengoPhone::showWengoAccount() {
	_cWengoPhone.showWengoAccount();
}

void QtWengoPhone::openWengoAccount() {
	showLoginWindow();
}

void QtWengoPhone::editMyProfile(){
	QtEditMyProfile profile(_cWengoPhone.getWengoPhone(), _tabWidget);
	profile.exec();
}

void QtWengoPhone::exitApplication() {
	_wengoPhoneWindow->hide();
	_cWengoPhone.terminate();
	_trayIcon->hide();
	QCoreApplication::exit(EXIT_SUCCESS);
}

void QtWengoPhone::addContact() {
	QtAddContact * qtAddContact = new QtAddContact(_cWengoPhone, _wengoPhoneWindow);
	LOG_DEBUG("add contact");
}

void QtWengoPhone::actionSetLogin() {
	QtSetLogin * qtSetLogin = new QtSetLogin(_wengoPhoneWindow);

	if (qtSetLogin->exec()) {
		EnumIMProtocol::IMProtocol protocol;
		string selProtocol = qtSetLogin->getProtocol();
		string login = qtSetLogin->getLogin();
		string password = qtSetLogin->getPassword();

		if (selProtocol == "MSN") {
			protocol = EnumIMProtocol::IMProtocolMSN;
		} else if (selProtocol == "Yahoo") {
			protocol = EnumIMProtocol::IMProtocolYahoo;
		} else if ((selProtocol == "AIM") || (selProtocol == "ICQ")) {
			protocol = EnumIMProtocol::IMProtocolAIMICQ;
		} else if (selProtocol == "Jabber") {
			protocol = EnumIMProtocol::IMProtocolJabber;
			login += "/WengoPhone";
		}

		_cWengoPhone.addIMAccount(login, password, protocol);
		_cWengoPhone.getWengoPhone().getCurrentUserProfile().getConnectHandler().connect(IMAccount(login, password, protocol));
		LOG_DEBUG("set login");
	}
}

void QtWengoPhone::showConfig() {
	QtWengoConfigDialog dialog(_wengoPhoneWindow);
	dialog.exec();
}

void QtWengoPhone::showForum() {
	_cWengoPhone.showWengoForum();
}

void QtWengoPhone::showHelp() {
}

void QtWengoPhone::showFaq() {
	_cWengoPhone.showWengoFAQ();
}

void QtWengoPhone::showByOut(){
	_cWengoPhone.showWengoBuyWengos();
}
void QtWengoPhone::showCallOut(){
	_cWengoPhone.showWengoCallOut();
}

void QtWengoPhone::showSms() {
	_cWengoPhone.showWengoSMS();
}

void QtWengoPhone::showAbout() {
	static QtAbout * aboutWindow = new QtAbout(_wengoPhoneWindow);
	aboutWindow->getWidget()->show();
}

void QtWengoPhone::showVoiceMail(){
	_cWengoPhone.showWengoVoiceMail();
}

void QtWengoPhone::showHome() {
}

void QtWengoPhone::showContactList() {
}

void QtWengoPhone::showCallHistory() {
}

void QtWengoPhone::sendSms() {
	if (_qtSms) {
		_qtSms->getWidget()->show();
	}
}

void QtWengoPhone::showAdvancedConfig() {
	static QtAdvancedConfig * configWindow = new QtAdvancedConfig(_wengoPhoneWindow);

	configWindow->populate();
	configWindow->getWidget()->show();
}

void QtWengoPhone::showAccountSettings(){
	QtAccountManager accountManager(_cWengoPhone.getWengoPhone(),_wengoPhoneWindow);

	accountManager.exec();
}


//FIXME
#include <model/phonecall/ConferenceCall.h>
void QtWengoPhone::showCreateConferenceCall() {
	QDialog * conferenceDialog = qobject_cast<QDialog *>(WidgetFactory::create(":/forms/phonecall/ConferenceCallWidget.ui", _wengoPhoneWindow));
	QLineEdit * phoneNumber1LineEdit = Object::findChild<QLineEdit *>(conferenceDialog, "phoneNumber1LineEdit");
	QLineEdit * phoneNumber2LineEdit = Object::findChild<QLineEdit *>(conferenceDialog, "phoneNumber2LineEdit");

	int ret = conferenceDialog->exec();

	if (ret == QDialog::Accepted) {
		IPhoneLine * phoneLine = _cWengoPhone.getWengoPhone().getCurrentUserProfile().getActivePhoneLine();

		if (phoneLine != NULL) {
			ConferenceCall * confCall = new ConferenceCall(*phoneLine);
			confCall->addPhoneNumber(phoneNumber1LineEdit->text().toStdString());
			confCall->addPhoneNumber(phoneNumber2LineEdit->text().toStdString());
		} else {
			LOG_DEBUG("phoneLine is NULL");
		}
	}
}
//!FIXME


void QtWengoPhone::setTrayMenu() {
	QAction * action;

	if (!_trayMenu) {
		_trayMenu = new QMenu(_wengoPhoneWindow);
	}
	_trayMenu->clear();
	// Open the wengophone window
	_trayMenu->addAction(tr("Open Wengophone"));
	// Change status
	_trayMenu->addAction(tr("Status"));
	// Start a call session
	_trayMenu->addAction(tr("Call"));
	// Send  SMS
	action = _trayMenu->addAction(tr("Send a SMS"));
	connect (action,SIGNAL(triggered()),this,SLOT(sendSms()));
	// Start a chat
	_trayMenu->addAction(tr("Start a chat"));
	// Exit
	action = _trayMenu->addAction(tr("Quit Wengophone"));
	connect (action,SIGNAL(triggered()),this,SLOT(exitApplication()));

	_trayIcon->setPopup(_trayMenu);
}

void QtWengoPhone::wrongProxyAuthenticationEventHandler(SipAccount & sender,
		const std::string & proxyAddress, unsigned proxyPort,
		const std::string & proxyLogin, const std::string & proxyPassword) {

	typedef PostEvent3<void (SipAccount & sender, const std::string & proxyAddress, int proxyPort),
			SipAccount &, std::string, unsigned> MyPostEvent;
	MyPostEvent * event = new MyPostEvent(boost::bind(&QtWengoPhone::proxyNeedsAuthenticationEventHandlerThreadSafe, this, _1, _2, _3),
				sender, proxyAddress, proxyPort);
	postEvent(event);
}

void QtWengoPhone::proxyNeedsAuthenticationEventHandler(SipAccount & sender, const std::string & proxyAddress, unsigned proxyPort) {
	typedef PostEvent3<void (SipAccount & sender, const std::string & proxyAddress, int proxyPort),
			SipAccount &, std::string, unsigned> MyPostEvent;
	MyPostEvent * event = new MyPostEvent(boost::bind(&QtWengoPhone::proxyNeedsAuthenticationEventHandlerThreadSafe, this, _1, _2, _3),
				sender, proxyAddress, proxyPort);
	postEvent(event);
}

void QtWengoPhone::proxyNeedsAuthenticationEventHandlerThreadSafe(SipAccount & sender,
		const std::string & proxyAddress, unsigned proxyPort) {

	static QtHttpProxyLogin * httpProxy = new QtHttpProxyLogin(_wengoPhoneWindow, proxyAddress, proxyPort);

	int ret = httpProxy->exec();

	if (ret == QDialog::Accepted) {
		sender.setProxySettings(httpProxy->getProxyAddress(), httpProxy->getProxyPort(),
			httpProxy->getLogin(), httpProxy->getPassword());
	}
}

void QtWengoPhone::urlClickedEventHandler(std::string url) {
	LOG_DEBUG(url);

	//find anchor
	std::string anchor = "";
	int sharpPos = QString::fromStdString(url).indexOf('#');
	if( sharpPos != -1 ){
		anchor = QString::fromStdString(url).right(url.length() - sharpPos - 1).toStdString();
	}

	if( anchor == ANCHOR_CONTACTLIST ) {
		showContactList();
	}
	else if( anchor == ANCHOR_HISTORY ) {
		showCallHistory();
	}
	else if( anchor == ANCHOR_CONFIGURATION ) {
		showConfig();
	}
	else if( anchor == ANCHOR_DIALPAD ) {

	}
	else if( anchor == ANCHOR_ADDCONTACT ) {
		addContact();
	}
	else if( anchor == ANCHOR_SELFCARE ) {
		showWengoAccount();
	}
	else if( anchor == ANCHOR_FORUM ) {
		showForum();
	}
}
