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

#include <model/account/wengo/WengoAccount.h>
#include <model/NetworkDiscovery.h>
#include <model/account/wengo/WengoAccount.h>
#include <model/imwrapper/IMAccountHandler.h>
#include <model/imwrapper/IMContactMap.h>

#include <Event.h>
#include <Thread.h>
#include <List.h>

#include <string>
#include <vector>

class IPhoneLine;
class PhoneCall;
class WengoPhoneLogger;
class WengoAccount;
class WengoAccountDataLayer;
class WenboxPlugin;
class ContactList;
class Contact;
class StringList;
class ConnectHandler;
class PresenceHandler;
class ChatHandler;

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

	/*
	 * FIXME
	 * Temporary hack, to remove when a configuration framework will be implemented.
	 */
	static std::string HTTP_PROXY_HOSTNAME;
	static unsigned HTTP_PROXY_PORT;
	static std::string CONFIG_FILES_PATH;
	static std::string getConfigFilesPath();

	enum LoginState {
		LoginOk,
		LoginPasswordError,
		LoginNetworkError,
		LoginNoAccount
	};

	/**
	 * Login procedure is done, event with the procedure result.
	 *
	 * @param sender this class
	 * @param state login procedure result
	 * @param login Wengo login used
	 * @param password Wengo password used
	 */
	Event<void (WengoPhone & sender, LoginState state, const std::string & login, const std::string & password)> wengoLoginEvent;

	/**
	 * A PhoneLine has been created.
	 *
	 * @param sender this class
	 * @param phoneLine PhoneLine created
	 */
	Event<void (WengoPhone & sender, IPhoneLine & phoneLine)> phoneLineCreatedEvent;

	/**
	 * The WengoPhoneLogger has been created.
	 *
	 * @param sender this class
	 * @param logger WengoPhoneLogger created
	 */
	Event<void (WengoPhone & sender, WengoPhoneLogger & logger)> loggerCreatedEvent;

	/**
	 * WenboxPlugin has been created.
	 *
	 * @param sender this class
	 * @param wenboxPlugin WenboxPlugin created
	 */
	Event<void (WengoPhone & sender, WenboxPlugin & wenboxPlugin)> wenboxPluginCreatedEvent;

	/**
	 * ConnectHandler has been created.
	 *
	 * @param sender this class
	 * @param imHandler ConnectHandler created
	 */
	Event<void (WengoPhone & sender, ConnectHandler & connectHandler)> connectHandlerCreatedEvent;

	/**
	 * Initialization step is finished (i.e we exit the init() method.
	 *
	 * @param sender this class
	 */
	Event<void (WengoPhone & sender)> initFinishedEvent;

	/**
	 * The ContactList has been created.
	 *
	 * @param sender this class
	 * @param contactList ContactList created
	 */
	Event<void (WengoPhone & sender, ContactList & contactList)> contactListCreatedEvent;

/**
	 * PresenceHandler has been created.
	 *
	 * @param sender this class
	 * @param imHandler ConnectHandler created
	 */
	Event<void (WengoPhone & sender, PresenceHandler & presenceHandler)> presenceHandlerCreatedEvent;

	/**
	 * ChatHandler has been created.
	 *
	 * @param sender this class
	 * @param imHandler ConnectHandler created
	 */
	Event<void (WengoPhone & sender, ChatHandler & chatHandler)> chatHandlerCreatedEvent;

	/** Defines the vector of IPhoneLine. */
	typedef List < IPhoneLine * > PhoneLines;

	WengoPhone();

	~WengoPhone();

	/**
	 * Adds a Wengo account.
	 *
	 * @param login Wengo account login
	 * @param password Wengo account password
	 * @param autoLogin if true login the user automatically (save it to the file)
	 */
	void addWengoAccount(const std::string & login, const std::string & password, bool autoLogin);

	/**
	 * Adds a Contact to the ContactList.
	 *
	 * A new Contact() should be done.
	 * The Contact will be deleted if asked otherwise when the application will be closed.
	 *
	 * @param contact Contact to add to the ContactList
	 * @param contactGroupName name of the ContactGroup where the Contact should be added
	 */
	void addContact(Contact * contact, const std::string & contactGroupName);

	/**
	 * Gets the list of PhoneLine.
	 *
	 * Only one PhoneLine is active at a time.
	 *
	 * @return the list of PhoneLine
	 */
	const PhoneLines & getPhoneLineList() const {
		return _phoneLineList;
	}

	/**
	 * Makes a call given a phone number.
	 *
	 * @param phoneNumber phone number to call
	 */
	void makeCall(const std::string & phoneNumber);

	/**
	 * Gets the active/current PhoneLine.
	 *
	 * Only one PhoneLine is active at a time.
	 *
	 * @return the active PhoneLine
	 */
	IPhoneLine * getActivePhoneLine() const {
		return _activePhoneLine;
	}

	/**
	 * Gets the Wenbox (USB phone device).
	 *
	 * @return the Wenbox
	 */
	WenboxPlugin & getWenboxPlugin() const;

	/**
	 * Sets the active PhoneCall.
	 *
	 * @param phoneCall the active PhoneCall to set
	 */
	void setActivePhoneCall(PhoneCall * phoneCall) {
		_activePhoneCall = phoneCall;
	}

	/**
	 * Gets the active PhoneCall.
	 *
	 * @return the active PhoneCall
	 */
	PhoneCall * getActivePhoneCall() const {
		return _activePhoneCall;
	}

	/**
	 * Gets the StringList representation of all the ContactGroup inside ContactList.
	 *
	 * @return ContactGroup list as a StringList
	 */
	StringList getContactGroupStringList() const;

	/**
	 * Terminates the model component thread i.e this thread.
	 *
	 * This method should be called when you exit definitly WengoPhone.
	 * You cannot call start() then terminate() several times.
	 */
	void terminate();

	ConnectHandler & getConnectHandler() const {
		return *_connectHandler;
	}

	PresenceHandler & getPresenceHandler() const {
		return *_presenceHandler;
	}

	ChatHandler & getChatHandler() const {
		return *_chatHandler;
	}

	IMAccountHandler & getIMAccountHandler() {
		return _imAccountHandler;
	}

	NetworkDiscovery & getNetworkDiscovery() {
			return _networkDiscovery;
	}

	IMContactMap & getIMContactMap() {
		return _imContactMap;
	}

	void setProxySettings(const std::string & proxyAddress, int proxyPort,
		const std::string & proxyLogin, const std::string & proxyPassword);

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
	 * @see addWengoAccount()
	 */
	void addWengoAccountThreadSafe(const std::string & login, const std::string & password, bool autoLogin);

	/**
	 * @see addContact()
	 */
	void addContactThreadSafe(Contact * contact, const std::string & contactGroupName);

	void discoveryDoneEventHandler(NetworkDiscovery & sender,
		NetworkDiscovery::DiscoveryResult result);

	void wengoLoginEventHandler(WengoAccount & sender, WengoAccount::LoginState state,
		const std::string & login, const std::string & password);

		/**
	 * Creates and adds a new PhoneLine given a SipAccount.
	 *
	 * This is a helper method.
	 *
	 * @param account SipAccount associated with the newly created PhoneLine
	 */
	void addPhoneLine(const SipAccount & account);

	/**
	 * @see terminate()
	 */
	void terminateThreadSafe();

	/** The active/current PhoneLine. */
	IPhoneLine * _activePhoneLine;

	/** The active PhoneCall. */
	PhoneCall * _activePhoneCall;

	/** List of PhoneLine. */
	PhoneLines _phoneLineList;

	/** List of Contact. */
	ContactList * _contactList;

	/** The logging system. */
	WengoPhoneLogger * _logger;

	/** Wenbox. */
	WenboxPlugin * _wenboxPlugin;

	/** Data layer to save the Wengo account. */
	WengoAccountDataLayer * _wengoAccountDataLayer;

	/** NetworkDiscovery service. */
	NetworkDiscovery _networkDiscovery;

	ConnectHandler * _connectHandler;

	PresenceHandler * _presenceHandler;

	ChatHandler * _chatHandler;

	IMAccountHandler _imAccountHandler;

	WengoAccount * _wengoAccount;

	IMContactMap _imContactMap;

	/**
	 * If this thread should be terminate or not.
	 *
	 * By default _terminate == false.
	 * @see terminate()
	 */
	bool _terminate;
};

#endif	//WENGOPHONE_H
