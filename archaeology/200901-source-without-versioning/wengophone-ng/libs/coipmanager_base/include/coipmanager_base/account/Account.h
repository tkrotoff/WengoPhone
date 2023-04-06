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

#ifndef OWACCOUNT_H
#define OWACCOUNT_H

#include <coipmanager_base/account/IAccount.h>

/**
 * CoIp account.
 *
 * @author Philippe Bernery
 */
class COIPMANAGER_BASE_API Account : public IAccount {
	friend class AccountXMLSerializer;
public:

	Account();

	Account(const std::string & login, const std::string & password,
		EnumAccountType::AccountType accountType);

	Account(const Account & account);

	Account(const IAccount * iAccount);

	Account & operator = (const Account & account);

	virtual Account * clone() const;

	virtual ~Account();

	virtual std::string getLogin() const;
	virtual void setLogin(const std::string & login);

	virtual std::string getPassword() const;
	virtual void setPassword(const std::string & password);

	virtual std::string getAlias() const;
	virtual void setAlias(const std::string & alias);

	virtual EnumAccountType::AccountType getAccountType() const;

	virtual void setPresenceState(EnumPresenceState::PresenceState presenceState);
	virtual EnumPresenceState::PresenceState getPresenceState() const;

	virtual void setConnectionState(EnumConnectionState::ConnectionState state);
	virtual EnumConnectionState::ConnectionState getConnectionState() const;

	virtual void setConnectionConfigurationSet(bool set);
	virtual bool isConnectionConfigurationSet() const;

	virtual void setSavePassword(bool savePassword);
	virtual bool isPasswordSaved() const;

	/**
	 * Returns the actual IAccount.
	 * This can be casted safely to the type of IAccount given by
	 * getAccountType()
	 *
	 * Example:
	 * <pre>
	 * Account account = session->getAccount();
	 * if (account.getAccountType() == EnumAccountType::AccountTypeSIP) {
	 *     SipAccount * sipAccount = static_cast<SipAccount *>(account.getPrivateAccount());
	 * }
	 * </pre>
	 */
	IAccount * getPrivateAccount() const;

protected:

	void copy(const Account & account);

	virtual void setAccountType(EnumAccountType::AccountType accountType);

	IAccount * _privateAccount;
};

#endif	//OWACCOUNT_H
