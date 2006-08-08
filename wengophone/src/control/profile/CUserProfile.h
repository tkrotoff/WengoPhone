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

#ifndef OWCUSERPROFILE_H
#define OWCUSERPROFILE_H

#include <control/chat/CChatHandler.h>
#include <control/contactlist/CContactList.h>
#include <control/wenbox/CWenboxPlugin.h>

#include <model/account/SipAccount.h>

#include <imwrapper/EnumIMProtocol.h>

#include <util/Trackable.h>

#include <set>

class CHistory;
class CPhoneLine;
class CSms;
class CSoftUpdate;
class CWengoPhone;
class CWsCallForward;
class CWsDirectory;
class History;
class IMAccount;
class IPhoneLine;
class PhoneCall;
class PUserProfile;
class UserProfile;
class WengoAccount;
class WsCallForward;
class WsDirectory;
class WsSms;
class WsSoftUpdate;

/**
 * Control layer for UserProfile.
 *
 * FIXME: 'init()' must be called on UserProfile before constructing CUserProfile.
 *
 * @author Philippe Bernery
 */
class CUserProfile : public Trackable {
public:

	Event <void (CUserProfile & sender)> makeCallErrorEvent;

	CUserProfile(UserProfile & userProfile, CWengoPhone & cWengoPhone);

	~CUserProfile();

	/**
	 * @see UserProfile::disconnect
	 */
	void disconnect();

	/**
	 * @see UserProfile::makeCall()
	 */
	void makeContactCall(const std::string & contactId);

	/**
	 * @see UserProfile::makeCall()
	 */
	void makeCall(const std::string & phoneNumber);

	/**
	 * @see UserProfile::startIM
	 */
	void startIM(const std::string & contactId);

	/**
	 * @see UserProfile::setWengoAccount
	 */
	void setWengoAccount(const WengoAccount & wengoAccount);

	/**
	 * Gets IMAccounts of a desired protocol.
	 */
	std::set<IMAccount *> getIMAccountsOfProtocol(EnumIMProtocol::IMProtocol protocol) const;

	/**
	 * Gets the active phone call.
	 *
	 * Used for playing DTMF.
	 *
	 * @return active phone call or NULL
	 */
	PhoneCall * getActivePhoneCall() const;

	/**
	 * Gets the CHistory.
	 *
	 * @return the CHistory
	 */
	CHistory * getCHistory() const {
		return _cHistory;
	}

	/**
	 * Gets the CContactList.
	 *
	 * @return the CContactList
	 */
	CContactList & getCContactList() {
		return _cContactList;
	}

	/**
	 * @return the CWsCallForward object.
	 */
	CWsCallForward * getCWsCallForward() {
		return _cWsCallForward;
	}

	/**
	 * @return the CWengoPhone object.
	 */
	CWengoPhone & getCWengoPhone() {
		return _cWengoPhone;
	}

	/**
	 * Gets the UserProfile.
	 *
	 * TODO: This method should not exist (because it allows the GUI to access
	 * the model directly) and so should be removed ASAP.
	 */
	UserProfile & getUserProfile() {
		return _userProfile;
	}

	/**
	 * Gets the Presentation layer.
	 */
	PUserProfile * getPresentation() {
		return _pUserProfile;
	}

private:

	/**
	 * @see UserProfile::loginStateChangedEvent
	 */
	void loginStateChangedEventHandler(SipAccount & sender,
		EnumSipLoginState::SipLoginState state);

	/**
	 * @see UserProfile::networkDiscoveryStateChangedEvent
	 */
	void networkDiscoveryStateChangedEventHandler(SipAccount & sender,
		SipAccount::NetworkDiscoveryState state);

	/**
	 * @see PresenceHandler::authorizationRequestEvent
	 */
	void authorizationRequestEventHandler(PresenceHandler & sender,
		const IMContact & imContact, const std::string & message);

	/**
	 * @see UserProfile::historyLoadedEvent
	 */
	void historyLoadedEventHandler(History & sender);

	/**
	 * @see UserProfile::phoneLineCreatedEvent
	 */
	void phoneLineCreatedEventHandler(UserProfile & sender, IPhoneLine & phoneLine);

	/**
	 * @see UserProfile::wsDirectoryCreatedEvent
	 */
	void wsDirectoryCreatedEventHandler(UserProfile & sender, WsDirectory & wsDirectory);

	/**
	 * @see UserProfile::wsSmsCreatedEvent
	 */
	void wsSmsCreatedEventHandler(UserProfile & sender, WsSms & wsSms);

	/**
	 * @see UserProfile::wsSoftUpdateCreatedEvent
	 */
	void wsSoftUpdateCreatedEventHandler(UserProfile & sender, WsSoftUpdate & wsSoftUpdate);

	/**
	 * @see UserProfile::wsCallForwardCreatedEvent
	 */
	void wsCallForwardCreatedEventHandler(UserProfile & sender, WsCallForward & wsCallForward);

	/**
	 * @see disconnect()
	 */
	void disconnectThreadSafe();

	/**
	 * @see makeCall()
	 */
	void makeContactCallThreadSafe(std::string contactId);

	/**
	 * @see makeCall()
	 */
	void makeCallThreadSafe(std::string phoneNumber);

	/**
	 * @see startIM
	 */
	void startIMThreadSafe(std::string contactId);

	/**
	 * @see setWengoAccount
	 */
	void setWengoAccountThreadSafe(WengoAccount wengoAccount);

	UserProfile & _userProfile;

	PUserProfile * _pUserProfile;

	CContactList _cContactList;

	CHistory * _cHistory;

	CWsCallForward * _cWsCallForward;

	CWsDirectory * _cWsDirectory;

	CWenboxPlugin _cWenboxPlugin;

	CChatHandler _cChatHandler;

	CSms * _cSms;

	CSoftUpdate * _cSoftUpdate;

	CPhoneLine * _cPhoneLine;

	CWengoPhone & _cWengoPhone;
};

#endif	//OWCUSERPROFILE_H
