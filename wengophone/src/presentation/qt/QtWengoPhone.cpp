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

#include "ui_WengoPhoneWindow.h"

#include <WengoPhoneBuildId.h>

#include <model/account/wengo/WengoAccount.h>
#include <model/connect/ConnectHandler.h>
#include <model/phonecall/PhoneCall.h>
#include <model/profile/UserProfile.h>
#include <model/history/History.h>
#include <model/phonecall/SipAddress.h>
#include <model/phonecall/ConferenceCall.h>
#include <model/config/ConfigManager.h>
#include <model/config/Config.h>
#include <model/phoneline/IPhoneLine.h>

#include <control/CWengoPhone.h>
#include <control/profile/CUserProfile.h>
#include <model/config/ConfigManager.h>
#include <model/config/Config.h>

#include <imwrapper/EnumIMProtocol.h>

#include "QtWengoPhoneEventFilter.h"
#include "QtWebcamButton.h"
#include "QtIdle.h"
#include "QtLanguage.h"
#include "statusbar/QtStatusBar.h"
#include "phoneline/QtPhoneLine.h"
#include "phonecall/QtPhoneCall.h"
#include "phonecall/QtContactCallListWidget.h"
#include "login/QtLogin.h"
#include "profile/QtProfileDetails.h"
#include "imaccount/QtIMAccountManager.h"
#include "contactlist/QtContactList.h"
#include "QtDialpad.h"
#include "QtAbout.h"
#include "QtConfigPanel.h"
#include "webservices/sms/QtSms.h"
#include "webservices/directory/QtWsDirectory.h"
#include "QtHttpProxyLogin.h"
#include "config/QtWengoConfigDialog.h"
#include "profilebar/QtProfileBar.h"
#include "history/QtHistoryWidget.h"
#include "toaster/QtToaster.h"
#include "callbar/QtCallBar.h"

#include <qtutil/WidgetFactory.h>
#include <qtutil/Widget.h>
#include <qtutil/Object.h>
#include <qtutil/MouseEventFilter.h>

#include <thread/Thread.h>
#include <util/Logger.h>
#include <QtBrowser.h>
#include <cutil/global.h>
#include <qtutil/QtWengoStyleLabel.h>

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
	: QObjectThreadSafe(NULL),
	_cWengoPhone(cWengoPhone) {

	_qtSms = NULL;
	_qtWsDirectory = NULL;
	_qtProfileBar = NULL;
	_activeTabBeforeCall = NULL;

	_cWengoPhone.loginStateChangedEvent +=
		boost::bind(&QtWengoPhone::loginStateChangedEventHandler, this, _1, _2);
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
	_wengoPhoneWindow = new QMainWindow(NULL);

	qRegisterMetaType<QVariant>("QVariant");

	_ui = new Ui::WengoPhoneWindow();
	_ui->setupUi(_wengoPhoneWindow);

    // Install the close event filter
    QtWengoPhoneEventFilter * qtWengoPhoneEventFilter;
    qtWengoPhoneEventFilter = new QtWengoPhoneEventFilter(this);
    _wengoPhoneWindow->installEventFilter(qtWengoPhoneEventFilter);
    if (!connect (qtWengoPhoneEventFilter,SIGNAL(closeWindow()),_wengoPhoneWindow, SLOT(hide()))){
        LOG_FATAL("Can't connect closeWindow() signal\n");
    }

	_chatWindow = NULL;

	_qtLogin = new QtLogin(_wengoPhoneWindow, *this);

	QGridLayout * callBarLayout = new QGridLayout(_ui->callBarFrame);
	_qtCallBar = new QtCallBar(_ui->callBarFrame);
	callBarLayout->addWidget(_qtCallBar);
	callBarLayout->setMargin(0);
	callBarLayout->setSpacing(0);

	//callButton
	_callButton = _qtCallBar->getCallButton();

	//hangUpButton
	_hangUpButton = _qtCallBar->getHangButton();

	//phoneComboBox
	_phoneComboBox = _qtCallBar->getComboBox();
	MousePressEventFilter * leftMouseFilter = new MousePressEventFilter(
		this, SLOT(phoneComboBoxClicked()), Qt::LeftButton);
	_phoneComboBox->installEventFilter(leftMouseFilter);

	//Add contact button
	connect(_ui->addContactButton, SIGNAL(clicked()), SLOT(addContact()));

	// Search button
	connect(_ui->findContactButton,SIGNAL(clicked()),SLOT(showSearchContactWindows()));

	// Send sms button
	connect(_ui->sendSmsButton,SIGNAL(clicked()),SLOT(sendSms()));

	// Start conference button
	connect(_ui->startConferenceButton,SIGNAL(clicked()),SLOT(showCreateConferenceCall()));

	// Open chat window button
	_ui->openChatWindowButton->setEnabled(false);
	connect ( _ui->openChatWindowButton,SIGNAL(clicked()),SLOT(showChatWindow()));

	// Search button
	connect(_ui->findContactButton,SIGNAL(clicked()),SLOT(showSearchContactWindows()));

	// Send sms button
	connect(_ui->sendSmsButton,SIGNAL(clicked()),SLOT(sendSms()));

	// Open chat window button
	_ui->openChatWindowButton->setEnabled(false);
	connect ( _ui->openChatWindowButton,SIGNAL(clicked()),SLOT(showChatWindow()));

/*
    // Disable call items in Action menu
    _ui->actionAccept->setEnabled(false);
    _ui->actionHangup->setEnabled(false);
    _ui->actionHold->setEnabled(false);
    _ui->actionResume->setEnabled(false);
*/
	//webcamButton
	new QtWebcamButton(_ui->webcamButton);

	//Buttons initialization
	initButtons();

	//Dialpad
	QtDialpad * qtDialpad = new QtDialpad(this);
	Widget::createLayout(_ui->tabDialpad)->addWidget(qtDialpad->getWidget());

	_qtHistoryWidget = NULL;

	// Create the profilebar
	_qtProfileBar = new QtProfileBar(_cWengoPhone,
		*_cWengoPhone.getCUserProfile(),
		_cWengoPhone.getCUserProfile()->getUserProfile().getConnectHandler(),
		_ui->profileBar);
	connect(this, SIGNAL(modelInitializedEventSignal()), _qtProfileBar, SLOT(userProfileUpdated()));

	//Add the profile bar
	int profileBarIndex = _ui->profileBar->addWidget(_qtProfileBar);
	_ui->profileBar->setCurrentIndex(profileBarIndex);
	_ui->profileBar->widget(profileBarIndex)->setLayout(new QGridLayout());

	//Systray
	_trayMenu = NULL;
	_trayIcon = new TrayIcon(QPixmap(":pics/status/online.png"), QString("Wengophone"), _trayMenu, _wengoPhoneWindow);
	setTrayMenu();
	_trayIcon->show();

	connect(_trayIcon, SIGNAL(doubleClicked(const QPoint &)), SLOT(sysTrayDoubleClicked(const QPoint &)));

	connect(_qtProfileBar, SIGNAL(myPresenceStatusEventSignal(QVariant )), SLOT(setSystrayIcon(QVariant )));

	//actionShowWengoAccount
	connect(_ui->actionShowWengoAccount, SIGNAL(triggered()), SLOT(showWengoAccount()));

	//actionOpenWengoAccount
	connect(_ui->actionOpenWengoAccount, SIGNAL(triggered()), SLOT(openWengoAccount()));

	//actionEditMyProfile
	connect(_ui->actionEditMyProfile, SIGNAL(triggered()), SLOT(editMyProfile()));

	//actionClose
	connect(_ui->actionClose, SIGNAL(triggered()),_wengoPhoneWindow, SLOT(hide()));

	//actionAddContact
	connect(_ui->actionAddContact, SIGNAL(triggered()), SLOT(addContact()));

	//actionConfiguration
	connect(_ui->actionConfiguration, SIGNAL(triggered()), SLOT(showConfig()));

	//actionShowForum
	connect(_ui->actionShowForum, SIGNAL(_ui->actionAccept), SLOT(showForum()));

	//actionShowHelp
	/*
	connect(_ui->actionShowHelp, SIGNAL(triggered()), SLOT(showHelp()));
	*/

	//actionShowAbout
	connect(_ui->actionShowAbout, SIGNAL(triggered()), SLOT(showAbout()));

	//actionSendSms
	connect(_ui->actionSendSms, SIGNAL(triggered()), SLOT(sendSms()));

	//actionAdvancedConfiguration
	connect(_ui->actionAdvancedConfiguration, SIGNAL(triggered()), SLOT(showAdvancedConfig()));

	//actionFaq
	connect(_ui->actionFaq, SIGNAL (triggered()), SLOT(showFaq()));

	//actionBuyCallOutCredits
	connect(_ui->actionBuyCallOutCredits, SIGNAL(triggered()), SLOT(showBuyOut()));

	//actionCallOutService
	connect(_ui->actionCallOutService, SIGNAL(triggered()), SLOT (showCallOut()));

	//actionSms
	//connect(_ui->actionSms, SIGNAL(triggered()), SLOT (showSms()));

	//actionVoiceMail
	//connect(_ui->actionVoiceMail, SIGNAL(triggered()), SLOT (showVoiceMail()));

	//actionIM_Account_Settings
	connect(_ui->actionIMAccountSettings, SIGNAL(triggered()), SLOT(showAccountSettings()));

	//actionShow_Hide_contacts_offline
	connect(_ui->actionHideContactsOffline, SIGNAL(triggered()), SLOT(showHideOffLineContacts()));

	//actionCreateConferenceCall
	connect(_ui->actionCreateConferenceCall, SIGNAL(triggered()), SLOT(showCreateConferenceCall()));

	//actionOutgoing_Calls
	connect(_ui->actionOutgoingCalls, SIGNAL(triggered()), SLOT(eraseHistoryOutgoingCalls()));

	//actionIncoming_Calls
	connect(_ui->actionIncomingCalls, SIGNAL(triggered()), SLOT(eraseHistoryIncomingCalls()));

	//actionMissed_Calls
	connect(_ui->actionMissedCalls, SIGNAL(triggered()), SLOT(eraseHistoryMissedCalls()));

	//actionChat_Sessions
	connect(_ui->actionChatSessions, SIGNAL(triggered()), SLOT(eraseHistoryChatSessions()));

	//actionSMS
	connect(_ui->actionSMS, SIGNAL(triggered()), SLOT(eraseHistorySms()));

	//actionAll
	connect(_ui->actionAll, SIGNAL(triggered()), SLOT(eraseHistory()));

	//actionSearchContact
	connect(_ui->actionSearchWengoUsers, SIGNAL(triggered()), SLOT(showSearchContactWindows()));

	//actionLog_off
	connect(_ui->actionLog_off, SIGNAL(triggered()), SLOT(logoff()));

	// Tab selection
	connect(_ui->tabWidget,SIGNAL(currentChanged (int)),SLOT(tabSelectionChanged(int)));

	// Accept a call
	connect (_ui->actionAccept,SIGNAL(triggered()),SLOT(acceptCall()));

    // Resume a call
    connect (_ui->actionHold_Resume,SIGNAL(triggered()),SLOT(resumeCall()));

	// Hangup a call
    connect (_ui->actionHangup,SIGNAL(triggered()),SLOT(hangupCall()));

	connect(this, SIGNAL(connectionStatusEventHandlerSignal(int, int, QString)),
		SLOT(connectionStatusEventHandlerSlot(int, int, QString)));

	//Translation
	new QtLanguage(_wengoPhoneWindow);

	Config & config = ConfigManager::getInstance().getCurrentConfig();

#if (defined OS_WINDOWS) && (defined QT_COMMERCIAL)
	if (config.getIEActiveX()) {
		//Embedded Browser
		_browser = new QtBrowser(NULL);
		_browser->urlClickedEvent += boost::bind(&QtWengoPhone::urlClickedEventHandler, this, _1);
		_ui->tabWidget->insertTab(_ui->tabWidget->count(), (QWidget*) _browser->getWidget(), tr("Home"));
		_browser->setUrl(qApp->applicationDirPath().toStdString() + "/" + LOCAL_WEB_DIR + "/connecting_fr.htm");
		_ui->tabWidget->setCurrentWidget((QWidget*) _browser->getWidget());
	}
#endif

	//Idle detection
	new QtIdle(_cWengoPhone.getCUserProfile()->getUserProfile(), _wengoPhoneWindow);

	//configPanel
	QtConfigPanel * qtConfigPanel = new QtConfigPanel(_cWengoPhone, _wengoPhoneWindow);
	_configPanelWidget = qtConfigPanel->getWidget();
	int configPanelIndex = _ui->configPanel->addWidget(_configPanelWidget);
	_ui->configPanel->setCurrentIndex(configPanelIndex);
	_ui->configPanel->hide();

	MousePressEventFilter * mousePressEventFilter = new MousePressEventFilter(this, SLOT(expandConfigPanel()));
	_ui->configPanelLabel->installEventFilter(mousePressEventFilter);

	//Status bar
	_statusBar = new QtStatusBar(_cWengoPhone, _ui->statusBar);

	//FIXME: can i create the widget here ?
//	setPhoneCall(new QtContactCallListWidget(_cWengoPhone,(_wengoPhoneWindow)));

	updatePresentation();
	_wengoPhoneWindow->show();
}

void QtWengoPhone::initButtons() {
	//callButton
	connect(_qtCallBar, SIGNAL(ButtonClicked()), SLOT(callButtonClicked()));
	connect(_qtCallBar, SIGNAL(OffClicked()), SLOT(hangupButtonClicked()));
	enableCallButton();

	//hangUpButton
	_hangUpButton->setEnabled(false);

	//phoneComboBox
	connect(_phoneComboBox, SIGNAL(activated(int)), SLOT(callButtonClicked()));
	connect(_phoneComboBox, SIGNAL(editTextChanged(const QString &)), SLOT(enableCallButton()));
}

void QtWengoPhone::enableCallButton() {
	std::string phoneNumber = _phoneComboBox->currentText().toStdString();
	_callButton->setEnabled(!phoneNumber.empty());
	//_videoCallButton->setEnabled(!phoneNumber.empty());
}

void QtWengoPhone::hangupButtonClicked(){
	QtContactCallListWidget * widget = dynamic_cast<QtContactCallListWidget *>(_ui->tabWidget->currentWidget());
	if ( widget ){
		widget->hangup();
		//Widget is deleted automagically
        //set the last active page
        if( _activeTabBeforeCall ) {
            _ui->tabWidget->setCurrentWidget(_activeTabBeforeCall);
        } else {
            _ui->tabWidget->setCurrentIndex(0);
        }
        return;
	}
    for (int i = 0; i < _ui->tabWidget->count(); i++){
		widget = dynamic_cast<QtContactCallListWidget *>(_ui->tabWidget->widget(i));
		if ( widget ){
		    widget->hangup();
            //set the last active page
            if( _activeTabBeforeCall ) {
                _ui->tabWidget->setCurrentWidget(_activeTabBeforeCall);
            } else {
                _ui->tabWidget->setCurrentIndex(0);
            }
            return;
		}
    }
}

int QtWengoPhone::findFirstCallTab(){
    QtContactCallListWidget * widget;
    for (int i = 0; i < _ui->tabWidget->count(); i++){
        widget = dynamic_cast<QtContactCallListWidget *>(_ui->tabWidget->widget(i));
        if ( widget ){
            return i;
        }
    }
    return -1;
}

void QtWengoPhone::callButtonClicked() {
	std::string phoneNumber = _phoneComboBox->currentText().toStdString();
	if (!phoneNumber.empty()) {
		_cWengoPhone.getCUserProfile()->makeCall(phoneNumber);
	}
	_phoneComboBox->clearEditText();
}

void QtWengoPhone::addPhoneCall(QtPhoneCall * qtPhoneCall) {

	_activeTabBeforeCall = _ui->tabWidget->currentWidget();

	QtContactCallListWidget * qtContactCallListWidget = new QtContactCallListWidget(_cWengoPhone,_wengoPhoneWindow);
	_ui->tabWidget->addTab(qtContactCallListWidget,tr("Call"));
	_ui->tabWidget->setCurrentWidget(qtContactCallListWidget);
	qtContactCallListWidget->addPhoneCall(qtPhoneCall);

	connect ( qtContactCallListWidget, SIGNAL(startConferenceSignal(PhoneCall *, PhoneCall *)),
	          SLOT(addToConference(PhoneCall *, PhoneCall *)));

	_hangUpButton->setEnabled(true);
}

void QtWengoPhone::addToConference(QString phoneNumber, PhoneCall * targetCall){
	QtContactCallListWidget * qtContactCallListWidget;

	int nbtab = _ui->tabWidget->count();

	for ( int i = 0; i < nbtab; i++){
		if ( _ui->tabWidget->tabText(i) == QString(tr("Conference"))){
			return;
		}

	for (int i = 0; i < _ui->tabWidget->count(); i++){
		QtContactCallListWidget * qtContactCallListWidget = dynamic_cast<QtContactCallListWidget *>(_ui->tabWidget->widget(i));
		if ( qtContactCallListWidget ){
				if ( qtContactCallListWidget->hasPhoneCall( targetCall) ){
					_ui->tabWidget->setTabText(i,tr("Conference"));
					IPhoneLine * phoneLine = _cWengoPhone.getWengoPhone().getCurrentUserProfile().getActivePhoneLine();

					if (phoneLine != NULL) {
						ConferenceCall * confCall = new ConferenceCall(*phoneLine);
						confCall->addPhoneCall(*targetCall);
						confCall->addPhoneNumber(phoneNumber.toStdString());
					} else {
						LOG_DEBUG("phoneLine is NULL");
					}
				}
			}
		}
	}
}

void QtWengoPhone::logoff(){
	if (_cWengoPhone.getCUserProfile()) {
		_cWengoPhone.getCUserProfile()->disconnect();
	}
}


void QtWengoPhone::addToConference(PhoneCall * sourceCall, PhoneCall * targetCall){
	// Bad and Ugly but works...

	QtContactCallListWidget * qtContactCallListWidget;

	int nbtab = _ui->tabWidget->count();

	for ( int i = 0; i < nbtab; i++){
		if ( _ui->tabWidget->tabText(i) == QString(tr("Conference"))){
			return;
		}
	}

	for (int i = 0; i < _ui->tabWidget->count(); i++){
		QtContactCallListWidget * qtContactCallListWidget = dynamic_cast<QtContactCallListWidget *>(_ui->tabWidget->widget(i));
		if ( qtContactCallListWidget ){
			if ( qtContactCallListWidget->hasPhoneCall( sourceCall ) ){
				_ui->tabWidget->setTabText(i,tr("Conference"));
				IPhoneLine * phoneLine = _cWengoPhone.getWengoPhone().getCurrentUserProfile().getActivePhoneLine();
				if (phoneLine != NULL) {
					ConferenceCall * confCall = new ConferenceCall(*phoneLine);
					confCall->addPhoneCall(*targetCall);
					confCall->addPhoneCall(*sourceCall);
					// Add the target to source and remove the target tab
					for (int j = 0; j < _ui->tabWidget->count(); j++){
						QtContactCallListWidget * toRemove =
							dynamic_cast<QtContactCallListWidget *>(_ui->tabWidget->widget(j));
						if ( toRemove )
						{
							if ( toRemove->hasPhoneCall(targetCall) ){
								QtPhoneCall * qtPhoneCall = toRemove->takeQtPhoneCall(targetCall);
								if ( qtPhoneCall ){
									toRemove->close();
									qtContactCallListWidget->addPhoneCall(qtPhoneCall);
									break;
								}

							}
						}

					}

				}else {
					LOG_DEBUG("phoneLine is NULL");
				}
				break;
			}
		}
	}
}

void QtWengoPhone::addToConference(QtPhoneCall * qtPhoneCall){

	QtContactCallListWidget * qtContactCallListWidget;

	int nbtab = _ui->tabWidget->count();

	for ( int i = 0; i < nbtab; i++){
		if ( _ui->tabWidget->tabText(i) == QString(tr("Conference"))){
			// i is the index of the conference tab
			qtContactCallListWidget = dynamic_cast<QtContactCallListWidget *>(_ui->tabWidget->widget(i));
			qtContactCallListWidget->addPhoneCall(qtPhoneCall);
			_ui->tabWidget->setCurrentWidget(qtContactCallListWidget);
			return;
		}
	}

	_activeTabBeforeCall = _ui->tabWidget->currentWidget();

	// conference tab not found, create a new one
	qtContactCallListWidget = new QtContactCallListWidget(_cWengoPhone,_wengoPhoneWindow);
	_ui->tabWidget->addTab(qtContactCallListWidget,tr("Conference"));
	_ui->tabWidget->setCurrentWidget(qtContactCallListWidget);
	qtContactCallListWidget->addPhoneCall(qtPhoneCall);
	_hangUpButton->setEnabled(true);
}

void QtWengoPhone::showLoginWindow() {
	int ret = _qtLogin->show();

	if (ret == QDialog::Accepted) {
		_cWengoPhone.addWengoAccount(_qtLogin->getLogin(), _qtLogin->getPassword(), _qtLogin->hasAutoLogin());
	}
}

void QtWengoPhone::setContactList(QtContactList * qtContactList) {
	Widget::createLayout(_ui->tabContactList)->addWidget(qtContactList->getWidget());
	_contactList = qtContactList;
	LOG_DEBUG("QtContactList added");
}

void QtWengoPhone::setHistory(QtHistoryWidget * qtHistoryWidget) {
	Widget::createLayout(_ui->tabHistory)->addWidget(qtHistoryWidget);
	_qtHistoryWidget = qtHistoryWidget;
}

void QtWengoPhone::setPhoneCall(QtContactCallListWidget * qtContactCallListWidget) {
}

void QtWengoPhone::setSms(QtSms * qtSms) {
	_qtSms = qtSms;
}

void QtWengoPhone::setWsDirectory(QtWsDirectory * qtWsDirectory) {
	_qtWsDirectory = qtWsDirectory;
}

QtSms * QtWengoPhone::getSms() const {
	return _qtSms;
}

QtWsDirectory * QtWengoPhone::getQtWsDirectory() const {
	return _qtWsDirectory;
}

QtLogin * QtWengoPhone::getLogin() const {
	return _qtLogin;
}

QtStatusBar & QtWengoPhone::getStatusBar() const {
	return *_statusBar;
}

void QtWengoPhone::setSubscribe(QtSubscribe * qtSubscribe) {
	_qtSubscribe = qtSubscribe;
}

QtSubscribe * QtWengoPhone::getSubscribe() const {
	return _qtSubscribe;
}

void QtWengoPhone::updatePresentation() {
	typedef PostEvent0<void ()> MyPostEvent;
	MyPostEvent * event = new MyPostEvent(boost::bind(&QtWengoPhone::updatePresentationThreadSafe, this));
	postEvent(event);
}

void QtWengoPhone::updatePresentationThreadSafe() {
	if (_cWengoPhone.getCUserProfile()) {
		//disabled some actions if no WengoAccount is used
		bool hasWengoAccount = _cWengoPhone.getCUserProfile()->getUserProfile().hasWengoAccount();

		_ui->actionShowWengoAccount->setEnabled(hasWengoAccount);
		_ui->actionSendSms->setEnabled(hasWengoAccount);
		_ui->actionCreateConferenceCall->setEnabled(hasWengoAccount);
		_ui->actionSearchWengoUsers->setEnabled(hasWengoAccount);
	}
}

void QtWengoPhone::loginStateChangedEventHandler(SipAccount & sender, SipAccount::LoginState state) {
	typedef PostEvent2<void (SipAccount & sender, SipAccount::LoginState), SipAccount &, SipAccount::LoginState> MyPostEvent;
	MyPostEvent * event = new MyPostEvent(boost::bind(&QtWengoPhone::loginStateChangedEventHandlerThreadSafe, this, _1, _2), sender, state);
	postEvent(event);
}

void QtWengoPhone::loginStateChangedEventHandlerThreadSafe(SipAccount & sender, SipAccount::LoginState state) {
#ifdef OS_WINDOWS
	const WengoAccount * wengoAccount = dynamic_cast<const WengoAccount *>(&sender);
#endif

	Config & config = ConfigManager::getInstance().getCurrentConfig();

	switch (state) {
	case SipAccount::LoginStateReady:
#ifdef OS_WINDOWS
		if (config.getIEActiveX() && wengoAccount) {
			//TODO: retrive the lang code from the current language
			std::string data = "?login=" + wengoAccount->getWengoLogin() + "&password=" + wengoAccount->getWengoPassword()
				+ "&lang=" + "eng" + "&wl=" + std::string(WengoPhoneBuildId::SOFTPHONE_NAME) + "&page=softphone-web";
			_browser->setUrl(URL_WENGO_MINI_HOME + data);
		}
#endif
	break;

	case SipAccount::LoginStateConnected:
	break;

	case SipAccount::LoginStateDisconnected:
#ifdef OS_WINDOWS
		if (config.getIEActiveX() && wengoAccount) {
			_browser->setUrl(qApp->applicationDirPath().toStdString() + "/" + LOCAL_WEB_DIR + "/connecting_fr.htm");
		}
#endif
		break;

	case SipAccount::LoginStatePasswordError:
		QMessageBox::warning(
			_wengoPhoneWindow,
			tr("Login error"),
			tr("Login and/or password incorrect"),
			tr("&Ok"));
		break;

	case SipAccount::LoginStateNetworkError:
		break;

	default:
		LOG_FATAL("unknown state=" + String::fromNumber(state));
	}

	updatePresentation();
}

void QtWengoPhone::noAccountAvailableEventHandler(UserProfile & sender) {
	typedef PostEvent1<void (UserProfile & sender), UserProfile &> MyPostEvent;
	MyPostEvent * event = new MyPostEvent(boost::bind(&QtWengoPhone::noAccountAvailableEventHandlerThreadSafe, this, _1), sender);
	postEvent(event);
}

void QtWengoPhone::noAccountAvailableEventHandlerThreadSafe(UserProfile & sender) {
	showLoginWindow();
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

void QtWengoPhone::showWengoAccount() {
	_cWengoPhone.showWengoAccount();
}

void QtWengoPhone::openWengoAccount() {
	showLoginWindow();
}

void QtWengoPhone::editMyProfile() {
	if (_cWengoPhone.getCUserProfile()) {
		//FIXME: this method should not be called if no UserProfile has been set
		QtProfileDetails qtProfileDetails(_cWengoPhone,
			_cWengoPhone.getCUserProfile()->getUserProfile(), _wengoPhoneWindow);
		//TODO: UserProfile must be updated if QtProfileDetails was accepted
		qtProfileDetails.show();

		LOG_DEBUG("edit user profile");
	}
}

void QtWengoPhone::exitApplication() {
	_wengoPhoneWindow->hide();
	_cWengoPhone.terminate();
	_trayIcon->hide();
	QCoreApplication::exit(EXIT_SUCCESS);
}

void QtWengoPhone::addContact() {
	if (_cWengoPhone.getCUserProfile()) {
		//FIXME: this method should not be called if no UserProfile has been set
		LOG_DEBUG("add contact");
		ContactProfile (contactProfile);
		QtProfileDetails qtProfileDetails(_cWengoPhone,
			contactProfile, _wengoPhoneWindow);
		if (qtProfileDetails.show()) {
			_cWengoPhone.getCUserProfile()->getCContactList().addContact(contactProfile);
		}
	}
}

void QtWengoPhone::showConfig() {
	QtWengoConfigDialog dialog(_cWengoPhone, _wengoPhoneWindow);
	dialog.show();
}

void QtWengoPhone::showForum() {
	_cWengoPhone.showWengoForum();
}

void QtWengoPhone::showHelp() {
}

void QtWengoPhone::showFaq() {
	_cWengoPhone.showWengoFAQ();
}

void QtWengoPhone::showBuyOut() {
	_cWengoPhone.showWengoBuyWengos();
}
void QtWengoPhone::showCallOut() {
	_cWengoPhone.showWengoCallOut();
}

void QtWengoPhone::showSms() {
	_cWengoPhone.showWengoSMS();
}

void QtWengoPhone::showSearchContactWindows() {
	if( _qtWsDirectory ) {
		_qtWsDirectory->show();
	}
}

void QtWengoPhone::showAbout() {
	static QtAbout * aboutWindow = new QtAbout(_wengoPhoneWindow);
	aboutWindow->getWidget()->show();
}

void QtWengoPhone::showVoiceMail() {
	_cWengoPhone.showWengoVoiceMail();
}

void QtWengoPhone::showHideOffLineContacts() {
	static bool hidden = false;

	if (hidden) {
		_contactList->showAllUsers();
	} else {
		_contactList->hideOffLineUser();
	}
	hidden = !hidden;
}

void QtWengoPhone::showHome() {
}

void QtWengoPhone::showContactList() {
}

void QtWengoPhone::sendSms() {
	if (_qtSms) {
		_qtSms->getWidget()->show();
	}
}

void QtWengoPhone::showAccountSettings() {
	QtIMAccountManager imAccountManager(_cWengoPhone.getCUserProfile()->getUserProfile(),
		_cWengoPhone, true, _wengoPhoneWindow);
}


//FIXME
void QtWengoPhone::showCreateConferenceCall() {
	QDialog * conferenceDialog = qobject_cast<QDialog *>(WidgetFactory::create(":/forms/phonecall/ConferenceCallWidget.ui", _wengoPhoneWindow));

	int ret = conferenceDialog->exec();

    QLineEdit * phoneNumber1LineEdit;
    QLineEdit * phoneNumber2LineEdit;

    phoneNumber1LineEdit = Object::findChild<QLineEdit *>(conferenceDialog,"phoneNumber1LineEdit");
    phoneNumber2LineEdit = Object::findChild<QLineEdit *>(conferenceDialog,"phoneNumber2LineEdit");

	if (ret == QDialog::Accepted) {
		IPhoneLine * phoneLine = _cWengoPhone.getCUserProfile()->getUserProfile().getActivePhoneLine();

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
	action = _trayMenu->addAction(tr("Open Wengophone"));
	connect ( action,SIGNAL(triggered()),_wengoPhoneWindow,SLOT(show()));
	// Change status
	//_trayMenu->addAction(tr("Status"));
	_trayMenu->addMenu(createStatusMenu());
	// Start a call session
	action = _trayMenu->addAction(QIcon(":/pics/contact/call.png"),tr("Call"));
	// Send  SMS
	action = _trayMenu->addAction(QIcon(":/pics/contact/sms.png"),tr("Send a SMS"));
	connect (action,SIGNAL(triggered()),this,SLOT(sendSms()));
	// Start a chat
	_trayMenu->addAction(QIcon(":/pics/contact/chat.png"),tr("Start a chat"));
	// Exit
	action = _trayMenu->addAction(tr("Quit Wengophone"));
	connect (action,SIGNAL(triggered()),this,SLOT(exitApplication()));

	_trayIcon->setPopup(_trayMenu);
}

void QtWengoPhone::wrongProxyAuthenticationEventHandler(SipAccount & sender,
		const std::string & proxyAddress, unsigned proxyPort,
		const std::string & proxyLogin, const std::string & proxyPassword) {

	typedef PostEvent3<void (SipAccount & sender, const std::string & proxyAddress, unsigned proxyPort),
			SipAccount &, std::string, unsigned> MyPostEvent;
	MyPostEvent * event = new MyPostEvent(boost::bind(&QtWengoPhone::proxyNeedsAuthenticationEventHandlerThreadSafe, this, _1, _2, _3),
				sender, proxyAddress, proxyPort);
	postEvent(event);
	QCoreApplication::processEvents();
}

void QtWengoPhone::proxyNeedsAuthenticationEventHandler(SipAccount & sender, const std::string & proxyAddress, unsigned proxyPort) {
	typedef PostEvent3<void (SipAccount & sender, const std::string & proxyAddress, unsigned proxyPort),
			SipAccount &, std::string, unsigned> MyPostEvent;
	MyPostEvent * event = new MyPostEvent(boost::bind(&QtWengoPhone::proxyNeedsAuthenticationEventHandlerThreadSafe, this, _1, _2, _3),
				sender, proxyAddress, proxyPort);
	postEvent(event);
	QCoreApplication::processEvents();
}

void QtWengoPhone::proxyNeedsAuthenticationEventHandlerThreadSafe(SipAccount & sender,
		const std::string & proxyAddress, unsigned proxyPort) {

	static QtHttpProxyLogin * httpProxy = new QtHttpProxyLogin(_wengoPhoneWindow, proxyAddress, proxyPort);

	int ret = httpProxy->show();

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
		showHistory();
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

void QtWengoPhone::toasterClosed(QtToaster * toaster) {
	delete toaster;
}

void QtWengoPhone::expandConfigPanel() {
	static bool expand = true;

	if (expand) {
		_wengoPhoneWindow->resize(_wengoPhoneWindow->width(), _wengoPhoneWindow->height() + _configPanelWidget->height());
		_ui->configPanel->show();
	} else {
		_ui->configPanel->hide();
		_wengoPhoneWindow->resize(_wengoPhoneWindow->width(), _wengoPhoneWindow->height() - _configPanelWidget->height());
	}
	expand = !expand;
}

void QtWengoPhone::authorizationRequestEventHandler(PresenceHandler & sender, const IMContact & imContact,
	const std::string & message) {

	typedef PostEvent3<void (PresenceHandler & sender, const IMContact & imContact, const std::string & message),
		PresenceHandler &, IMContact, std::string> MyPostEvent;

	MyPostEvent * event = new MyPostEvent(boost::bind(&QtWengoPhone::authorizationRequestEventHandlerThreadSafe, this, _1, _2, _3),
		sender, imContact, message);

	postEvent(event);
}

void QtWengoPhone::authorizationRequestEventHandlerThreadSafe(PresenceHandler & sender, const IMContact & imContact,
	const std::string & message) {

	QString request = QString("%1 (from %2) wants to see the presence state of %3.\n")
		.arg(QString::fromStdString(imContact.getContactId()))
		.arg(QString::fromStdString(EnumIMProtocol::toString(imContact.getProtocol())))
		.arg(QString::fromStdString(imContact.getIMAccount()->getLogin()));

	if (!message.empty()) {
		request += QString("<i>%1</i>\n").arg(QString::fromStdString(message));
	}

	int buttonClicked = QMessageBox::question(_wengoPhoneWindow,
		tr("WengoPhone - Authorization request"), request,
		tr("&Authorize"), tr("&Block"));

	if (buttonClicked == 0) {
		//TODO: give a personal message
		sender.authorizeContact(imContact, true, "");

		ContactProfile contactProfile;
		contactProfile.addIMContact(imContact);
		QtProfileDetails qtProfileDetails(_cWengoPhone, contactProfile, _wengoPhoneWindow);
		if (qtProfileDetails.show()) {
			_cWengoPhone.getCUserProfile()->getCContactList().addContact(contactProfile);
		}
	} else {
		//TODO: give a personal message
		sender.authorizeContact(imContact, false, "");
	}

	//TODO: Ask if the user wants to add this contact to his contact list
}

void QtWengoPhone::eraseHistoryOutgoingCalls() {
	_cWengoPhone.getCHistory().clearOutgoingCallEntries();
}

void QtWengoPhone::eraseHistoryIncomingCalls() {
	_cWengoPhone.getCHistory().clearIncomingCallEntries();
}

void QtWengoPhone::eraseHistoryMissedCalls() {
	_cWengoPhone.getCHistory().clearMissedCallEntries();
}

void QtWengoPhone::eraseHistoryChatSessions() {
	_cWengoPhone.getCHistory().clearChatEntries();
}

void QtWengoPhone::eraseHistorySms() {
	_cWengoPhone.getCHistory().clearSmsEntries();
}

void QtWengoPhone::eraseHistory() {
	_cWengoPhone.getCHistory().clearAllEntries();
}

void QtWengoPhone::phoneComboBoxClicked() {
	//if _qtHistoryWidget is set it means that History has been created
	if( _qtHistoryWidget ) {
		HistoryMementoCollection * mementos = _cWengoPhone.getCHistory().getMementos(HistoryMemento::OutgoingCall, 10);
		_phoneComboBox->clear();
		_phoneComboBox->clearEditText();
		for(HistoryMap::iterator it = mementos->begin(); it != mementos->end(); it++ ) {
			HistoryMemento * memento = (*it).second;
			SipAddress sipAddress(memento->getPeer());

			switch(memento->getState()) {
				case HistoryMemento::IncomingCall:
					_phoneComboBox->addItem(QIcon(QPixmap(":/pics/history/call_incoming.png")),
						QString::fromStdString(sipAddress.getUserName()));
					break;
				case HistoryMemento::OutgoingCall:
					_phoneComboBox->addItem(QIcon(QPixmap(":/pics/history/call_outgoing.png")),
						QString::fromStdString(sipAddress.getUserName()));
					break;
				case HistoryMemento::MissedCall:
					_phoneComboBox->addItem(QIcon(QPixmap(":/pics/history/call_missed.png")),
						QString::fromStdString(sipAddress.getUserName()));
					break;
				case HistoryMemento::RejectedCall:
					_phoneComboBox->addItem(QIcon(QPixmap(":/pics/history/call_missed.png")),
						QString::fromStdString(sipAddress.getUserName()));
					break;
				case HistoryMemento::OutgoingSmsOk:
					_phoneComboBox->addItem(QIcon(QPixmap(":/pics/history/sms_send.png")),
						QString::fromStdString(sipAddress.getUserName()));
					break;
				case HistoryMemento::OutgoingSmsNok:
					break;
				case HistoryMemento::ChatSession:
					break;
				case HistoryMemento::None:
					break;
				case HistoryMemento::Any:
					break;
				default:
					LOG_FATAL("Unknown HistoryMemento::state");
			}
		}
	}
}

void QtWengoPhone::modelInitializedEvent() {
	modelInitializedEventSignal();
}

QMenu * QtWengoPhone::createStatusMenu(){

   QMenu * menu = new QMenu(tr("Status"));

   switch ( _cWengoPhone.getCUserProfile()->getUserProfile().getPresenceState() ){
        case EnumPresenceState::PresenceStateAway:
            menu->setIcon ( QIcon(":/pics/status/away.png"));
            break;
        case EnumPresenceState::PresenceStateOnline:
            menu->setIcon ( QIcon(":/pics/status/online.png"));
            break;
        case EnumPresenceState::PresenceStateInvisible:
            menu->setIcon ( QIcon(":/pics/status/offline.png"));
            break;
        case EnumPresenceState::PresenceStateDoNotDisturb:
            menu->setIcon ( QIcon(":/pics/status/donotdisturb.png"));
            break;
        default:
            menu->setIcon ( QIcon(":/pics/status/online.png"));
            break;
    }

	QAction * action;

	action = menu->addAction(QIcon(":/pics/status/online.png"),tr( "Online" ) );
	connect(action,SIGNAL( triggered (bool) ),_qtProfileBar, SLOT( onlineClicked(bool)) );

	action = menu->addAction(QIcon(":/pics/status/donotdisturb.png"), tr( "DND" ) );
	connect(action,SIGNAL( triggered (bool) ),_qtProfileBar,SLOT( dndClicked(bool) ) );

	action = menu->addAction(QIcon(":/pics/status/offline.png"), tr( "Invisible" ) );
	connect(action,SIGNAL( triggered (bool) ),_qtProfileBar,SLOT( invisibleClicked(bool) ) );

	action = menu->addAction(QIcon(":/pics/status/away.png"), tr( "Away" ) );
	connect(action,SIGNAL( triggered (bool) ),_qtProfileBar,SLOT( awayClicked(bool) ) );

    return menu;
}

void QtWengoPhone::setSystrayIcon(QVariant status){


    if ( status.toInt() == (int)EnumPresenceState::MyPresenceStatusOk)

    switch ( _cWengoPhone.getCUserProfile()->getUserProfile().getPresenceState() ){
        case EnumPresenceState::PresenceStateAway:
            _trayIcon->setIcon(QPixmap(":/pics/status/away.png"));
            break;
        case EnumPresenceState::PresenceStateOnline:
            _trayIcon->setIcon(QPixmap(":/pics/status/online.png"));
            break;
        case EnumPresenceState::PresenceStateInvisible:
            _trayIcon->setIcon(QPixmap(":/pics/status/offline.png"));
            break;
        case EnumPresenceState::PresenceStateDoNotDisturb:
            _trayIcon->setIcon(QPixmap(":/pics/status/donotdisturb.png"));
            break;
        default:
            LOG_DEBUG("Change presence state display to -- Not yet handled\n");
            break;
    }
    setTrayMenu();
}

void QtWengoPhone::sysTrayDoubleClicked(const QPoint& ){

    if ( _wengoPhoneWindow->isVisible () )
        _wengoPhoneWindow->setVisible(false);
    else
        _wengoPhoneWindow->setVisible(true);
}

void QtWengoPhone::setChatWindow(QWidget * chatWindow){
    if ( chatWindow == NULL){
        _chatWindow = NULL;
        _ui->openChatWindowButton->setEnabled(false);
    }
    else
    {
        _chatWindow = chatWindow;
        _ui->openChatWindowButton->setEnabled(true);
    }

}

void QtWengoPhone::showChatWindow(){
 if ( _chatWindow )
    _chatWindow->setVisible(true);
}

void QtWengoPhone::showHistory() {
	_ui->tabWidget->setCurrentWidget(_ui->tabHistory);
}

void QtWengoPhone::connectionStatusEventHandler(int totalSteps, int curStep, const std::string & infoMsg) {
	connectionStatusEventHandlerSignal(totalSteps, curStep, QString::fromStdString(infoMsg));
}

void QtWengoPhone::connectionStatusEventHandlerSlot(int totalSteps, int curStep, QString infoMsg) {

	//int buttonClicked;

	//if (totalSteps == 0 && curStep == 0)
	//	buttonClicked = QMessageBox::information(_wengoPhoneWindow, tr("WengoPhone - Network information"),
	//		infoMsg, QMessageBox::Ok);
}

void QtWengoPhone::tabSelectionChanged(int index){

}

void QtWengoPhone::acceptCall(){
    int callIndex = findFirstCallTab();
    if (callIndex == -1 )
        return;
    QtContactCallListWidget * widget;

    widget = dynamic_cast<QtContactCallListWidget *> (_ui->tabWidget->widget(callIndex));
    if (!widget)
        return;

    QtPhoneCall * qtPhoneCall = widget->getFirstQtPhoneCall();
    if (qtPhoneCall)
        qtPhoneCall->acceptActionTriggered(true);
}

void QtWengoPhone::resumeCall(){
    int callIndex = findFirstCallTab();
    if (callIndex == -1 )
        return;
    QtContactCallListWidget * widget;

    widget = dynamic_cast<QtContactCallListWidget *> (_ui->tabWidget->widget(callIndex));
    if (!widget)
        return;

    QtPhoneCall * qtPhoneCall = widget->getFirstQtPhoneCall();
    if (qtPhoneCall)
        qtPhoneCall->holdResumeActionTriggered(true);
}

void QtWengoPhone::hangupCall(){

    int callIndex = findFirstCallTab();
    if (callIndex == -1 )
        return;
    QtContactCallListWidget * widget;

    widget = dynamic_cast<QtContactCallListWidget *> (_ui->tabWidget->widget(callIndex));
    if (!widget)
        return;
    widget->hangup();
}

void QtWengoPhone::hideMainWindow(){
    _wengoPhoneWindow->hide();
}

