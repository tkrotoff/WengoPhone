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

#include "QtIMMenu.h"

#include <control/profile/CUserProfile.h>

#include <model/profile/UserProfile.h>

#include <imwrapper/IMAccount.h>

QtIMMenu::QtIMMenu(CUserProfile & cUserProfile, IMAccount & imAccount, QWidget * parent) 
: QMenu(QString::fromStdString(imAccount.getLogin()), parent),
_cUserProfile(cUserProfile),
_imAccount(imAccount) {
	QAction * action = NULL;

	action = addAction(tr("Online"));
	if (_imAccount.getPresenceState() == EnumPresenceState::PresenceStateOnline) {
		action->setChecked(true);
	}
	connect(action, SIGNAL(triggered(bool)), SLOT(onlineClicked(bool)));

	action = addAction(tr("Do Not Disturb"));
	if (_imAccount.getPresenceState() == EnumPresenceState::PresenceStateDoNotDisturb) {
		action->setChecked(true);
	}
	connect(action, SIGNAL(triggered(bool)), SLOT(dndClicked(bool)));

	action = addAction(tr("Invisible"));
	if (_imAccount.getPresenceState() == EnumPresenceState::PresenceStateInvisible) {
		action->setChecked(true);
	}
	connect(action, SIGNAL(triggered(bool)), SLOT(invisibleClicked(bool)));

	action = addAction(tr("Away"));
	if (_imAccount.getPresenceState() == EnumPresenceState::PresenceStateAway) {
		action->setChecked(true);
	}
	connect(action, SIGNAL(triggered(bool)), SLOT(awayClicked(bool)));

	if (_imAccount.getProtocol() == EnumIMProtocol::IMProtocolWengo) {
		action = addAction(tr("Forward"));
		if (_imAccount.getPresenceState() == EnumPresenceState::PresenceStateForward) {
			action->setChecked(true);
		}
		connect(action, SIGNAL(triggered(bool)), SLOT(forwardClicked(bool)));
	}

	addSeparator();

	if (_imAccount.isConnected()) {
		action = addAction(tr("Disconnect"));
		connect(action, SIGNAL(triggered(bool)), SLOT(disconnectClicked(bool)));
	} else {
		action = addAction(tr("Connect"));
		connect(action, SIGNAL(triggered(bool)), SLOT(connectClicked(bool)));
	}
}

void QtIMMenu::onlineClicked(bool checked) {
	_cUserProfile.getUserProfile().setPresenceState(EnumPresenceState::PresenceStateOnline, &_imAccount);
}

void QtIMMenu::dndClicked(bool checked) {
	_cUserProfile.getUserProfile().setPresenceState(EnumPresenceState::PresenceStateDoNotDisturb, &_imAccount);
}

void QtIMMenu::invisibleClicked(bool checked) {
	_cUserProfile.getUserProfile().setPresenceState(EnumPresenceState::PresenceStateInvisible, &_imAccount);
}

void QtIMMenu::awayClicked(bool checked) {
	_cUserProfile.getUserProfile().setPresenceState(EnumPresenceState::PresenceStateAway, &_imAccount);
}

void QtIMMenu::forwardClicked(bool checked) {
	_cUserProfile.getUserProfile().setPresenceState(EnumPresenceState::PresenceStateForward, &_imAccount);
}

void QtIMMenu::disconnectClicked(bool checked) {
	_cUserProfile.getUserProfile().setPresenceState(EnumPresenceState::PresenceStateOffline, &_imAccount);
	_cUserProfile.getUserProfile().getConnectHandler().disconnect(_imAccount);
}

void QtIMMenu::connectClicked(bool checked) {
	_cUserProfile.getUserProfile().getConnectHandler().connect(_imAccount);
}
