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

#include <model/profile/UserProfileHandler.h>

#include "classic/ClassicExterminator.h"
#include "config/ConfigImporter.h"
#include "config/ConfigManagerFileStorage.h"
#include "config/ConfigManager.h"
#include "config/Config.h"
#include "network/NetworkObserver.h"
#include "network/NetworkProxyDiscovery.h"
#include "webservices/subscribe/WsSubscribe.h"
#include "commandserver/CommandServer.h"

#include "WengoPhoneBuildId.h"

#include <http/HttpRequest.h>
#include <thread/Timer.h>
#include <util/Logger.h>
#include <system/RegisterProtocol.h>
#include <util/Path.h>
#include <cutil/global.h>

#include <sstream>

WengoPhone::WengoPhone() {
	_startupSettingListener = new StartupSettingListener();
	_running = false;
	_wsSubscribe = NULL;

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

	//Creating instance of NetworkObserver
	NetworkObserver::getInstance();
	////

	//Creating instance of NetworkProxyDiscovery
	NetworkProxyDiscovery::getInstance();
	////

	Config & config = ConfigManager::getInstance().getCurrentConfig();

	//Binding events
	config.valueChangedEvent +=
		boost::bind(&WengoPhone::valueChangedEventHandler, this, _1, _2);
	////

	//Creating the UserProfileHandler instance
	_userProfileHandler = new UserProfileHandler(*this);
	////

#ifdef OS_WINDOWS
	RegisterProtocol registerProtocol("wengo");
	std::string executableFullName = Path::getApplicationDirPath() + Path::getPathSeparator() + config.getExecutableName();
	registerProtocol.bind(executableFullName + " -c %1", executableFullName + ",0", "http://www.wengo.com");
#endif

	//Creating instance of CommandServer
	CommandServer::getInstance(*this);
	////
}

void WengoPhone::shutdownAfterTimeout() {
	timeoutEvent();
}

WengoPhone::~WengoPhone() {
	while (_running) {
		//Waiting for end of model thread
		Thread::msleep(100);
	}

	//Deleting created objects
	if (_userProfileHandler) {
		delete _userProfileHandler;
	}

	if (_startupSettingListener) {
		delete _startupSettingListener;
	}
	////

	saveConfiguration();

	/**
	 * Set up a timeout triggered if SIP registering is too long
	 * so that closing WengoPhone NG is not too long.
	 */
	static Timer shutdownTimeout;
	shutdownTimeout.timeoutEvent += boost::bind(&WengoPhone::shutdownAfterTimeout, this);
	shutdownTimeout.start(3000, 3000);
}

void WengoPhone::init() {
	Config & config = ConfigManager::getInstance().getCurrentConfig();

	//remove WengoPhone Classic from startup registry
	ClassicExterminator::killClassicExecutable();

	//remove WengoPhone Classic from startup registry
	ClassicExterminator::removeClassicFromStartup();

	//Imports the Config from WengoPhone Classic.
	ConfigImporter importer(*_userProfileHandler, *this);
	importer.importConfig(config.getConfigDir());


	_wsSubscribe = new WsSubscribe();
	wsSubscribeCreatedEvent(*this, *_wsSubscribe);

	//LocalNetworkAccount always created and added by default
	/*LocalNetworkAccount * localAccount = new LocalNetworkAccount();
	localAccount->init();
	addPhoneLine(localAccount);*/

	_userProfileHandler->init();

	//initFinishedEvent
	initFinishedEvent(*this);
}

void WengoPhone::run() {
	init();
	LOG_DEBUG("The model thread is ready for events");

	_running = true;
	runEvents();
	_running = false;
}

void WengoPhone::terminate() {
/*
	typedef ThreadEvent0<void ()> MyThreadEvent;
	MyThreadEvent * event = new MyThreadEvent(boost::bind(&WengoPhone::terminateThreadSafe, this));
	postEvent(event);
*/
	Thread::terminate();
}

void WengoPhone::terminateThreadSafe() {
	_terminate = true;
}

void WengoPhone::saveConfiguration() {
	Config & config = ConfigManager::getInstance().getCurrentConfig();

	ConfigManagerFileStorage configManagerStorage(ConfigManager::getInstance());
	configManagerStorage.save(config.getConfigDir());
}

void WengoPhone::valueChangedEventHandler(Settings & sender, const std::string & key) {
	saveConfiguration();
}
