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
#include "profilebar/QtProfileBar.h"
#include "statusbar/QtStatusBar.h"
#include "webservices/directory/QtWsDirectory.h"
#include "webservices/sms/QtSms.h"

#include <control/CWengoPhone.h>
#include <control/profile/CUserProfile.h>
#include <control/profile/CUserProfileHandler.h>
#include <control/history/CHistory.h>

#include <model/WengoPhone.h>
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
#include <model/profile/UserProfile.h>

#include <imwrapper/EnumPresenceState.h>

#include <qtutil/CloseEventFilter.h>
#include <qtutil/Widget.h>
#include <qtutil/SafeConnect.h>

#include <cutil/global.h>
#include <thread/Thread.h>
#include <util/Logger.h>
#include <util/SafeDelete.h>

#include <QtGui/QtGui>

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

	_ui = new Ui::WengoPhoneWindow();
	_ui->setupUi(_wengoPhoneWindow);

#ifdef OS_LINUX
	_wengoPhoneWindow->setWindowIcon(QIcon(QPixmap(":/pics/avatar_default.png")));
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

	//QtDialpad
	QtDialpad * qtDialpad = new QtDialpad(this);
	Widget::createLayout(_ui->tabDialpad)->addWidget(qtDialpad->getWidget());

	//QtHistoryWidget
	_qtHistoryWidget = NULL;

	//QtSystray
	_qtSystray = new QtSystray(this);

	//QtStatusBar
	_qtStatusBar = new QtStatusBar(_cWengoPhone, _ui->statusBar);

	Config & config = ConfigManager::getInstance().getCurrentConfig();

	//QtBrowserWidget
	_qtBrowserWidget = new QtBrowserWidget(*this);
#if (defined OS_WINDOWS) && (defined QT_COMMERCIAL)
	if (config.getIEActiveXEnable()) {
		_ui->tabWidget->insertTab(_ui->tabWidget->count(), _qtBrowserWidget->getWidget(), tr("Home"));
		_ui->tabWidget->setCurrentWidget(_qtBrowserWidget->getWidget());
	}
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
	Widget::createLayout(_ui->tabContactList)->addWidget(qtContactList->getWidget());

	LOG_DEBUG("QtContactList added");
}

QtContactList * QtWengoPhone::getQtContactList() const {
	return _qtContactList;
}

void QtWengoPhone::setQtHistoryWidget(QtHistoryWidget * qtHistoryWidget) {
	_qtHistoryWidget = qtHistoryWidget;
	Widget::createLayout(_ui->tabHistory)->addWidget(_qtHistoryWidget->getWidget());

	LOG_DEBUG("QtHistoryWidget added");
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
		(QtContactCallListWidget *) _ui->tabWidget->currentWidget();
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
		widget = (QtContactCallListWidget *) _ui->tabWidget->widget(i);
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
	_activeTabBeforeCall = _ui->tabWidget->currentWidget();

	QtContactCallListWidget * qtContactCallListWidget = new QtContactCallListWidget(_cWengoPhone,_wengoPhoneWindow);
	_ui->tabWidget->addTab(qtContactCallListWidget,tr("Call"));
	_ui->tabWidget->setCurrentWidget(qtContactCallListWidget);
	qtContactCallListWidget->addPhoneCall(qtPhoneCall);

	SAFE_CONNECT(qtContactCallListWidget, SIGNAL(startConferenceSignal(PhoneCall *, PhoneCall *)),
			SLOT(addToConference(PhoneCall *, PhoneCall *)));

	_qtCallBar->setEnabledHangUpButton(true);

	if (qtPhoneCall->isIncoming()) {
		_qtCallBar->setEnabledCallButton(true);
	}

	_wengoPhoneWindow->showNormal();
	_wengoPhoneWindow->raise();
	_wengoPhoneWindow->setVisible(true);
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
					if (phoneLine != NULL) {
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
			_qtCallBar->setPhoneComboBoxEditText(_qtCallBar->getPhoneComboBoxCurrentText() + tone);
		}
	}
}

void QtWengoPhone::exitApplication() {
	Config & config = ConfigManager::getInstance().getCurrentConfig();

	_qtSystray->hide();
	OWSAFE_DELETE(_qtSystray);

	//Save the window size
	QSize winsize = _wengoPhoneWindow->size();
	config.set(Config::PROFILE_WIDTH, winsize.width());
	config.set(Config::PROFILE_HEIGHT, winsize.height());

	//Save the window position
	QPoint winpos = _wengoPhoneWindow->pos();
	config.set(Config::PROFILE_POSX,winpos.x());
	config.set(Config::PROFILE_POSY,winpos.y());

	QApplication::closeAllWindows();
	QCoreApplication::processEvents();
	QCoreApplication::exit(EXIT_SUCCESS);
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
				_qtCallBar->addPhoneComboBoxItem(QIcon(QPixmap(":/pics/history/call_incoming.png")),
					sipAddress.getUserName());
				break;
			case HistoryMemento::OutgoingCall:
				_qtCallBar->addPhoneComboBoxItem(QIcon(QPixmap(":/pics/history/call_outgoing.png")),
					sipAddress.getUserName());
				break;
			case HistoryMemento::MissedCall:
				_qtCallBar->addPhoneComboBoxItem(QIcon(QPixmap(":/pics/history/call_missed.png")),
					sipAddress.getUserName());
				break;
			case HistoryMemento::RejectedCall:
				_qtCallBar->addPhoneComboBoxItem(QIcon(QPixmap(":/pics/history/call_missed.png")),
					sipAddress.getUserName());
				break;
			case HistoryMemento::OutgoingSmsOk:
				_qtCallBar->addPhoneComboBoxItem(QIcon(QPixmap(":/pics/history/sms_sent.png")),
					sipAddress.getUserName());
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
	}
}

void QtWengoPhone::exitEvent() {
	QCoreApplication::exit(EXIT_SUCCESS);
}

void QtWengoPhone::showHistory() {
	_ui->tabWidget->setCurrentWidget(_ui->tabHistory);
}

void QtWengoPhone::currentUserProfileWillDieEventHandlerSlot() {
	OWSAFE_DELETE(_qtFileTransfer);

	OWSAFE_DELETE(_qtIdle);

	if (_qtProfileBar) {
		_ui->profileBar->layout()->removeWidget(_qtProfileBar);
		OWSAFE_DELETE(_qtProfileBar);
	}

	if (_qtContactList) {
		_ui->profileBar->layout()->removeWidget(_qtContactList->getWidget());
		_qtContactList->cleanup();
		//_contactList is deleted in CContactList
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
	_ui->profileBar->widget(profileBarIndex)->setLayout(new QGridLayout());

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
	_wengoPhoneWindow->showMinimized();
	_wengoPhoneWindow->hide();
}
