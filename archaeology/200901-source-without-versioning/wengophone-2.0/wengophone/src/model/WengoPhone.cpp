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

#include <model/profile/UserProfile.h>
#include <model/profile/UserProfileHandler.h>

#include "classic/ClassicExterminator.h"
#include "commandserver/CommandServer.h"
#include "config/Config.h"
#include "config/ConfigImporter.h"
#include "config/ConfigManager.h"
#include "config/ConfigManagerFileStorage.h"
#include "network/NetworkObserver.h"
#include "network/NetworkProxyDiscovery.h"

#include "WengoPhoneBuildId.h"

#include <cutil/global.h>
#include <http/HttpRequest.h>
#include <http/HttpRequestManager.h>
#include <system/RegisterProtocol.h>
#include <thread/Timer.h>
#include <util/Logger.h>
#include <util/Path.h>
#include <util/SafeDelete.h>
#include <webcam/WebcamDriver.h>

#include <sstream>

WengoPhone::WengoPhone() {
	_startupSettingListener = new StartupSettingListener();

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

	// Creating instance of HttpRequestManager
	HttpRequestManager::getInstance();
	////

	//Creating instance of NetworkObserver
	NetworkObserver::getInstance();
	////

	//Creating instance of NetworkProxyDiscovery
	NetworkProxyDiscovery::getInstance();
	////

	//Initialize libwebcam for the model thread.
	WebcamDriver::apiInitialize();
	////

	Config & config = ConfigManager::getInstance().getCurrentConfig();

	//Binding events
	config.valueChangedEvent += boost::bind(&WengoPhone::valueChangedEventHandler, this, _1, _2);
	////

	//Creating the UserProfileHandler instance
	_userProfileHandler = new UserProfileHandler();
	////

	RegisterProtocol registerProtocol("wengo");
	std::string executableFullName = Path::getApplicationDirPath() + config.getExecutableName();
	registerProtocol.bind(executableFullName + " -c %1", executableFullName + ",0", "http://www.wengo.com");

	//Creating instance of CommandServer
	CommandServer::getInstance(*this);
	////
}

void WengoPhone::exitAfterTimeout() {
	exitEvent(*this);
}

WengoPhone::~WengoPhone() {
	//Deleting created objects
	OWSAFE_DELETE(_userProfileHandler);
	OWSAFE_DELETE(_startupSettingListener);
	////

	saveConfiguration();
}

void WengoPhone::init() {
	ConfigManager::getInstance().getCurrentConfig();

	//Remove WengoPhone Classic from startup registry
	ClassicExterminator::killClassicExecutable();

	//Remove WengoPhone Classic from startup registry
	ClassicExterminator::removeClassicFromStartup();

	//Imports the config from a previous WengoPhone version
	ConfigImporter importer(*_userProfileHandler);
	importer.importConfig();

	_userProfileHandler->init();

	//initFinishedEvent
	initFinishedEvent(*this);
}

void WengoPhone::run() {
	init();

	LOG_DEBUG("model thread is ready for events");

	runEvents();
}

void WengoPhone::terminate() {
	Thread::terminate();
}

void WengoPhone::prepareToTerminate() {
	/**
	 * Set up a timeout triggered if SIP registering is too long
	 * so that closing WengoPhone is not too long.
	 */
	static Timer shutdownTimeout;
	shutdownTimeout.timeoutEvent += boost::bind(&WengoPhone::exitAfterTimeout, this);
	shutdownTimeout.start(3000, 3000);
}

void WengoPhone::saveConfiguration() {
	Config & config = ConfigManager::getInstance().getCurrentConfig();

	ConfigManagerFileStorage configManagerStorage(ConfigManager::getInstance());
	configManagerStorage.save(config.getConfigDir());
}

void WengoPhone::valueChangedEventHandler(Settings & sender, const std::string & key) {
	saveConfiguration();
}

CoIpManager * WengoPhone::getCoIpManager() const {
	CoIpManager *result = NULL;
	UserProfile *userProfile = _userProfileHandler->getCurrentUserProfile();

	if (userProfile) {
		result = &userProfile->getCoIpManager();
	}

	return result;
}
