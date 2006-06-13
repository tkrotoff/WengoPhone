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

#include <util/Logger.h>

#include "QtTrayIcon.h"
#include "QtWengoPhone.h"
#include "profilebar/QtProfileBar.h"
#include "contactlist/QtUserList.h"
#include "webservices/sms/QtSms.h"

#include <control/CWengoPhone.h>
#include <control/profile/CUserProfile.h>
#include <control/profile/CUserProfileHandler.h>

#include <trayicon.h>

QtTrayIcon::QtTrayIcon(QObject * parent)
	: QObject(parent) {
	_qtWengoPhone = qobject_cast<QtWengoPhone *>(parent);

	_trayMenu = NULL;
	_callMenu = NULL;
	_sendSmsMenu = NULL;
	_startChatMenu = NULL;
	_callWengoMenu = NULL;
	_callMobileMenu = NULL;
	_callLandLineMenu = NULL;

	_trayMenu = new QMenu(_qtWengoPhone->getWidget());
	connect(_trayMenu, SIGNAL(aboutToShow()), this, SLOT(setTrayMenu()));
	_trayIcon = new TrayIcon(QPixmap(":pics/status/online.png"), QString("WengoPhone"), _trayMenu, _qtWengoPhone->getWidget());
	connect(_trayIcon, SIGNAL(doubleClicked(const QPoint &)), SLOT(sysTrayDoubleClicked(const QPoint &)));
	_trayIcon->show();
	setTrayMenu();
}

void QtTrayIcon::setTrayMenu() {
	QAction * action = NULL;

	char * gdmSession = getenv("GDMSESSION");
	if (gdmSession) {
		LOG_DEBUG("GDMSSESSION environment variable is set to: " + std::string(gdmSession));
	} else {
		LOG_DEBUG("No GDMSESSION environment variable.");
	}
	_trayMenu->clear();
#ifdef OS_LINUX
	if (gdmSession && !strcmp(gdmSession, "gnome")) {
		LOG_DEBUG("We're running a GDM session.");
		action = _trayMenu->addAction(QIcon(":/pics/exit.png"), tr("Quit WengoPhone"));
		connect (action,SIGNAL(triggered()),_qtWengoPhone,SLOT(exitApplication()));
	} else {
		action = _trayMenu->addAction(QIcon(":/pics/open.png"), tr("Open WengoPhone"));
		connect ( action,SIGNAL(triggered()),_qtWengoPhone->getWidget(),SLOT(show()));
	}
#else
	// open
	action = _trayMenu->addAction(QIcon(":/pics/open.png"), tr("Open WengoPhone"));
	connect ( action,SIGNAL(triggered()),_qtWengoPhone->getWidget(),SLOT(show()));
#endif

	// Change status
	if (_qtWengoPhone->getCWengoPhone().getCUserProfileHandler().getCUserProfile()) {
		_trayMenu->addMenu(createStatusMenu());
	}

	// Start a call session
	_callMenu = new QMenu(tr("Call"));
	_callMenu->setIcon(QIcon(":/pics/contact/call.png"));
	updateCallMenu();
	_trayMenu->addMenu(_callMenu);

#ifdef OS_LINUX
	if (gdmSession && (!strcmp(gdmSession, "gnome"))) {
		action = _trayMenu->addAction(QIcon(":/pics/open.png"), tr("Open WengoPhone"));
		connect ( action,SIGNAL(triggered()),_qtWengoPhone->getWidget(),SLOT(show()));
	} else {
		action = _trayMenu->addAction(QIcon(":/pics/exit.png"), tr("Quit WengoPhone"));
		connect (action,SIGNAL(triggered()),_qtWengoPhone,SLOT(exitApplication()));
	}
#else
	action = _trayMenu->addAction(QIcon(":/pics/exit.png"), tr("Quit WengoPhone"));
	connect (action,SIGNAL(triggered()),_qtWengoPhone,SLOT(exitApplication()));
#endif
	_trayIcon->setPopup(_trayMenu);
}

QMenu * QtTrayIcon::createStatusMenu() {
	QMenu * menu = new QMenu(tr("Status"));
	CWengoPhone & cWengoPhone = _qtWengoPhone->getCWengoPhone();

	//FIXME: GUI must not access model directly
	EnumPresenceState::PresenceState presenceState = cWengoPhone.getCUserProfileHandler().getCUserProfile()->getUserProfile().getPresenceState();
	switch (presenceState) {
	case EnumPresenceState::PresenceStateAway:
		menu->setIcon ( QIcon(":/pics/status/away.png"));
		break;
	case EnumPresenceState::PresenceStateOnline:
		menu->setIcon ( QIcon(":/pics/status/online.png"));
		break;
	case EnumPresenceState::PresenceStateOffline:
		menu->setIcon ( QIcon(":/pics/status/offline.png"));
		break;
	case EnumPresenceState::PresenceStateInvisible:
		menu->setIcon ( QIcon(":/pics/status/offline.png"));
		break;
	case EnumPresenceState::PresenceStateDoNotDisturb:
		menu->setIcon ( QIcon(":/pics/status/donotdisturb.png"));
		break;
	default:
		LOG_FATAL("unknown presenceState=" + String::fromNumber(presenceState));
		break;
	}

	QtProfileBar * profileBar = _qtWengoPhone->getProfileBar();

	QAction * action = menu->addAction(QIcon(":/pics/status/online.png"),tr( "Online" ));
	connect(action,SIGNAL( triggered (bool) ),profileBar, SLOT( onlineClicked(bool)));

	action = menu->addAction(QIcon(":/pics/status/donotdisturb.png"),tr( "DND" ));
	connect(action,SIGNAL( triggered (bool) ),profileBar,SLOT(dndClicked(bool)));

	action = menu->addAction(QIcon(":/pics/status/offline.png"), tr("Invisible"));
	connect(action,SIGNAL( triggered (bool) ),profileBar,SLOT( invisibleClicked(bool)));

	action = menu->addAction(QIcon(":/pics/status/away.png"), tr("Away"));
	connect(action,SIGNAL( triggered (bool) ),profileBar,SLOT( awayClicked(bool)));

	return menu;
}

void QtTrayIcon::updateCallMenu() {
    CWengoPhone & cWengoPhone = _qtWengoPhone->getCWengoPhone();

	// Send  SMS
	if (!_sendSmsMenu) {
		_sendSmsMenu = new QMenu(tr("Send a SMS"));
		_sendSmsMenu->setIcon(QIcon(":/pics/contact/sms.png"));
		connect (_sendSmsMenu,SIGNAL(triggered(QAction*)), SLOT(slotSystrayMenuSendSms(QAction*)));
	} else {
		_sendSmsMenu->clear();
		_sendSmsMenu->setTitle(tr("Send a SMS"));
	}

	QAction *sendSmsBlankAction = _sendSmsMenu->addAction(tr("Send SMS"));
	sendSmsBlankAction->setData(String::null);
	_trayMenu->addMenu(_sendSmsMenu);

	if (!_startChatMenu) {
		_startChatMenu = new QMenu(_qtWengoPhone->getWidget());
		connect(_startChatMenu, SIGNAL(triggered(QAction*)), SLOT(slotSystrayMenuStartChat(QAction*)));
		_startChatMenu->setIcon(QIcon(":/pics/contact/chat.png"));
	}
	_startChatMenu->clear();
	_startChatMenu->setTitle(tr("Start a chat"));
	_trayMenu->addMenu(_startChatMenu);

	if (!_callWengoMenu) {
		_callWengoMenu = createCallWengoTrayMenu();
		connect(_callWengoMenu, SIGNAL(triggered(QAction*)),SLOT(slotSystrayMenuCallWengo(QAction*)));
	} else {
		_callWengoMenu->clear();
		_callWengoMenu->setTitle(tr("Call SIP"));
	}
	QAction *placeCallBlankAction =_callMenu->addAction(tr("Place Call"));
	connect(placeCallBlankAction, SIGNAL(triggered(bool)), _qtWengoPhone, SLOT(slotSystrayMenuCallBlank(bool)));
	placeCallBlankAction->setData(String::null);
	_callMenu->addMenu(_callWengoMenu);

	if (!_callMobileMenu) {
		_callMobileMenu = createCallMobileTrayMenu();
		connect(_callMobileMenu, SIGNAL(triggered(QAction*)), SLOT(slotSystrayMenuCallMobile(QAction*)));
	}  else {
		_callMobileMenu->clear();
		_callMobileMenu->setTitle(tr("Call Mobile"));
	}
	_callMenu->addMenu(_callMobileMenu);


	if (!_callLandLineMenu) {
		_callLandLineMenu = createCallLandLineTrayMenu();
		connect(_callLandLineMenu, SIGNAL(triggered(QAction*)), SLOT(slotSystrayMenuCallLandLine(QAction*)));
	} else {
		_callLandLineMenu->clear();
		_callLandLineMenu->setTitle(tr("Call land line"));
	}
	_callMenu->addMenu(_callLandLineMenu);

	CUserProfile *currentCUserProfile = cWengoPhone.getCUserProfileHandler().getCUserProfile();
	if (currentCUserProfile) {
		CContactList &currentCContactList = currentCUserProfile->getCContactList();
		std::vector<std::string> currentContactsIds = currentCContactList.getContactIds();

		for (std::vector<std::string>::const_iterator it = currentContactsIds.begin();
			 it != currentContactsIds.end(); ++it)
		{
			ContactProfile tmpContactProfile = currentCContactList.getContactProfile(*it);

			if (tmpContactProfile.hasFreeCall()) {
				LOG_DEBUG("Adding :" + tmpContactProfile.getFreePhoneNumber());
				QAction * tmpAction =_callWengoMenu->addAction(QString::fromStdString(tmpContactProfile.getFreePhoneNumber()));
				tmpAction->setData(QVariant(QString::fromStdString(tmpContactProfile.getFreePhoneNumber())));
			}

			if (!tmpContactProfile.getMobilePhone().empty()) {
				/* Call mobile action */
				QAction * tmpAction =_callMobileMenu->addAction(QString::fromStdString(tmpContactProfile.getDisplayName() +
																					   ": " +
																					   tmpContactProfile.getMobilePhone()));
				tmpAction->setData(QVariant(QString::fromStdString(tmpContactProfile.getMobilePhone())));
				/* Send SMS action */
				tmpAction =_sendSmsMenu->addAction(QString::fromStdString(tmpContactProfile.getDisplayName() +
																		  ": " +
																		  tmpContactProfile.getMobilePhone()));
				tmpAction->setData(QVariant(QString::fromStdString(tmpContactProfile.getMobilePhone())));
			}

			if (!tmpContactProfile.getHomePhone().empty()) {
				QAction * tmpAction =_callLandLineMenu->addAction(QString::fromStdString(tmpContactProfile.getDisplayName() +
																						 ": " +
																						 tmpContactProfile.getHomePhone()));
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

QMenu * QtTrayIcon::createCallWengoTrayMenu() {
    return new QMenu(tr("Call SIP"));
}

QMenu * QtTrayIcon::createCallMobileTrayMenu() {
    return new QMenu(tr("Call Mobile"));
}

QMenu * QtTrayIcon::createCallLandLineTrayMenu() {
	return new QMenu(tr("Call land line"));
}

void QtTrayIcon::setSystrayIcon(QVariant status) {
    CWengoPhone & cWengoPhone = _qtWengoPhone->getCWengoPhone();

	if (status.toInt() == (int) EnumPresenceState::MyPresenceStatusOk) {
		EnumPresenceState::PresenceState presenceState = cWengoPhone.getCUserProfileHandler().getCUserProfile()->getUserProfile().getPresenceState();

		switch (presenceState) {
		case EnumPresenceState::PresenceStateAway:
			_trayIcon->setIcon(QPixmap(":/pics/status/away.png"));
			_trayIcon->setToolTip(QString("WengoPhone - ") + tr("Away"));
			break;
		case EnumPresenceState::PresenceStateOnline:
			_trayIcon->setIcon(QPixmap(":/pics/status/online.png"));
			_trayIcon->setToolTip(QString("WengoPhone - ") + tr("Online"));
			break;
		case EnumPresenceState::PresenceStateOffline:
			_trayIcon->setIcon(QPixmap(":/pics/status/offline.png"));
			_trayIcon->setToolTip(QString("WengoPhone - ") + tr("Offline"));
			break;
		case EnumPresenceState::PresenceStateInvisible:
			_trayIcon->setIcon(QPixmap(":/pics/status/offline.png"));
			_trayIcon->setToolTip(QString("WengoPhone - ") + tr("Offline"));
			break;
		case EnumPresenceState::PresenceStateDoNotDisturb:
			_trayIcon->setIcon(QPixmap(":/pics/status/donotdisturb.png"));
			_trayIcon->setToolTip(QString("WengoPhone - ") + tr("Do not disturb"));
			break;
		default:
			LOG_FATAL("unknown presenceState=" + String::fromNumber(presenceState));
			break;
		}
	}
}

void QtTrayIcon::sysTrayDoubleClicked(const QPoint& ) {

    if ( _qtWengoPhone->getWidget()->isVisible() ) {
        _qtWengoPhone->getWidget()->setVisible(false);
    }
    else{
        _qtWengoPhone->getWidget()->showMinimized();
        _qtWengoPhone->getWidget()->showNormal();
    }
}

void QtTrayIcon::hide() {
    _trayIcon->hide();
}

void QtTrayIcon::slotSystrayMenuCallWengo(QAction * action) {
    CWengoPhone & cWengoPhone = _qtWengoPhone->getCWengoPhone();
    if (action) {
        LOG_DEBUG("Call " + action->data().toString().toStdString());
        cWengoPhone.getCUserProfileHandler().getCUserProfile()->makeCall(action->data().toString().toStdString());
    } else {
        LOG_FATAL("QAction cannot be NULL");
    }
}

void QtTrayIcon::slotSystrayMenuCallMobile(QAction * action) {
    CWengoPhone & cWengoPhone = _qtWengoPhone->getCWengoPhone();
    if (action) {
        LOG_DEBUG("Call " + action->data().toString().toStdString());
        cWengoPhone.getCUserProfileHandler().getCUserProfile()->makeCall(action->data().toString().toStdString());
    } else {
        LOG_FATAL("QAction cannot be NULL");
    }
}

void QtTrayIcon::slotSystrayMenuCallLandLine(QAction * action) {
    CWengoPhone & cWengoPhone = _qtWengoPhone->getCWengoPhone();
    if (action) {
        LOG_DEBUG("Call " + action->data().toString().toStdString());
        cWengoPhone.getCUserProfileHandler().getCUserProfile()->makeCall(action->data().toString().toStdString());
    } else {
       LOG_FATAL("QAction cannot be NULL");
    }
}

void QtTrayIcon::slotSystrayMenuStartChat(QAction * action) {
	if (action) {
		QtUserList * ul = QtUserList::getInstance();
		if (ul) {
			LOG_DEBUG("Starting IM chat with " + action->data().toString().toStdString());
			ul->startChat(action->data().toString());
		} else {
			LOG_FATAL("No action passed to slot! Shouldn't reach this code.");
		}
	} else {
		LOG_FATAL("QAction cannot be NULL");
	}
}

void QtTrayIcon::slotSystrayMenuSendSms(QAction * action) {
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
			QMessageBox::warning(0,
								 QObject::tr("WengoPhone"),
								 QObject::tr("Can't send SMS unless you're connected to a SIP service."),
								 QMessageBox::Ok, 0);
        }
    } else {
        LOG_FATAL("No action passed to slot! Shouldn't reach this code.");
    }
}
