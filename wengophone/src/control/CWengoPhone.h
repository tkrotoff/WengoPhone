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

#include <model/WengoPhone.h>
#include <model/account/SipAccount.h>

#include <imwrapper/EnumIMProtocol.h>

#include <string>

class Contact;
class PWengoPhone;
class IPhoneLine;
class CPhoneLine;
class ContactList;
class CContactList;
class CHistory;
class StringList;
class WenboxPlugin;
class IMHandler;
class PresenceHandler;
class ConnectHandler;
class ChatHandler;
class PhoneCall;
class Sms;
class SoftUpdate;
class History;
class IMAccount;
class UserProfile;
class WsWengoSubscribe;

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
	 * @see UserProfile::noAccountAvailableEvent
	 */
	Event<void (UserProfile & sender)> noAccountAvailableEvent;

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

	CWengoPhone(WengoPhone & wengoPhone);

	PWengoPhone * getPresentation() const {
		return _pWengoPhone;
	}

	WengoPhone & getWengoPhone() const {
		return _wengoPhone;
	}

	/**
	 * @see WengoPhone::makeCall()
	 */
	void makeCall(const std::string & phoneNumber, bool enableVideo);

	/**
	 * @see WengoPhone::addWengoAccount()
	 */
	void addWengoAccount(const std::string & login, const std::string & password, bool autoLogin);

	/**
	 * @see WengoPhone::addIMAccount()
	 */
	void addIMAccount(const std::string & login, const std::string & password, EnumIMProtocol::IMProtocol protocol);

	/**
	 * Opens a web browser and shows the Wengo account informations inside.
	 */
	void showWengoAccount();

	/**
	 * Opens a web browser and shows the Wengo account creation page.
	 */
	void showWengoAccountCreation();

	/**
	 * Opens a web browser and shows the Wengo help center page.
	 */
	void showWengoFAQ();

	/**
	 * Opens a web browser and shows the Wengo forum page.
	 */
	void showWengoForum();

	/**
	 * Opens a web browser and shows the Wengo smart directory page.
	 */
	void showWengoSmartDirectory();

	/**
	 * Opens a web browser and shows the Wengo callout page.
	 */
	void showWengoCallOut();

	/**
	 * Opens a web browser and shows the Wengo SMS page.
	 */
	void showWengoSMS();

	/**
	 * Opens a web browser and shows the Wengo voice mail page.
	 */
	void showWengoVoiceMail();

	/**
	 * Opens a web browser and shows the Wengo buy page.
	 */
	void showWengoBuyWengos();

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
	 * Gets the active phone call.
	 *
	 * Used for playing DTMF.
	 *
	 * @return active phone call or NULL
	 */
	PhoneCall * getActivePhoneCall() const;

	/**
	 * Gets the CContactList.
	 *
	 * @return the CContactList
	 */
	CContactList & getCContactList() const {
		return *_cContactList;
	}

	/**
	 * Gets the CHistory.
	 *
	 * @return the CHistory
	 */
	CHistory & getCHistory() const {
		return *_cHistory;
	}

private:

	/**
	 * Opens a web browser with the given url
	 * @param url the url to open
	 */
	void openWengoUrlWithoutAuth(std::string url);

	/**
	 * Opens a web browser with the given url
	 * @param url the url to open
	 */
	void openWengoUrlWithAuth(std::string url);

	/**
	 * Handle History::historyLoadedEvent
	 */
	void historyLoadedEventHandler(History & sender);

	void phoneLineCreatedEventHandler(UserProfile & sender, IPhoneLine & phoneLine);

	void wenboxPluginCreatedEventHandler(WengoPhone & sender, WenboxPlugin & wenboxPlugin);

	void wsWengoSubscribeCreatedEventHandler(WengoPhone & sender, WsWengoSubscribe & wsWengoSubscribe);

	void smsCreatedEventHandler(UserProfile & sender, Sms & sms);

	void softUpdateCreatedEventHandler(UserProfile & sender, SoftUpdate & softUpdate);

	void initFinishedEventHandler(WengoPhone & sender);

	void newIMAccountAddedEventHandler(UserProfile & sender, IMAccount & imAccount);


	/**
	 * @see IMPresence::authorizationRequestEvent
	 */
	void authorizationRequestEventHandler(PresenceHandler & sender, const IMContact & imContact,
		const std::string & message);

	/** Direct link to the model. */
	WengoPhone & _wengoPhone;

	/** Direct link to the presentation via an interface. */
	PWengoPhone * _pWengoPhone;

	CContactList * _cContactList;

	CHistory * _cHistory;

	static const std::string URL_WENGO_ACCOUNTCREATION;

	static const std::string URL_WENGO_FORUM;

	static const std::string URL_WENGO_FAQ;

	static const std::string URL_WENGO_SEARCH_EXT;

	static const std::string URL_WENGO_SEARCH_INT;

	static const std::string URL_WENGO_CALLOUT;

	static const std::string URL_WENGO_SMS;

	static const std::string URL_WENGO_VOICEMAIL;

	static const std::string URL_WENGO_ACCOUNT;

	static const std::string URL_WENGO_BUYWENGOS;
};

#endif	//CWENGOPHONE_H
