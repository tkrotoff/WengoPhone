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

#ifndef OWWENGOACCOUNT_H
#define OWWENGOACCOUNT_H

#include <model/account/SipAccount.h>
#include <model/webservices/WengoWebService.h>

#include <thread/Timer.h>
#include <util/Event.h>

class NetworkObserver;

/**
 * Connects to the single sign-on (SSO) system of the Wengo SIP service.
 *
 * @ingroup model
 * @author Tanguy Krotoff
 * @author Philippe Bernery
 * @author Mathieu Stute
 */
class WengoAccount : public SipAccount, public WengoWebService {

	friend class WengoAccountXMLSerializer;

	friend class WengoAccountParser;

public:

	/** Empty WengoAccount. */
	static WengoAccount empty;

	WengoAccount();

	WengoAccount(const std::string & login, const std::string & password, bool autoLogin);

	virtual ~WengoAccount();

	WengoAccount(const WengoAccount & wengoAccount);

	// Inherited from Account
	virtual Account * createCopy() const;
	////

	WengoAccount & operator = (const WengoAccount & wengoAccount);

	void init();

	/** Gets the Wengo login (e-mail address). */
	const std::string & getWengoLogin() const { return _wengoLogin; }

	/** Gets the Wengo Password. */
	const std::string & getWengoPassword() const { return _wengoPassword; }

	/** @return _isValid */
	bool isValid() const { return _isValid; }

	SipAccountType getType() const {
		return SipAccountTypeWengo;
	}

private:

	// Inherited from Account
	virtual void copyTo(Account * account) const;
	////

	void copy(const WengoAccount & wengoAccount);

	EnumSipLoginState::SipLoginState discoverNetwork();

	/**
	 * Discovers network for SSO request.
	 *
	 * Please contact network@openwengo.com before any modifications.
	 */
	bool discoverForSSO();

	/**
	 * Discovers network for SIP.
	 *
	 * Please contact network@openwengo.com before any modifications.
	 */
	bool discoverForSIP();

	void wengoLoginEventHandler();

	void answerReceived(const std::string & answer, int requestId);

	void ssoTimeoutEventHandler();

	void ssoLastTimeoutEventHandler();

	void initTimeoutEventHandler();

	void initLastTimeoutEventHandler();

	void connectionIsUpEventHandler(NetworkObserver & sender);

	void connectionIsDownEventHandler(NetworkObserver & sender);

	std::string _wengoLogin;

	std::string _wengoPassword;

	Timer _ssoTimer;

	Timer _initTimer;

	/** True if SSO request can be done with SSL. */
	bool _ssoWithSSL;

	/** True if SSO request is Ok. */
	bool _ssoRequestOk;

	/** True if SSO request is Ok and login/password are valid. */
	bool _wengoLoginOk;

	/** True when _timer is finished. */
	bool _ssoTimerFinished;

	/** True if network discovery is running. */
	bool _discoveringNetwork;

	/** True if the WengoAccount has been tested and is valid (SSO request ok). */
	bool _isValid;

	/** Number of testSIP retry. */
	static unsigned short _testSIPRetry;

	/** Number of testStun retry. */
	static unsigned short _testStunRetry;
};

#endif	//OWWENGOACCOUNT_H
