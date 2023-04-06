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

#ifndef OWIACCOUNT_H
#define OWIACCOUNT_H

#include <coipmanager_base/peer/Peer.h>

/**
 * Interface for CoIp account.
 *
 * @author Philippe Bernery
 */
class COIPMANAGER_BASE_API IAccount : public Peer {
	friend class Account;
	friend class AccountXMLSerializer;
	friend class IAccountXMLSerializer;
public:

	virtual IAccount * clone() const;

	virtual ~IAccount();

	virtual std::string getLogin() const;
	virtual void setLogin(const std::string & login);

	virtual std::string getPassword() const;
	virtual void setPassword(const std::string & password);

	virtual void setConnectionConfigurationSet(bool set);
	virtual bool isConnectionConfigurationSet() const;

	virtual void setSavePassword(bool savePassword);
	virtual bool isPasswordSaved() const;

protected:

	IAccount();

	IAccount(const std::string & login, const std::string & password,
		EnumAccountType::AccountType accountType);

	IAccount(const IAccount & iAccount);

	void copy(const IAccount & iAccount);

	virtual void setPeerId(const std::string & peerId);
	virtual std::string getPeerId() const;

	virtual void setAccountType(EnumAccountType::AccountType accountType);

	/** Password of this IAccount. */
	std::string _password;

	/** True if configuration for connection has been detected. */
	bool _connectionConfigurationSet;

	/** True if the password must be saved. */
	bool _savePassword;
};

#endif	//OWIACCOUNT_H
