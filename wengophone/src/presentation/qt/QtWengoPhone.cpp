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
#include "contactlist/QtUserList.h"

#include "ui_WengoPhoneWindow.h"

#include <presentation/PFactory.h>

#include <control/CWengoPhone.h>
#include <control/profile/CUserProfile.h>

#include <model/account/wengo/WengoAccount.h>
#include <model/connect/ConnectHandler.h>
#include <model/phonecall/PhoneCall.h>
#include <model/profile/UserProfile.h>
#include <model/history/History.h>
#include <model/phonecall/SipAddress.h>
#include <model/phonecall/ConferenceCall.h>
#include <model/contactlist/ContactList.h>
#include <model/contactlist/Contact.h>
#include <model/config/ConfigManager.h>
#include <model/config/Config.h>
#include <model/phoneline/IPhoneLine.h>
#include <model/webservices/url/WsUrl.h>

#include <imwrapper/EnumIMProtocol.h>
#include <imwrapper/EnumPresenceState.h>

#include "QtWengoPhoneEventFilter.h"
#include "QtWebcamButton.h"
#include "QtIdle.h"
#include "QtLanguage.h"
#include "statusbar/QtStatusBar.h"
#include "phoneline/QtPhoneLine.h"
#include "phonecall/QtPhoneCall.h"
#include "phonecall/QtContactCallListWidget.h"
#include "profile/QtProfileDetails.h"
#include "imaccount/QtIMAccountManager.h"
#include "contactlist/QtContactList.h"
#include "QtDialpad.h"
#include "QtAbout.h"
#include "QtConfigPanel.h"
#include "webservices/sms/QtSms.h"
#include "webservices/directory/QtWsDirectory.h"
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
#include <QAction>

#include "QtTrayIcon.h"

using namespace std;

const std::string QtWengoPhone::ANCHOR_CONTACTLIST = "openwengo_phonebook";
const std::string QtWengoPhone::ANCHOR_HISTORY = "openwengo_log";
const std::string QtWengoPhone::ANCHOR_CONFIGURATION = "openwengo_configuration";
const std::string QtWengoPhone::ANCHOR_DIALPAD = "openwengo_dial";
const std::string QtWengoPhone::ANCHOR_ADDCONTACT = "openwengo_addcontact";
const std::string QtWengoPhone::ANCHOR_SELFCARE = "openwengo_selfcare";
const std::string QtWengoPhone::ANCHOR_FORUM = "openwengo_forum";
const std::string QtWengoPhone::LOCAL_WEB_DIR = "webpages/windows";

QtWengoPhone::QtWengoPhone(CWengoPhone & cWengoPhone, bool background)
	: QObjectThreadSafe(NULL),
	_cWengoPhone(cWengoPhone), _background(background) {

	_qtSms = NULL;
	_qtWsDirectory = NULL;
	_qtProfileBar = NULL;
	_activeTabBeforeCall = NULL;
	_contactList = NULL;
	_contactListTabLayout = NULL;

	_chatWindow = NULL;

	_cWengoPhone.controlTimeoutEvent += boost::bind(&QtWengoPhone::timeoutEventHandler, this);
	connect(this, SIGNAL(signalTimeoutEventReached()), SLOT(slotTimeoutEventReachedThreadSafe()));

	typedef PostEvent0<void ()> MyPostEvent;
	MyPostEvent * event = new MyPostEvent(boost::bind(&QtWengoPhone::initThreadSafe, this));
	postEvent(event);
}

QtWengoPhone::~QtWengoPhone() {
	//TODO: unregister events, delete created objects
}

void QtWengoPhone::initThreadSafe() {
	_wengoPhoneWindow = new QMainWindow(NULL);
    QApplication::setQuitOnLastWindowClosed ( false );
	qRegisterMetaType<QVariant>("QVariant");

	_ui = new Ui::WengoPhoneWindow();
	_ui->setupUi(_wengoPhoneWindow);

	//Translation
	_qtLanguage = new QtLanguage(_wengoPhoneWindow);
	connect(_qtLanguage, SIGNAL(translationChangedSignal()), SLOT(slotTranslationChanged()));
	_qtLanguage->updateTranslation();

	// Install the close event filter
	QtWengoPhoneEventFilter * qtWengoPhoneEventFilter;
	qtWengoPhoneEventFilter = new QtWengoPhoneEventFilter(this);
	_wengoPhoneWindow->installEventFilter(qtWengoPhoneEventFilter);
	if (!connect (qtWengoPhoneEventFilter,SIGNAL(closeWindow()),_wengoPhoneWindow, SLOT(hide()))){
	  LOG_FATAL("Can't connect closeWindow() signal\n");
	}

	QGridLayout * callBarLayout = new QGridLayout(_ui->callBarFrame);
	_qtCallBar = new QtCallBar(_ui->callBarFrame);
	connect(_qtLanguage, SIGNAL(translationChangedSignal()), _qtCallBar, SLOT(slotUpdatedTranslation()));
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

	//webcamButton
	new QtWebcamButton(_ui->webcamButton);

	//Buttons initialization
	initButtons();

	//Dialpad
	QtDialpad * qtDialpad = new QtDialpad(this);

	Widget::createLayout(_ui->tabDialpad)->addWidget(qtDialpad->getWidget());
	qtDialpad->getWidget()->setMaximumSize(196,228);

	_qtHistoryWidget = NULL;

	//Systray
    _qtTrayIcon = new QtTrayIcon(this);
	connect(_qtProfileBar, SIGNAL(myPresenceStatusEventSignal(QVariant )), _qtTrayIcon, SLOT(setSystrayIcon(QVariant )));

	//actionShowWengoAccount
	connect(_ui->actionShowWengoAccount, SIGNAL(triggered()), SLOT(showWengoAccount()));

	//actionOpenWengoAccount
	//connect(_ui->actionOpenWengoAccount, SIGNAL(triggered()), SLOT(openWengoAccount()));

	//actionEditMyProfile
	connect(_ui->actionEditMyProfile, SIGNAL(triggered()), SLOT(editMyProfile()));

	//actionClose
	connect(_ui->actionClose, SIGNAL(triggered()),_wengoPhoneWindow, SLOT(hide()));

	//actionAddContact
	connect(_ui->actionAddContact, SIGNAL(triggered()), SLOT(addContact()));

	//actionConfiguration
	connect(_ui->actionConfiguration, SIGNAL(triggered()), SLOT(showConfig()));

	//actionShowForum
	connect(_ui->actionShowForum, SIGNAL(triggered()), SLOT(showForum()));

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

    // Show / Hide groups
    connect(_ui->actionShow_Hide_groups, SIGNAL(triggered()), SLOT(showHideGroups()));

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

	//actionChangeProfile
	connect(_ui->actionChangeProfile, SIGNAL(triggered()), SLOT(showLoginWindow()));

	//actionLog_off
	connect(_ui->actionLogOff, SIGNAL(triggered()), SLOT(logoff()));

	// Tab selection
	connect(_ui->tabWidget, SIGNAL(currentChanged (int)),SLOT(tabSelectionChanged(int)));

	// Accept a call
	connect (_ui->actionAccept, SIGNAL(triggered()), SLOT(acceptCall()));

	// Resume a call
	connect (_ui->actionHold_Resume, SIGNAL(triggered()), SLOT(resumeCall()));

	// Hangup a call
	connect (_ui->actionHangup, SIGNAL(triggered()), SLOT(hangupCall()));

	connect(this, SIGNAL(connectionStatusEventHandlerSignal(int, int, QString)),
		SLOT(connectionStatusEventHandlerSlot(int, int, QString)));

	Config & config = ConfigManager::getInstance().getCurrentConfig();

#if (defined OS_WINDOWS) && (defined QT_COMMERCIAL)
	if (config.getIEActiveX()) {
		//Embedded Browser
		_browser = new QtBrowser(NULL);
		_browser->urlClickedEvent += boost::bind(&QtWengoPhone::urlClickedEventHandler, this, _1);
		_ui->tabWidget->insertTab(_ui->tabWidget->count(), (QWidget*) _browser->getWidget(), tr("Home"));
		_browser->setUrl(qApp->applicationDirPath().toStdString() + "/" + LOCAL_WEB_DIR + "/loading.html");
		_ui->tabWidget->setCurrentWidget((QWidget*) _browser->getWidget());
	}
#endif

	//configPanel
	QtConfigPanel * qtConfigPanel = new QtConfigPanel(_cWengoPhone, _wengoPhoneWindow);
	connect(_qtLanguage, SIGNAL(translationChangedSignal()),qtConfigPanel, SLOT(slotTranslationChanged()));
	_configPanelWidget = qtConfigPanel->getWidget();
	int configPanelIndex = _ui->configPanel->addWidget(_configPanelWidget);
	_ui->configPanel->setCurrentIndex(configPanelIndex);
	_ui->configPanel->hide();

	MousePressEventFilter * mousePressEventFilter = new MousePressEventFilter(this, SLOT(expandConfigPanel()));
	_ui->configPanelLabel->installEventFilter(mousePressEventFilter);

	//Status bar
	_statusBar = new QtStatusBar(_cWengoPhone, _ui->statusBar);

	updatePresentation();
	_wengoPhoneWindow->resize(QSize(config.getProfileWidth(),config.getProfileHeight()));
	_wengoPhoneWindow->move(QPoint(config.getProfilePosX(),config.getProfilePoxY()));

	if(!_background) {
		_wengoPhoneWindow->show();
	}

	_qtLanguage->updateTranslation();
}

void QtWengoPhone::initButtons() {
	//callButton
	connect(_qtCallBar, SIGNAL(ButtonClicked()), SLOT(callButtonClicked()));
	connect(_qtCallBar, SIGNAL(OffClicked()), SLOT(hangupButtonClicked()));
	enableCallButton();

	//hangUpButton
	_hangUpButton->setEnabled(false);

	//phoneComboBox
	connect(_phoneComboBox->lineEdit(), SIGNAL(returnPressed ()), SLOT(callButtonClicked()));
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
        if (widget) {
            return i;
        }
    }
    return -1;
}

void QtWengoPhone::callButtonClicked() {
	if (_cWengoPhone.getCUserProfileHandler().getCUserProfile()) {
		std::string phoneNumber = _phoneComboBox->currentText().toStdString();
		if (!phoneNumber.empty()) {
			_cWengoPhone.getCUserProfileHandler().getCUserProfile()->makeCall(phoneNumber);
		}
		_phoneComboBox->clearEditText();
	}
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

	if( qtPhoneCall->isIncoming() ) {
		_callButton->setEnabled(true);
	}
}

void QtWengoPhone::addToConference(QString phoneNumber, PhoneCall * targetCall) {
	if (_cWengoPhone.getCUserProfileHandler().getCUserProfile()) {
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
						IPhoneLine * phoneLine = _cWengoPhone.getCUserProfileHandler().getCUserProfile()->getUserProfile().getActivePhoneLine();

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
}

void QtWengoPhone::logoff(){
	if (_cWengoPhone.getCUserProfileHandler().getCUserProfile()) {
		_cWengoPhone.getCUserProfileHandler().getCUserProfile()->disconnect();
	}
}

void QtWengoPhone::addToConference(PhoneCall * sourceCall, PhoneCall * targetCall){
	if (_cWengoPhone.getCUserProfileHandler().getCUserProfile()) {
		// Bad and Ugly but works...

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
					IPhoneLine * phoneLine = _cWengoPhone.getCUserProfileHandler().getCUserProfile()->getUserProfile().getActivePhoneLine();
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

void QtWengoPhone::setContactList(QtContactList * qtContactList) {
	if (!_contactListTabLayout) {
		_contactListTabLayout = Widget::createLayout(_ui->tabContactList);
	}

	_contactList = qtContactList;
	_contactListTabLayout->addWidget(_contactList->getWidget());

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
	if (_cWengoPhone.getCUserProfileHandler().getCUserProfile()) {
		//disabled some actions if no WengoAccount is used
		bool hasWengoAccount = _cWengoPhone.getCUserProfileHandler().getCUserProfile()->getUserProfile().hasWengoAccount();

		_ui->actionShowWengoAccount->setEnabled(hasWengoAccount);
		_ui->actionSendSms->setEnabled(hasWengoAccount);
		_ui->actionCreateConferenceCall->setEnabled(hasWengoAccount);
		_ui->actionSearchWengoUsers->setEnabled(hasWengoAccount);
	}
}

void QtWengoPhone::loginStateChangedEventHandlerThreadSafe(SipAccount & sender, SipAccount::LoginState state) {
}

void QtWengoPhone::dialpad(const std::string & tone, const std::string & soundFile) {
	if (_cWengoPhone.getCUserProfileHandler().getCUserProfile()) {
		PhoneCall * phoneCall = _cWengoPhone.getCUserProfileHandler().getCUserProfile()->getActivePhoneCall();
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
}

void QtWengoPhone::showWengoAccount() {
	WsUrl::showWengoAccount();
}

void QtWengoPhone::openWengoAccount() {
	//showLoginWindow();
}

void QtWengoPhone::editMyProfile() {
	if (_cWengoPhone.getCUserProfileHandler().getCUserProfile()) {
		//FIXME: this method should not be called if no UserProfile has been set
		QtProfileDetails qtProfileDetails(*_cWengoPhone.getCUserProfileHandler().getCUserProfile(),
			_cWengoPhone.getCUserProfileHandler().getCUserProfile()->getUserProfile(), _wengoPhoneWindow);
		//TODO: UserProfile must be updated if QtProfileDetails was accepted
		qtProfileDetails.show();

		LOG_DEBUG("edit user profile");
	}
}

void QtWengoPhone::exitApplication() {

    Config & config = ConfigManager::getInstance().getCurrentConfig();
    // Save the window size
    QSize winsize = _wengoPhoneWindow->size();
    config.set(Config::PROFILE_WIDTH,winsize.width());
    config.set(Config::PROFILE_HEIGHT,winsize.height());
    // Save the window position
    QPoint winpos = _wengoPhoneWindow->pos();
    config.set(Config::PROFILE_POSX,winpos.x());
    config.set(Config::PROFILE_POSY,winpos.y());
	QApplication::closeAllWindows();
	_qtTrayIcon->hide();
	QCoreApplication::processEvents();
	_cWengoPhone.terminate();
	QCoreApplication::exit(EXIT_SUCCESS);
}

void QtWengoPhone::addContact() {
	if (_cWengoPhone.getCUserProfileHandler().getCUserProfile()) {
		//FIXME: this method should not be called if no UserProfile has been set
		LOG_DEBUG("add contact");
		ContactProfile (contactProfile);
		QtProfileDetails qtProfileDetails(*_cWengoPhone.getCUserProfileHandler().getCUserProfile(),
			contactProfile, _wengoPhoneWindow);
		if (qtProfileDetails.show()) {
			_cWengoPhone.getCUserProfileHandler().getCUserProfile()->getCContactList().addContact(contactProfile);
		}
	}
}

void QtWengoPhone::showConfig() {
	QtWengoConfigDialog dialog(_cWengoPhone, _wengoPhoneWindow);
	dialog.show();
}

void QtWengoPhone::showForum() {
	WsUrl::showWengoForum();
}

void QtWengoPhone::showHelp() {
}

void QtWengoPhone::showFaq() {
	WsUrl::showWengoFAQ();
}

void QtWengoPhone::showBuyOut() {
	WsUrl::showWengoBuyWengos();
}
void QtWengoPhone::showCallOut() {
	WsUrl::showWengoCallOut();
}

void QtWengoPhone::showSms() {
	WsUrl::showWengoSMS();
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
	WsUrl::showWengoVoiceMail();
}

void QtWengoPhone::showContactList() {
}

void QtWengoPhone::sendSms() {
	if (_qtSms) {
		_qtSms->getWidget()->show();
	}
}

void QtWengoPhone::showAccountSettings() {
	if (_cWengoPhone.getCUserProfileHandler().getCUserProfile()) {
		//FIXME: IMAccountManager must not use UserProfile but only CUserProfile
		QtIMAccountManager imAccountManager(_cWengoPhone.getCUserProfileHandler().getCUserProfile()->getUserProfile(),
			true, _wengoPhoneWindow);
	}
}

//FIXME
void QtWengoPhone::showCreateConferenceCall() {
	if (_cWengoPhone.getCUserProfileHandler().getCUserProfile()) {
		QDialog * conferenceDialog = qobject_cast<QDialog *>(WidgetFactory::create(":/forms/phonecall/ConferenceCallWidget.ui", _wengoPhoneWindow));

		int ret = conferenceDialog->exec();

		QLineEdit * phoneNumber1LineEdit;
		QLineEdit * phoneNumber2LineEdit;

		phoneNumber1LineEdit = Object::findChild<QLineEdit *>(conferenceDialog,"phoneNumber1LineEdit");
		phoneNumber2LineEdit = Object::findChild<QLineEdit *>(conferenceDialog,"phoneNumber2LineEdit");

		if (ret == QDialog::Accepted) {
			IPhoneLine * phoneLine = _cWengoPhone.getCUserProfileHandler().getCUserProfile()->getUserProfile().getActivePhoneLine();

			if (phoneLine != NULL) {
				ConferenceCall * confCall = new ConferenceCall(*phoneLine);
				confCall->addPhoneNumber(phoneNumber1LineEdit->text().toStdString());
				confCall->addPhoneNumber(phoneNumber2LineEdit->text().toStdString());
			} else {
				LOG_DEBUG("phoneLine is NULL");
			}
		}
	}
}
//!FIXME

void QtWengoPhone::urlClickedEventHandler(std::string url) {
	LOG_DEBUG(url);

	//find anchor
	std::string anchor;
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

void QtWengoPhone::expandConfigPanel() {
	static bool expand = true;

	if (expand) {
		/*_wengoPhoneWindow->resize(_wengoPhoneWindow->width(),
					  _wengoPhoneWindow->height() + _configPanelWidget->height());*/
		_ui->configPanel->show();
	} else {
	  /*_wengoPhoneWindow->resize(_wengoPhoneWindow->width(),
				    _wengoPhoneWindow->height() - _configPanelWidget->height());*/

	  _ui->configPanel->hide();
	}
	expand = !expand;
}

void QtWengoPhone::eraseHistoryOutgoingCalls() {
	if (_cWengoPhone.getCUserProfileHandler().getCUserProfile() &&
		_cWengoPhone.getCUserProfileHandler().getCUserProfile()->getCHistory()) {
		_cWengoPhone.getCUserProfileHandler().getCUserProfile()->getCHistory()->clear(HistoryMemento::OutgoingCall);
	}
}

void QtWengoPhone::eraseHistoryIncomingCalls() {
	if (_cWengoPhone.getCUserProfileHandler().getCUserProfile() &&
		_cWengoPhone.getCUserProfileHandler().getCUserProfile()->getCHistory()) {
		_cWengoPhone.getCUserProfileHandler().getCUserProfile()->getCHistory()->clear(HistoryMemento::IncomingCall);
	}
}

void QtWengoPhone::eraseHistoryMissedCalls() {
	if (_cWengoPhone.getCUserProfileHandler().getCUserProfile() &&
		_cWengoPhone.getCUserProfileHandler().getCUserProfile()->getCHistory()) {
		_cWengoPhone.getCUserProfileHandler().getCUserProfile()->getCHistory()->clear(HistoryMemento::MissedCall);
	}
}

void QtWengoPhone::eraseHistoryChatSessions() {
	if (_cWengoPhone.getCUserProfileHandler().getCUserProfile() &&
		_cWengoPhone.getCUserProfileHandler().getCUserProfile()->getCHistory()) {
		_cWengoPhone.getCUserProfileHandler().getCUserProfile()->getCHistory()->clear(HistoryMemento::ChatSession);
	}
}

void QtWengoPhone::eraseHistorySms() {
	if (_cWengoPhone.getCUserProfileHandler().getCUserProfile() &&
		_cWengoPhone.getCUserProfileHandler().getCUserProfile()->getCHistory()) {
		_cWengoPhone.getCUserProfileHandler().getCUserProfile()->getCHistory()->clear(HistoryMemento::OutgoingSmsOk);
	}
}

void QtWengoPhone::eraseHistory() {
	if (_cWengoPhone.getCUserProfileHandler().getCUserProfile() &&
		_cWengoPhone.getCUserProfileHandler().getCUserProfile()->getCHistory()) {
		_cWengoPhone.getCUserProfileHandler().getCUserProfile()->getCHistory()->clear(HistoryMemento::Any);
	}
}

void QtWengoPhone::phoneComboBoxClicked() {
	//if _qtHistoryWidget is set it means that History has been created
	if (_qtHistoryWidget) {
		HistoryMementoCollection * mementos =
			_cWengoPhone.getCUserProfileHandler().getCUserProfile()->getCHistory()->getMementos(HistoryMemento::OutgoingCall, 10);
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

QMenu * QtWengoPhone::createStatusMenu() {
   QMenu * menu = new QMenu(tr("Status"));

   switch ( _cWengoPhone.getCUserProfileHandler().getCUserProfile()->getUserProfile().getPresenceState() ){
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
    if (_chatWindow) {
        _chatWindow->showNormal();
        _chatWindow->activateWindow();
    }
}

void QtWengoPhone::slotSystrayMenuCallBlank(bool checked) {
    LOG_DEBUG("Placing a blank call from systray.");
    _ui->tabWidget->setCurrentIndex(2);
    getWidget()->show();
    getWidget()->raise();
    getWidget()->setWindowState(getWidget()->windowState() & ~Qt::WindowMinimized | Qt::WindowActive);
}

void QtWengoPhone::timeoutEventHandler() {
  signalTimeoutEventReached();
}

void QtWengoPhone::slotTimeoutEventReachedThreadSafe() {
  QCoreApplication::exit(EXIT_SUCCESS);
}

void QtWengoPhone::showHistory() {
	_ui->tabWidget->setCurrentWidget(_ui->tabHistory);
}

void QtWengoPhone::connectionStatusEventHandler(int totalSteps, int currentStep, const std::string & infoMsg) {
	connectionStatusEventHandlerSignal(totalSteps, currentStep, QString::fromStdString(infoMsg));
}

void QtWengoPhone::connectionStatusEventHandlerSlot(int totalSteps, int currentStep, QString infoMsg) {

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

void QtWengoPhone::slotTranslationChanged() {
  _ui->retranslateUi(_wengoPhoneWindow);
}

void QtWengoPhone::currentUserProfileWillDieEventHandlerSlot() {
	if (_qtIdle) {
		delete _qtIdle;
		_qtIdle = NULL;
	}

	if (_qtProfileBar) {
		_ui->profileBar->layout()->removeWidget(_qtProfileBar);
		delete _qtProfileBar;
		_qtProfileBar = NULL;
	}

	if (_contactList) {
		_contactListTabLayout->removeWidget(_contactList->getWidget());
		_contactList->cleanup();
		// _contactList is deleted in CContactList
		_contactList = NULL;
	}
}

void QtWengoPhone::userProfileInitializedEventHandlerSlot() {
	//Idle detection
	//FIXME: QtIdle must not use UserProfile but CUserProfile
	_qtIdle = new QtIdle(_cWengoPhone.getCUserProfileHandler().getCUserProfile()->getUserProfile(), _wengoPhoneWindow);

	// Create the profilebar
	//FIXME: QtProfileBar must not use ConnectHandler directly
	_qtProfileBar = new QtProfileBar(_cWengoPhone,
		*_cWengoPhone.getCUserProfileHandler().getCUserProfile(),
		_cWengoPhone.getCUserProfileHandler().getCUserProfile()->getUserProfile().getConnectHandler(),
		_ui->profileBar);
	connect(_qtLanguage, SIGNAL(translationChangedSignal()), _qtProfileBar, SLOT(slotTranslationChanged()));

	//Add the profile bar
	int profileBarIndex = _ui->profileBar->addWidget(_qtProfileBar);
	_ui->profileBar->setCurrentIndex(profileBarIndex);
	_ui->profileBar->widget(profileBarIndex)->setLayout(new QGridLayout());
	_qtTrayIcon->setTrayMenu();
}
void QtWengoPhone::showHideGroups(){

    Config & config = ConfigManager::getInstance().getCurrentConfig();
    bool showHide = config.getShowGroups();
    if ( showHide ){
        config.set(Config::GENERAL_SHOW_GROUPS_KEY,false);
        _contactList->showHideGroups();
    } else {
        config.set(Config::GENERAL_SHOW_GROUPS_KEY,true);
        _contactList->showHideGroups();
    }
}

void QtWengoPhone::showHideOffLineContacts() {
    Config & config = ConfigManager::getInstance().getCurrentConfig();
    bool showHide = config.getShowOfflineContacts();
    if ( showHide ){
        config.set(Config::GENERAL_SHOW_OFFLINE_CONTACTS_KEY,false);
    } else {
        config.set(Config::GENERAL_SHOW_OFFLINE_CONTACTS_KEY,true);
    }
	_contactList->hideOffLineUser();
}
