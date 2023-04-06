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

#include "QtToolBar.h"

#include "ui_WengoPhoneWindow.h"

#include <cutil/global.h>

#include "QtWengoPhone.h"
#include "QtAbout.h"
#include "QtNoWengoAlert.h"
#include "profile/QtProfileDetails.h"
#include "profile/QtUserProfileHandler.h"
#include "config/QtWengoConfigDialog.h"
#include "webservices/sms/QtSms.h"
#include "phonecall/QtContactCallListWidget.h"
#include "phonecall/QtPhoneCall.h"
#include "contactlist/QtContactList.h"
#include "filetransfer/QtFileTransfer.h"
#include "filetransfer/QtFileTransferWidget.h"
#include "conference/QtConferenceCallWidget.h"

#if (defined OS_WINDOWS) && (QT_EDITION == QT_EDITION_DESKTOP)
	#include "webdirectory/QtWebDirectory.h"
#endif

#include <control/CWengoPhone.h>
#include <control/history/CHistory.h>
#include <control/phonecall/CPhoneCall.h>
#include <control/phoneline/CPhoneLine.h>
#include <control/profile/CUserProfile.h>
#include <control/profile/CUserProfileHandler.h>

#include <model/contactlist/ContactProfile.h>
#include <model/phonecall/ConferenceCall.h>
#include <model/account/wengo/WengoAccount.h>
#include <model/profile/UserProfile.h>
#include <model/history/History.h>
#include <model/config/ConfigManager.h>
#include <model/config/Config.h>

#include <util/Logger.h>

#include <qtutil/WidgetBackgroundImage.h>
#include <qtutil/SafeConnect.h>
#include <qtutil/Object.h>
#include <qtutil/WidgetFactory.h>

#include <QtGui/QMessageBox>
//#include <QtGui/QToolButton>

static const int TOOLBAR_BUTTON_MINIMUM_WIDTH = 60;

static const char* TOOLBARMODE_HIDDEN = "Hidden";
static const char* TOOLBARMODE_ICONS_ONLY = "IconsOnly";
static const char* TOOLBARMODE_TEXT_UNDER_ICONS = "TextUnderIcons";


void QtToolBar::connectOrHide(bool condition, QAction* action, QObject* receiver, const char* slot) {
	if (condition) {
		SAFE_CONNECT_RECEIVER(action, SIGNAL(triggered()), receiver, slot);
	} else {
#if QT_VERSION >= 0x040200
		Q_FOREACH(QWidget* widget, action->associatedWidgets()) {
			widget->removeAction(action);
		}
#else
		Q_FOREACH(QMenu* menu, _ui->menuBar->findChildren<QMenu*>()) {
			menu->removeAction(action);
		}
		_ui->toolBar->removeAction(action);
#endif
	}
}


void QtToolBar::connectOrHide(WsUrl::Page page, QAction* action, QObject* receiver, const char* slot) {
	connectOrHide(WsUrl::hasPage(page), action, receiver, slot);
}


QtToolBar::QtToolBar(QtWengoPhone & qtWengoPhone, Ui::WengoPhoneWindow * qtWengoPhoneUi, QWidget * parent)
	: QObject(parent),
	_qtWengoPhone(qtWengoPhone),
	_cWengoPhone(_qtWengoPhone.getCWengoPhone()) {

	_ui = qtWengoPhoneUi;

	Config & config = ConfigManager::getInstance().getCurrentConfig();
	bool canSendSms = config.getSmsFeatureEnabled();
	bool canDoConference = config.getAudioConferenceFeatureEnabled();

	SAFE_CONNECT(&_qtWengoPhone, SIGNAL(userProfileDeleted()), SLOT(userProfileDeleted()));

	// actions initialization
	_ui->actionLogOff->setEnabled(false);
	////

	//menuWengo
	connectOrHide(WsUrl::Account, _ui->actionShowWengoAccount, this, SLOT(showWengoAccount()) );
	SAFE_CONNECT(_ui->actionEditMyProfile, SIGNAL(triggered()), SLOT(editMyProfile()));
	SAFE_CONNECT(_ui->actionLogOff, SIGNAL(triggered()), SLOT(logOff()));
	connectOrHide(WsUrl::CallOut, _ui->actionWengoServices, this, SLOT(showWengoServices()) );
	SAFE_CONNECT_RECEIVER(_ui->actionClose, SIGNAL(triggered()), &_qtWengoPhone, SLOT(closeWindow()));
	SAFE_CONNECT_RECEIVER(_ui->actionQuit, SIGNAL(triggered()), &_qtWengoPhone, SLOT(prepareToExitApplication()));

	//menuContacts
	SAFE_CONNECT(_ui->actionAddContact, SIGNAL(triggered()), SLOT(addContact()));
	connectOrHide(WsUrl::Directory, _ui->actionSearchWengoContact, this, SLOT(searchWengoContact()) );
	SAFE_CONNECT(_ui->actionShowHideOfflineContacts, SIGNAL(triggered()), SLOT(showHideOfflineContacts()));
	SAFE_CONNECT(_ui->actionShowHideContactGroups, SIGNAL(triggered()), SLOT(showHideContactGroups()));

	//menuActions
	connectOrHide(canDoConference, _ui->actionCreateConferenceCall, this, SLOT(createConferenceCall()));
	connectOrHide(canSendSms, _ui->actionSendSms, this, SLOT(sendSms()));
	SAFE_CONNECT(_ui->actionAcceptCall, SIGNAL(triggered()), SLOT(acceptCall()));
	SAFE_CONNECT(_ui->actionHangUpCall, SIGNAL(triggered()), SLOT(hangUpCall()));
	SAFE_CONNECT(_ui->actionHoldResumeCall, SIGNAL(triggered()), SLOT(holdResumeCall()));

	//menuHelp
	connectOrHide(WsUrl::Forum, _ui->actionShowWengoForum, this, SLOT(showWengoForum()));
	connectOrHide(WsUrl::FAQ, _ui->actionWengoFAQ, this, SLOT(showWengoFAQ()));
	SAFE_CONNECT(_ui->actionShowAbout, SIGNAL(triggered()), SLOT(showAbout()));

	//menuTools
	SAFE_CONNECT(_ui->actionShowConfig, SIGNAL(triggered()), SLOT(showConfig()));
	SAFE_CONNECT(_ui->actionShowFileTransfer, SIGNAL(triggered()), SLOT(showFileTransferWindow()));
	SAFE_CONNECT(_ui->actionToggleVideo, SIGNAL(triggered()), SLOT(toggleVideo()));
	QActionGroup* toolBarActionGroup = new QActionGroup(_ui->toolBar);
	toolBarActionGroup->setExclusive(true);
	toolBarActionGroup->addAction(_ui->actionToolBarHidden);
	toolBarActionGroup->addAction(_ui->actionToolBarTextUnderIcons);
	toolBarActionGroup->addAction(_ui->actionToolBarIconsOnly);
	SAFE_CONNECT(toolBarActionGroup, SIGNAL(triggered(QAction*)), SLOT(updateToolBarFromAction(QAction*)));

	//menuClearHistory
	SAFE_CONNECT(_ui->actionClearOutgoingCalls, SIGNAL(triggered()), SLOT(clearHistoryOutgoingCalls()));
	SAFE_CONNECT(_ui->actionClearIncomingCalls, SIGNAL(triggered()), SLOT(clearHistoryIncomingCalls()));
	SAFE_CONNECT(_ui->actionClearMissedCalls, SIGNAL(triggered()), SLOT(clearHistoryMissedCalls()));
	SAFE_CONNECT(_ui->actionClearRejectedCalls, SIGNAL(triggered()), SLOT(clearHistoryRejectedCalls()));
	SAFE_CONNECT(_ui->actionClearChatSessions, SIGNAL(triggered()), SLOT(clearHistoryChatSessions()));
	connectOrHide(canSendSms, _ui->actionClearSms, this, SLOT(clearHistorySms()));
	SAFE_CONNECT(_ui->actionClearAll, SIGNAL(triggered()), SLOT(clearHistoryAll()));

	//Open chat window button
	_ui->actionOpenChatWindow->setEnabled(false);
	SAFE_CONNECT(_ui->actionOpenChatWindow, SIGNAL(triggered()), SLOT(showChatWindow()));

	//Get notified of config changes
	config.valueChangedEvent += boost::bind(&QtToolBar::configChangedEventHandler, this, _1);
	SAFE_CONNECT(this, SIGNAL(configChanged(QString)), SLOT(slotConfigChanged(QString)) );

	// Update toolbar mode
	updateToolBarFromConfig();
	
	//a menu is about to be displayed
	SAFE_CONNECT(_ui->menuActions, SIGNAL(aboutToShow()), SLOT(updateMenuActions()));
}

QtToolBar::~QtToolBar() {
}

QWidget * QtToolBar::getWidget() const {
	return _qtWengoPhone.getWidget();
}

void QtToolBar::showWengoAccount() {
	if (_cWengoPhone.getCUserProfileHandler().getCUserProfile()) {
		//WARNING should not be called when no UserProfile set
		WsUrl::showWengoAccount();
	}
}

void QtToolBar::editMyProfile() {
	if (_cWengoPhone.getCUserProfileHandler().getCUserProfile()) {
		QWidget * parent = qobject_cast<QWidget *>(sender()->parent());

		//FIXME this method should not be called if no UserProfile has been set
		QtProfileDetails qtProfileDetails(*_cWengoPhone.getCUserProfileHandler().getCUserProfile(),
			_cWengoPhone.getCUserProfileHandler().getCUserProfile()->getUserProfile(),
			parent, tr("Edit My Profile"));

		//TODO UserProfile must be updated if QtProfileDetails was accepted
		qtProfileDetails.show();
	}
}

void QtToolBar::addContact() {
	if (_cWengoPhone.getCUserProfileHandler().getCUserProfile()) {
		QWidget * parent = qobject_cast<QWidget *>(sender()->parent());

		//FIXME this method should not be called if no UserProfile has been set
		ContactProfile contactProfile;
		QtProfileDetails qtProfileDetails(*_cWengoPhone.getCUserProfileHandler().getCUserProfile(),
			contactProfile, parent, tr("Add a Contact"));
		if (qtProfileDetails.show()) {
			_cWengoPhone.getCUserProfileHandler().getCUserProfile()->getCContactList().addContact(contactProfile);
		}
	}
}

void QtToolBar::showConfig() {
	QtWengoConfigDialog::showInstance(_cWengoPhone);
}

void QtToolBar::updateShowHideOfflineContactsAction() {
	Config & config = ConfigManager::getInstance().getCurrentConfig();
	QString text;
	if (config.getShowOfflineContacts()) {
		text = tr("Hide Unavailable Contacts");
	} else {
		text = tr("Show Unavailable Contacts");
	}

	_ui->actionShowHideOfflineContacts->setText(text);
}

void QtToolBar::updateShowHideContactGroupsAction() {
	Config & config = ConfigManager::getInstance().getCurrentConfig();
	QString text;
	if (config.getShowGroups()) {
		text = tr("Hide Groups");
	} else {
		text = tr("Show Groups");
	}

	_ui->actionShowHideContactGroups->setText(text);
}

void QtToolBar::updateToggleVideoAction() {
	Config & config = ConfigManager::getInstance().getCurrentConfig();

	bool videoEnabled = config.getVideoEnable();
	QString text;
	if (videoEnabled) {
		text = tr("Disable &Video Calls");
	} else {
		text = tr("Enable &Video Calls");
	}

	_ui->actionToggleVideo->setText(text);
	_ui->actionToggleVideo->setEnabled(!config.getVideoWebcamDevice().empty());
}

void QtToolBar::showWengoForum() {
	WsUrl::showWengoForum();
}

void QtToolBar::showWengoServices() {
	WsUrl::showWengoCallOut();
}

void QtToolBar::searchWengoContact() {
#if (defined OS_WINDOWS) && (QT_EDITION == QT_EDITION_DESKTOP)
	QtWebDirectory * qtWebDirectory = _qtWengoPhone.getQtWebDirectory();
	if (qtWebDirectory) {
		qtWebDirectory->raise();
	}
#else
	WsUrl::showWengoSmartDirectory();
#endif
}

void QtToolBar::showAbout() {
	QWidget * parent = qobject_cast<QWidget *>(sender()->parent());
	static QtAbout * qtAbout = new QtAbout(parent);
	qtAbout->getWidget()->show();
}

void QtToolBar::sendSms() {

	CUserProfile * cuserprofile = _cWengoPhone.getCUserProfileHandler().getCUserProfile();
	if (cuserprofile) {
		if (cuserprofile->getUserProfile().hasWengoAccount()) {
			QtSms * qtSms = _qtWengoPhone.getQtSms();
			if (qtSms) {
				QWidget * parent = qobject_cast<QWidget *>(sender()->parent());
				qtSms->getWidget()->setParent(parent, Qt::Dialog);
				qtSms->getWidget()->show();
			}
			return;
		} else {
			QtNoWengoAlert noWengoAlert(NULL, _qtWengoPhone);
			noWengoAlert.getQDialog()->exec();
		}
	}
}

void QtToolBar::createConferenceCall() {

	CUserProfile * cUserProfile = _cWengoPhone.getCUserProfileHandler().getCUserProfile();
	if (cUserProfile) {

		IPhoneLine * phoneLine = cUserProfile->getUserProfile().getActivePhoneLine();
		if (phoneLine) {
			if (phoneLine->getActivePhoneCall()) {
				QMessageBox::information(
					_qtWengoPhone.getWidget(),
					tr("@product@ - Proceed Conference"), 
					tr("<b>Conference can't be started while there are active phone calls.</b><br><br>"
					"Finish all phone calls and try again.")
					);
			} else {
				QtConferenceCallWidget conferenceDialog(_qtWengoPhone.getWidget(), _cWengoPhone, phoneLine);
				conferenceDialog.exec();
			}
		}
	}
}

void QtToolBar::showWengoFAQ() {
	WsUrl::showWengoFAQ();
}

void QtToolBar::showHideOfflineContacts() {
	QtContactList * qtContactList = _qtWengoPhone.getQtContactList();
	if (qtContactList) {
		Config & config = ConfigManager::getInstance().getCurrentConfig();
		config.set(Config::GENERAL_SHOW_OFFLINE_CONTACTS_KEY, !config.getShowOfflineContacts());
		qtContactList->updatePresentation();

		updateShowHideOfflineContactsAction();
	}
}

void QtToolBar::showHideContactGroups() {
	QtContactList * qtContactList = _qtWengoPhone.getQtContactList();
	if (qtContactList) {
		Config & config = ConfigManager::getInstance().getCurrentConfig();
		config.set(Config::GENERAL_SHOW_GROUPS_KEY, !config.getShowGroups());
		qtContactList->updatePresentation();

		updateShowHideContactGroupsAction();
	}
}

void QtToolBar::toggleVideo() {
	Config & config = ConfigManager::getInstance().getCurrentConfig();
	config.set(Config::VIDEO_ENABLE_KEY, !config.getVideoEnable());
}

void QtToolBar::clearHistoryOutgoingCalls() {
	if (_cWengoPhone.getCUserProfileHandler().getCUserProfile() &&
		_cWengoPhone.getCUserProfileHandler().getCUserProfile()->getCHistory()) {
		_cWengoPhone.getCUserProfileHandler().getCUserProfile()->getCHistory()->clear(HistoryMemento::OutgoingCall);
	}
}

void QtToolBar::clearHistoryIncomingCalls() {
	if (_cWengoPhone.getCUserProfileHandler().getCUserProfile() &&
		_cWengoPhone.getCUserProfileHandler().getCUserProfile()->getCHistory()) {
		_cWengoPhone.getCUserProfileHandler().getCUserProfile()->getCHistory()->clear(HistoryMemento::IncomingCall);
	}
}

void QtToolBar::clearHistoryMissedCalls() {
	if (_cWengoPhone.getCUserProfileHandler().getCUserProfile() &&
		_cWengoPhone.getCUserProfileHandler().getCUserProfile()->getCHistory()) {
		_cWengoPhone.getCUserProfileHandler().getCUserProfile()->getCHistory()->clear(HistoryMemento::MissedCall);
	}
}

void QtToolBar::clearHistoryRejectedCalls() {
	if (_cWengoPhone.getCUserProfileHandler().getCUserProfile() &&
		_cWengoPhone.getCUserProfileHandler().getCUserProfile()->getCHistory()) {
		_cWengoPhone.getCUserProfileHandler().getCUserProfile()->getCHistory()->clear(HistoryMemento::RejectedCall);
	}
}

void QtToolBar::clearHistoryChatSessions() {
	if (_cWengoPhone.getCUserProfileHandler().getCUserProfile() &&
		_cWengoPhone.getCUserProfileHandler().getCUserProfile()->getCHistory()) {
		_cWengoPhone.getCUserProfileHandler().getCUserProfile()->getCHistory()->clear(HistoryMemento::ChatSession);
	}
}

void QtToolBar::clearHistorySms() {
	if (_cWengoPhone.getCUserProfileHandler().getCUserProfile() &&
		_cWengoPhone.getCUserProfileHandler().getCUserProfile()->getCHistory()) {
		_cWengoPhone.getCUserProfileHandler().getCUserProfile()->getCHistory()->clear(HistoryMemento::OutgoingSmsOk);
	}
}

void QtToolBar::clearHistoryAll() {
	if (_cWengoPhone.getCUserProfileHandler().getCUserProfile() &&
		_cWengoPhone.getCUserProfileHandler().getCUserProfile()->getCHistory()) {
		_cWengoPhone.getCUserProfileHandler().getCUserProfile()->getCHistory()->clear(HistoryMemento::Any);
	}
}

void QtToolBar::logOff() {
	// check for pending calls
	CUserProfile *cUserProfile = _cWengoPhone.getCUserProfileHandler().getCUserProfile();
	if (cUserProfile) {
		CPhoneLine *cPhoneLine = cUserProfile->getCPhoneLine();
		if (cPhoneLine) {
			if (cPhoneLine->hasPendingCalls()) {
				
				if (QMessageBox::question(
					_qtWengoPhone.getWidget(),
					tr("@product@ - Warning"),
					tr("You have unfinished call(s).") + "\n" +
						tr("Are you sure you want to log off?"),
					tr("&Log Off"),
					tr("&Cancel")
				) == 1)  {
					return;
				}
			}
		}
	}
	////
	
	// desactivate menubar and toolbar to avoid crashes
	_ui->menuBar->setEnabled(false);
	_ui->toolBar->setEnabled(false);
	////
	
	_cWengoPhone.getCUserProfileHandler().logOff();
}

int QtToolBar::findFirstCallTab() {
	QtContactCallListWidget * widget;
	for (int i = 0; i < _ui->tabWidget->count(); i++) {
		widget = dynamic_cast<QtContactCallListWidget *>(_ui->tabWidget->widget(i));
		if (widget) {
			return i;
		}
	}
	return -1;
}

void QtToolBar::acceptCall() {
	int callIndex = findFirstCallTab();
	if (callIndex == -1) {
		return;
	}
	QtContactCallListWidget * widget = (QtContactCallListWidget *) _ui->tabWidget->widget(callIndex);
	if (!widget) {
		return;
	}

	QtPhoneCall * qtPhoneCall = widget->getFirstQtPhoneCall();
	if (qtPhoneCall) {
		qtPhoneCall->acceptCall();
	}
}

void QtToolBar::holdResumeCall() {
	int callIndex = findFirstCallTab();
	if (callIndex == -1) {
		return;
	}
	QtContactCallListWidget * widget = (QtContactCallListWidget *) _ui->tabWidget->widget(callIndex);
	if (!widget) {
		return;
	}

	QtPhoneCall * qtPhoneCall = widget->getFirstQtPhoneCall();
	if (qtPhoneCall) {
		qtPhoneCall->holdOrResume();
	}
}

void QtToolBar::hangUpCall() {
	int callIndex = findFirstCallTab();
	if (callIndex == -1) {
		return;
	}
	QtContactCallListWidget * widget;

	widget = (QtContactCallListWidget *) _ui->tabWidget->widget(callIndex);
	if (!widget) {
		return;
	}
	widget->hangup();
}

void QtToolBar::updateMenuActions() {

	bool accept = false, holdResume = false, hangUp = false;
	
	int callIndex = findFirstCallTab();
	if (callIndex > 0) {

		QtContactCallListWidget * widget = (QtContactCallListWidget *) _ui->tabWidget->widget(callIndex);
		if (widget) {

			QtPhoneCall * qtPhoneCall = widget->getFirstQtPhoneCall();

			if (qtPhoneCall) {

				switch (qtPhoneCall->getCPhoneCall().getState()) {
					
					case EnumPhoneCallState::PhoneCallStateResumed:
					case EnumPhoneCallState::PhoneCallStateTalking: {
						holdResume = true;
						hangUp = true;
						break;
					}
					
					case EnumPhoneCallState::PhoneCallStateDialing:
					case EnumPhoneCallState::PhoneCallStateRinging: {
						hangUp = true;
						break;
					}
					
					case EnumPhoneCallState::PhoneCallStateIncoming: {
						accept = true;
						hangUp = true;
						break;
					}
					
					case EnumPhoneCallState::PhoneCallStateHold: {
						holdResume= true;
						hangUp = true;
						break;
					}
					
					case EnumPhoneCallState::PhoneCallStateError: {
						hangUp = true;
						break;
					}
					
					// other cases -> set all to false
					case EnumPhoneCallState::PhoneCallStateMissed :
					case EnumPhoneCallState::PhoneCallStateRedirected:
					case EnumPhoneCallState::PhoneCallStateClosed:
					case EnumPhoneCallState::PhoneCallStateUnknown:
					default: {
						//accept = holdResume = hangUp = false;
					}
				}
			}
		}
	}

	_ui->actionAcceptCall->setEnabled(accept);
	_ui->actionHangUpCall->setEnabled(hangUp);
	_ui->actionHoldResumeCall->setEnabled(holdResume);
}

void QtToolBar::showChatWindow() {
	QWidget * chatWindow = _qtWengoPhone.getChatWindow();
	if (chatWindow) {
		chatWindow->showNormal();
		chatWindow->activateWindow();
	}
}

void QtToolBar::showFileTransferWindow() {

	QtFileTransfer * fileTransfer = _qtWengoPhone.getFileTransfer();
	if (fileTransfer) {
			fileTransfer->getFileTransferWidget()->show();
	}
}

void QtToolBar::retranslateUi() {
	updateShowHideContactGroupsAction();
	updateShowHideOfflineContactsAction();
	updateToggleVideoAction();
}

void QtToolBar::configChangedEventHandler(const std::string & key) {
	configChanged(QString::fromStdString(key));
}

void QtToolBar::slotConfigChanged(QString qkey) {
	std::string key = qkey.toStdString();
	if (key == Config::VIDEO_ENABLE_KEY || key == Config::VIDEO_WEBCAM_DEVICE_KEY) {
		updateToggleVideoAction();
	}
}

void QtToolBar::updateToolBarFromConfig() {
	Config & config = ConfigManager::getInstance().getCurrentConfig();
	std::string toolBarMode = config.getToolBarMode();

	if (toolBarMode == TOOLBARMODE_HIDDEN) {
		_ui->actionToolBarHidden->trigger();
	} else if (toolBarMode == TOOLBARMODE_ICONS_ONLY) {
		_ui->actionToolBarIconsOnly->trigger();
	} else if (toolBarMode == TOOLBARMODE_TEXT_UNDER_ICONS) {
		_ui->actionToolBarTextUnderIcons->trigger();
	} else {
		LOG_WARN("Unknown toolbar mode: " + toolBarMode);
	}
}

void QtToolBar::updateToolBarFromAction(QAction* action) {
	Config & config = ConfigManager::getInstance().getCurrentConfig();
	const char* mode = 0;

	if (action == _ui->actionToolBarHidden) {
		_ui->toolBar->hide();
		mode = TOOLBARMODE_HIDDEN;

	} else if (action == _ui->actionToolBarIconsOnly) {
		_ui->toolBar->show();
		_ui->toolBar->setToolButtonStyle(Qt::ToolButtonIconOnly);

		// Restore icon width in case it was set by TextUnderIcon mode
		QSize size = _ui->toolBar->iconSize();
		size.setWidth(size.height());
		_ui->toolBar->setIconSize(size);
		mode = TOOLBARMODE_ICONS_ONLY;

	} else if (action == _ui->actionToolBarTextUnderIcons) {
		_ui->toolBar->show();
		_ui->toolBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
		QSize size = _ui->toolBar->iconSize();
		size.setWidth(TOOLBAR_BUTTON_MINIMUM_WIDTH);
		_ui->toolBar->setIconSize(size);
		mode = TOOLBARMODE_TEXT_UNDER_ICONS;

	} else {
		LOG_FATAL("Unknown action");
	}
	
	config.set(Config::GENERAL_TOOLBARMODE_KEY, std::string(mode));
}

void QtToolBar::userProfileDeleted() {
	_ui->actionLogOff->setEnabled(false);
}

void QtToolBar::userProfileIsInitialized() {
	_ui->actionLogOff->setEnabled(true);
	_ui->menuBar->setEnabled(true);
	_ui->toolBar->setEnabled(true);
}
