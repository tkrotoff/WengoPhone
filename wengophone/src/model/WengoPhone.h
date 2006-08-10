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

#ifndef OWWENGOPHONE_H
#define OWWENGOPHONE_H

#include <model/config/StartupSettingListener.h>

#include <thread/Thread.h>

#include <util/Event.h>
#include <util/Trackable.h>

#include <string>

class ConfigImporter;
class UserProfileHandler;
class WsSubscribe;

/**
 * @defgroup model Model Component
 *
 * The model component is the heart of WengoPhone, it implements all the basic functionnalities
 * above the SIP stack like the ContactList, the History, it handles SIP accounts and more.
 * It contains all the 'intelligence' of WengoPhone.
 * The model is developped in pure C++ inside libraries like LibUtil, Boost, TinyXML and Curl.
 *
 * The model component communicates only with the control component via the desgin
 * pattern observer implemented inside LibUtil with the class Event.
 */

/**
 * Entry point of the application.
 *
 * Design Pattern Mediator.
 *
 * This class starts the model thread.
 * 2 threads are running: the model (including the control) and the presentation.
 * Thus the presentation component can be loaded and showed to the user
 * even before everything is ready.
 *
 * @ingroup model
 * @author Tanguy Krotoff
 */
class WengoPhone : public Thread, public Trackable {
public:

	/**
	 * Initialization step is finished (i.e we exit the init() method.
	 *
	 * @param sender this class
	 */
	Event<void (WengoPhone & sender)> initFinishedEvent;

	/**
	 * WsSubscribe has been created.
	 *
	 * @param sender this class
	 * @param wsWengoSubscribe WsWengoSubscribe created
	 */
	Event<void (WengoPhone & sender, WsSubscribe & wsSubscribe)> wsSubscribeCreatedEvent;

	/**
	 * Application should be closed.
	 *
	 * This event goes to the presentation layer and the presentation layer
	 * should exit the application.
	 *
	 * @param sender this class
	 */
	Event<void (WengoPhone & sender)> exitEvent;

	WengoPhone();

	~WengoPhone();

	/**
	 * Gets the UserProfileHandler.
	 */
	UserProfileHandler & getUserProfileHandler() {
		return *_userProfileHandler;
	}

	/**
	 * Terminates the model component thread i.e this thread.
	 *
	 * This method should be called when you exit definitly WengoPhone.
	 * You cannot call start() then terminate() several times.
	 */
	void terminate();

	/**
	 * Starts the thread of the model component.
	 */
	void run();

private:

	/**
	 * Entry point of the application, equivalent to main().
	 */
	void init();

	/**
	 * Saves the Configuration.
	 */
	void saveConfiguration();

	/**
	 * @see Settings::valueChangedEvent
	 */
	void valueChangedEventHandler(Settings & sender, const std::string & key);

	/** Wengo subscribe web service. */
	WsSubscribe * _wsSubscribe;

	/**
	 * Emergency timeout handler called when SIP unregistering
	 * is not quick enough or timed out.
	 */
	void exitAfterTimeout();

	UserProfileHandler * _userProfileHandler;

	StartupSettingListener * _startupSettingListener;
};

#endif	//OWWENGOPHONE_H
