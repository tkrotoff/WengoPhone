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

#include "config/ConfigManager.h"
#include "config/Config.h"
#include "profile/UserProfile.h"
#include "wenbox/WenboxPlugin.h"

#include <util/Logger.h>

WengoPhone::WengoPhone()
	: _userProfile(*this) {
	_wenboxPlugin = new WenboxPlugin(*this);
	_terminate = false;
}

WengoPhone::~WengoPhone() {
	delete _wenboxPlugin;

	//UserProfileXMLSerializer serializer(_userProfile);
	//serializer.serialize(_userProfile);
}

void WengoPhone::init() {
	// Get a config instance to create the config instance in the model thread.
	Config & config = ConfigManager::getInstance().getCurrentConfig();

	//Creates the history
	//historyCreatedEvent


	//Sends the Wenbox creation event
	wenboxPluginCreatedEvent(*this, *_wenboxPlugin);

	//LocalNetworkAccount always created and added by default
	/*LocalNetworkAccount * localAccount = new LocalNetworkAccount();
	localAccount->init();
	addPhoneLine(localAccount);*/

	//UserProfileXMLSerializer serializer(_userProfile);
	//serializer.unserialize(_userProfile);
	_userProfile.init();
	_userProfile.connect();

	//initFinishedEvent
	initFinishedEvent(*this);
}

void WengoPhone::run() {
	init();
	LOG_DEBUG("The model thread is ready for events");

	//Keeps the thread running until terminate() is called
	while (!_terminate) {
		runEvents();
	}
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
