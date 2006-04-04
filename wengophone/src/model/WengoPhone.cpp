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

#include "WengoPhone.h"

#include "config/ClassicConfigImporter.h"
#include "config/ConfigManagerFileStorage.h"
#include "config/ConfigManager.h"
#include "config/Config.h"
#include "profile/UserProfileFileStorage.h"
#include "profile/UserProfile.h"
#include "wenbox/WenboxPlugin.h"

#include <util/Logger.h>

WengoPhone::WengoPhone()
	: _userProfile(*this) {
	_wenboxPlugin = new WenboxPlugin(*this);
	_terminate = false;
	_running = false;
}

WengoPhone::~WengoPhone() {

	while (_running) {
		Thread::msleep(100);
	}

	delete _wenboxPlugin;

	Config & config = ConfigManager::getInstance().getCurrentConfig();

	ConfigManagerFileStorage configManagerStorage(ConfigManager::getInstance());
	configManagerStorage.save(config.getConfigDir());

	_userProfile.disconnect();

	UserProfileStorage * userProfileStorage = new UserProfileFileStorage(_userProfile);
	userProfileStorage->save(config.getConfigDir());
	delete userProfileStorage;
}

void WengoPhone::init() {
	//Get a config instance to create the config instance in the model thread.
	Config & config = ConfigManager::getInstance().getCurrentConfig();

	//Import the Config from WengoPhone Classic.
	ClassicConfigImporter::importConfig(config.getConfigDir());

	ConfigManagerFileStorage configManagerStorage(ConfigManager::getInstance());
	configManagerStorage.load(config.getConfigDir());

	//Creates the history
	//historyCreatedEvent

	//Sends the Wenbox creation event
	wenboxPluginCreatedEvent(*this, *_wenboxPlugin);

	//LocalNetworkAccount always created and added by default
	/*LocalNetworkAccount * localAccount = new LocalNetworkAccount();
	localAccount->init();
	addPhoneLine(localAccount);*/

	//Loading the UserProfile
	UserProfileStorage * userProfileStorage = new UserProfileFileStorage(_userProfile);
	userProfileStorage->load(config.getConfigDir());
	delete userProfileStorage;

	_userProfile.connect();
	////

	//initFinishedEvent
	initFinishedEvent(*this);
}

void WengoPhone::run() {
	init();
	LOG_DEBUG("The model thread is ready for events");

	//Keeps the thread running until terminate() is called
	_running = true;

	while (!_terminate) {
		runEvents();
	}

	_running = false;
}

void WengoPhone::terminate() {
	typedef ThreadEvent0<void ()> MyThreadEvent;
	MyThreadEvent * event = new MyThreadEvent(boost::bind(&WengoPhone::terminateThreadSafe, this));
	postEvent(event);
}

void WengoPhone::terminateThreadSafe() {
	_userProfile.disconnect();
	_terminate = true;
}

WenboxPlugin & WengoPhone::getWenboxPlugin() const {
	return *_wenboxPlugin;
}
