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

#ifndef WENGOPHONE_H
#define WENGOPHONE_H

#include <model/account/SipAccount.h>
#include <model/profile/UserProfile.h>

#include <util/Event.h>
#include <thread/Thread.h>

#include <string>

class WenboxPlugin;

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
class WengoPhone : public Thread {
public:

	/**
	 * Initialization step is finished (i.e we exit the init() method.
	 *
	 * @param sender this class
	 */
	Event<void (WengoPhone & sender)> initFinishedEvent;

	/**
	 * WenboxPlugin has been created.
	 *
	 * @param sender this class
	 * @param wenboxPlugin WenboxPlugin created
	 */
	Event<void (WengoPhone & sender, WenboxPlugin & wenboxPlugin)> wenboxPluginCreatedEvent;

	WengoPhone();

	~WengoPhone();

	/**
	 * Gets the Wenbox (USB phone device).
	 *
	 * @return the Wenbox
	 */
	WenboxPlugin & getWenboxPlugin() const;

	/**
	 * Terminates the model component thread i.e this thread.
	 *
	 * This method should be called when you exit definitly WengoPhone.
	 * You cannot call start() then terminate() several times.
	 */
	void terminate();

	/**
	 * Gets the current UserProfile.
	 *
	 * @return the current UserProfile
	 */
	UserProfile & getCurrentUserProfile() {
		return _userProfile;
	}

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
	 * @see terminate()
	 */
	void terminateThreadSafe();

	/** Wenbox. */
	WenboxPlugin * _wenboxPlugin;

	/**
	 * True if this thread should be terminate or not.
	 *
	 * By default _terminate == false.
	 * @see terminate()
	 */
	bool _terminate;

	/**
	 * True if the thread is running.
	 */
	bool _running;

	//FIXME: currently only one UserProfile exists
	UserProfile _userProfile;
};

#endif	//WENGOPHONE_H
