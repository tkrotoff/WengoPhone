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

#ifndef OWUSERPROFILE_H
#define OWUSERPROFILE_H

#include "Profile.h"

#include <imwrapper/EnumPresenceState.h>
#include <imwrapper/IMAccountHandler.h>

#include <model/account/SipAccount.h>
#include <model/connect/ConnectHandler.h>
#include <model/contactlist/ContactList.h>
#include <model/contactlist/IMContactListHandler.h>
#include <model/history/History.h>
#include <model/presence/PresenceHandler.h>
#include <model/webservices/callforward/WsCallForward.h>

#include <thread/Condition.h>
#include <util/Event.h>
#include <util/Trackable.h>

#include <list>

class Contact;
class ContactList;
class History;
class History;
class IMAccount;
class IPhoneLine;
class NetworkObserver;
class PhoneCall;
class SipAccount;
class Thread;
class WenboxPlugin;
class WengoAccount;
class WsDirectory;
class WsInfo;
class WsSms;
class WsSoftUpdate;

/**
 * Handle the profile of a User.
 *
 * @author Philippe Bernery
 */
class UserProfile : public Profile, public Trackable {
	friend class UserProfileFileStorage;
	friend class UserProfileXMLSerializer;
	friend class Connect;
public:

	static const std::string DEFAULT_USERPROFILE_NAME;

	/**
	 * Login procedure is done, event with the procedure result.
	 *
	 * @see SipAccount::loginStateChangedEvent
	 */
	Event<void (SipAccount & sender, EnumSipLoginState::SipLoginState state)> loginStateChangedEvent;

	/**
	 * Network event while trying to connect a SipAccount.
	 *
	 * @see SipAccount::networkDiscoveryStateChangedEvent
	 */
	Event< void (SipAccount & sender, SipAccount::NetworkDiscoveryState state) > networkDiscoveryStateChangedEvent;

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
	 * @param imAccount the removed IMAccount
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
	Event<void (UserProfile & sender, WsSms & sms)> wsSmsCreatedEvent;

	/**
	 * SoftUpdate has been created.
	 *
	 * @param sender this class
	 * @param sms Sms created
	 */
	Event<void (UserProfile & sender, WsSoftUpdate & softUpdate)> wsSoftUpdateCreatedEvent;

	/**
	 * WsInfo has been created.
	 *
	 * @param sender this class
	 * @param wsInfo WsInfo created
	 */
	Event<void (UserProfile & sender, WsInfo & wsInfo)> wsInfoCreatedEvent;

	/**
	 * WsCallForward has been created.
	 *
	 * @param sender this class
	 * @param wsCallForward WsCallForward created
	 */
	Event<void (UserProfile & sender, WsCallForward & wsCallForward)> wsCallForwardCreatedEvent;

	/**
	 * WsDirectory has been created.
	 *
	 * @param sender this class
	 * @param wsDirectory WsDirectory created
	 */
	Event<void (UserProfile & sender, WsDirectory & wsDirectory)> wsDirectoryCreatedEvent;

	UserProfile(Thread & modelThread);

	virtual ~UserProfile();

	/**
	 * Initializes the UserProfile.
	 */
	void init();

	/**
	 * Return the profile directory
	 * @return profile directory.
	 */
	std::string getProfileDirectory() const;

	/**
	 * Connects all this UserProfile accounts (SipAccount and IMAccount) set
	 * to be automatically connected.
	 */
	void connect();

	/**
	 * Disconnects all accounts.
	 */
	void disconnect();

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
	 * Only one PhoneLine is active at a time
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

	/** Gets the ConnectHandler reference. */
	ConnectHandler & getConnectHandler() { return _connectHandler; }

	/** Gets the PresenceHandler reference. */
	PresenceHandler & getPresenceHandler() { return _presenceHandler; }

	/** Gets the ChatHandler reference. */
	ChatHandler & getChatHandler() { return _chatHandler; }

	/**
	 * Gets the IMAccountHandler reference. Here we can safely derefence
	 * the pointer as the IMAccountHandler is constructed in
	 * the UserProfile constructor.
	 */
	IMAccountHandler & getIMAccountHandler() { return *_imAccountHandler; }

	/** Gets the IMContactListHandler reference. */
	IMContactListHandler & getIMContactListHandler() { return _imContactListHandler; }

	/** Gets the ContactList reference. */
	ContactList & getContactList() { return _contactList; }

	/**
	 * Gets the History reference. Here we can safely derefence the pointer
	 * as the History is constructed in the UserProfile constructor.
	 */
	History & getHistory() { return *_history; }

	/** Gets the WenboxPlugin pointer. */
	WenboxPlugin * getWenboxPlugin() { return _wenboxPlugin; }

	/** Gets the WsInfo pointer. */
	WsInfo * getWsInfo() { return _wsInfo; }

	/** Gets the WengoPhone update WebService pointer. */
	WsSoftUpdate * getWsSoftUpdate() { return _wsSoftUpdate; }

	/** Gets the SMS WebService pointer. */
	WsSms * getWsSms() { return _wsSms; }

	/** Gets the WsCallForward WebService pointer. */
	WsCallForward * getWsCallForward() { return _wsCallForward; }

	/** Gets the WsDirectory */
	WsDirectory * getWsDirectory()  { return _wsDirectory; }

	/** True if history has been loaded. */
	bool isHistoryLoaded() const {
		return _historyLoaded;
	}

	/**
	 * Loads the history.
	 */
	void loadHistory();

	/**
	 * Saves the history.
	 */
	void saveHistory();

	/**
	 * Sets the WengoAccount of this UserProfile.
	 *
	 * The WengoAccount os copied internally.
	 *
	 * @param wengoAccount the WengoAccount to set
	 */
	void setWengoAccount(const WengoAccount & wengoAccount);

	/**
	 * Return true if a WengoAccount is active.
	 *
	 * @return true if a WengoAccount is active
	 */
	bool hasWengoAccount() const;

	/**
	 * Gets the WengoAccount.
	 *
	 * @return the WengoAccount
	 */
	WengoAccount * getWengoAccount() const {
		return _wengoAccount;
	}

	/**
	 * Check if the WengoAccount is valid.
	 *
	 * If no wengo account is set true is returned.
	 * If a wengo account is set but the initalization has not been made
	 * initialization is launched. The method blocks until initialization
	 * is finished (it can take from few seconds to several minutes).
	 */
	bool isWengoAccountValid();

	/**
	 * Adds an IMAccount to this UserProfile.
	 * This method should currently not be called to add a Wengo
	 * IMAccount. A Wengo IMAccount is created internally when setWengoAccount
	 * is called. There is an assert to prevent that.
	 *
	 * The IMAccount is copied internally.
	 *
	 * @param imAccount the IMAccount to add
	 */
	void addIMAccount(const IMAccount & imAccount);

	/**
	 * Removes an IMAccount from this UserProfile.
	 *
	 * An IMAccount of type Wengo must currently not be removed by this method.
	 * setWengoAccount must be used for this purpose.
	 * There is an assert to prevent that.
	 *
	 * @param imAccount the IMAccount to remove
	 */
	void removeIMAccount(const IMAccount & imAccount);

	/**
	 * @see IPhoneLine::makeCall()
	 */
	int makeCall(Contact & contact);

	/**
	 * @see IPhoneLine::makeCall()
	 */
	int makeCall(const std::string & phoneNumber);

	/**
	 * Start a instant messaging with a Contact.
	 *
	 * @param contact the Contact to talk to
	 */
	void startIM(Contact & contact);

	/**
	 * Changes alias of this user.
	 *
	 * @param alias the alias to set
	 * @param imAccount the IMAccount to apply the alias to;
	 *        pass NULL to set the alias to all IMAccount
	 */
	void setAlias(const std::string & alias, IMAccount * imAccount);

	/**
	 * Changes icon of this user.
	 *
	 * @param icon the icon to set
	 * @param imAccount the IMAccount to apply the icon to
	 *        pass NULL to set the icon to all IMAccount
	 */
	void setIcon(const Picture & icon, IMAccount * account);

	/**
	 * Gets the PresenceState of this UserProfile.
	 */
	EnumPresenceState::PresenceState getPresenceState() const {
		return _presenceState;
	}

	/**
	 * Changes the PresenceState of this user.
	 *
	 * @param presenceState the PresenceState to set
	 * @param imAccount the IMAccount to apply the PresenceState to;
	 *        pass NULL to set the alias to all IMAccount
	 */
	void setPresenceState(EnumPresenceState::PresenceState presenceState, IMAccount * imAccount);

	/**
	 * Gets the name of this UserProfile.
	 *
	 * The name is computed from the WengoAccount. "Default" is returned if no
	 * WengoAccount is set. There should be only one Default UserProfile at
	 * a time.
	 */
	std::string getName() const {
		return _name;
	}

private:

	// Inherited from Profile
	virtual void setWengoPhoneId(const std::string & wengoPhoneId) {}
	virtual std::string getWengoPhoneId() const {}
	////

	/**
	 * @see ConnectHandler::connectedEvent
	 */
	void connectedEventHandler(ConnectHandler & sender, IMAccount & imAccount);

	/**
	 * @see WsCallForward::wsCallForwardEvent
	 */
	void wsCallForwardEventHandler(WsCallForward & sender, int id, WsCallForward::WsCallForwardStatus status);

	/**
	 * Handle SipAccount::loginStateEvent.
	 */
	void loginStateChangedEventHandler(SipAccount & sender, EnumSipLoginState::SipLoginState state);

	/**
	 * Handle History::mementoUpdatedEvent & History::mementoAddedEvent
	 */
	void historyChangedEventHandler(History & sender, int id);

	/**
	 * Compute the name of the UserProfile from the WengoAccount
	 * and set the _name variable.
	 */
	void computeName();

	/* Inherited from Profile */
	void setAlias(const std::string & alias) { _alias = alias; };

	void setIcon(const Picture & icon) { setIcon(icon, NULL); };
	/**/

	/**
	 * Actually adds an IMAccount. Used internally and by addIMAccount after
	 * checking the given IMAccount.
	 */
	void _addIMAccount(const IMAccount & imAccount);

	/**
	 * Actually removes an IMAccount. Used internally and by removeIMAccount after
	 * checking the given IMAccount.
	 */
	void _removeIMAccount(const IMAccount & imAccount);

	/**
	 * Connect all IMAccounts.
	 */
	void connectIMAccounts();

	/**
	 * Disconnect all IMAccounts.
	 */
	void disconnectIMAccounts();

	/**
	 * Connect all SipAccounts.
	 */
	void connectSipAccounts();

	/**
	 * Disconnect all SipAccounts.
	 *
 	 * @param now if true, disconnect without doing any transactions
	 */
	void disconnectSipAccounts(bool now = false);

	/**
	 * Initializes the WengoAccount.
	 */
	void wengoAccountInit();

	/**
	 * Creates and adds a new PhoneLine given a SipAccount.
	 *
	 * This is a helper method.
	 *
	 * @param account SipAccount associated with the newly created PhoneLine
	 */
	void addPhoneLine(SipAccount & account);

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

	/** History. */
	History * _history;

	/** WengoPhone update system. */
	WsSoftUpdate * _wsSoftUpdate;

	/** SMS. */
	WsSms * _wsSms;

	/** WsInfo */
	WsInfo * _wsInfo;

	/** WsCallForward. */
	WsCallForward * _wsCallForward;

	/** WsDirectory */
	WsDirectory * _wsDirectory;

	//TODO: create a list of SipAccount
	WengoAccount * _wengoAccount;

	/** Wenbox. */
	WenboxPlugin * _wenboxPlugin;

	bool _wengoAccountConnected;

	bool _wengoAccountInitializationFinished;

	bool _wengoAccountIsValid;

	Condition _wengoAccountIsValidCondition;

	Mutex _wengoAccountIsValidMutex;

	/**
	 * True if the UserProfile must connect after intialization of the
	 * WengoAccount (after SSO request).
	 */
	bool _wengoAccountMustConnectAfterInit;

	bool _historyLoaded;

	IMAccountHandler * _imAccountHandler;

	/** List of Contact. */
	IMContactListHandler _imContactListHandler;

	ConnectHandler _connectHandler;

	PresenceHandler _presenceHandler;

	ChatHandler _chatHandler;

	ContactList _contactList;

	EnumPresenceState::PresenceState _presenceState;

	/** Name of the UserProfile. */
	std::string _name;

	Thread & _modelThread;
};

#endif //OWUSERPROFILE_H
