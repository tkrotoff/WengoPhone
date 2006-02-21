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

#ifndef WENGOACCOUNT_H
#define WENGOACCOUNT_H

#include <model/account/SipAccount.h>
#include <model/config/ConfigItem.h>

#include <http/HttpRequest.h>
#include <Timer.h>
#include <Event.h>

/**
 * Connects to the single sign-on (SSO) system of the Wengo SIP service.
 *
 * @ingroup model
 * @author Tanguy Krotoff
 * @author Philippe Bernery
 */
class WengoAccount : public SipAccount, public ConfigItem {

	/**
	 * WengoAccountParser can directly access to _identity, _username ect...
	 * This avoid getters and setters.
	 */
	friend class WengoAccountParser;

	friend class WengoAccountSerializer;

public:

	WengoAccount(const std::string & login, const std::string & password, bool autoLogin);

	~WengoAccount();

	bool init();

	const std::string & getWengoLogin() const {
		return _wengoLogin;
	}

	const std::string & getWengoPassword() const {
		return _wengoPassword;
	}

	std::string serialize();

	bool unserialize(const std::string & data);

	std::string getName() const {
		return "WengoAccount";
	}

private:

	bool discoverForSSO();

	bool discoverForSIP();

	void wengoLoginEventHandler();

	void answerReceivedEventHandler(const std::string & answer, HttpRequest::Error error);

	void timeoutEventHandler();

	void lastTimeoutEventHandler();

	HttpRequest * _httpRequest;

	std::string _wengoLogin;

	std::string _wengoPassword;

	Timer _timer;

	static const std::string _SSOServer;

	static const std::string _SSOLoginPath;

	/** True if SSO request can be done with SSL. */
	bool _SSOWithSSL;

	/** True if SSO request is Ok. */
	bool _ssoRequestOk;

	/** True if SSO request is Ok and login/password are valid. */
	bool _wengoLoginOk;

	/** True when _timer is finished. */
	bool _timerFinished;
};

#endif	//WENGOACCOUNT_H
