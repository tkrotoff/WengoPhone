/*
 * WengoPhone, a voice over Internet phone
 * Copyright (C) 2004-2006 Wengo
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

#ifndef USERPROFILE_H
#define USERPROFILE_H

#include "Profile.h"

#include <imwrapper/EnumPresenceState.h>
#include <imwrapper/IMAccountHandler.h>

#include <model/account/SipAccount.h>
#include <model/history/History.h>
#include <model/connect/ConnectHandler.h>
#include <model/contactlist/ContactList.h>
#include <model/contactlist/IMContactListHandler.h>
#include <model/presence/PresenceHandler.h>

#include <util/Event.h>

#include <list>

class SipAccount;
class WengoAccount;
class IMAccount;
class WengoPhone;
class Contact;
class ContactList;
class History;
class IPhoneLine;
class PhoneCall;
class Sms;
class SoftUpdate;
class History;
class WsInfo;

/**
 * Handle the profile of a User.
 *
 * @author Philippe Bernery
 */
class UserProfile : public Profile {
	friend class UserProfileFileStorage;
public:

	/**
	 * Login procedure is done, event with the procedure result.
	 *
	 * @see SipAccount::loginStateChangedEvent
	 */
	Event<void (SipAccount & sender, SipAccount::LoginState state)> loginStateChangedEvent;

	/**
	 * Network event while trying to connect a SipAccount.
	 *
	 * @see SipAccount::networkDiscoveryStateChangedEvent
	 */
	Event< void (SipAccount & sender, SipAccount::NetworkDiscoveryState state) > networkDiscoveryStateChangedEvent;

	/**
	 * @see SipAccount::proxyNeedsAuthenticationEvent
	 */
	Event<void (SipAccount & sender, const std::string & proxyAddress, unsigned proxyPort)> proxyNeedsAuthenticationEvent;

	/**
	 * @see SipAccount::wrongProxyAuthenticationEvent
	 */
	Event<void (SipAccount & sender,
		const std::string & proxyAddress, unsigned proxyPort,
		const std::string & proxyLogin, const std::string & proxyPassword)> wrongProxyAuthenticationEvent;

	/**
	 * Emitted when no account exists.
	 */
	Event<void (UserProfile & sender)> noAccountAvailableEvent;

	/**
	 * A new IMAccount has been added.
	 *
	 * @param sender this class
	 * @param imAccount the added IMAccount. The reference remains valid during
	 * runtime or until the IMAccount has been removed.
	 */
	Event< void (UserProfile & sender, IMAccount & imAccount) > newIMAccountAddedEvent;

	/**
	 * An IMAccount has been removed.
	 *
	 * @param sender this class
	 * @param imAccount the removed IMAccount.
	 */
	Event< void (UserProfile & sender, IMAccount & imAccount) > imAccountRemovedEvent;

	/**
	 * The history has been loaded.
	 *
	 * @param sender this class
	 * @param History History loaded
	 */
	Event<void (UserProfile & sender, History & history)> historyLoadedEvent;


	/**
	 * A PhoneLine has been created.
	 *
	 * @param sender this class
	 * @param phoneLine PhoneLine created
	 */
	Event<void (UserProfile & sender, IPhoneLine & phoneLine)> phoneLineCreatedEvent;

	/**
	 * Sms has been created.
	 *
	 * @param sender this class
	 * @param sms Sms created
	 */
	Event<void (UserProfile & sender, Sms & sms)> smsCreatedEvent;

	/**
	 * SoftUpdate has been created.
	 *
	 * @param sender this class
	 * @param sms Sms created
	 */
	Event<void (UserProfile & sender, SoftUpdate & softUpdate)> softUpdateCreatedEvent;

	/**
	 * WsInfo has been created.
	 *
	 * @param sender this class
	 * @param wsInfo WsInfo created
	 */
	Event<void (UserProfile & sender, WsInfo & wsInfo)> wsInfoCreatedEvent;


	UserProfile(WengoPhone & wengoPhone);

	/**
	 * Connects all this UserProfile accounts (SipAccount and IMAccount) set
	 * to be automatically connected.
	 */
	void connect();

	/**
	 * Disconnects all accounts.
	 */
	void disconnect();

	virtual ~UserProfile();

	/** List of IPhoneLine. */
	typedef List < IPhoneLine * > PhoneLines;

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
	 * Gets the active phone call.
	 *
	 * Used for playing DTMF.
	 *
	 * @return active phone call or NULL
	 */
	PhoneCall * getActivePhoneCall() const;

	ConnectHandler & getConnectHandler() {
		return _connectHandler;
	}

	PresenceHandler & getPresenceHandler() {
		return _presenceHandler;
	}

	ChatHandler & getChatHandler() {
		return _chatHandler;
	}

	IMAccountHandler & getIMAccountHandler() {
		return *_imAccountHandler;
	}

	IMContactListHandler & getIMContactListHandler() {
		return _imContactListHandler;
	}

	ContactList & getContactList() {
		return _contactList;
	}

	History & getHistory() {
		return *_history;
	}

	/**
	 * Load the history.
	 */
	void loadHistory();
	
	/**
	 * Save the history.
	 */
	void saveHistory();
	
	void addSipAccount(const std::string & login, const std::string & password, bool autoLogin);

	/**
	 * Adds an IMAccount to this UserProfile.
	 *
	 * The IMAccount is copied internally.
	 *
	 * @param imAccount the IMAccount to add
	 */
	void addIMAccount(const IMAccount & imAccount);

	/**
	 * Removes an IMAccount from this UserProfile.
	 *
	 * @param imAccount the IMAccount to remove
	 */
	void removeIMAccount(const IMAccount & imAccount);

	/**
	 * Makes a call given a Contact.
	 *
	 * @param contact the Contact to call
	 */
	void makeCall(Contact & contact);

	/**
	 * Makes a call given a phone number.
	 *
	 * @param phoneNumber the phone number to call
	 */
	void makeCall(const std::string & phoneNumber);

	/**
	 * Start a instant messaging with a Contact.
	 *
	 * @param contact the Contact to talk to
	 */
	void startIM(Contact & contact);

	/**
	 * Changes alias of this user.
	 *
	 * @param alias the alias to set.
	 * @param imAccount the IMAccount to apply the alias to.
	 * Pass NULL to set the alias to all IMAccount.
	 */
	void setAlias(const std::string & alias, IMAccount * imAccount);

	/**
	 * Changes icon of this user.
	 *
	 * @param icon the icon to set
	 * @param imAccount the IMAccount to apply the icon to. 
	 * Pass NULL to set the icon to all IMAccount.
	 */
	void setIcon(const Picture & icon, IMAccount * account);

	EnumPresenceState::PresenceState getPresenceState() const;

	/**
	 * Changes the PresenceState of this user.
	 *
	 * @param presenceState the PresenceState to set
	 * @param imAccount the IMAccount to apply the PresenceState to.
	 * Pass NULL to set the alias to all IMAccount.
	 */
	void setPresenceState(EnumPresenceState::PresenceState presenceState, IMAccount * imAccount);

private:

	/* Inherited from Profile */
	void setAlias(const std::string & alias) { _alias = alias; };

	void setIcon(const Picture & icon) { setIcon(icon, NULL); };

	void setPresenceState(EnumPresenceState::PresenceState presenceState) {};
	/**/

	/*
	 * @see ConnectHandler::connectedEvent
	 */
	void connectedEventHandler(ConnectHandler & sender, IMAccount & imAccount);

	/**
	 * Connect all IMAccounts.
	 */
	void connectIMAccounts();

	/**
	 * Connect all SipAccounts.
	 */
	void connectSipAccounts();

	/**
	 * Login the WengoAccount.
	 */
	void wengoAccountLogin();

	/**
	 * Handle SipAccount::loginStateEvent.
	 */
	void loginStateChangedEventHandler(SipAccount & sender, SipAccount::LoginState loginState);

	/**
	 * @see addSipAccount()
	 */
	void addSipAccountThreadSafe(const std::string & login, const std::string & password, bool autoLogin);

	/**
	 * Creates and adds a new PhoneLine given a SipAccount.
	 *
	 * This is a helper method.
	 *
	 * @param account SipAccount associated with the newly created PhoneLine
	 */
	void addPhoneLine(SipAccount & account);

	/**
	 * Handle History::mementoUpdatedEvent & History::mementoAddedEvent
	 */
	void historyChangedEventHandler(History & sender, int id);

	WengoPhone & _wengoPhone;

	/**
	 * find the wengo phone line
	 */
	IPhoneLine * findWengoPhoneLine();

	/** The active/current PhoneLine. */
	IPhoneLine * _activePhoneLine;

	/** The active PhoneCall. */
	PhoneCall * _activePhoneCall;

	/** List of PhoneLine. */
	PhoneLines _phoneLineList;

	/** SMS. */
	Sms * _sms;

	/** History. */
	History * _history;
	
	/** WengoPhone update system. */
	SoftUpdate * _softUpdate;

	/** WsInfo */
	WsInfo * _wsInfo;
	
	//TODO: create a list of SipAccount
	WengoAccount * _wengoAccount;

	IMAccountHandler * _imAccountHandler;

	/** List of Contact. */
	IMContactListHandler _imContactListHandler;

	ConnectHandler _connectHandler;

	PresenceHandler _presenceHandler;

	ChatHandler _chatHandler;
	ContactList _contactList;

};

#endif //USERPROFILE_H
