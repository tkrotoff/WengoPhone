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

#include "QtUserProfilePresenceMenu.h"

#include <QtGui/QtGui>

#include <util/Logger.h>

QtUserProfilePresenceMenu::QtUserProfilePresenceMenu(EnumPresenceState::PresenceState myPresenceState, bool connected,
	const QString & title, QWidget * parent)
	: QMenu(title, parent) {

	switch (myPresenceState) {
	case EnumPresenceState::PresenceStateAway:
		setIcon(QIcon(":/pics/status/away.png"));
		break;
	case EnumPresenceState::PresenceStateOnline:
		setIcon(QIcon(":/pics/status/online.png"));
		break;
	case EnumPresenceState::PresenceStateOffline:
		setIcon(QIcon(":/pics/status/offline.png"));
		break;
	case EnumPresenceState::PresenceStateInvisible:
		setIcon(QIcon(":/pics/status/invisible.png"));
		break;
	case EnumPresenceState::PresenceStateDoNotDisturb:
		setIcon(QIcon(":/pics/status/donotdisturb.png"));
		break;
	case EnumPresenceState::PresenceStateUnknown:
		break;
	default:
		LOG_FATAL("unknown presenceState=" + String::fromNumber(myPresenceState));
		break;
	}

	QAction * action = addAction(QPixmap(":/pics/status/online.png"), tr("Online"));
	connect(action, SIGNAL(triggered(bool)), SLOT(onlineClicked(bool)));

	action = addAction(QPixmap(":/pics/status/away.png"), tr("Away"));
	connect(action, SIGNAL(triggered(bool)), SLOT(awayClicked(bool)));

	action = addAction(QPixmap(":/pics/status/donotdisturb.png"), tr("Do Not Disturb"));
	connect(action, SIGNAL(triggered(bool)), SLOT(doNotDisturbClicked(bool)));

	action = addAction(QPixmap(":/pics/status/invisible.png"), tr("Invisible"));
	connect(action, SIGNAL(triggered(bool)), SLOT(invisibleClicked(bool)));

	action = addAction(QPixmap(":/pics/status/offline.png"), tr("Offline (Disconnect)"));
	action->setEnabled(connected);
	connect(action, SIGNAL(triggered(bool)), SLOT(disconnectClicked(bool)));
}

void QtUserProfilePresenceMenu::onlineClicked(bool checked) {
	onlineClicked();
}

void QtUserProfilePresenceMenu::doNotDisturbClicked(bool checked) {
	doNotDisturbClicked();
}

void QtUserProfilePresenceMenu::invisibleClicked(bool checked) {
	invisibleClicked();
}

void QtUserProfilePresenceMenu::awayClicked(bool checked) {
	awayClicked();
}

void QtUserProfilePresenceMenu::disconnectClicked(bool checked) {
	disconnectClicked();
}
