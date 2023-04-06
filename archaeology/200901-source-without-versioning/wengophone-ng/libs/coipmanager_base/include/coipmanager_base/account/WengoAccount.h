/*
 * WengoPhone, a voice over Internet phone
 * Copyright (C) 2004-2007  Wengo
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

#include <coipmanager_base/account/SipAccount.h>

/**
 * WengoAccount.
 *
 * @ingroup coipmanager
 * @author Tanguy Krotoff
 * @author Philippe Bernery
 * @author Mathieu Stute
 */
class COIPMANAGER_BASE_API WengoAccount : public SipAccount {
	friend class WengoAccountXMLSerializer;
	friend class WengoAccountParser;
public:

	WengoAccount();

	WengoAccount(const std::string & wengoLogin, const std::string & wengoPassword);

	WengoAccount(const WengoAccount & wengoAccount);

	explicit WengoAccount(const IAccount * iAccount);

	virtual WengoAccount * clone() const;

	virtual ~WengoAccount();

	/**
	 * Sets the Wengo login (this is the email address of the user).
	 *
	 * Calling this method will reset Sip parameters of the Account.
	 */
	void setWengoLogin(const std::string & wengoLogin);
	const std::string & getWengoLogin() const;

	/**
	 * Sets the Wengo password.
	 *
	 * Calling this method will reset Sip parameters of the Account.
	 */
	void setWengoPassword(const std::string & wengoPassword);
	const std::string & getWengoPassword() const;

	virtual std::string getLogin() const;
	virtual void setLogin(const std::string & login);

	// Used for SSORequest
	void setSSORequestMade(bool made);
	bool isSSORequestMade() const;

	static std::string getWengoServerHostname();
	static std::string getWengoSMSServicePath();
	static std::string getWengoSSOServicePath();
	////

	void setStunServer(const std::string & server);
	const std::string & getStunServer() const;

	//SipAccountType getType() const;

private:

	void copy(const WengoAccount & wengoAccount);

	/**
	 * Resets data that needs to be reset after a setWengoLogin or
	 * a setWengoPassword.
	 */
	void reset();

	std::string _wengoLogin;

	std::string _wengoPassword;

	bool _ssoRequestMade;

	std::string _stunServer;
};

#endif	//OWWENGOACCOUNT_H
