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

#include "account/NetworkObserver.h"
#include "config/ClassicConfigImporter.h"
#include "config/ConfigManagerFileStorage.h"
#include "config/ConfigManager.h"
#include "config/Config.h"
#include "profile/UserProfileFileStorage.h"
#include "profile/UserProfile.h"
#include "wenbox/WenboxPlugin.h"
#include "webservices/subscribe/Subscribe.h"
#include "WengoPhoneBuildId.h"
#include "classic/ClassicExterminator.h"

#include <util/Logger.h>
#include <thread/Timer.h>
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
	////

	// Creating instance of Config
	Config & config = ConfigManager::getInstance().getCurrentConfig();
	////

	//Loads the configuration: this is the first thing to do before anything else
	ConfigManagerFileStorage configManagerStorage(ConfigManager::getInstance());
	configManagerStorage.load(config.getConfigDir());
	////

	// Binding events
	config.valueChangedEvent +=
		boost::bind(&WengoPhone::valueChangedEventHandler, this, _1, _2);
	////
}

void WengoPhone::shutdownAfterTimeout() {
  timeoutEvent();
}

WengoPhone::~WengoPhone() {
	while (_running) {
		Thread::msleep(100);
	}
	delete _wenboxPlugin;

	saveConfiguration();

	saveUserProfile();

	/**
	 * Set up a timeout triggered if SIP registering is too long
	 * so that closing WengoPhone NG is not too long.
	 */
	static Timer shutdownTimeout;
	shutdownTimeout.timeoutEvent += boost::bind(&WengoPhone::shutdownAfterTimeout, this);
	shutdownTimeout.start(3000, 3000);

	// Disconnect the UserProfile
	_userProfile.disconnect();
	////

	delete _startupSettingListener;
}

void WengoPhone::init() {
	Config & config = ConfigManager::getInstance().getCurrentConfig();

	//remove WengoPhone Classic from startup registry
	ClassicExterminator::killClassicExecutable();

	//remove WengoPhone Classic from startup registry
	ClassicExterminator::removeClassicFromStartup();

	_wenboxPlugin = new WenboxPlugin(*this);
	//Sends the Wenbox creation event
	wenboxPluginCreatedEvent(*this, *_wenboxPlugin);

	//Imports the Config from WengoPhone Classic.
	ClassicConfigImporter importer(*this);
	importer.importConfig(config.getConfigDir());

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

	_userProfile.profileChangedEvent +=
		boost::bind(&WengoPhone::profileChangedEventHandler, this, _1);

	_userProfile.init();
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

void WengoPhone::profileChangedEventHandler(Profile & sender) {
	saveUserProfile();
}

void WengoPhone::saveUserProfile() {
	Config & config = ConfigManager::getInstance().getCurrentConfig();

	UserProfileStorage * userProfileStorage = new UserProfileFileStorage(_userProfile);
	userProfileStorage->save(config.getConfigDir());
	delete userProfileStorage;
}

void WengoPhone::saveConfiguration() {
	Config & config = ConfigManager::getInstance().getCurrentConfig();

	ConfigManagerFileStorage configManagerStorage(ConfigManager::getInstance());
	configManagerStorage.save(config.getConfigDir());
}

void WengoPhone::valueChangedEventHandler(Settings & sender, const std::string & key) {
	saveConfiguration();
}

UserProfile & WengoPhone::getCurrentUserProfile() const {
	return (UserProfile &) _userProfile;
}
