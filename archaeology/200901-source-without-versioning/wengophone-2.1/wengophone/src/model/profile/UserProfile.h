/*
 * WengoPhone, a voice over Internet phone
 * Copyright (C) 2004-2007 Wengo
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
#include "IMAccountManager.h"

#include <imwrapper/EnumPresenceState.h>

#include <model/account/wengo/WengoAccount.h>
#include <model/history/History.h>
#include <model/phoneline/IPhoneLine.h>
#include <model/webservices/callforward/WsCallForward.h>

#include <thread/Condition.h>
#include <thread/RecursiveMutex.h>

#include <util/Event.h>
#include <util/String.h>
#include <util/Trackable.h>

#include <list>

class ChatHandler;
class CoIpManager;
class ConnectHandler;
class Contact;
class ContactList;
class History;
class IMAccount;
class IMAccountManager;
class IMContactListHandler;
class IPhoneLine;
class NetworkObserver;
class PhoneCall;
class PresenceHandler;
class WenboxPlugin;
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
	friend class UserProfileFileStorage1;
	friend class UserProfileXMLSerializer;
	friend class Connect;
public:

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
	 * Emitted when WengoAccount has been found valid after a SSO Request.
	 *
	 * @param valid true if WengoAccount is valid.
	 */
	Event<void (UserProfile & sender, bool valid) > wengoAccountValidityEvent;

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

	UserProfile();

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
	ConnectHandler & getConnectHandler() { return *_connectHandler; }

	/** Gets the PresenceHandler reference. */
	PresenceHandler & getPresenceHandler() { return *_presenceHandler; }

	/** Gets the ChatHandler reference. */
	ChatHandler & getChatHandler() { return *_chatHandler; }

	/**
	 * Gets the IMAccountManager reference. Here we can safely derefence
	 * the pointer as the IMAccountHandler is constructed in
	 * the UserProfile constructor.
	 */
	IMAccountManager & getIMAccountManager() { return *_imAccountManager; }

	/** Gets the IMContactListHandler reference. */
	IMContactListHandler & getIMContactListHandler() { return *_imContactListHandler; }

	/** Gets the ContactList reference. */
	ContactList & getContactList() { return *_contactList; }

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

	/** Gets the CoIpManager. */
	CoIpManager & getCoIpManager() { return *_coIpManager; }

	/** True if history has been loaded. */
	bool isHistoryLoaded() const {
		return _historyLoaded;
	}

	/**
	 * Loads the history.
	 */
	bool loadHistory(const std::string & path);

	/**
	 * Saves the history.
	 */
	bool saveHistory(const std::string & path);

	/**
	 * Sets the SipAccount of this UserProfile.
	 *
	 * The SipAccount os copied internally.
	 *
	 * @param SipAccount the SipAccount to set
	 */
	void setSipAccount(const SipAccount & sipAccount, bool needInitialization = true);

	/**
	 * Return true if a WengoAccount is active.
	 *
	 * @return true if a WengoAccount is active
	 */
	bool hasWengoAccount() const;

	/**
	 * Return true if a SipAccount is active.
	 *
	 * @return true if a SipAccount is active
	 */
	bool hasSipAccount() const;

	/**
	 * Gets the SipAccount.
	 *
	 * @return the SipAccount
	 */
	SipAccount * getSipAccount() const {
		return _sipAccount;
	}

	/**
	 * Gets the WengoAccount.
	 *
	 * @return the WengoAccount
	 */
	WengoAccount * getWengoAccount() const {
		if (hasWengoAccount()) {
			return dynamic_cast<WengoAccount *>(_sipAccount);
		} else {
			return NULL;
		}
	}

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
	 * Updates an IMAccount from this UserProfile.
	 *
	 * An IMAccount of type Wengo must currently not be removed by this method.
	 * setWengoAccount must be used for this purpose.
	 * There is an assert to prevent that.
	 *
	 * @param imAccount the IMAccount to remove
	 */
	void updateIMAccount(const IMAccount & imAccount);

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
	void setIcon(const OWPicture & icon, IMAccount * account);

	/**
	 * Gets the PresenceState of this UserProfile.
	 */
	EnumPresenceState::PresenceState getPresenceState() const;

	/**
	 * Checks if at least one IMAccount is in connected state.
	 *
	 * @return true if at least 1 IMAccount is connected; false otherwise
	 */
	bool isConnected() const;

	/**
	 * Changes the PresenceState of an IMAccount.
	 *
	 * @param presenceState the PresenceState to set
	 * @param imAccountId the id of the IMAccount to apply the PresenceState.
	 */
	void setPresenceState(EnumPresenceState::PresenceState presenceState, std::string imAccountId);

	/**
	 * Changes the PresenceState of all IMAccount.
	 *
	 * @param presenceState the PresenceState to set
	 */
	void setAllPresenceState(EnumPresenceState::PresenceState presenceState);

	/**
	 * Gets the name of this UserProfile.
	 *
	 * The name is computed from the SipAccount. "Default" is returned if no
	 * SipAccount is set. There should be only one Default UserProfile at
	 * a time.
	 */
	std::string getName() const {
		return _name;
	}

private:

	// Inherited from Profile
	virtual void setWengoPhoneId(const std::string & /*wengoPhoneId*/) {}
	virtual std::string getWengoPhoneId() const { return String::null; }
	////

	/**
	 * @see WsCallForward::wsCallForwardEvent
	 */
	void wsCallForwardEventHandler(WsCallForward & sender, int id, WsCallForward::WsCallForwardStatus status);

	/**
	 * Handles SipAccount::loginStateChangedEvent.
	 */
	void loginStateChangedEventHandler(SipAccount & sender, EnumSipLoginState::SipLoginState state);
	void loginStateChangedEventHandlerThreadSafe(SipAccount & sender, EnumSipLoginState::SipLoginState state);

	/** 
	 * Handles SipAccount::networkDiscoveryStateChangedEvent
	 */
	void networkDiscoveryStateChangedEventHandler(SipAccount & sender, SipAccount::NetworkDiscoveryState state);

	/**
	 * Handle History::mementoUpdatedEvent & History::mementoAddedEvent
	 */
	void historyChangedEventHandler(History & sender, int id);

	/**
	 * Compute the name of the UserProfile from the SipAccount
	 * and set the _name variable.
	 */
	void computeName();

	/* Inherited from Profile */
	void setAlias(const std::string & alias) { _alias = alias; };

	void setIcon(const OWPicture & icon) { setIcon(icon, NULL); };
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
	void disconnectSipAccounts();

	/**
	 * Initializes the SipAccount.
	 */
	void sipAccountInit();

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

	/** WsInfo. */
	WsInfo * _wsInfo;

	/** WsCallForward. */
	WsCallForward * _wsCallForward;

	//TODO: create a list of SipAccount
	SipAccount * _sipAccount;

	/** Wenbox. */
	WenboxPlugin * _wenboxPlugin;

	RecursiveMutex _mutex;

	/**
	 * True if the UserProfile must connect after intialization of the
	 * SipAccount
	 */
	bool _sipAccountMustConnectAfterInit;

	bool _sipAccountReadyToConnect;

	bool _historyLoaded;

	IMAccountManager * _imAccountManager;

	IMContactListHandler * _imContactListHandler;

	ConnectHandler * _connectHandler;

	PresenceHandler * _presenceHandler;

	ChatHandler * _chatHandler;

	ContactList * _contactList;

	CoIpManager * _coIpManager;

	/** Name of the UserProfile. */
	std::string _name;
};

#endif	//OWUSERPROFILE_H
