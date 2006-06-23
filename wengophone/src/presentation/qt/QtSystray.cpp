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
#include "profilebar/QtProfileBar.h"
#include "contactlist/QtContactListManager.h"
#include "webservices/sms/QtSms.h"

#include <control/CWengoPhone.h>
#include <control/profile/CUserProfile.h>
#include <control/profile/CUserProfileHandler.h>

#include <model/network/NetworkObserver.h>
#include <model/profile/UserProfile.h>

#include <util/Logger.h>

#include <QtGui>

#include <trayicon.h>

QtSystray::QtSystray(QObject * parent)
	: QObjectThreadSafe(NULL) {

	_qtWengoPhone = qobject_cast<QtWengoPhone *>(parent);

	_callMenu = NULL;
	_sendSmsMenu = NULL;
	_startChatMenu = NULL;
	_callWengoMenu = NULL;
	_callMobileMenu = NULL;
	_callLandLineMenu = NULL;


	NetworkObserver::getInstance().connectionIsDownEvent +=
		boost::bind(&QtSystray::connectionIsDownEventHandler, this);

	NetworkObserver::getInstance().connectionIsUpEvent +=
		boost::bind(&QtSystray::connectionIsUpEventHandler, this);


	//trayMenu
	_trayMenu = new QMenu(_qtWengoPhone->getWidget());
	connect(_trayMenu, SIGNAL(aboutToShow()), SLOT(setTrayMenu()));

	//trayIcon
	_trayIcon = new TrayIcon(QPixmap(":pics/status/online.png"), QString("WengoPhone"), _trayMenu, _qtWengoPhone->getWidget());
	connect(_trayIcon, SIGNAL(doubleClicked(const QPoint &)), SLOT(sysTrayDoubleClicked(const QPoint &)));
	_trayIcon->setIcon(QPixmap(":/pics/systray/connecting.png"));
	_trayIcon->setToolTip(QString("WengoPhone - ") + tr("Connecting..."));
	_trayIcon->show();

	setTrayMenu();
}

void QtSystray::setTrayMenu() {
	_trayMenu->clear();

	//openAction
	QAction * openAction = _trayMenu->addAction(QIcon(":/pics/open.png"), tr("Open WengoPhone"));

	//Start a call session
	_callMenu = new QMenu(tr("Call"));
	_callMenu->setIcon(QIcon(":/pics/contact/call.png"));
	updateCallMenu();
	_trayMenu->addMenu(_callMenu);

	//Change status menu
	if (_qtWengoPhone->getCWengoPhone().getCUserProfileHandler().getCUserProfile()) {
		_trayMenu->addMenu(createStatusMenu());
	}

	//quitAction
	QAction * quitAction = _trayMenu->addAction(QIcon(":/pics/exit.png"), tr("Quit WengoPhone"));

	//GDMSESSION
	char * env = getenv("GDMSESSION");
	std::string gdmSession;
	if (env) {
		gdmSession = env;
	}
	if (!gdmSession.empty()) {
		LOG_DEBUG("GDMSSESSION environment variable=" + std::string(gdmSession));
	} else {
		LOG_DEBUG("no GDMSESSION environment variable");
	}

#ifdef OS_LINUX
	if (gdmSession == "gnome") {
		LOG_DEBUG("GDMSESSION running");
		connect(openAction, SIGNAL(triggered()), _qtWengoPhone->getWidget(), SLOT(show()));
		connect(quitAction, SIGNAL(triggered()), _qtWengoPhone, SLOT(exitApplication()));
	} else {
		connect(openAction, SIGNAL(triggered()), _qtWengoPhone->getWidget(), SLOT(show()));
		connect(quitAction, SIGNAL(triggered()), _qtWengoPhone, SLOT(exitApplication()));
	}
#else
	connect(openAction, SIGNAL(triggered()), _qtWengoPhone->getWidget(), SLOT(show()));
	connect(quitAction, SIGNAL(triggered()), _qtWengoPhone, SLOT(exitApplication()));
#endif

	_trayIcon->setPopup(_trayMenu);
}

QMenu * QtSystray::createStatusMenu() {
	QMenu * menu = new QMenu(tr("Status"));
	CWengoPhone & cWengoPhone = _qtWengoPhone->getCWengoPhone();

	//FIXME GUI must not access model directly
	EnumPresenceState::PresenceState presenceState = cWengoPhone.getCUserProfileHandler().getCUserProfile()->getUserProfile().getPresenceState();
	switch (presenceState) {
	case EnumPresenceState::PresenceStateAway:
		menu->setIcon(QIcon(":/pics/status/away.png"));
		break;
	case EnumPresenceState::PresenceStateOnline:
		menu->setIcon(QIcon(":/pics/status/online.png"));
		break;
	case EnumPresenceState::PresenceStateOffline:
		menu->setIcon(QIcon(":/pics/status/offline.png"));
		break;
	case EnumPresenceState::PresenceStateInvisible:
		menu->setIcon(QIcon(":/pics/status/offline.png"));
		break;
	case EnumPresenceState::PresenceStateDoNotDisturb:
		menu->setIcon(QIcon(":/pics/status/donotdisturb.png"));
		break;
	default:
		LOG_FATAL("unknown presenceState=" + String::fromNumber(presenceState));
		break;
	}

	QtProfileBar * profileBar = _qtWengoPhone->getProfileBar();

	QAction * action = menu->addAction(QIcon(":/pics/status/online.png"), tr("Online"));
	connect(action, SIGNAL(triggered(bool)), profileBar, SLOT(onlineClicked(bool)));

	action = menu->addAction(QIcon(":/pics/status/donotdisturb.png"), tr("Do Not Disturb"));
	connect(action, SIGNAL(triggered(bool)), profileBar, SLOT(dndClicked(bool)));

	action = menu->addAction(QIcon(":/pics/status/offline.png"), tr("Invisible"));
	connect(action, SIGNAL(triggered(bool)), profileBar, SLOT(invisibleClicked(bool)));

	action = menu->addAction(QIcon(":/pics/status/away.png"), tr("Away"));
	connect(action, SIGNAL(triggered(bool)), profileBar, SLOT(awayClicked(bool)));

	return menu;
}

void QtSystray::updateCallMenu() {
	CWengoPhone & cWengoPhone = _qtWengoPhone->getCWengoPhone();

	//_sendSmsMenu
	if (!_sendSmsMenu) {
		_sendSmsMenu = new QMenu(_qtWengoPhone->getWidget());
		_sendSmsMenu->setIcon(QIcon(":/pics/contact/sms.png"));
		connect(_sendSmsMenu,SIGNAL(triggered(QAction *)), SLOT(slotSystrayMenuSendSms(QAction *)));
	}
	_sendSmsMenu->clear();
	_sendSmsMenu->setTitle(tr("Send a SMS"));

	QAction * sendSmsBlankAction = _sendSmsMenu->addAction(QIcon(":/pics/contact/sms.png"), tr("Send SMS"));
	sendSmsBlankAction->setData(String::null);
	_trayMenu->addMenu(_sendSmsMenu);


	//_startChatMenu
	if (!_startChatMenu) {
		_startChatMenu = new QMenu(_qtWengoPhone->getWidget());
		_startChatMenu->setIcon(QIcon(":/pics/contact/chat.png"));
		connect(_startChatMenu, SIGNAL(triggered(QAction *)), SLOT(slotSystrayMenuStartChat(QAction *)));
	}
	_startChatMenu->clear();
	_startChatMenu->setTitle(tr("Start a chat"));
	_trayMenu->addMenu(_startChatMenu);


	//_callWengoMenu
	if (!_callWengoMenu) {
		_callWengoMenu = new QMenu(_qtWengoPhone->getWidget());
		connect(_callWengoMenu, SIGNAL(triggered(QAction *)),SLOT(slotSystrayMenuCallWengo(QAction *)));
	}
	_callWengoMenu->clear();
	_callWengoMenu->setTitle(tr("Call SIP"));

	QAction * placeCallBlankAction =_callMenu->addAction(QIcon(":/pics/contact/call.png"), tr("Place Call"));
	connect(placeCallBlankAction, SIGNAL(triggered(bool)), _qtWengoPhone, SLOT(slotSystrayMenuCallBlank(bool)));
	placeCallBlankAction->setData(String::null);
	_callMenu->addMenu(_callWengoMenu);


	//_callMobileMenu
	if (!_callMobileMenu) {
		_callMobileMenu = new QMenu(_qtWengoPhone->getWidget());
		connect(_callMobileMenu, SIGNAL(triggered(QAction *)), SLOT(slotSystrayMenuCallMobile(QAction *)));
	}
	_callMobileMenu->clear();
	_callMobileMenu->setTitle(tr("Call Mobile"));
	_callMenu->addMenu(_callMobileMenu);


	//_callLandLineMenu
	if (!_callLandLineMenu) {
		_callLandLineMenu = new QMenu(_qtWengoPhone->getWidget());
		connect(_callLandLineMenu, SIGNAL(triggered(QAction *)), SLOT(slotSystrayMenuCallLandLine(QAction *)));
	}
	_callLandLineMenu->clear();
	_callLandLineMenu->setTitle(tr("Call land line"));
	_callMenu->addMenu(_callLandLineMenu);


	CUserProfile * currentCUserProfile = cWengoPhone.getCUserProfileHandler().getCUserProfile();
	if (currentCUserProfile) {
		CContactList & currentCContactList = currentCUserProfile->getCContactList();
		StringList currentContactsIds = currentCContactList.getContactIds();

		for (StringList::const_iterator it = currentContactsIds.begin(); it != currentContactsIds.end(); ++it) {

			ContactProfile tmpContactProfile = currentCContactList.getContactProfile(*it);

			if (tmpContactProfile.hasFreeCall()) {
				LOG_DEBUG("adding=" + tmpContactProfile.getFreePhoneNumber());
				QAction * tmpAction = _callWengoMenu->addAction(QString::fromStdString(tmpContactProfile.getFreePhoneNumber()));
				tmpAction->setData(QVariant(QString::fromStdString(tmpContactProfile.getFreePhoneNumber())));
			}

			if (!tmpContactProfile.getMobilePhone().empty()) {
				//Call mobile action
				QAction * tmpAction = _callMobileMenu->addAction(QString::fromStdString(tmpContactProfile.getDisplayName() +
							": " + tmpContactProfile.getMobilePhone()));
				tmpAction->setData(QVariant(QString::fromStdString(tmpContactProfile.getMobilePhone())));

				//Send SMS action
				tmpAction = _sendSmsMenu->addAction(QString::fromStdString(tmpContactProfile.getDisplayName() +
							": " + tmpContactProfile.getMobilePhone()));
				tmpAction->setData(QVariant(QString::fromStdString(tmpContactProfile.getMobilePhone())));
			}

			if (!tmpContactProfile.getHomePhone().empty()) {
				QAction * tmpAction = _callLandLineMenu->addAction(QString::fromStdString(tmpContactProfile.getDisplayName() +
							": " + tmpContactProfile.getHomePhone()));
				tmpAction->setData(QVariant(QString::fromStdString(tmpContactProfile.getHomePhone())));
			}

			if (tmpContactProfile.getPreferredIMContact() != NULL &&
				tmpContactProfile.getPresenceState() != EnumPresenceState::PresenceStateOffline) {

				QAction * tmpAction = _startChatMenu->addAction(QString::fromStdString(tmpContactProfile.getDisplayName()));
				tmpAction->setData(QVariant(QString::fromStdString(*it)));

				EnumPresenceState::PresenceState presenceState = tmpContactProfile.getPresenceState();
				switch (presenceState) {
				case EnumPresenceState::PresenceStateOnline:
					tmpAction->setIcon(QIcon(":/pics/status/online.png"));
					break;
				case EnumPresenceState::PresenceStateOffline:
					tmpAction->setIcon(QIcon(":/pics/status/offline.png"));
					break;
				case EnumPresenceState::PresenceStateInvisible:
					tmpAction->setIcon(QIcon(":/pics/status/invisible.png"));
					break;
				case EnumPresenceState::PresenceStateAway:
					tmpAction->setIcon(QIcon(":/pics/status/away.png"));
					break;
				case EnumPresenceState::PresenceStateDoNotDisturb:
					tmpAction->setIcon(QIcon(":/pics/status/donotdisturb.png"));
					break;
				default:
					LOG_FATAL("unknown presenceState=" + String::fromNumber(presenceState));
					break;
				}
			}
		}
	}
}

void QtSystray::setSystrayIcon(QVariant status) {
	CWengoPhone & cWengoPhone = _qtWengoPhone->getCWengoPhone();

	if (status.toInt() == (int) EnumPresenceState::MyPresenceStatusOk) {
		EnumPresenceState::PresenceState presenceState = cWengoPhone.getCUserProfileHandler().getCUserProfile()->getUserProfile().getPresenceState();

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
		default:
			LOG_FATAL("unknown presenceState=" + String::fromNumber(presenceState));
			break;
		}
	}
}

void QtSystray::connectionIsDownEventHandler() {
	typedef PostEvent1<void (bool), bool> MyPostEvent;
	MyPostEvent * event = new MyPostEvent(boost::bind(&QtSystray::connectionStateEventHandlerThreadSafe, this, _1), false);
	postEvent(event);
}

void QtSystray::connectionIsUpEventHandler() {
	typedef PostEvent1<void (bool), bool> MyPostEvent;
	MyPostEvent * event = new MyPostEvent(boost::bind(&QtSystray::connectionStateEventHandlerThreadSafe, this, _1), true);
	postEvent(event);
}

void QtSystray::connectionStateEventHandlerThreadSafe(bool connected) {
	if (connected) {
		setSystrayIcon(EnumPresenceState::MyPresenceStatusOk);
	} else {
		_trayIcon->setIcon(QPixmap(":/pics/systray/disconnected.png"));
		_trayIcon->setToolTip(QString("WengoPhone - ") + tr("Internet Connection Error"));
	}
}

void QtSystray::sysTrayDoubleClicked(const QPoint &) {
	if (_qtWengoPhone->getWidget()->isVisible()) {
		_qtWengoPhone->getWidget()->setVisible(false);
	}
	else {
		_qtWengoPhone->getWidget()->showMinimized();
		_qtWengoPhone->getWidget()->showNormal();
	}
}

void QtSystray::hide() {
	_trayIcon->hide();
}

void QtSystray::slotSystrayMenuCallWengo(QAction * action) {
	CWengoPhone & cWengoPhone = _qtWengoPhone->getCWengoPhone();
	if (action) {
		LOG_DEBUG("call=" + action->data().toString().toStdString());
		cWengoPhone.getCUserProfileHandler().getCUserProfile()->makeCall(action->data().toString().toStdString());
	} else {
		LOG_FATAL("QAction cannot be NULL");
	}
}

void QtSystray::slotSystrayMenuCallMobile(QAction * action) {
	CWengoPhone & cWengoPhone = _qtWengoPhone->getCWengoPhone();
	if (action) {
		LOG_DEBUG("call=" + action->data().toString().toStdString());
		cWengoPhone.getCUserProfileHandler().getCUserProfile()->makeCall(action->data().toString().toStdString());
	} else {
		LOG_FATAL("QAction cannot be NULL");
	}
}

void QtSystray::slotSystrayMenuCallLandLine(QAction * action) {
	CWengoPhone & cWengoPhone = _qtWengoPhone->getCWengoPhone();
	if (action) {
		LOG_DEBUG("call=" + action->data().toString().toStdString());
		cWengoPhone.getCUserProfileHandler().getCUserProfile()->makeCall(action->data().toString().toStdString());
	} else {
		LOG_FATAL("QAction cannot be NULL");
	}
}

void QtSystray::slotSystrayMenuStartChat(QAction * action) {
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

void QtSystray::slotSystrayMenuSendSms(QAction * action) {
	CWengoPhone & cWengoPhone = _qtWengoPhone->getCWengoPhone();
	if (action) {
		if (_qtWengoPhone->getSms() &&
			cWengoPhone.getCUserProfileHandler().getCUserProfile() &&
			cWengoPhone.getCUserProfileHandler().getCUserProfile()->getUserProfile().hasWengoAccount() &&
			cWengoPhone.getCUserProfileHandler().getCUserProfile()->getUserProfile().getWengoAccount()->isConnected()) {

			_qtWengoPhone->getSms()->getWidget()->show();
			_qtWengoPhone->getWidget()->show();
			_qtWengoPhone->getWidget()->setWindowState(_qtWengoPhone->getWidget()->windowState() & ~Qt::WindowMinimized | Qt::WindowActive);
			_qtWengoPhone->getSms()->setPhoneNumber(action->data().toString());
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
