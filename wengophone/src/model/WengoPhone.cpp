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
#include "webservices/subscribe/Subscribe.h"
#include "WengoPhoneBuildId.h"

#include <util/Logger.h>
#include <http/HttpRequest.h>

#include <sstream>

WengoPhone::WengoPhone()
	: _userProfile(*this) {
	_terminate = false;
	_startupSettingListener = new StartupSettingListener();
	_running = false;
	_wenboxPlugin = NULL;
	_wsWengoSubscribe = NULL;
	
	//set HttpRequest User Agent
	std::stringstream ss;
	ss << WengoPhoneBuildId::SOFTPHONE_NAME;
	ss << "-";
	ss << WengoPhoneBuildId::VERSION;
	ss << "-";
	ss << WengoPhoneBuildId::BUILDID;
	ss << "-";
	ss << WengoPhoneBuildId::REVISION;
	HttpRequest::setUserAgent(ss.str());
}

WengoPhone::~WengoPhone() {
	while (_running) {
		Thread::msleep(100);
	}

	_userProfile.disconnect();

	delete _wenboxPlugin;

	Config & config = ConfigManager::getInstance().getCurrentConfig();

	//Saves the configuration
	ConfigManagerFileStorage configManagerStorage(ConfigManager::getInstance());
	configManagerStorage.save(config.getConfigDir());

	_userProfile.disconnect();

	UserProfileStorage * userProfileStorage = new UserProfileFileStorage(_userProfile);
	userProfileStorage->save(config.getConfigDir());
	delete userProfileStorage;

	delete _startupSettingListener;
}

void WengoPhone::init() {
	//Gets a config instance to create the config instance in the model thread.
	Config & config = ConfigManager::getInstance().getCurrentConfig();

	//Imports the Config from WengoPhone Classic.
	ClassicConfigImporter::importConfig(config.getConfigDir());

	//Loads the configuration: this is the first thing to do before anything else
	ConfigManagerFileStorage configManagerStorage(ConfigManager::getInstance());
	configManagerStorage.load(config.getConfigDir());

	_wenboxPlugin = new WenboxPlugin(*this);
	//Sends the Wenbox creation event
	wenboxPluginCreatedEvent(*this, *_wenboxPlugin);

	_wsWengoSubscribe = new WsWengoSubscribe();
	wsWengoSubscribeCreatedEvent(*this, *_wsWengoSubscribe);
	
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

	_running = true;

	//Keeps the thread running until terminate() is called
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
	_terminate = true;
}

WenboxPlugin & WengoPhone::getWenboxPlugin() const {
	return *_wenboxPlugin;
}
