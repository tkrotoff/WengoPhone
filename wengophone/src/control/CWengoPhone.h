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

#ifndef CWENGOPHONE_H
#define CWENGOPHONE_H

#include <control/profile/CUserProfileHandler.h>

#include <model/WengoPhone.h>
#include <model/account/SipAccount.h>

#include <imwrapper/EnumIMProtocol.h>

#include <string>

class Contact;
class CUserProfile;
class IMAccount;
class PresenceHandler;
class PWengoPhone;
class UserProfile;
class WengoAccount;
class WsSubscribe;


/**
 * @defgroup control Control Component
 *
 * Like the model component, the control is part of the heart of WengoPhone.
 * It is developped in pure C++ using libraries like LibUtil, Boost, TinyXML and Curl.
 * This component has a very limited 'intelligence', everything is in fact done
 * inside the model component.
 *
 * The control component goal is to make the jonction between the model component
 * and the presentation component.
 *
 * The control component receives informations from the model component via the design
 * pattern observer. On the other hand it sends informations to the model component directly.
 * The control component dialogs with the presentation component via interfaces thus allowing
 * different graphical interfaces to be implemented (Qt, GTK+...).
 *
 * All classes inside the control component are named using a leading 'C' letter and
 * are inside the subdirectory control.
 */

/**
 * @ingroup control
 * @author Tanguy Krotoff
 * @author Philippe Bernery
 * @author Mathieu Stute
 */
class CWengoPhone {
public:

	/**
	 * @see UserProfile::loginStateChangedEvent
	 */
	Event<void (SipAccount & sender, SipAccount::LoginState state)> loginStateChangedEvent;

	/**
	 * @see UserProfile::networkDiscoveryStateChangedEvent
	 */
	Event<void (SipAccount & sender, SipAccount::NetworkDiscoveryState state) > networkDiscoveryStateChangedEvent;

	/**
	 * @see UserProfile::proxyNeedsAuthenticationEvent
	 */
	Event<void(SipAccount & sender, const std::string & proxyAddress, unsigned proxyPort)> proxyNeedsAuthenticationEvent;

	/**
	 * @see UserProfile::wrongProxyAuthenticationEvent
	 */
	Event<void(SipAccount & sender,
		const std::string & proxyAddress, unsigned proxyPort,
		const std::string & proxyLogin, const std::string & proxyPassword)> wrongProxyAuthenticationEvent;

	/**
	 * @see WengoPhone::timeoutEvent
	 */
	Event<void ()> controlTimeoutEvent;

	CWengoPhone(WengoPhone & wengoPhone);

	PWengoPhone * getPresentation() const {
		return _pWengoPhone;
	}

	WengoPhone & getWengoPhone() const {
		return _wengoPhone;
	}

	Thread & getModelThread() const  {
		return _wengoPhone;
	}

	/**
	 * Entry point of the application, equivalent to main().
	 *
	 * Starts the object WengoPhone thus almost everything.
	 * Starts the model component thread (the thread WengoPhone).
	 *
	 * @see WengoPhone::init()
	 * @see WengoPhone::run()
	 */
	void start();

	/**
	 * @see WengoPhone::terminate()
	 */
	void terminate();

	/**
	 * Used by GUI to prevent control that the UserProfile can be deleted.
	 */
	void currentUserProfileReleased();

	/**
	 * Gets the CUserProfile.
	 *
	 * @return the CUserProfile
	 */
	CUserProfile * getCUserProfile() const {
		return _cUserProfile;
	}

	/**
	 * Gets the CUserProfileHandler
	 */
	CUserProfileHandler & getCUserProfileHandler() {
		return _cUserProfileHandler;
	}

private:

	void initFinishedEventHandler(WengoPhone & sender);

	void wsSubscribeCreatedEventHandler(WengoPhone & sender, WsSubscribe & wsSubscribe);

	/**
	 * @see IMPresence::authorizationRequestEvent
	 */
	void authorizationRequestEventHandler(PresenceHandler & sender, const IMContact & imContact,
		const std::string & message);

	/**
	 * @see UserProfileHandler::noCurrentUserProfileSetEvent
	 */
	void noCurrentUserProfileSetEventHandler(UserProfileHandler & sender);

	/**
	 * @see UserProfileHandler::currentUserProfileWillDieEvent
	 */
	void currentUserProfileWillDieEventHandler(UserProfileHandler & sender);

	/**
	 * @see UserProfileHandler::userProfileInitializedEvent
	 */
	void userProfileInitializedEventHandler(UserProfileHandler & sender, UserProfile & userProfile);

	/**
	 * @see UserProfileHandler::wengoAccountNotValidEvent
	 */
	void wengoAccountNotValidEventHandler(UserProfileHandler & sender, WengoAccount & wengoAccount);

	/** Direct link to the model. */
	WengoPhone & _wengoPhone;

	/** Direct link to the presentation via an interface. */
	PWengoPhone * _pWengoPhone;

	CUserProfile * _cUserProfile;

	CUserProfileHandler _cUserProfileHandler;
};

#endif	//CWENGOPHONE_H
