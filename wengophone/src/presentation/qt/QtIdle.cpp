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

#include "QtIdle.h"

#include <model/profile/UserProfile.h>
#include <model/config/ConfigManager.h>
#include <model/config/Config.h>

#include <util/Logger.h>

#include <QtGui>

QtIdle::QtIdle(UserProfile & userProfile, QObject * parent)
	: QObjectThreadSafe(parent),
	_userProfile(userProfile) {

	_idle = NULL;

	Config & config = ConfigManager::getInstance().getCurrentConfig();
	config.valueChangedEvent += boost::bind(&QtIdle::configChangedEventHandler, this, _1, _2);

	//Init the idle class
	configChangedEventHandler(config, Config::GENERAL_AWAY_TIMER_KEY);
}

void QtIdle::idleStatusChangedEventHandler(Idle & sender, Idle::Status status) {
	static EnumPresenceState::PresenceState presenceStateSaved;

	switch (status) {
	case Idle::StatusIdle:
		presenceStateSaved = _userProfile.getPresenceState();
		if( presenceStateSaved != EnumPresenceState::PresenceStateInvisible ) {
			_userProfile.setPresenceState(EnumPresenceState::PresenceStateAway, NULL);
		}
		break;

	case Idle::StatusActive:
		//Back to the previous presence state
		_userProfile.setPresenceState(presenceStateSaved, NULL);
		break;

	default:
		LOG_FATAL("unknow idle status=" + String::fromNumber(status));
	}
}

void QtIdle::configChangedEventHandler(Settings & sender, const std::string & key) {
	typedef PostEvent2<void (Settings & sender, const std::string &), Settings &, std::string> MyPostEvent;
	MyPostEvent * event = new MyPostEvent(boost::bind(&QtIdle::configChangedEventHandlerThreadSafe, this, _1, _2), sender, key);
	postEvent(event);
}

void QtIdle::configChangedEventHandlerThreadSafe(Settings & sender, const std::string & key) {
	if (key != Config::GENERAL_AWAY_TIMER_KEY) {
		return;
	}

	Config & config = ConfigManager::getInstance().getCurrentConfig();

	if (!_idle) {
		_idle = new Idle(this);
		_idle->statusChangedEvent += boost::bind(&QtIdle::idleStatusChangedEventHandler, this, _1, _2);
	}

	_idle->setIntervalBeforeIdleStatus(config.getGeneralAwayTimer() * 60000);
	_idle->start();
}
