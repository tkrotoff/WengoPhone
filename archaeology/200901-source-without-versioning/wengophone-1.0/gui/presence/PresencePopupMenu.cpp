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

#include "PresencePopupMenu.h"

#include "Presence.h"
#include "util/PhoneLineEdit.h"

#include <timer/Timer.h>

#include <qmime.h>
#include <qlineedit.h>

#include <map>
#include <iostream>
using namespace std;

class PresencePopupMenu::PublishPresenceTask : public TimerTask {
public:

	PublishPresenceTask(PresencePopupMenu * presencePopupMenu) {
		_presencePopupMenu = presencePopupMenu;
	}

	virtual void run() {
		_presencePopupMenu->publishMyPresenceStatus();
	}

private:

	PresencePopupMenu * _presencePopupMenu;
};


PresencePopupMenu::PresencePopupMenu(QWidget * parent)
: QPopupMenu(parent) {
	setCheckable(true);

	_presenceStatusList = PresenceStatus::getPresenceStatusList();
	for (unsigned int i = 0; i < _presenceStatusList.size() ; i++) {
		addPresenceStatus(i);
	}

	//New QLineEdit
	_userDefineText = new PhoneLineEdit(this);
	connect(_userDefineText, SIGNAL(returnPressed()),
		this, SLOT(userDefineTextReturnPressed()));
	_userDefineText->setMaxLength(40);
	_userDefineText->setFocus();
	_userDefineText->setHelpText(tr("My Own Status"));
	insertItem(_userDefineText);

	//Sends publish every 9 minutes
	Timer * publishMyPresenceStatusTimer = new Timer(new PublishPresenceTask(this));
	publishMyPresenceStatusTimer->start(9 * 60 * 1000);
	publishMyPresenceStatusTimer->singleShotNow();
}

void PresencePopupMenu::publishOfflineStatus() {
	Presence::publishOfflineStatus();
}

PresencePopupMenu::~PresencePopupMenu() {
	//When the application closes, user is set to offline
	publishOfflineStatus();
}

void PresencePopupMenu::defaultStatus() {
	static const int onlinePresence = 0;
	presenceStatusSelected(onlinePresence);
}

void PresencePopupMenu::addPresenceStatus(int index) {
	int id = insertItem(_presenceStatusList[index].getStatusIcon(), _presenceStatusList[index].getStatusTextTranslated(),
			this, SLOT(presenceStatusSelected(int)));
	setItemParameter(id, index);
}

void PresencePopupMenu::presenceStatusSelected(int index) {
	_myPresenceStatus = _presenceStatusList[index];
	if (_myPresenceStatus.getStatus() == PresenceStatus::UserDefine) {
		_myPresenceStatus.setStatusText(_userDefineText->text());
	}
	publishMyPresenceStatus();
	emit presenceChanged(_myPresenceStatus);
}

void PresencePopupMenu::userDefineTextReturnPressed() {
	//PresenceStatus::UserDefine
	presenceStatusSelected(_presenceStatusList.size() - 1);
	this->hide();
}

void PresencePopupMenu::publishMyPresenceStatus() {
	cerr << "PresencePopupMenu::publishMyPresenceStatus()" << endl;

	switch(_myPresenceStatus.getStatus()) {
	case PresenceStatus::Offline:
		Presence::publishOfflineStatus();
		break;

	default:
		Presence::publishOnlineStatus(_myPresenceStatus.getStatusText());
		break;
	}
}
