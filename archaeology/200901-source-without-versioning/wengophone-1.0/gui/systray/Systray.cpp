/*
 * WengoPhone, a voice over Internet phone
 * Copyright (C) 2004-2005  Wengo
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

#include "Systray.h"

#include "SystrayPopupWindow.h"
#include "Softphone.h"
#include "MainWindow.h"
#include "SessionWindow.h"
#include "AudioCallManager.h"
#include "contact/ContactList.h"

#include <trayicon.h>

#include <qmainwindow.h>
#include <qapplication.h>
#include <qpopupmenu.h>
#include <qpixmap.h>
#include <qaction.h>
#include <qdialog.h>
#include <qtimer.h>

#include <iostream>
using namespace std;

const int Systray::TIMER_TIMEOUT = 1000;

Systray::Systray(MainWindow & mainWindow)
: _mainWindow(mainWindow) {

	//Popup menu
	_menu = new QPopupMenu(_mainWindow.getWidget());
	connect(_menu, SIGNAL(aboutToShow()), this, SLOT(updateMenu()));

	//Tray icon
	_trayIcon = new TrayIcon(_mainWindow.getWidget());
	_trayIcon->setPopup(_menu);
	setToDefault();
	connect(_trayIcon, SIGNAL(clicked(const QPoint &, int)),
		this, SLOT(setToDefaultIfCallMissed(const QPoint &, int)));
	_trayIcon->show();
	
	//Timer
	_timer = new QTimer(this);
	_timer->start(TIMER_TIMEOUT);
	connect(_timer, SIGNAL(timeout()),
		this, SLOT(animatedIcon()));
}

Systray::~Systray() {
	hide();
	delete _timer;
	delete _trayIcon;
}

void Systray::updateMenu() {
	//clear the menu before rebuilt it
	_menu->clear();
	
	bool contacts_landline = false;
	bool contacts_online = false;
	bool contacts_mobile = false;
	
	//add current call entries (current audio/video call & chat sessions)
	AudioCallManager::AudioCalls audioCalls = AudioCallManager::getInstance().getAudioCallList();
	AudioCallManager::AudioCalls::iterator it;
	for( it = audioCalls.begin(); it != audioCalls.end(); it++ ) {
		if(*it) {
			if (((*it)->getSessionWindow().getCurrentPageIndex() == SessionWindow::INDEX_CHAT_PAGE) ||
				(((*it) == AudioCallManager::getInstance().getActiveAudioCall()) &&
				((*it)->isCalling()))) {
				_menu->insertItem(
					((*it)->getSessionWindow()).getWidget()->caption(),
					&((*it)->getSessionWindow()), SLOT(showCurrentCallPhonePage()));
			}
		}
	}

	//separator if audio call have been found
	if( _menu->count() != 0 ) {
		_menu->insertSeparator();
	}

	//show Wengophone entry
	_menu->insertItem(QPixmap::fromMimeSource("contact_list.png"), tr("Show Wengophone"), &_mainWindow, SLOT(showAsActiveWindow()));

	//separator
	_menu->insertSeparator();

	/*
	//make call entry
	_menu->insertItem(QPixmap::fromMimeSource("dialpad.png"), tr("Open Dialpad"), &_mainWindow, SLOT(showDialpad()));

	//compose SMS entry
	_menu->insertItem(QPixmap::fromMimeSource("sms_send.png"), tr("Send SMS"), &_mainWindow, SLOT(showSMSSessionWindow()));

	//add a contact entry
	_menu->insertItem(QPixmap::fromMimeSource("contact_add.png"), tr("Add Contact"), &_mainWindow, SLOT(showAddContact()));
	*/

	//call menu
	QPopupMenu * callMenu = new QPopupMenu(_menu);

	QPopupMenu * onNetCallMenu = new QPopupMenu(callMenu);
	QPopupMenu * fixCallMenu = new QPopupMenu(callMenu);
	QPopupMenu * mobileCallMenu = new QPopupMenu(callMenu);

	callMenu->insertItem(
		QPixmap::fromMimeSource("dialpad.png"),
		tr("Compose"),
		&_mainWindow,
		SLOT(showDialpad()));

	callMenu->insertItem(tr("Online Buddies"), onNetCallMenu);
	callMenu->insertItem(tr("Land-Line"), fixCallMenu);
	callMenu->insertItem(tr("Cell-Phone"), mobileCallMenu);

	//chat menu
	QPopupMenu * chatMenu = new QPopupMenu(_menu);

	//sms menu
	QPopupMenu * smsMenu = new QPopupMenu(_menu);
	QAction * sendSMSAction = (QAction *) _mainWindow.getWidget()->child("sendSMSAction", "QAction");
	sendSMSAction->addTo(smsMenu);

	//iterate over all contacts
	ContactList & contactList = ContactList::getInstance();

	for (unsigned int i = 0; i < contactList.size(); i++) {
		//current contact
		Contact & contact = contactList[i];

		//if the contact at at least one phone number, we add an entry
		if( contact.getPhoneList().size() > 0 ) {

			QString id;

			// if the contact has a wengo name & is not offline
			// we add an entry in on net call menu & in start chat menu
			if( (contact.getWengoPhone() != QString::null) && contact.getPresenceStatus().getStatus() != PresenceStatus::Offline) {

				//choose the status pixmap
				QPixmap pix;
				switch(contact.getPresenceStatus().getStatus()) {
					case PresenceStatus::Online:
						pix = QPixmap::fromMimeSource("online.png");
						break;
					case PresenceStatus::Away:
						pix = QPixmap::fromMimeSource("away.png");
						break;
					case PresenceStatus::DoNotDisturb:
						pix = QPixmap::fromMimeSource("dnd.png");
						break;
					case PresenceStatus::UserDefine:
						pix = QPixmap::fromMimeSource("user.png");
						break;
				}
				
				id = MainWindow::ONNETCALL_TAG + contact.getId().toString();
				onNetCallMenu->insertItem(
						pix,
						contact.toString() + ": " + contact.getWengoPhone(),
						&_mainWindow,
						SLOT(actionMenuSlot(int)),
						0,
						id.toInt());
				
				id = MainWindow::CHAT_TAG + contact.getId().toString();
				chatMenu->insertItem(
						pix,
						contact.toString() + ": " + contact.getWengoPhone(),
						&_mainWindow,
						SLOT(actionMenuSlot(int)),
						0,
						id.toInt());
				contacts_online = true;
			}
	
			// if the contact has a mobile phone
			// we add an entry in mobile call menu & in the SMS menu
			if( (contact.getMobilePhone() != QString::null) && (contact.getMobilePhone() != "") ) {

				id = MainWindow::CELLPHONECALL_TAG + contact.getId().toString();
				mobileCallMenu->insertItem(
						QPixmap::fromMimeSource("contact_phone_mobile.png"),
						contact.toString() + ": " + contact.getMobilePhone(),
						&_mainWindow,
						SLOT(actionMenuSlot(int)),
						0,
						id.toInt());

				id = MainWindow::SMS_TAG + contact.getId().toString();
				smsMenu->insertItem(
						QPixmap::fromMimeSource("contact_phone_mobile.png"),
						contact.toString() + ": " + contact.getMobilePhone(),
						&_mainWindow,
						SLOT(actionMenuSlot(int)),
						0,
						id.toInt());
				contacts_mobile = true;
			}

			// if the contact has a land line phone (home & work)
			// we add an entry in phone call menu
			id = MainWindow::HOMECALL_TAG + contact.getId().toString();
			if( (contact.getHomePhone() != QString::null) && (contact.getHomePhone() != "") ) {
				fixCallMenu->insertItem(
						QPixmap::fromMimeSource("contact_phone_work.png"),
						contact.toString() + ": " + contact.getHomePhone(),
						&_mainWindow,
						SLOT(actionMenuSlot(int)),
						0,
						id.toInt());
				contacts_landline = true;
			}

			id = MainWindow::WORKCALL_TAG + contact.getId().toString();
			if( (contact.getWorkPhone() != QString::null) && (contact.getWorkPhone() != "") ) {
				fixCallMenu->insertItem(
						QPixmap::fromMimeSource("contact_phone_home.png"),
						contact.toString() + ": " + contact.getWorkPhone(),
						&_mainWindow,
						SLOT(actionMenuSlot(int)),
						0,
						id.toInt());
				contacts_landline = true;
			}
		}
	}
	if(!contacts_online) {
		onNetCallMenu->insertItem(tr("No contact online"));
		chatMenu->insertItem(tr("No contact online"));
	}
	if(!contacts_mobile) {
		mobileCallMenu->insertItem(tr("No contact match"));
	}
	if(!contacts_landline) {
		fixCallMenu->insertItem(tr("No contact match"));
	}

	_menu->insertItem(QPixmap::fromMimeSource("dialpad.png"), tr("Make Call"), callMenu);
	_menu->insertItem(QPixmap::fromMimeSource("sms_send.png"), tr("Send SMS"), smsMenu);
	_menu->insertItem(QPixmap::fromMimeSource("smileys.png"), tr("Start Chat"), chatMenu);

	//separator
	_menu->insertSeparator();

	//show configuration window entry
	_menu->insertItem(QPixmap::fromMimeSource("configuration.png"), tr("Configuration"), &_mainWindow, SLOT(showConfigurationWindow()));

	//sub menu "internet links"
	QPopupMenu * links_menu = new QPopupMenu(_menu);
	_menu->insertItem(QPixmap::fromMimeSource("html.png"), tr("Internet Links"), links_menu);

	//go to selfcare entry
	links_menu->insertItem(QPixmap::fromMimeSource("html.png"), tr("View My Profile..."), &_mainWindow, SLOT(showMyWengoAccount()));

	//go to help center entry
	links_menu->insertItem(QPixmap::fromMimeSource("html.png"), tr("Help Center"), &_mainWindow, SLOT(showHelp()));

	//separator
	_menu->insertSeparator();

	//quit wengophone entry
	_menu->insertItem(tr("Quit"), this, SLOT(quit()));
}

void Systray::setCallIncoming(const QString & phoneNumber, Contact * contact) {
	QString text = tr("Wengo - Incoming Call");

	_state = CallIncoming;
	_trayIcon->setIcon(QPixmap::fromMimeSource("call_incoming.png"));
	_currentIcon = _trayIcon->icon();
	_trayIcon->setToolTip(text);

	AudioCall * audioCall = AudioCallManager::getInstance().getActiveAudioCall();
	if (!audioCall) {
		audioCall = AudioCallManager::getInstance().createAudioCall();
	}
	SessionWindow * sessionWindow = &audioCall->getSessionWindow();

	_trayIcon->disconnect(SIGNAL(doubleClicked(const QPoint &)));
	connect(_trayIcon, SIGNAL(doubleClicked(const QPoint &)),
		sessionWindow, SLOT(showCurrentCallPhonePage()));

	SystrayPopupWindow * popup = new SystrayPopupWindow();
	popup->setTitle(text);
	popup->setContact(phoneNumber, contact);
}

void Systray::setCallOutgoing() {
	_state = CallOutgoing;
	_trayIcon->setIcon(QPixmap::fromMimeSource("call_outgoing.png"));
	_currentIcon = _trayIcon->icon();
	_trayIcon->setToolTip(tr("Wengo - Outgoing Call"));

	AudioCall * audioCall = AudioCallManager::getInstance().getActiveAudioCall();
	if (!audioCall) {
		audioCall = AudioCallManager::getInstance().createAudioCall();
	}
	SessionWindow * sessionWindow = &audioCall->getSessionWindow();

	_trayIcon->disconnect(SIGNAL(doubleClicked(const QPoint &)));
	connect(_trayIcon, SIGNAL(doubleClicked(const QPoint &)),
		sessionWindow, SLOT(showCurrentCallPhonePage()));
}

void Systray::setCallMissed() {
	_state = CallMissed;
	_trayIcon->setIcon(QPixmap::fromMimeSource("call_missed.png"));
	_currentIcon = _trayIcon->icon();
	_trayIcon->setToolTip(tr("Wengo - Missed Call"));

	_trayIcon->disconnect(SIGNAL(doubleClicked(const QPoint &)));
}

void Systray::setToConfigError() {
	_state = Default;
	_trayIcon->setIcon(QPixmap::fromMimeSource("systray_icon_error.png"));
	_currentIcon = _trayIcon->icon();
	_trayIcon->setToolTip(tr("Wengo"));

	_trayIcon->disconnect(SIGNAL(doubleClicked(const QPoint &)));
	connect(_trayIcon, SIGNAL(doubleClicked(const QPoint &)),
		&_mainWindow, SLOT(showAsActiveWindow()));
}

void Systray::setToDefault() {
	_state = Default;
	_trayIcon->setIcon(QPixmap::fromMimeSource("systray_icon.png"));
	_currentIcon = _trayIcon->icon();
	_trayIcon->setToolTip(tr("Wengo"));

	_trayIcon->disconnect(SIGNAL(doubleClicked(const QPoint &)));
	connect(_trayIcon, SIGNAL(doubleClicked(const QPoint &)),
		&_mainWindow, SLOT(showAsActiveWindow()));
}

void Systray::animatedIcon() {
	static const QPixmap defaultPixmap = QPixmap::fromMimeSource("systray_icon.png");
	static bool changeIcon = true;

	if (changeIcon) {
		_trayIcon->setIcon(defaultPixmap);
		changeIcon = false;
	} else {
		_trayIcon->setIcon(_currentIcon);
		changeIcon = true;
	}
}

void Systray::setToDefaultIfCallMissed(const QPoint & point, int) {
	if (_state == CallMissed) {
		setToDefault();
	}
	//_menu->popup(point);
}

void Systray::hide() {
	_trayIcon->hide();
}

void Systray::quit() {
	Softphone::getInstance().exit();
}

