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

#include <QtGui/QtGui>

QtIdle::QtIdle(UserProfile & userProfile, QObject * parent)
	: QObjectThreadSafe(parent),
	_userProfile(userProfile) {

	_idle = NULL;

	Config & config = ConfigManager::getInstance().getCurrentConfig();
	config.valueChangedEvent += boost::bind(&QtIdle::configChangedEventHandler, this, _1, _2);

	//Init the idle class
	configChangedEventHandler(config, Config::GENERAL_AWAY_TIMER_KEY);
}

QtIdle::~QtIdle() {
}

void QtIdle::setOnlineIMAccountsAway() {
	_onlineIMAccountIdList.clear();
	IMAccountManager& imAccountManager = _userProfile.getIMAccountManager();
	IMAccountList imAccountList = imAccountManager.getIMAccountListCopy();

	IMAccountList::const_iterator
		it = imAccountList.begin(),
		end = imAccountList.end();
	for (; it!=end; ++it) {
		if (it->getPresenceState() == EnumPresenceState::PresenceStateOnline) {
			std::string id = it->getUUID();
			_onlineIMAccountIdList.append(id);

			_userProfile.setPresenceState(EnumPresenceState::PresenceStateAway, id);
		}
	}
}

void QtIdle::restoreOnlineIMAccounts() {
	Q_FOREACH(std::string id, _onlineIMAccountIdList) {
		_userProfile.setPresenceState(EnumPresenceState::PresenceStateOnline, id);
	}
}

void QtIdle::idleStatusChangedEventHandler(Idle & sender, Idle::Status status) {
	switch (status) {
	case Idle::StatusIdle:
		setOnlineIMAccountsAway();
		break;

	case Idle::StatusActive:
		restoreOnlineIMAccounts();
		break;

	default:
		LOG_FATAL("unknow idle status=" + String::fromNumber(status));
	}
}

void QtIdle::configChangedEventHandler(Settings & sender, const std::string & key) {
	typedef PostEvent1<void (std::string), std::string> MyPostEvent;
	MyPostEvent * event = new MyPostEvent(boost::bind(&QtIdle::configChangedEventHandlerThreadSafe, this, _1), key);
	postEvent(event);
}

void QtIdle::configChangedEventHandlerThreadSafe(std::string key) {
	if (key != Config::GENERAL_AWAY_TIMER_KEY) {
		return;
	}

	if (!_idle) {
		_idle = new Idle(this);
		_idle->statusChangedEvent += boost::bind(&QtIdle::idleStatusChangedEventHandler, this, _1, _2);
	}

	Config & config = ConfigManager::getInstance().getCurrentConfig();
	_idle->setIntervalBeforeIdleStatus(config.getGeneralAwayTimer() * 60000);
	_idle->start();
}
