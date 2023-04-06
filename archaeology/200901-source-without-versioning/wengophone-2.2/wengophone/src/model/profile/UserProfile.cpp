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

#include "UserProfile.h"

#include <coipmanager/CoIpManager.h>

#include <model/WengoPhone.h>
#include <model/account/wengo/WengoAccount.h>
#include <model/chat/ChatHandler.h>
#include <model/config/ConfigManager.h>
#include <model/config/Config.h>
#include <model/connect/ConnectHandler.h>
#include <model/contactlist/Contact.h>
#include <model/contactlist/ContactGroup.h>
#include <model/contactlist/ContactList.h>
#include <model/contactlist/IMContactListHandler.h>
#include <model/history/History.h>
#include <model/phonecall/PhoneCall.h>
#include <model/phoneline/PhoneLine.h>
#include <model/phoneline/PhoneLineState.h>
#include <model/phoneline/IPhoneLine.h>
#include <model/presence/PresenceHandler.h>
#include <model/profile/AvatarList.h>
#include <model/history/History.h>
#include <model/webservices/sms/WsSms.h>
#include <model/webservices/softupdate/WsSoftUpdate.h>
#include <model/webservices/info/WsInfo.h>
#include <model/wenbox/WenboxPlugin.h>

#include <sipwrapper/SipWrapper.h>

#include <thread/Thread.h>
#include <thread/ThreadEvent.h>

#include <util/File.h>
#include <util/Logger.h>
#include <util/SafeDelete.h>

#include <exception>

using namespace std;

UserProfile::UserProfile() {
	_wsSms = NULL;
	_wsInfo = NULL;
	_wsCallForward = NULL;
	_wsSoftUpdate = NULL;
	_activePhoneLine = NULL;
	_activePhoneCall = NULL;
	_sipAccount = NULL;

	_imAccountManager = new IMAccountManager();
	_imContactListHandler = new IMContactListHandler(*this);
	_connectHandler = new ConnectHandler(this);
	_presenceHandler = new PresenceHandler(*this);
	_chatHandler = new ChatHandler(*this);
	_contactList = new ContactList(*this);
	_coIpManager = new CoIpManager(*this);

	_sipAccountMustConnectAfterInit = false;
	_sipAccountReadyToConnect = false;
	_wenboxPlugin = NULL;
	_historyLoaded = false;

	// Settings Avatar to default
	_icon = AvatarList::getInstance().getDefaultAvatarPicture();
	////

	_history = new History(*this);
}

UserProfile::~UserProfile() {
	RecursiveMutex::ScopedLock lock(_mutex);

	// Disconnect events to prevent some crashes (call on deleted objects)
	if (_sipAccount) {
		_sipAccount->loginStateChangedEvent -=
			boost::bind(&UserProfile::loginStateChangedEventHandler, this, _1, _2);
		_sipAccount->networkDiscoveryStateChangedEvent -=
			boost::bind(&UserProfile::networkDiscoveryStateChangedEventHandler, this, _1, _2);
	}

	// Prevent some synchronization crashes by unregistering to these Events.
	_connectHandler->unregisterAllEvents();
	_imContactListHandler->unregisterAllEvents();
	_chatHandler->unregisterAllEvents();
	_presenceHandler->unregisterAllEvents();

	OWSAFE_DELETE(_activePhoneCall);
	OWSAFE_DELETE(_wsSms);
	OWSAFE_DELETE(_wsSoftUpdate);
	OWSAFE_DELETE(_wsInfo);
	OWSAFE_DELETE(_wsCallForward);
	OWSAFE_DELETE(_history);
	OWSAFE_DELETE(_wenboxPlugin);

	OWSAFE_DELETE(_coIpManager);
	OWSAFE_DELETE(_contactList);
	OWSAFE_DELETE(_imContactListHandler);
	OWSAFE_DELETE(_chatHandler);
	OWSAFE_DELETE(_presenceHandler);

	disconnect();

	if (_activePhoneLine) {
		Thread::sleep(5); // Sleep for PhApi: lets time to it to unregister
		_activePhoneLine->getSipWrapper().terminate();
	}

	_phoneLineList.clear();
	OWSAFE_DELETE(_activePhoneLine);
	OWSAFE_DELETE(_connectHandler);
	OWSAFE_DELETE(_imAccountManager);

	// HACK haaaack if _sipAccount is detroy when user try to
	//	connect with a bad password the softphone freeze....
	if (_sipAccount && (_sipAccount->getLastLoginState() != EnumSipLoginState::SipLoginStatePasswordError)) {
		OWSAFE_DELETE(_sipAccount);
	}
	////
}

void UserProfile::init() {
	_wenboxPlugin = new WenboxPlugin(*this);

	// Emits IMAccountManager::imAccountAddedEvent
	_imAccountManager->emitIMAccountAddedEvent();
	////

	// Connect sip account
	_sipAccountMustConnectAfterInit = true;
	sipAccountInit();
	////

	// IMAccounts are now connected later ;
	// when SIPAccount is OK
}

std::string UserProfile::getProfileDirectory() const {
	Config & config = ConfigManager::getInstance().getCurrentConfig();
	return File::convertPathSeparators(config.getConfigDir() + "profiles/" + _name + "/");
}

void UserProfile::connect() {
	connectIMAccounts();
}

void UserProfile::connectIMAccounts() {
	// Connects all IMAccounts
	// This will also connect the Wengo IMAccount. This will actually
	// call UserProfile::connectSipAccounts after being through the Connect class.
	IMAccountList imAccountList = _imAccountManager->getIMAccountListCopy();
	for (IMAccountList::const_iterator it = imAccountList.begin();
		it != imAccountList.end(); ++it) {
		if ((*it).getPresenceState() != EnumPresenceState::PresenceStateOffline) {
			_connectHandler->connect((*it).getUUID());
		}
	}
	////
}

bool UserProfile::connectSipAccounts() {
	RecursiveMutex::ScopedLock lock(_mutex);

	// This method is called by the Connect class.

	// Connect the SipAccount if not connected
	if (_activePhoneLine && !_sipAccount->isConnected()) {
		if (!_activePhoneLine->connect()) {
			LOG_ERROR("Couldn't connect phoneline");
			return false;
		}
		LOG_DEBUG("SIP connection OK");
		return true;
	}
	return false;
	////
}

void UserProfile::disconnect() {
	disconnectIMAccounts();
}

void UserProfile::disconnectIMAccounts() {
	IMAccountList imAccountList = _imAccountManager->getIMAccountListCopy();
	for (IMAccountList::const_iterator it = imAccountList.begin();
		it != imAccountList.end(); ++it) {
		_connectHandler->disconnect((*it).getUUID());
	}
}

void UserProfile::disconnectSipAccounts(bool force) {
	RecursiveMutex::ScopedLock lock(_mutex);

	// This method is called by the Connect class.

	if (_activePhoneLine && _sipAccount->isConnected()) {
		_activePhoneLine->disconnect(force);
	}
}

EnumMakeCallError::MakeCallError UserProfile::makeCall(Contact & contact) {
	if (_activePhoneLine) {
		return _activePhoneLine->makeCall(contact.getPreferredNumber());
	} else {
		return EnumMakeCallError::NotConnected;
	}
}

EnumMakeCallError::MakeCallError UserProfile::makeCall(const std::string & phoneNumber) {
	if (_activePhoneLine) {
		return _activePhoneLine->makeCall(phoneNumber);
	} else {
		return EnumMakeCallError::NotConnected;
	}
}

void UserProfile::startIM(Contact & contact) {
	IMContactSet imContactSet;
	IMContact * imContact = contact.getPreferredIMContact();

	if (imContact) {
		imContactSet.insert(*imContact);
		_chatHandler->createSession(imContact->getIMAccountId(), imContactSet);
	} else {
		LOG_ERROR("There is no IMContact available");
	}
}

void UserProfile::setSipAccount(const SipAccount & sipAccount, bool needInitialization) {
	if (_sipAccount) {
		IPhoneLine * phoneLine = findWengoPhoneLine();
		if (phoneLine) {
			phoneLine->disconnect();
			//TODO remove the PhoneLine from _phoneLines & destroy it
		}

		std::vector<EnumIMProtocol::IMProtocol> protocols;
		protocols.push_back(EnumIMProtocol::IMProtocolWengo);
		protocols.push_back(EnumIMProtocol::IMProtocolSIP);
		protocols.push_back(EnumIMProtocol::IMProtocolSIPSIMPLE);
		IMAccountList imAccountList = _imAccountManager->getIMAccountsOfProtocolVector(protocols);
		if (imAccountList.size() > 0) {
			_removeIMAccount(*imAccountList.begin());
		}
		OWSAFE_DELETE(_sipAccount);
	}

	if (sipAccount.getType() == SipAccount::SipAccountTypeWengo) {
		const WengoAccount & wengoAccount = dynamic_cast<const WengoAccount &>(sipAccount);
		//Empty login or password
		if (wengoAccount.getWengoLogin().empty() || wengoAccount.getWengoPassword().empty()) {
			loginStateChangedEvent(*_sipAccount, EnumSipLoginState::SipLoginStatePasswordError);
			return;
		}

		_sipAccount = new WengoAccount(wengoAccount);
	} else {
		if (sipAccount.getIdentity().empty()) {
			loginStateChangedEvent(*_sipAccount, EnumSipLoginState::SipLoginStatePasswordError);
			return;
		}

		_sipAccount = new SipAccount(sipAccount);
	}

	computeName();
	_sipAccount->networkDiscoveryStateChangedEvent +=
		boost::bind(&UserProfile::networkDiscoveryStateChangedEventHandler, this, _1, _2);
	_sipAccount->loginStateChangedEvent +=
		boost::bind(&UserProfile::loginStateChangedEventHandler, this, _1, _2);

	if (needInitialization) {
		// Discovers network.
		// Sends the HTTP request to the SSO if _sipAccount is a Wengo account.
		_sipAccountMustConnectAfterInit = false;
		_sipAccountReadyToConnect = false;
		_sipAccount->init();
	}
}

void UserProfile::addIMAccount(const IMAccount & imAccount) {
	/*
	This code is not used anymore because of ConfigImporter update.
	if (imAccount.getProtocol() == EnumIMProtocol::IMProtocolWengo) {
		LOG_FATAL("cannot add directly a Wengo IMAccount. Must use setWengoAccount");
		return;
	}
	*/
	

	_addIMAccount(imAccount);
}

void UserProfile::_addIMAccount(const IMAccount & imAccount) {
	LOG_DEBUG("adding an IMAccount");

	_imAccountManager->addIMAccount(imAccount);
}

void UserProfile::removeIMAccount(const IMAccount & imAccount) {
	if (imAccount.getProtocol() == EnumIMProtocol::IMProtocolWengo) {
		LOG_FATAL("cannot remove directly a Wengo IMAccount. Must use setWengoAccount");
		return;
	}

	_removeIMAccount(imAccount);
}

void UserProfile::_removeIMAccount(const IMAccount & imAccount) {
	LOG_DEBUG("removing an IMAccount");

	if (_imAccountManager->contains(imAccount.getUUID())) {
		//_connectHandler->disconnect(imAccount.getUUID());
		_imAccountManager->removeIMAccount(imAccount);
	} else {
		LOG_ERROR("IMAccount not in IMAccountManager");
	}
}

void UserProfile::updateIMAccount(const IMAccount & imAccount) {
	LOG_DEBUG("updating IMAccount");

	_imAccountManager->updateIMAccount(imAccount);
}

EnumPresenceState::PresenceState UserProfile::getPresenceState() const {
	IMAccountList imAccountList = _imAccountManager->getIMAccountListCopy();

	IMAccountList::const_iterator
		it = imAccountList.begin(),
		end = imAccountList.end();

	// There is no account
	if (it == end) {
		return EnumPresenceState::PresenceStateUnavailable;
	}

	// Check whether all states are the same
	EnumPresenceState::PresenceState state = it->getPresenceState();
	++it;

	for (; it!= end; ++it) {
		if ((*it).getPresenceState() != state) {
			// They are not
			return EnumPresenceState::PresenceStateMulti;
		}
	}

	// All states are the same
	return state;
}

void UserProfile::setPresenceState(EnumPresenceState::PresenceState presenceState, std::string imAccountId) {
	IMAccount * realIMAccount = _imAccountManager->getIMAccount(imAccountId);
	if (realIMAccount) {
		if (!realIMAccount->isConnected()) {
			_connectHandler->connect(imAccountId);
		}

		_presenceHandler->changeMyPresenceState(presenceState, String::null, realIMAccount);
		OWSAFE_DELETE(realIMAccount);
	}
}

void UserProfile::setAllPresenceState(EnumPresenceState::PresenceState presenceState) {
	if (!isConnected()) {
		connect();
	}

	_presenceHandler->changeMyPresenceState(presenceState, String::null, NULL);
}

void UserProfile::setAlias(const string & alias, IMAccount * imAccount) {
	_alias = alias;
	_presenceHandler->changeMyAlias(_alias, imAccount);
}

bool UserProfile::addPhoneLine(SipAccount & account) {
	//Creates new a PhoneLine associated with the account just added
	PhoneLine * phoneLine = new PhoneLine(account, *this);
	if (!phoneLine->init()) {
		LOG_ERROR("PhoneLine initialisation failed");
		return false;
	}

	//Adds the PhoneLine to the list of PhoneLine
	_phoneLineList += phoneLine;

	//Sets the active PhoneLine
	//FIXME should the last added PhoneLine be the active one all the time?
	if (!_activePhoneLine) {
		_activePhoneLine = phoneLine;
	}

	return true;
}

void UserProfile::sipAccountInit() {
	if (_sipAccount) {
		_sipAccount->networkDiscoveryStateChangedEvent +=
			boost::bind(&UserProfile::networkDiscoveryStateChangedEventHandler, this, _1, _2);
		_sipAccount->loginStateChangedEvent +=
			boost::bind(&UserProfile::loginStateChangedEventHandler, this, _1, _2);

		// Detect network and launch SSO request if needed WengoAccount
		_sipAccountReadyToConnect = false;
		_sipAccount->init();
	}
}

void UserProfile::loginStateChangedEventHandler(SipAccount & sender, EnumSipLoginState::SipLoginState state) {
	// FIXME: due to a threads synchronization problem, it's better to comment this part of code temporarily

	//typedef ThreadEvent2<void (SipAccount sender, EnumSipLoginState::SipLoginState state), SipAccount, EnumSipLoginState::SipLoginState> MyThreadEvent;
	//MyThreadEvent * event =
	//	new MyThreadEvent(boost::bind(&UserProfile::loginStateChangedEventHandlerThreadSafe, this, _1, _2),
	//		dynamic_cast<SipAccount&>(sender), state);

	//WengoPhone::getInstance().postEvent(event);

	loginStateChangedEvent(sender, state);
	loginStateChangedEventHandlerThreadSafe(sender, state);
}

void UserProfile::loginStateChangedEventHandlerThreadSafe(SipAccount & sender, EnumSipLoginState::SipLoginState state) {
	RecursiveMutex::ScopedLock lock(_mutex);

	LOG_DEBUG("SIP_ " + _name);

	switch (state) {
	case EnumSipLoginState::SipLoginStateReady: {
		LOG_DEBUG("SIP_ READY");

		// if no sip account (generic or wengo) is set : create one !!
		IMAccountList imAccountList;
		if (_sipAccount->getType() == SipAccount::SipAccountTypeWengo) {
			imAccountList = _imAccountManager->getIMAccountsOfProtocol(EnumIMProtocol::IMProtocolWengo);
			if (imAccountList.size() == 0) {
				IMAccount imAccount(_sipAccount->getIdentity(), _sipAccount->getPassword(), EnumIMProtocol::IMProtocolWengo);
				_addIMAccount(imAccount);
			}
		} else {
			std::vector<EnumIMProtocol::IMProtocol> protocols;
			protocols.push_back(EnumIMProtocol::IMProtocolSIP);
			protocols.push_back(EnumIMProtocol::IMProtocolSIPSIMPLE);
			imAccountList = _imAccountManager->getIMAccountsOfProtocolVector(protocols);
			if (imAccountList.size() == 0) {
				IMAccount imAccount(_sipAccount->getIdentity(), _sipAccount->getPassword(), EnumIMProtocol::IMProtocolSIP);
				imAccount.setConnected(true);
				_addIMAccount(imAccount);
			}
		}
		////

		if (_sipAccountMustConnectAfterInit) {
			_sipAccountMustConnectAfterInit = false;

			if (!addPhoneLine(*_sipAccount)) {
				LOG_ERROR("Couldn't add phoneline");
				sipAccountConnectionFailedEvent(EnumSipLoginState::SipLoginStateNetworkError);
				break;
			}

			if (_sipAccount->getType() == SipAccount::SipAccountTypeWengo) {

				WengoAccount * wengoAccount = dynamic_cast<WengoAccount*>(_sipAccount);

				//Creates SMS, SMS needs a WengoAccount
				_wsSms = new WsSms(wengoAccount, *this);
				wsSmsCreatedEvent(*this, *_wsSms);
				LOG_DEBUG("SMS created");

				//Creates SoftUpdate, SoftUpdate needs a WengoAccount
				_wsSoftUpdate = new WsSoftUpdate(wengoAccount);
				wsSoftUpdateCreatedEvent(*this, *_wsSoftUpdate);
				LOG_DEBUG("SoftUpdate created");

				_wsInfo = new WsInfo(wengoAccount);
				wsInfoCreatedEvent(*this, *_wsInfo);
				LOG_DEBUG("WsInfo created");

				//callforward
				_wsCallForward = new WsCallForward(wengoAccount);
				wsCallForwardCreatedEvent(*this, *_wsCallForward);
				_wsCallForward->wsCallForwardEvent += boost::bind(&UserProfile::wsCallForwardEventHandler, this, _1, _2, _3);
			}
			phoneLineCreatedEvent(*this, *_activePhoneLine);

			loadHistory(getProfileDirectory());

			//Connect SipAccounts and IMAccounts;
			connect();
		}

		sipAccountConnectedEvent();

		_sipAccountReadyToConnect = true;

		break;
	}

	case EnumSipLoginState::SipLoginStatePasswordError: {
		LOG_DEBUG("SIP_ PASSWORD ERROR");
		if (!_sipAccountReadyToConnect) {
			sipAccountConnectionFailedEvent(EnumSipLoginState::SipLoginStatePasswordError);
		}
		break;
	}

	case EnumSipLoginState::SipLoginStateConnected: {
		LOG_DEBUG("SIP_ CONNECTED");
		break;
	}

	case EnumSipLoginState::SipLoginStateDisconnected: {
		LOG_DEBUG("SIP_ DISCONNECTED");
		break;
	}

	default:
		;
	}
}

void UserProfile::networkDiscoveryStateChangedEventHandler(SipAccount & sender, SipAccount::NetworkDiscoveryState state) {
	networkDiscoveryStateChangedEvent(sender, state);
}

IPhoneLine * UserProfile::findWengoPhoneLine() {
	for (unsigned i = 0; i != _phoneLineList.size() ; i++) {
		const SipAccount & sipAccount = _phoneLineList[i]->getSipAccount();

		if (sipAccount.getType() == SipAccount::SipAccountTypeWengo) {
			return _phoneLineList[i];
		} else {
			LOG_DEBUG("this SipAccount is not a WengoAccount");
		}
	}
	return NULL;
}

void UserProfile::setIcon(const OWPicture & icon, IMAccount * imAccount) {
	Profile::setIcon(icon);
	_presenceHandler->changeMyIcon(icon, imAccount);
}

bool UserProfile::loadHistory(const std::string & path) {
	bool toReturn = _history->load(path + "history.xml");
	_historyLoaded = true;
	historyLoadedEvent(*this, *_history);
	return toReturn;
}

bool UserProfile::saveHistory(const std::string & path) {
	return _history->save(path + "history.xml");
}

bool UserProfile::hasWengoAccount() const {
	return (hasSipAccount() && (_sipAccount->getType() == SipAccount::SipAccountTypeWengo));
}

bool UserProfile::hasSipAccount() const {
	return _sipAccount != NULL;
}

void UserProfile::wsCallForwardEventHandler(WsCallForward & sender,
	int id, WsCallForward::WsCallForwardStatus status) {

	if (status == WsCallForward::WsCallForwardStatusOk) {
		_wsInfo->getWengosCount(false);
		_wsInfo->getSmsCount(false);
		_wsInfo->getActiveMail(false);
		_wsInfo->getUnreadVoiceMail(false);
		_wsInfo->getLandlineNumber(false);
		_wsInfo->getCallForwardInfo(true);
		_wsInfo->execute();
	}
}

void UserProfile::computeName() {
	_name = _sipAccount->getFullIdentity();
}

bool UserProfile::isConnected() const {
	//Checks all IMAccounts
	IMAccountList imAccountList = _imAccountManager->getIMAccountListCopy();

	for (IMAccountList::const_iterator it = imAccountList.begin();
		it != imAccountList.end(); ++it) {

		if ((*it).isConnected()) {
			return true;
		}
	}

	return false;
}
