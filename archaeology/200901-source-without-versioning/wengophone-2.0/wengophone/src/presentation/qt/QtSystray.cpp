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

#include "QtSystray.h"

#include "QtWengoPhone.h"
#include "QtUserProfilePresenceMenu.h"
#include "profilebar/QtProfileBar.h"
#include "contactlist/QtContactListManager.h"
#include "contactlist/QtContactMenu.h"
#include "webservices/sms/QtSms.h"

#include <control/CWengoPhone.h>
#include <control/profile/CUserProfile.h>
#include <control/profile/CUserProfileHandler.h>

#include <model/network/NetworkObserver.h>
#include <model/profile/UserProfile.h>

#include <cutil/global.h>
#include <util/Logger.h>

#include <qtutil/SafeConnect.h>
#include <qtutil/LanguageChangeEventFilter.h>

#include <QtGui/QtGui>

#include <trayicon.h>

#if defined(OS_MACOSX)
	#include "macosx/QtMacApplication.h"
#endif

#ifdef OS_WINDOWS
	#include <windows.h>
#endif

QtSystray::QtSystray(QObject * parent)
	: QObjectThreadSafe(NULL) {

	_qtWengoPhone = (QtWengoPhone *) parent;

	_callMenu = NULL;

	//Check Internet connection status
	NetworkObserver::getInstance().connectionIsDownEvent +=
		boost::bind(&QtSystray::connectionIsDownEventHandler, this);

	NetworkObserver::getInstance().connectionIsUpEvent +=
		boost::bind(&QtSystray::connectionIsUpEventHandler, this);

	//trayMenu
	_trayMenu = new QMenu(_qtWengoPhone->getWidget());
	SAFE_CONNECT(_trayMenu, SIGNAL(aboutToShow()), SLOT(setTrayMenu()));

	//trayIcon
	_trayIcon = new TrayIcon(QPixmap(":pics/status/online.png"), QString("WengoPhone"), _trayMenu, _qtWengoPhone->getWidget());
	SAFE_CONNECT(_trayIcon, SIGNAL(doubleClicked(const QPoint &)), SLOT(showMainWindow()));
	phoneLineStateChanged(EnumPhoneLineState::PhoneLineStateProgress);
	_trayIcon->show();

	//systray re-initialization
	SAFE_CONNECT(_qtWengoPhone, SIGNAL(userProfileDeleted()), SLOT(userProfileDeleted()));

	setTrayMenu();

	LANGUAGE_CHANGE(_trayIcon);
}

QtSystray::~QtSystray() {
}

void QtSystray::setTrayMenu() {
	_trayMenu->clear();

	//openAction
#if defined(OS_MACOSX)
	// On Mac OS X, clicking the Dock icon should show the application thus the
	// 'Open WengoPhone' entry is not necessary. We get the Dock Icon click event
	// from our QtMacApplication class.
	QtMacApplication * qMacApp = dynamic_cast<QtMacApplication *>(QApplication::instance());
	SAFE_CONNECT_RECEIVER(qMacApp, SIGNAL(applicationMustShow()), _qtWengoPhone->getWidget(), SLOT(show()));
#else
	QAction * openAction = _trayMenu->addAction(QIcon(":/pics/open.png"), tr("Open WengoPhone"));
#endif

	//Start a call session
	_callMenu = new QMenu(tr("Call"));
	_callMenu->setIcon(QIcon(":/pics/contact/call.png"));
	updateMenu();
	_trayMenu->addMenu(_callMenu);

	//Change status menu
	if (_qtWengoPhone->getCWengoPhone().getCUserProfileHandler().getCUserProfile()) {
		_trayMenu->addMenu(createStatusMenu());
	}

	//quitAction
#if !defined(OS_MACOSX)
	// There is already a Quit on Mac OS X
	QAction * quitAction = _trayMenu->addAction(QIcon(":/pics/exit.png"), tr("Quit WengoPhone"));
#endif

#if !defined(OS_MACOSX)
	SAFE_CONNECT(openAction, SIGNAL(triggered()), SLOT(showMainWindow()));
	SAFE_CONNECT_RECEIVER(quitAction, SIGNAL(triggered()), _qtWengoPhone, SLOT(prepareToExitApplication()));
#endif

	_trayIcon->setPopup(_trayMenu);
}

QMenu * QtSystray::createStatusMenu() {
	CWengoPhone & cWengoPhone = _qtWengoPhone->getCWengoPhone();

	//FIXME GUI must not access model directly
	UserProfile & userProfile = cWengoPhone.getCUserProfileHandler().getCUserProfile()->getUserProfile();
	EnumPresenceState::PresenceState presenceState = userProfile.getPresenceState();
	bool connected = userProfile.isConnected();

	QtUserProfilePresenceMenu * menu = new QtUserProfilePresenceMenu(presenceState, connected, tr("Status"), NULL);

	QtProfileBar * qtProfileBar = _qtWengoPhone->getQtProfileBar();
	if (qtProfileBar) {
		SAFE_CONNECT_RECEIVER(menu, SIGNAL(onlineClicked()), qtProfileBar, SLOT(onlineClicked()));
		SAFE_CONNECT_RECEIVER(menu, SIGNAL(doNotDisturbClicked()), qtProfileBar, SLOT(doNotDisturbClicked()));
		SAFE_CONNECT_RECEIVER(menu, SIGNAL(invisibleClicked()), qtProfileBar, SLOT(invisibleClicked()));
		SAFE_CONNECT_RECEIVER(menu, SIGNAL(awayClicked()), qtProfileBar, SLOT(awayClicked()));
		SAFE_CONNECT_RECEIVER(menu, SIGNAL(disconnectClicked()), qtProfileBar, SLOT(disconnectClicked()));
	}

	return menu;
}

void QtSystray::updateMenu() {
	CWengoPhone & cWengoPhone = _qtWengoPhone->getCWengoPhone();

	//sendSmsMenu
	QMenu * sendSmsMenu = new QMenu(_qtWengoPhone->getWidget());
	sendSmsMenu->setTitle(tr("Send a SMS"));
	sendSmsMenu->setIcon(QIcon(":/pics/contact/sms.png"));
	SAFE_CONNECT(sendSmsMenu, SIGNAL(triggered(QAction *)), SLOT(sendSms(QAction *)));
	sendSmsMenu->clear();

	QAction * sendSmsBlankAction = sendSmsMenu->addAction(QIcon(":/pics/contact/sms.png"), tr("Send SMS"));
	sendSmsBlankAction->setData(String::null);
	_trayMenu->addMenu(sendSmsMenu);
	QtContactMenu::populateMobilePhoneMenu(sendSmsMenu, cWengoPhone);
	///

	//startChatMenu
	QMenu * startChatMenu = new QMenu(_qtWengoPhone->getWidget());
	startChatMenu->setIcon(QIcon(":/pics/contact/chat.png"));
	SAFE_CONNECT(startChatMenu, SIGNAL(triggered(QAction *)), SLOT(startChat(QAction *)));
	startChatMenu->clear();
	startChatMenu->setTitle(tr("Start a chat"));
	_trayMenu->addMenu(startChatMenu);
	QtContactMenu::populateChatMenu(startChatMenu, cWengoPhone);
	///

	//call menu
	QAction * placeCallBlankAction = _callMenu->addAction(QIcon(":/pics/contact/call.png"), tr("Place Call"));
	placeCallBlankAction->setData(String::null);

	//callWengoMenu
	QMenu * callWengoMenu = new QMenu(_qtWengoPhone->getWidget());
	callWengoMenu->setTitle(tr("Call SIP"));
	SAFE_CONNECT(callWengoMenu, SIGNAL(triggered(QAction *)), SLOT(makeCall(QAction *)));
	callWengoMenu->clear();
	_callMenu->addMenu(callWengoMenu);
	QtContactMenu::populateFreeCallMenu(callWengoMenu, cWengoPhone);
	///

	//callMobileMenu
	QMenu * callMobileMenu = new QMenu(_qtWengoPhone->getWidget());
	callMobileMenu->setTitle(tr("Call Mobile"));
	SAFE_CONNECT(callMobileMenu, SIGNAL(triggered(QAction *)), SLOT(makeCall(QAction *)));
	callMobileMenu->clear();
	_callMenu->addMenu(callMobileMenu);
	QtContactMenu::populateMobilePhoneMenu(callMobileMenu, cWengoPhone);
	///

	//callLandlineMenu
	QMenu * callLandlineMenu = new QMenu(_qtWengoPhone->getWidget());
	callLandlineMenu->setTitle(tr("Call land line"));
	SAFE_CONNECT(callLandlineMenu, SIGNAL(triggered(QAction *)), SLOT(makeCall(QAction *)));
	callLandlineMenu->clear();
	_callMenu->addMenu(callLandlineMenu);
	QtContactMenu::populateHomePhoneMenu(callLandlineMenu, cWengoPhone);
	///

	//sendFileMenu
	QMenu * sendFileMenu = new QMenu(_qtWengoPhone->getWidget());
	sendFileMenu->setTitle(tr("Send File"));
	sendFileMenu->setIcon(QIcon(":/pics/actions/send_file.png"));
	SAFE_CONNECT(sendFileMenu, SIGNAL(triggered(QAction *)), SLOT(sendFile(QAction *)));
	sendFileMenu->clear();
	_trayMenu->addMenu(sendFileMenu);
	//TODO: call a new method populateFreeCallMenu with contactId
	QtContactMenu::populateWengoUsersContactId(sendFileMenu, cWengoPhone);
	///
}

void QtSystray::setSystrayIcon(QVariant status) {
	CWengoPhone & cWengoPhone = _qtWengoPhone->getCWengoPhone();

	if (status.toInt() == (int) EnumPresenceState::MyPresenceStatusOk) {
		EnumPresenceState::PresenceState presenceState =
			cWengoPhone.getCUserProfileHandler().getCUserProfile()->getUserProfile().getPresenceState();

		switch (presenceState) {
		case EnumPresenceState::PresenceStateAway:
			_trayIcon->setIcon(QPixmap(":/pics/systray/away.png"));
			_trayIcon->setToolTip(QString("WengoPhone - ") + tr("Away"));
			break;
		case EnumPresenceState::PresenceStateOnline:
			_trayIcon->setIcon(QPixmap(":/pics/systray/online.png"));
			_trayIcon->setToolTip(QString("WengoPhone - ") + tr("Online"));
			break;
		case EnumPresenceState::PresenceStateOffline:
			_trayIcon->setIcon(QPixmap(":/pics/systray/offline.png"));
			_trayIcon->setToolTip(QString("WengoPhone - ") + tr("Offline"));
			break;
		case EnumPresenceState::PresenceStateInvisible:
			_trayIcon->setIcon(QPixmap(":/pics/systray/invisible.png"));
			_trayIcon->setToolTip(QString("WengoPhone - ") + tr("Invisible"));
			break;
		case EnumPresenceState::PresenceStateDoNotDisturb:
			_trayIcon->setIcon(QPixmap(":/pics/systray/donotdisturb.png"));
			_trayIcon->setToolTip(QString("WengoPhone - ") + tr("Do Not Disturb"));
			break;
		case EnumPresenceState::PresenceStateUnknown:
			break;
		default:
			LOG_FATAL("unknown presenceState=" + String::fromNumber(presenceState));
			break;
		}
	}
}

void QtSystray::userProfileDeleted() {
	connectionStateEventHandlerThreadSafe(false);
}

void QtSystray::phoneLineStateChanged(EnumPhoneLineState::PhoneLineState state) {
	bool connected = false;

	switch (state) {
	case EnumPhoneLineState::PhoneLineStateUnknown:
		break;
	case EnumPhoneLineState::PhoneLineStateServerError:
		break;
	case EnumPhoneLineState::PhoneLineStateTimeout:
		break;
	case EnumPhoneLineState::PhoneLineStateOk:
		connected = true;
		break;
	case EnumPhoneLineState::PhoneLineStateClosed:
		break;
	case EnumPhoneLineState::PhoneLineStateProgress:
		_trayIcon->setIcon(QPixmap(":/pics/systray/connecting.png"));
		_trayIcon->setToolTip(QString("WengoPhone - ") + tr("Connecting..."));
		return;
	default:
		LOG_FATAL("unknown state=" + EnumPhoneLineState::toString(state));
	};

	connectionStateEventHandlerThreadSafe(connected);
}

void QtSystray::connectionIsDownEventHandler() {
	//typedef PostEvent1<void (bool), bool> MyPostEvent;
	//MyPostEvent * event = new MyPostEvent(boost::bind(&QtSystray::connectionStateEventHandlerThreadSafe, this, _1), false);
	//FIXME Replaced by phoneLineStateChanged()
	//postEvent(event);
}

void QtSystray::connectionIsUpEventHandler() {
	//typedef PostEvent1<void (bool), bool> MyPostEvent;
	//MyPostEvent * event = new MyPostEvent(boost::bind(&QtSystray::connectionStateEventHandlerThreadSafe, this, _1), true);
	//FIXME Replaced by phoneLineStateChanged()
	//postEvent(event);
}

void QtSystray::connectionStateEventHandlerThreadSafe(bool connected) {
	if (connected) {
		setSystrayIcon(EnumPresenceState::MyPresenceStatusOk);
	} else {
		_trayIcon->setIcon(QPixmap(":/pics/systray/disconnected.png"));
		_trayIcon->setToolTip(QString("WengoPhone - ") + tr("Internet Connection Error"));
	}
}

void QtSystray::showMainWindow() {
	_qtWengoPhone->ensureVisible();
}

void QtSystray::hide() {
	_trayIcon->hide();
}

void QtSystray::makeCall(QAction * action) {
	CWengoPhone & cWengoPhone = _qtWengoPhone->getCWengoPhone();
	if (action) {
		LOG_DEBUG("call=" + action->data().toString().toStdString());
		cWengoPhone.getCUserProfileHandler().getCUserProfile()->makeCall(action->data().toString().toStdString());
	} else {
		LOG_FATAL("QAction cannot be NULL");
	}
}

void QtSystray::startChat(QAction * action) {
	if (action) {
		QtContactListManager * contactListManager = QtContactListManager::getInstance();
		if (contactListManager) {
			LOG_DEBUG("chat=" + action->data().toString().toStdString());
			contactListManager->startChat(action->data().toString());
		} else {
			LOG_FATAL("QtContactListManager cannot be NULL");
		}
	} else {
		LOG_FATAL("QAction cannot be NULL");
	}
}

void QtSystray::sendSms(QAction * action) {
	CWengoPhone & cWengoPhone = _qtWengoPhone->getCWengoPhone();
	if (action) {
		if (_qtWengoPhone->getQtSms() &&
			cWengoPhone.getCUserProfileHandler().getCUserProfile() &&
			cWengoPhone.getCUserProfileHandler().getCUserProfile()->getUserProfile().hasWengoAccount() &&
			cWengoPhone.getCUserProfileHandler().getCUserProfile()->getUserProfile().getWengoAccount()->isConnected()) {

			_qtWengoPhone->getQtSms()->getWidget()->show();
			_qtWengoPhone->getWidget()->show();
			_qtWengoPhone->getWidget()->setWindowState(_qtWengoPhone->getWidget()->windowState() & ~Qt::WindowMinimized | Qt::WindowActive);
			_qtWengoPhone->getQtSms()->setPhoneNumber(action->data().toString());
		} else {
			QMessageBox::warning(_qtWengoPhone->getWidget(),
				QObject::tr("WengoPhone"),
				QObject::tr("Can't send SMS unless you're connected to a SIP service."),
				QMessageBox::Ok, 0);
		}
	} else {
		LOG_FATAL("QAction cannot be NULL");
	}
}

void QtSystray::sendFile(QAction * action) {
	if (action) {
		QtContactListManager * contactListManager = QtContactListManager::getInstance();
		if (contactListManager) {
			LOG_DEBUG("sendfile=" + action->data().toString().toStdString());
			contactListManager->sendFile(action->data().toString());
		}
	} else {
		LOG_FATAL("QAction cannot be NULL");
	}
}

void QtSystray::languageChanged() {
	updateMenu();
}
