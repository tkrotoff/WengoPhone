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

#include "PresenceStatus.h"

using namespace std;

PresenceStatus::PresenceStatus(Status status) : QObject() {
	_status = status;
	switch(_status) {
	case Online:
		_statusText = "Online";
		_statusIcon = QPixmap::fromMimeSource("online.png");
		break;

	case Away:
		_statusText = "Away";
		_statusIcon = QPixmap::fromMimeSource("away.png");
		break;

	case DoNotDisturb:
		_statusText = "Do Not Disturb";
		_statusIcon = QPixmap::fromMimeSource("dnd.png");
		break;

	default:
		_statusText = "Offline";
		_statusIcon = QPixmap::fromMimeSource("offline.png");
		break;
	}
}

PresenceStatus::PresenceStatus(const QString & statusText) : QObject() {
	_statusText = statusText;
	if (statusText == "Online") {
		_status = Online;
		_statusIcon = QPixmap::fromMimeSource("online.png");
	}

	else if (statusText == "Away") {
		_status = Away;
		_statusIcon = QPixmap::fromMimeSource("away.png");
	}

	else if (statusText == "Do Not Disturb") {
		_status = DoNotDisturb;
		_statusIcon = QPixmap::fromMimeSource("dnd.png");
	}

	else if (statusText == "Offline") {
		_status = Offline;
		_statusIcon = QPixmap::fromMimeSource("offline.png");
	}

	else {
		_status = UserDefine;
		_statusIcon = QPixmap::fromMimeSource("user.png");
	}
}

QString PresenceStatus::getStatusTextTranslated() const {
	switch(_status) {
	case Online:
		return tr("Online");

	case Away:
		return tr("Away");

	case DoNotDisturb:
		return tr("Do Not Disturb");

	case Offline:
		return tr("Offline");

	case UserDefine:
		return _statusText;

	default:
		return _statusText;
	}
}

PresenceStatus::PresenceStatusList PresenceStatus::getPresenceStatusList() {
	PresenceStatusList presenceStatusList;
	presenceStatusList.push_back(PresenceStatus(Online));
	presenceStatusList.push_back(PresenceStatus(Away));
	presenceStatusList.push_back(PresenceStatus(DoNotDisturb));
	presenceStatusList.push_back(PresenceStatus(Offline));
	presenceStatusList.push_back(PresenceStatus(tr("User Define")));

	return presenceStatusList;
}
