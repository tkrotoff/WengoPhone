/*
 * WengoPhone, a voice over Internet phone
 * Copyright (C) 2004-2005  Wengo
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

#include "model/WengoPhone.h"

#include <string>

class Contact;
class PWengoPhone;
class IPhoneLine;
class CPhoneLine;
class WengoPhoneLogger;
class ContactList;
class StringList;
class WenboxPlugin;
class IMHandler;
class PresenceHandler;
class ChatHandler;
class CPresenceHandler;
class CChatHandler;

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
 */
class CWengoPhone {
public:

	CWengoPhone(WengoPhone & wengoPhone);

	PWengoPhone * getPresentation() const {
		return _pWengoPhone;
	}

	/**
	 * @see WengoPhone::makeCall()
	 */
	void makeCall(const std::string & phoneNumber);

	/**
	 * @see WengoPhone::addWengoAccount()
	 */
	void addWengoAccount(const std::string & login, const std::string & password, bool autoLogin);

	/**
	 * @see WengoPhone::addContact()
	 */
	void addContact(Contact * contact, const std::string & contactGroupName);

	/**
	 * @see WengoPhone::getContactGroupStringList()
	 */
	StringList getContactGroupStringList() const;

	/**
	 * Opens a web browser and shows the Wengo account informations inside.
	 */
	void showWengoAccount() const;

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

	WengoPhone & getWengoPhone() const {
		return _wengoPhone;
	}

private:

	void presenceHandlerCreatedEventHandler(WengoPhone & sender, PresenceHandler & presenceHandler);

	void chatHandlerCreatedEventHandler(WengoPhone & sender, ChatHandler & chatHandler);

	void phoneLineCreatedEventHandler(WengoPhone & sender, IPhoneLine & phoneLine);

	void loggerCreatedEventHandler(WengoPhone & sender, WengoPhoneLogger & logger);

	void contactListCreatedEventHandler(WengoPhone & sender, ContactList & contactList);

	void wenboxPluginCreatedEventHandler(WengoPhone & sender, WenboxPlugin & wenboxPlugin);

	void wengoLoginEventHandler(WengoPhone & sender, WengoPhone::LoginState state, const std::string & login, const std::string & password);

	void connectHandlerCreatedEventHandler(WengoPhone & sender, ConnectHandler & connectHandler);

	void initFinishedEventHandler(WengoPhone & sender);

	/** Direct link to the model. */
	WengoPhone & _wengoPhone;

	/** Direct link to the presentation via an interface. */
	PWengoPhone * _pWengoPhone;

	CPresenceHandler * _cPresenceHandler;

	CChatHandler * _cChatHandler;

};

#endif	//CWENGOPHONE_H
