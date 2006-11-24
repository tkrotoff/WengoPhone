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

#include <presentation/PFactory.h>

#include <cutil/global.h>

#include "QtSystray.h"
#include "QtHttpProxyLogin.h"
#include "QtConfigPanel.h"
#include "QtDialpad.h"
#include "QtIdle.h"
#include "QtLanguage.h"
#include "QtToolBar.h"
#include "QtWebcamButton.h"
#include "QtBrowserWidget.h"
#include "callbar/QtCallBar.h"
#include "contactlist/QtContactList.h"
#include "filetransfer/QtFileTransfer.h"
#include "history/QtHistoryWidget.h"
#include "imaccount/QtIMAccountManager.h"
#include "phonecall/QtContactCallListWidget.h"
#include "phonecall/QtPhoneCall.h"
#include "phoneline/QtPhoneLine.h"
#include "profile/QtProfileDetails.h"
#include "profilebar/QtProfileBar.h"
#include "statusbar/QtStatusBar.h"
#include "webservices/directory/QtWsDirectory.h"
#include "webservices/sms/QtSms.h"

#if (defined OS_WINDOWS) && (QT_EDITION == QT_EDITION_DESKTOP)
	#include "webdirectory/QtWebDirectory.h"
#endif

#include <control/CWengoPhone.h>
#include <control/profile/CUserProfile.h>
#include <control/profile/CUserProfileHandler.h>
#include <control/history/CHistory.h>

#include <model/WengoPhone.h>
#include <model/commandserver/ContactInfo.h>
#include <model/config/ConfigManager.h>
#include <model/config/Config.h>
#include <model/connect/ConnectHandler.h>
#include <model/contactlist/ContactList.h>
#include <model/contactlist/Contact.h>
#include <model/history/History.h>
#include <model/phonecall/PhoneCall.h>
#include <model/phonecall/SipAddress.h>
#include <model/phonecall/ConferenceCall.h>
#include <model/phoneline/IPhoneLine.h>
#include <model/profile/AvatarList.h>
#include <model/profile/UserProfile.h>

#include <imwrapper/EnumPresenceState.h>

#include <qtutil/CloseEventFilter.h>
#include <qtutil/Widget.h>
#include <qtutil/SafeConnect.h>
#include <qtutil/LanguageChangeEventFilter.h>

#include <cutil/global.h>
#include <thread/Thread.h>
#include <util/Logger.h>
#include <util/SafeDelete.h>

#include <QtGui/QtGui>

#if defined(OS_MACOSX)
	#include <Carbon/Carbon.h>
#endif

using namespace std;

QtWengoPhone::QtWengoPhone(CWengoPhone & cWengoPhone)
	: QObjectThreadSafe(NULL),
	_cWengoPhone(cWengoPhone) {

	_qtSms = NULL;
	_qtWsDirectory = NULL;
	_qtProfileBar = NULL;
	_activeTabBeforeCall = NULL;
	_qtContactList = NULL;
	_qtFileTransfer = NULL;
	_chatWindow = NULL;

	NetworkProxyDiscovery::getInstance().proxyNeedsAuthenticationEvent +=
		boost::bind(&QtWengoPhone::proxyNeedsAuthenticationEventHandler, this, _1, _2);
	NetworkProxyDiscovery::getInstance().wrongProxyAuthenticationEvent +=
		boost::bind(&QtWengoPhone::wrongProxyAuthenticationEventHandler, this, _1, _2);

	//Check if the event has not already been sent
	if (NetworkProxyDiscovery::getInstance().getState() ==
		NetworkProxyDiscovery::NetworkProxyDiscoveryStateNeedsAuthentication) {
		proxyNeedsAuthenticationEventHandler(NetworkProxyDiscovery::getInstance(),
			NetworkProxyDiscovery::getInstance().getNetworkProxy());
	}

	typedef PostEvent0<void ()> MyPostEvent;
	MyPostEvent * event = new MyPostEvent(boost::bind(&QtWengoPhone::initThreadSafe, this));
	postEvent(event);
}

QtWengoPhone::~QtWengoPhone() {
	//TODO delete created objects
}

void QtWengoPhone::initThreadSafe() {
	QApplication::setQuitOnLastWindowClosed(false);

	qRegisterMetaType<QVariant>("QVariant");

	_wengoPhoneWindow = new QMainWindow(NULL);

	LANGUAGE_CHANGE(_wengoPhoneWindow);

	_ui = new Ui::WengoPhoneWindow();
	_ui->setupUi(_wengoPhoneWindow);

#ifdef OS_LINUX
	_wengoPhoneWindow->setWindowIcon(QIcon(QPixmap(QString::fromStdString(AvatarList::getInstance().getDefaultAvatar().getFullPath()))));
#endif

	//Translation
	_qtLanguage = new QtLanguage(_wengoPhoneWindow);

	//Install the close event filter
	CloseEventFilter * closeEventFilter = new CloseEventFilter(this, SLOT(closeWindow()));
	_wengoPhoneWindow->installEventFilter(closeEventFilter);

	//QtCallBar
	_qtCallBar = new QtCallBar(_ui->callBarFrame);
	Widget::createLayout(_ui->callBarFrame)->addWidget(_qtCallBar);

	//QtToolBar
	_qtToolBar = new QtToolBar(*this, _ui, _wengoPhoneWindow);

	//phoneComboBox
	SAFE_CONNECT(_qtCallBar, SIGNAL(phoneComboBoxClicked()), SLOT(phoneComboBoxClicked()));

	//QtWebcamButton
	new QtWebcamButton(_ui->actionEnableVideo);

	//Buttons initialization
	initCallButtons();

	//QtHistoryWidget
	_qtHistoryWidget = NULL;

	//QtSystray
	_qtSystray = new QtSystray(this);

	//QtStatusBar
	_qtStatusBar = new QtStatusBar(_cWengoPhone, _ui->statusBar, _qtToolBar);

#if (defined OS_WINDOWS) && (QT_EDITION == QT_EDITION_DESKTOP)
	_qtWebDirectory = new QtWebDirectory(0);
#endif

	Config & config = ConfigManager::getInstance().getCurrentConfig();

	//QtBrowserWidget
	_qtBrowserWidget = new QtBrowserWidget(*this);
#if (defined OS_WINDOWS) && (QT_EDITION == QT_EDITION_DESKTOP)
	if (config.getIEActiveXEnable()) {
		_ui->tabWidget->insertTab(_ui->tabWidget->count(), _qtBrowserWidget->getWidget(), tr("Home"));
		_ui->tabWidget->setCurrentWidget(_qtBrowserWidget->getWidget());
	}
#else
	_ui->tabWidget->setCurrentIndex(0);
#endif

	//QtConfigPanel
	QtConfigPanel * qtConfigPanel = new QtConfigPanel(_cWengoPhone, _wengoPhoneWindow);
	int configPanelIndex = _ui->configPanel->addWidget(qtConfigPanel->getWidget());
	_ui->configPanel->setCurrentIndex(configPanelIndex);
	_ui->configPanel->hide();

	updatePresentation();

	//Main window size and position saved
	_wengoPhoneWindow->resize(QSize(config.getProfileWidth(), config.getProfileHeight()));
	_wengoPhoneWindow->move(QPoint(config.getProfilePosX(), config.getProfilePoxY()));

	if (!config.getCmdLineBackgroundModeEnable()) {
		_wengoPhoneWindow->show();
	}
}

QWidget * QtWengoPhone::getWidget() const {
	return _wengoPhoneWindow;
}

QtLanguage & QtWengoPhone::getQtLanguage() const {
	return *_qtLanguage;
}

QtProfileBar * QtWengoPhone::getQtProfileBar() const {
	return _qtProfileBar;
}

QtBrowserWidget & QtWengoPhone::getQtBrowserWidget() const {
	return *_qtBrowserWidget;
}

CWengoPhone & QtWengoPhone::getCWengoPhone() const {
	return _cWengoPhone;
}

void QtWengoPhone::setQtSms(QtSms * qtSms) {
	_qtSms = qtSms;
}

QtSms * QtWengoPhone::getQtSms() const {
	return _qtSms;
}

#if (defined OS_WINDOWS) && (QT_EDITION == QT_EDITION_DESKTOP)
QtWebDirectory * QtWengoPhone::getQtWebDirectory() const {
	return _qtWebDirectory;
}
#endif

void QtWengoPhone::setQtWsDirectory(QtWsDirectory * qtWsDirectory) {
	_qtWsDirectory = qtWsDirectory;
}

QtWsDirectory * QtWengoPhone::getQtWsDirectory() const {
	return _qtWsDirectory;
}

QtToolBar & QtWengoPhone::getQtToolBar() const {
	return *_qtToolBar;
}

QtStatusBar & QtWengoPhone::getQtStatusBar() const {
	return *_qtStatusBar;
}

QtSystray & QtWengoPhone::getQtSystray() const {
	return *_qtSystray;
}

QtCallBar & QtWengoPhone::getQtCallBar() const {
	return *_qtCallBar;
}

void QtWengoPhone::setQtSubscribe(QtSubscribe * qtSubscribe) {
	_qtSubscribe = qtSubscribe;
}

QtSubscribe * QtWengoPhone::getQtSubscribe() const {
	return _qtSubscribe;
}

void QtWengoPhone::setChatWindow(QWidget * chatWindow) {
	if (!chatWindow) {
		_ui->actionOpenChatWindow->setEnabled(false);
	}
	else {
		_chatWindow = chatWindow;
		_ui->actionOpenChatWindow->setEnabled(true);
	}
}

QWidget * QtWengoPhone::getChatWindow() const {
	return _chatWindow;
}

void QtWengoPhone::setQtContactList(QtContactList * qtContactList) {
	_qtContactList = qtContactList;

	if (!_ui->tabContactList->layout()) {
		Widget::createLayout(_ui->tabContactList);
	}

	_ui->tabContactList->layout()->addWidget(_qtContactList->getWidget());

	LOG_DEBUG("QtContactList added");
}

QtContactList * QtWengoPhone::getQtContactList() const {
	return _qtContactList;
}

void QtWengoPhone::setQtHistoryWidget(QtHistoryWidget * qtHistoryWidget) {
	_qtHistoryWidget = qtHistoryWidget;
	_qtHistoryWidget->setQtToolBar(_qtToolBar);

	if (!_ui->tabHistory->layout()) {
		Widget::createLayout(_ui->tabHistory);
	}

	_ui->tabHistory->layout()->addWidget(_qtHistoryWidget->getWidget());

	LOG_DEBUG("QtHistoryWidget added");
}

void QtWengoPhone::setQtDialpad(QtDialpad * qtDialpad) {
	Widget::createLayout(_ui->tabDialpad)->addWidget(qtDialpad);
	LOG_DEBUG("QtDialpad added");
}


void QtWengoPhone::initCallButtons() {
	//callButton
	SAFE_CONNECT(_qtCallBar, SIGNAL(callButtonClicked()), SLOT(callButtonClicked()));
	enableCallButton();

	//hangUpButton
	SAFE_CONNECT(_qtCallBar, SIGNAL(hangUpButtonClicked()), SLOT(hangUpButtonClicked()));
	_qtCallBar->setEnabledHangUpButton(false);

	//phoneComboBox
	SAFE_CONNECT(_qtCallBar, SIGNAL(phoneComboBoxReturnPressed()), SLOT(callButtonClicked()));
	SAFE_CONNECT(_qtCallBar, SIGNAL(phoneComboBoxEditTextChanged(const QString &)), SLOT(enableCallButton()));
}

void QtWengoPhone::enableCallButton() {
	std::string phoneNumber = _qtCallBar->getPhoneComboBoxCurrentText();
	_qtCallBar->setEnabledCallButton(!phoneNumber.empty());
}

void QtWengoPhone::hangUpButtonClicked() {
	QtContactCallListWidget * widget =
		dynamic_cast<QtContactCallListWidget *>(_ui->tabWidget->currentWidget());
	if (widget) {
		widget->hangup();
		//Widget is deleted automagically
		//set the last active page
		if (_activeTabBeforeCall) {
			_ui->tabWidget->setCurrentWidget(_activeTabBeforeCall);
		} else {
			_ui->tabWidget->setCurrentIndex(0);
		}
		return;
	}

	for (int i = 0; i < _ui->tabWidget->count(); i++) {
		widget = dynamic_cast<QtContactCallListWidget *>(_ui->tabWidget->widget(i));
		if (widget) {
			widget->hangup();
			//set the last active page
			if (_activeTabBeforeCall) {
				_ui->tabWidget->setCurrentWidget(_activeTabBeforeCall);
			} else {
				_ui->tabWidget->setCurrentIndex(0);
			}
			return;
		}
	}
}

void QtWengoPhone::callButtonClicked() {
	if (_cWengoPhone.getCUserProfileHandler().getCUserProfile()) {
		std::string phoneNumber = _qtCallBar->getPhoneComboBoxCurrentText();
		if (!phoneNumber.empty()) {
			CUserProfile * cUserProfile = _cWengoPhone.getCUserProfileHandler().getCUserProfile();
			cUserProfile->makeCallErrorEvent += boost::bind(&QtWengoPhone::makeCallErrorEventHandler, this);
			cUserProfile->makeCall(phoneNumber);
		}
		_qtCallBar->clearPhoneComboBoxEditText();
	}
}

void QtWengoPhone::makeCallErrorEventHandler() {
	typedef PostEvent0<void ()> MyPostEvent;
	MyPostEvent * event = new MyPostEvent(boost::bind(&QtWengoPhone::makeCallErrorEventHandlerThreadSafe, this));
	postEvent(event);
}

void QtWengoPhone::makeCallErrorEventHandlerThreadSafe() {
	QMessageBox::information(_wengoPhoneWindow, tr("WengoPhone - Call Error"),
		tr("Please hold all the phone calls before to place a new one"), QMessageBox::Ok);
}

void QtWengoPhone::addPhoneCall(QtPhoneCall * qtPhoneCall) {
	ensureVisible(false);

	_activeTabBeforeCall = _ui->tabWidget->currentWidget();

	QtContactCallListWidget * qtContactCallListWidget = new QtContactCallListWidget(_cWengoPhone,_wengoPhoneWindow);
	_ui->tabWidget->addTab(qtContactCallListWidget, tr("Call"));
	_ui->tabWidget->setCurrentWidget(qtContactCallListWidget);
	qtContactCallListWidget->addPhoneCall(qtPhoneCall);

	SAFE_CONNECT(qtContactCallListWidget, SIGNAL(startConferenceSignal(PhoneCall *, PhoneCall *)),
			SLOT(addToConference(PhoneCall *, PhoneCall *)));

	_qtCallBar->setEnabledHangUpButton(true);

	if (qtPhoneCall->isIncoming()) {
		_qtCallBar->setEnabledCallButton(true);
	}
}

void QtWengoPhone::addToConference(QString phoneNumber, PhoneCall * targetCall) {
	//FIXME conference has to be 100% rewritten...
	bool conferenceAlreadyStarted = false;

	if (_cWengoPhone.getCUserProfileHandler().getCUserProfile()) {
		int nbtab = _ui->tabWidget->count();

		for (int i = 0; i < nbtab; i++) {
			if (_ui->tabWidget->tabText(i) == QString(tr("Conference"))) {
				return;
			}

			for (int j = 0; j < _ui->tabWidget->count(); j++) {
				QtContactCallListWidget * qtContactCallListWidget = dynamic_cast<QtContactCallListWidget *>(_ui->tabWidget->widget(j));
				if (qtContactCallListWidget) {
					if (qtContactCallListWidget->hasPhoneCall( targetCall)) {
						_ui->tabWidget->setTabText(j, tr("Conference"));
						IPhoneLine * phoneLine = _cWengoPhone.getCUserProfileHandler().getCUserProfile()->getUserProfile().getActivePhoneLine();

						if (phoneLine != NULL) {
							if (!conferenceAlreadyStarted) {
								conferenceAlreadyStarted = true;
								ConferenceCall * confCall = new ConferenceCall(*phoneLine);
								confCall->addPhoneCall(*targetCall);
								confCall->addPhoneNumber(phoneNumber.toStdString());
							}
						} else {
							LOG_DEBUG("phoneLine is NULL");
						}
					}
				}
			}
		}
	}
}

void QtWengoPhone::addToConference(PhoneCall * sourceCall, PhoneCall * targetCall) {
	if (_cWengoPhone.getCUserProfileHandler().getCUserProfile()) {
		//Bad and Ugly but works...

		int nbtab = _ui->tabWidget->count();

		for (int i = 0; i < nbtab; i++) {
			if (_ui->tabWidget->tabText(i) == QString(tr("Conference"))) {
				return;
			}
		}

		for (int i = 0; i < _ui->tabWidget->count(); i++) {
			QtContactCallListWidget * qtContactCallListWidget = dynamic_cast<QtContactCallListWidget *>(_ui->tabWidget->widget(i));
			if (qtContactCallListWidget) {
				if (qtContactCallListWidget->hasPhoneCall(sourceCall)) {
					_ui->tabWidget->setTabText(i, tr("Conference"));
					IPhoneLine * phoneLine = _cWengoPhone.getCUserProfileHandler().getCUserProfile()->getUserProfile().getActivePhoneLine();
					if (phoneLine) {
						ConferenceCall * confCall = new ConferenceCall(*phoneLine);
						confCall->addPhoneCall(*targetCall);
						confCall->addPhoneCall(*sourceCall);
						//Add the target to source and remove the target tab
						for (int j = 0; j < _ui->tabWidget->count(); j++) {
							QtContactCallListWidget * toRemove =
								(QtContactCallListWidget *) _ui->tabWidget->widget(j);
							if (toRemove) {
								if (toRemove->hasPhoneCall(targetCall)) {
									QtPhoneCall * qtPhoneCall = toRemove->takeQtPhoneCall(targetCall);
									if (qtPhoneCall) {
										toRemove->close();
										qtContactCallListWidget->addPhoneCall(qtPhoneCall);
										break;
									}
								}
							}
						}

					} else {
						LOG_DEBUG("phoneLine is NULL");
					}
					break;
				}
			}
		}
	}
}

void QtWengoPhone::addToConference(QtPhoneCall * qtPhoneCall) {

	QtContactCallListWidget * qtContactCallListWidget;

	int nbtab = _ui->tabWidget->count();

	for (int i = 0; i < nbtab; i++) {
		if (_ui->tabWidget->tabText(i) == QString(tr("Conference"))) {
			//i is the index of the conference tab
			qtContactCallListWidget = (QtContactCallListWidget *) _ui->tabWidget->widget(i);
			qtContactCallListWidget->addPhoneCall(qtPhoneCall);
			_ui->tabWidget->setCurrentWidget(qtContactCallListWidget);
			return;
		}
	}

	_activeTabBeforeCall = _ui->tabWidget->currentWidget();

	//conference tab not found, create a new one
	qtContactCallListWidget = new QtContactCallListWidget(_cWengoPhone,_wengoPhoneWindow);
	_ui->tabWidget->addTab(qtContactCallListWidget, tr("Conference"));
	_ui->tabWidget->setCurrentWidget(qtContactCallListWidget);
	qtContactCallListWidget->addPhoneCall(qtPhoneCall);
	_qtCallBar->setEnabledHangUpButton(true);
}

QtFileTransfer * QtWengoPhone::getFileTransfer() const {
	return _qtFileTransfer;
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
		_ui->actionSearchWengoContact->setEnabled(hasWengoAccount);
	}
}

void QtWengoPhone::dialpad(const std::string & tone) {
	if (_cWengoPhone.getCUserProfileHandler().getCUserProfile()) {
		_qtCallBar->setPhoneComboBoxEditText(_qtCallBar->getPhoneComboBoxCurrentText() + tone);
	}
}

void QtWengoPhone::prepareToExitApplication() {
	Config & config = ConfigManager::getInstance().getCurrentConfig();

	_qtSystray->hide();

	//Save the window size
	QSize winsize = _wengoPhoneWindow->size();
	config.set(Config::PROFILE_WIDTH_KEY, winsize.width());
	config.set(Config::PROFILE_HEIGHT_KEY, winsize.height());

	//Save the window position
	QPoint winpos = _wengoPhoneWindow->pos();
	config.set(Config::PROFILE_POSX_KEY, winpos.x());
	config.set(Config::PROFILE_POSY_KEY, winpos.y());

	QApplication::closeAllWindows();
	QCoreApplication::processEvents();

	_cWengoPhone.terminate();
}

void QtWengoPhone::phoneComboBoxClicked() {
	//if _qtHistoryWidget is set it means that History has been created
	if (_qtHistoryWidget) {
		HistoryMementoCollection * mementos =
			_cWengoPhone.getCUserProfileHandler().getCUserProfile()->getCHistory()->getMementos(HistoryMemento::OutgoingCall, 10);
		_qtCallBar->clearPhoneComboBox();
		_qtCallBar->clearPhoneComboBoxEditText();
		for (HistoryMap::iterator it = mementos->begin(); it != mementos->end(); it++) {
			HistoryMemento * memento = (*it).second;
			SipAddress sipAddress(memento->getPeer());

			HistoryMemento::State state = memento->getState();
			switch (state) {
			case HistoryMemento::IncomingCall:
				break;
			case HistoryMemento::OutgoingCall:
				_qtCallBar->addPhoneComboBoxItem(sipAddress.getUserName());
				break;
			case HistoryMemento::MissedCall:
				break;
			case HistoryMemento::RejectedCall:
				break;
			case HistoryMemento::OutgoingSmsOk:
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
				LOG_FATAL("unknown HistoryMemento state=" + String::fromNumber(state));
			}
		}
		OWSAFE_DELETE(mementos);
	}
}

void QtWengoPhone::exitEvent() {
	QCoreApplication::exit(EXIT_SUCCESS);
}

void QtWengoPhone::showHistory() {
	_ui->tabWidget->setCurrentWidget(_ui->tabHistory);
}

void QtWengoPhone::currentUserProfileWillDieEventHandlerSlot() {
	//Signal for re-initializing graphical components
	//when there is no user profile anymore
	userProfileDeleted();

	OWSAFE_DELETE(_qtFileTransfer);
	OWSAFE_DELETE(_qtIdle);
	OWSAFE_DELETE(_qtProfileBar);

	if (_qtContactList) {
		_ui->tabContactList->layout()->removeWidget(_qtContactList->getWidget());
		OWSAFE_DELETE(_qtContactList);
	}

	if (_qtHistoryWidget) {
		_ui->tabHistory->layout()->removeWidget(_qtHistoryWidget->getWidget());
		OWSAFE_DELETE(_qtHistoryWidget);
	}
}

void QtWengoPhone::userProfileInitializedEventHandlerSlot() {
	//Idle detection
	//FIXME: QtIdle must not use UserProfile but CUserProfile
	_qtIdle = new QtIdle(_cWengoPhone.getCUserProfileHandler().getCUserProfile()->getUserProfile(), _wengoPhoneWindow);

	//Create the profilebar
	//FIXME: QtProfileBar must not use ConnectHandler directly
	_qtProfileBar = new QtProfileBar(_cWengoPhone,
		*_cWengoPhone.getCUserProfileHandler().getCUserProfile(),
		_cWengoPhone.getCUserProfileHandler().getCUserProfile()->getUserProfile().getConnectHandler(),
		_ui->profileBar);
	SAFE_CONNECT_RECEIVER_TYPE(_qtProfileBar, SIGNAL(myPresenceStatusEventSignal(QVariant)),
		_qtSystray, SLOT(setSystrayIcon(QVariant)), Qt::QueuedConnection);

	//Add the profile bar
	int profileBarIndex = _ui->profileBar->addWidget(_qtProfileBar);
	_ui->profileBar->setCurrentIndex(profileBarIndex);

	_qtSystray->setTrayMenu();
	_qtSystray->setSystrayIcon(EnumPresenceState::MyPresenceStatusOk);

	_qtFileTransfer = new QtFileTransfer(this, _cWengoPhone.getWengoPhone().getCoIpManager());
}

void QtWengoPhone::proxyNeedsAuthenticationEventHandler(NetworkProxyDiscovery & sender, NetworkProxy networkProxy) {
	typedef PostEvent1<void (NetworkProxy networkProxy), NetworkProxy> MyPostEvent;
	MyPostEvent * event =
		new MyPostEvent(boost::bind(&QtWengoPhone::proxyNeedsAuthenticationEventHandlerThreadSafe, this, _1), networkProxy);
	postEvent(event);
}

void QtWengoPhone::wrongProxyAuthenticationEventHandler(NetworkProxyDiscovery & sender, NetworkProxy networkProxy) {
	typedef PostEvent1<void (NetworkProxy networkProxy), NetworkProxy> MyPostEvent;
	MyPostEvent * event =
		new MyPostEvent(boost::bind(&QtWengoPhone::proxyNeedsAuthenticationEventHandlerThreadSafe, this, _1), networkProxy);
	postEvent(event);
}

void QtWengoPhone::proxyNeedsAuthenticationEventHandlerThreadSafe(NetworkProxy networkProxy) {
	static QtHttpProxyLogin * httpProxy =
		new QtHttpProxyLogin(getWidget(),
			networkProxy.getServer(), networkProxy.getServerPort());

	int ret = httpProxy->show();

	if (ret == QDialog::Accepted) {
		NetworkProxy myNetworkProxy;
		myNetworkProxy.setServer(httpProxy->getProxyAddress());
		myNetworkProxy.setServerPort(httpProxy->getProxyPort());
		myNetworkProxy.setLogin(httpProxy->getLogin());
		myNetworkProxy.setPassword(httpProxy->getPassword());

		NetworkProxyDiscovery::getInstance().setProxySettings(myNetworkProxy);
	}
}

void QtWengoPhone::closeWindow() {
#if !defined(OS_MACOSX)
	_wengoPhoneWindow->showMinimized();
#else
	_wengoPhoneWindow->hide();
#endif
}

void QtWengoPhone::languageChanged() {
	LOG_DEBUG("retranslate main window ui");
	_ui->retranslateUi(_wengoPhoneWindow);
#if defined(OS_MACOSX)
	// Avoids translation of these menus on Mac OS X. Thus Qt
	// will put these under the Application menu
	_ui->actionShowConfig->setText("Preferences");
	_ui->actionShowAbout->setText("About");
#endif
}

void QtWengoPhone::showAddContact(ContactInfo contactInfo) {

	ensureVisible(false);

	if (_cWengoPhone.getCUserProfileHandler().getCUserProfile()) {

		//FIXME this method should not be called if no UserProfile has been set
		ContactProfile contactProfile;
		QtProfileDetails qtProfileDetails(*_cWengoPhone.getCUserProfileHandler().getCUserProfile(),
			contactProfile, _wengoPhoneWindow, tr("Add a Contact"));

		//FIXME to remove when wdeal will be able to handle SIP presence
		if (contactInfo.group == "WDeal") {
			qtProfileDetails.setHomePhone(QString::fromUtf8(contactInfo.sip.c_str()));
		} else {
			qtProfileDetails.setWengoName(QString::fromUtf8(contactInfo.wengoName.c_str()));
		}
		///

		if (contactInfo.group == "WDeal") {
			qtProfileDetails.setFirstName(QString::fromUtf8(contactInfo.wdealServiceTitle.c_str()));
		} else {
			qtProfileDetails.setFirstName(QString::fromUtf8(contactInfo.firstname.c_str()));
		}

		qtProfileDetails.setLastName(QString::fromUtf8(contactInfo.lastname.c_str()));
		qtProfileDetails.setCountry(QString::fromUtf8(contactInfo.country.c_str()));
		qtProfileDetails.setCity(QString::fromUtf8(contactInfo.city.c_str()));
		qtProfileDetails.setState(QString::fromUtf8(contactInfo.state.c_str()));
		qtProfileDetails.setGroup(QString::fromUtf8(contactInfo.group.c_str()));
		qtProfileDetails.setWebsite(QString::fromUtf8(contactInfo.website.c_str()));

		if (qtProfileDetails.show()) {
			_cWengoPhone.getCUserProfileHandler().getCUserProfile()->getCContactList().addContact(contactProfile);
		}
	}
}

void QtWengoPhone::ensureVisible(bool doMinimizeStuff) {
	if (doMinimizeStuff) {
		_wengoPhoneWindow->showMinimized();
	}
	_wengoPhoneWindow->activateWindow();
	_wengoPhoneWindow->showNormal();
}
