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

#include "QtIMAccountPresenceMenu.h"

#include <control/profile/CUserProfile.h>

#include <model/profile/UserProfile.h>

#include <imwrapper/IMAccount.h>

#include <QtGui/QtGui>

QtIMAccountPresenceMenu::QtIMAccountPresenceMenu(CUserProfile & cUserProfile, IMAccount & imAccount, QWidget * parent)
	: QtUserProfilePresenceMenu(imAccount.getPresenceState(), imAccount.isConnected(),
			QString::fromStdString(imAccount.getLogin()), parent),
	_cUserProfile(cUserProfile),
	_imAccount(imAccount) {

	connect(this, SIGNAL(onlineClicked()), SLOT(onlineClicked()));

	connect(this, SIGNAL(doNotDisturbClicked()), SLOT(doNotDisturbClicked()));

	connect(this, SIGNAL(invisibleClicked()), SLOT(invisibleClicked()));

	connect(this, SIGNAL(awayClicked()), SLOT(awayClicked()));

	connect(this, SIGNAL(disconnectClicked()), SLOT(disconnectClicked()));
}

QtIMAccountPresenceMenu::~QtIMAccountPresenceMenu() {
}

void QtIMAccountPresenceMenu::onlineClicked() {
	_cUserProfile.getUserProfile().setPresenceState(EnumPresenceState::PresenceStateOnline, &_imAccount);
}

void QtIMAccountPresenceMenu::doNotDisturbClicked() {
	_cUserProfile.getUserProfile().setPresenceState(EnumPresenceState::PresenceStateDoNotDisturb, &_imAccount);
}

void QtIMAccountPresenceMenu::invisibleClicked() {
	_cUserProfile.getUserProfile().setPresenceState(EnumPresenceState::PresenceStateInvisible, &_imAccount);
}

void QtIMAccountPresenceMenu::awayClicked() {
	_cUserProfile.getUserProfile().setPresenceState(EnumPresenceState::PresenceStateAway, &_imAccount);
}

void QtIMAccountPresenceMenu::disconnectClicked() {
	_cUserProfile.getUserProfile().getConnectHandler().disconnect(_imAccount);
}

void QtIMAccountPresenceMenu::connectClicked() {
	_cUserProfile.getUserProfile().getConnectHandler().connect(_imAccount);
}
