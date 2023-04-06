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
#include "QtBrowserWidget.h"
#include "QtIMAccountMonitor.h"
#include "callbar/QtCallBar.h"
#include "callbar/QtPhoneComboBox.h"
#include "contactlist/QtContactList.h"
#include "filetransfer/QtFileTransfer.h"
#include "history/QtHistoryWidget.h"
#include "imaccount/QtIMAccountManager.h"
#include "phonecall/QtContactCallListWidget.h"
#include "phonecall/QtPhoneCall.h"
#include "profile/QtProfileDetails.h"
#include "profilebar/QtProfileBar.h"
#include "statusbar/QtStatusBar.h"
#include "webservices/sms/QtSms.h"

#if (defined OS_WINDOWS) && (QT_EDITION == QT_EDITION_DESKTOP)
	#include "webdirectory/QtWebDirectory.h"
#endif

#include <control/CWengoPhone.h>
#include <control/history/CHistory.h>
#include <control/phoneline/CPhoneLine.h>
#include <control/profile/CUserProfile.h>
#include <control/profile/CUserProfileHandler.h>

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

#include <qtutil/ExpandButton.h>
#include <qtutil/CloseEventFilter.h>
#include <qtutil/Widget.h>
#include <qtutil/SafeConnect.h>
#include <qtutil/LanguageChangeEventFilter.h>

#include <cutil/global.h>
#include <thread/Thread.h>
#include <util/Logger.h>
#include <util/SafeDelete.h>
#include <webcam/WebcamDriver.h>

#include <QtGui/QtGui>

#if defined(OS_MACOSX)
	#include <Carbon/Carbon.h>
#endif

static const char * CSS_DIR = "css";

using namespace std;

QtWengoPhone::QtWengoPhone(CWengoPhone & cWengoPhone)
	: QObjectThreadSafe(NULL),
	_cWengoPhone(cWengoPhone) {

	_wengoPhoneWindow = NULL;
	_qtSystray = NULL;
	_qtSms = NULL;
	_qtContactList = NULL;
	_qtHistoryWidget = NULL;
	_qtContactCallListWidget = NULL;
	_qtIdle = NULL;
	_qtLanguage = NULL;
	_qtCallBar = NULL;
	_qtStatusBar = NULL;
	_qtFileTransfer = NULL;
	_qtBrowserWidget = NULL;
	_chatWindow = NULL;
	_activeTabBeforeCall = NULL;
#if (defined OS_WINDOWS) && (QT_EDITION == QT_EDITION_DESKTOP)
	_qtWebDirectory = NULL;
#endif

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

	loadStyleSheets();

	qRegisterMetaType<QVariant>("QVariant");

	//Initialize libwebcam for the Qt thread.
	WebcamDriver::apiInitialize();
	////

	//Translation
	_qtLanguage = new QtLanguage(this);

	_wengoPhoneWindow = new QMainWindow(NULL);

	LANGUAGE_CHANGE(_wengoPhoneWindow);

	_ui = new Ui::WengoPhoneWindow();
	_ui->setupUi(_wengoPhoneWindow);

#ifdef OS_MACOSX
	fixMacOSXMenus();
#endif

#ifdef OS_LINUX
	std::string data = AvatarList::getInstance().getDefaultAvatarPicture().getData();
	QPixmap defaultAvatar;
	defaultAvatar.loadFromData((uchar*) data.c_str(), data.size());
	_wengoPhoneWindow->setWindowIcon(QIcon(defaultAvatar));
#endif

	//Install the close event filter
	CloseEventFilter * closeEventFilter = new CloseEventFilter(this, SLOT(closeWindow()));
	_wengoPhoneWindow->installEventFilter(closeEventFilter);

	//QtCallBar
	_qtCallBar = new QtCallBar(_ui->callBarFrame);
	Widget::createLayout(_ui->callBarFrame)->addWidget(_qtCallBar);
	_qtCallBar->getQtPhoneComboBox()->setQtWengoPhone(this);

	//QtToolBar
	_qtToolBar = new QtToolBar(*this, _ui, _wengoPhoneWindow);

	//phoneComboBox
	SAFE_CONNECT(_qtCallBar, SIGNAL(phoneComboBoxClicked()), SLOT(phoneComboBoxClicked()));
	SAFE_CONNECT(_ui->tabWidget, SIGNAL(currentChanged(int)), SLOT(activeTabChanged(int)));

	//Buttons initialization
	initCallButtons();

	//QtHistoryWidget
	_qtHistoryWidget = NULL;

	//QtSystray
	_qtSystray = new QtSystray(this);

	// ConfigPanel expand button
	_configPanelButton = new ExpandButton(_ui->statusBar);
	_configPanelButton->setExpandDirection(ExpandButton::ED_Up);
	_configPanelButton->setText(tr("Audio && Video Settings"));

	// Tweak the size policy to prevent IE widget status bar messages from
	// hiding the button
	_configPanelButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
	_ui->statusBar->addPermanentWidget(_configPanelButton, 1);

	//QtStatusBar
	_qtStatusBar = new QtStatusBar(_cWengoPhone, _ui->statusBar);

#if (defined OS_WINDOWS) && (QT_EDITION == QT_EDITION_DESKTOP)
	_qtWebDirectory = new QtWebDirectory(0);
#endif

	installQtBrowserWidget();

	//QtConfigPanel
	QtConfigPanel * qtConfigPanel = new QtConfigPanel(_cWengoPhone, _ui->configPanelFrame);
	QVBoxLayout* layout = new QVBoxLayout(_ui->configPanelFrame);
	layout->setMargin(0);
	layout->addWidget(qtConfigPanel->getWidget());
	_ui->configPanelFrame->hide();

	SAFE_CONNECT_RECEIVER(_configPanelButton, SIGNAL(toggled(bool)), _ui->configPanelFrame, SLOT(setVisible(bool)));
	SAFE_CONNECT_RECEIVER(_qtStatusBar, SIGNAL(toggleConfigPanel()), _configPanelButton, SLOT(animateClick()));

	// Update sound icon
	SAFE_CONNECT_RECEIVER(qtConfigPanel, SIGNAL(volumeControlChanged()), _qtStatusBar, SLOT(updateSoundIconSlot()));
	////

	updatePresentation();

	Config & config = ConfigManager::getInstance().getCurrentConfig();
	mainWindowGeometry(config);

	if (!config.getCmdLineBackgroundModeEnable()) {
		_wengoPhoneWindow->show();
	}
}

void QtWengoPhone::loadStyleSheets() {
	Config & config = ConfigManager::getInstance().getCurrentConfig();
	QDir dir(QString::fromStdString(config.getResourcesDir()) + CSS_DIR);

	QStringList filters;
	filters << "*.css";
	QStringList cssList;
	Q_FOREACH(QFileInfo fileInfo, dir.entryInfoList(filters)) {
		QString path = fileInfo.absoluteFilePath();
		QFile file(path);
		if (!file.open(QIODevice::ReadOnly)) {
			LOG_WARN("Can't open " + path.toStdString());
			continue;
		}
		QByteArray content = file.readAll();
		cssList << QString::fromUtf8(content);
	}
	QString styleSheet = cssList.join("\n");
	qApp->setStyleSheet(styleSheet);
}

void QtWengoPhone::mainWindowGeometry(Config & config) {
	//default position and size given by Qt
	QPoint defaultPos = _wengoPhoneWindow->pos();
	QSize defaultSize = _wengoPhoneWindow->size();

	int profileWidth = config.getProfileWidth();
	if (profileWidth == 0) {
		// Make sure the toolbar is fully visible
		profileWidth = qMax(
			_ui->toolBar->sizeHint().width(),
			_wengoPhoneWindow->sizeHint().width()
			);
	}

	int profileHeight = config.getProfileHeight();

	//Main window size and position saved
	_wengoPhoneWindow->resize(profileWidth, profileHeight);
	_wengoPhoneWindow->move(QPoint(config.getProfilePosX(), config.getProfilePoxY()));

	//tests if the Wengophone is visible, if not sends it back to its default position and size
	QDesktopWidget* desktop = QApplication::desktop();
	if (desktop->screenNumber(_wengoPhoneWindow) == -1) {
		LOG_DEBUG("Main window is NOT visible !!");
		_wengoPhoneWindow->resize(defaultSize);
	 	_wengoPhoneWindow->move(defaultPos);
	}
}

QWidget * QtWengoPhone::getWidget() const {
	return _wengoPhoneWindow;
}

QtProfileBar * QtWengoPhone::getQtProfileBar() const {
	return _ui->profileBar;
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
	_qtHistoryWidget->setQtCallBar(_qtCallBar);

	if (!_ui->tabHistory->layout()) {
		Widget::createLayout(_ui->tabHistory);
	}

	_qtHistoryWidget->getWidget()->setParent(_ui->tabHistory);
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
		//Widget is deleted automatically
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
	QMessageBox::information(_wengoPhoneWindow, tr("@product@ - Call Error"),
		tr("Please hold all the phone calls before to place a new one"), QMessageBox::Ok);
}

void QtWengoPhone::addPhoneCall(QtPhoneCall * qtPhoneCall) {
	_activeTabBeforeCall = _ui->tabWidget->currentWidget();

	QtContactCallListWidget * qtContactCallListWidget = new QtContactCallListWidget(_cWengoPhone, NULL);
	_ui->tabWidget->addTab(qtContactCallListWidget, tr("Call"));
	_ui->tabWidget->setCurrentWidget(qtContactCallListWidget);
	qtContactCallListWidget->addPhoneCall(qtPhoneCall);

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
					if (qtContactCallListWidget->hasPhoneCall(targetCall)) {
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
		//disabled some actions if no SIP Account is used
		bool hasSipAccount = _cWengoPhone.getCUserProfileHandler().getCUserProfile()->getUserProfile().hasSipAccount();

		_ui->actionShowWengoAccount->setEnabled(_cWengoPhone.getCUserProfileHandler().getCUserProfile()->getUserProfile().hasWengoAccount());
		_ui->actionSendSms->setEnabled(hasSipAccount);
		_ui->actionCreateConferenceCall->setEnabled(hasSipAccount);
		_ui->actionSearchWengoContact->setEnabled(hasSipAccount);
	}
}

void QtWengoPhone::dialpad(const std::string & tone) {
	if (_cWengoPhone.getCUserProfileHandler().getCUserProfile()) {
		_qtCallBar->setPhoneComboBoxEditText(_qtCallBar->getPhoneComboBoxCurrentText() + tone);
	}
}

void QtWengoPhone::prepareToExitApplication() {
	Config & config = ConfigManager::getInstance().getCurrentConfig();

	// check for pending calls
	CUserProfile *cUserProfile = _cWengoPhone.getCUserProfileHandler().getCUserProfile();
	if (cUserProfile) {
		CPhoneLine *cPhoneLine = cUserProfile->getCPhoneLine();
		if (cPhoneLine) {
			if (cPhoneLine->hasPendingCalls()) {

				if (QMessageBox::question(
					getWidget(),
					tr("@product@ - Warning"),
					tr("You have unfinished call(s).") + "\n" +
						tr("Are you sure you want to exit the application?"),
					tr("&Exit"),
					tr("&Cancel")
				) == 1)  {
					return;
				}
			}
		}
	}
	////


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

	//destroyed chatWindow so that chats are saved
	OWSAFE_DELETE(_chatWindow);
	////

	_cWengoPhone.terminate();
}

void QtWengoPhone::phoneComboBoxClicked() {

	_qtCallBar->clearPhoneComboBox();
	_qtCallBar->clearPhoneComboBoxEditText();
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

	// Reset _qtIMAccountMonitor so that it does not emit signals anymore
	_qtIMAccountMonitor.reset(0);

	OWSAFE_DELETE(_qtFileTransfer);
	OWSAFE_DELETE(_qtIdle);
	_ui->profileBar->reset();

	if (_qtContactList) {
		_ui->tabContactList->layout()->removeWidget(_qtContactList->getWidget());
		OWSAFE_DELETE(_qtContactList);
	}

	if (_qtHistoryWidget) {
		_ui->tabHistory->layout()->removeWidget(_qtHistoryWidget->getWidget());
		OWSAFE_DELETE(_qtHistoryWidget);
	}

	uninstallQtBrowserWidget();
}

void QtWengoPhone::userProfileInitializedEventHandlerSlot() {
	CUserProfile * cUserProfile = _cWengoPhone.getCUserProfileHandler().getCUserProfile();

	//Idle detection
	//FIXME: QtIdle must not use UserProfile but CUserProfile
	_qtIdle = new QtIdle(cUserProfile->getUserProfile(), _wengoPhoneWindow);

	// Create account monitor
	_qtIMAccountMonitor.reset( new QtIMAccountMonitor(0, cUserProfile) );

	//Create the profilebar
	_ui->profileBar->init(&_cWengoPhone, cUserProfile, _qtIMAccountMonitor.get());

	// Systray
	SAFE_CONNECT_RECEIVER_TYPE(_qtIMAccountMonitor.get(), SIGNAL(imAccountAdded(QString)),
		_qtSystray, SLOT(updateSystrayIcon()), Qt::QueuedConnection);
	SAFE_CONNECT_RECEIVER_TYPE(_qtIMAccountMonitor.get(), SIGNAL(imAccountUpdated(QString)),
		_qtSystray, SLOT(updateSystrayIcon()), Qt::QueuedConnection);
	SAFE_CONNECT_RECEIVER_TYPE(_qtIMAccountMonitor.get(), SIGNAL(imAccountRemoved(QString)),
		_qtSystray, SLOT(updateSystrayIcon()), Qt::QueuedConnection);

	_qtSystray->setTrayMenu();
	_qtSystray->updateSystrayIcon();

	_qtFileTransfer = new QtFileTransfer(this, _cWengoPhone.getWengoPhone().getCoIpManager());

	//menu
	_qtToolBar->userProfileIsInitialized();
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
	_wengoPhoneWindow->hide();
}

#if defined(OS_MACOSX)
void QtWengoPhone::fixMacOSXMenus() {
	// Avoids translation of these menus on Mac OS X. Thus Qt
	// will put these under the Application menu
	_ui->actionShowConfig->setText("Preferences");
	_ui->actionShowAbout->setText("About");
}
#endif

void QtWengoPhone::languageChanged() {
	LOG_DEBUG("retranslate main window ui");
	_ui->retranslateUi(_wengoPhoneWindow);
	_qtToolBar->retranslateUi();
#if defined(OS_MACOSX)
	fixMacOSXMenus();
#endif

	_configPanelButton->setText(tr("Audio && Video Settings"));

#if (defined OS_WINDOWS) && (QT_EDITION == QT_EDITION_DESKTOP)
	Config & config = ConfigManager::getInstance().getCurrentConfig();
	if (config.getIEActiveXEnable() && _qtBrowserWidget) {
		_ui->tabWidget->setTabText(_qtBrowserWidgetTabIndex, tr("Home"));
	}
#endif

	if (_qtHistoryWidget) {
		_qtHistoryWidget->retranslateUi();
	}
}

void QtWengoPhone::showAddContact(ContactInfo contactInfo) {

	ensureVisible();

	if (_cWengoPhone.getCUserProfileHandler().getCUserProfile()) {

		//FIXME this method should not be called if no UserProfile has been set
		ContactProfile contactProfile;
		QtProfileDetails qtProfileDetails(*_cWengoPhone.getCUserProfileHandler().getCUserProfile(),
			contactProfile, _wengoPhoneWindow, tr("Add a Contact"));

		//FIXME to remove when wdeal will be able to handle SIP presence
		if (contactInfo.group == "WDeal") {
			qtProfileDetails.setHomePhone(QUrl::fromPercentEncoding(QByteArray(contactInfo.sip.c_str())));
		} else {
			qtProfileDetails.setWengoName(QUrl::fromPercentEncoding(QByteArray(contactInfo.wengoName.c_str())));
		}
		///

		if (contactInfo.group == "WDeal") {
			qtProfileDetails.setFirstName(QUrl::fromPercentEncoding(QByteArray(contactInfo.wdealServiceTitle.c_str())));
		} else {
			qtProfileDetails.setFirstName(QUrl::fromPercentEncoding(QByteArray(contactInfo.firstname.c_str())));
		}

		qtProfileDetails.setLastName(QUrl::fromPercentEncoding(QByteArray(contactInfo.lastname.c_str())));
		qtProfileDetails.setCountry(QUrl::fromPercentEncoding(QByteArray(contactInfo.country.c_str())));
		qtProfileDetails.setCity(QUrl::fromPercentEncoding(QByteArray(contactInfo.city.c_str())));
		qtProfileDetails.setState(QUrl::fromPercentEncoding(QByteArray(contactInfo.state.c_str())));
		qtProfileDetails.setGroup(QUrl::fromPercentEncoding(QByteArray(contactInfo.group.c_str())));
		qtProfileDetails.setWebsite(QUrl::fromPercentEncoding(QByteArray(contactInfo.website.c_str())));

		if (qtProfileDetails.show()) {
			_cWengoPhone.getCUserProfileHandler().getCUserProfile()->getCContactList().addContact(contactProfile);
		}
	}
}

void QtWengoPhone::ensureVisible() {
	_wengoPhoneWindow->activateWindow();
	_wengoPhoneWindow->showNormal();
	_wengoPhoneWindow->raise();
}

void QtWengoPhone::activeTabChanged(int index) {
	QtContactCallListWidget *qtContactCallListWidget = qobject_cast<QtContactCallListWidget*>(_ui->tabWidget->currentWidget());
	if (qtContactCallListWidget) {
		QtPhoneCall * qtPhoneCall = qtContactCallListWidget->getFirstQtPhoneCall();
		if (qtPhoneCall) {
			_qtStatusBar->updateEncryptionState(qtPhoneCall->isCallEncrypted());
		}
	} else {
		_qtStatusBar->updateEncryptionState(false);
	}
}

void QtWengoPhone::installQtBrowserWidget() {
	_qtBrowserWidget = new QtBrowserWidget(*this);
#if (defined OS_WINDOWS) && (QT_EDITION == QT_EDITION_DESKTOP)
	Config & config = ConfigManager::getInstance().getCurrentConfig();
	if (config.getIEActiveXEnable()) {
		_qtBrowserWidgetTabIndex = _ui->tabWidget->insertTab(_ui->tabWidget->count(), _qtBrowserWidget->getWidget(), tr("Home"));
		_ui->tabWidget->setCurrentWidget(_qtBrowserWidget->getWidget());
		_qtBrowserWidget->loadDefaultURL();
	}
#else
	_ui->tabWidget->setCurrentIndex(0);
#endif
}

void QtWengoPhone::uninstallQtBrowserWidget() {
#if (defined OS_WINDOWS) && (QT_EDITION == QT_EDITION_DESKTOP)
	Config & config = ConfigManager::getInstance().getCurrentConfig();
	if (config.getIEActiveXEnable() && _qtBrowserWidget) {
		_ui->tabWidget->widget(_qtBrowserWidgetTabIndex)->layout()->removeWidget(_qtBrowserWidget->getWidget());
		_ui->tabWidget->removeTab(_qtBrowserWidgetTabIndex);
		OWSAFE_DELETE(_qtBrowserWidget);
	}
#endif
}
