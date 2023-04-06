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

#ifndef OWOSCARACCOUNT_H
#define OWOSCARACCOUNT_H

#include <coipmanager_base/account/IAccount.h>

/**
 * Oscar account.
 *
 * This class gather common attributes between AIM and ICQ Accounts. It cannot
 * be used directly (this is why there is no entry in EnumAccountType for Oscar.
 *
 * @author Philippe Bernery
 */
class COIPMANAGER_BASE_API OscarAccount : public IAccount {
public:

	/**
	 * @name defaults AIM/ICQ parameters
	 * @{
	 */
	static const std::string DEFAULT_OSCAR_SERVER;

	static const int DEFAULT_OSCAR_PORT;

	static const std::string DEFAULT_ENCODING;
	/** @} */

	OscarAccount();

	OscarAccount(const std::string & login, const std::string & password);

	OscarAccount(const OscarAccount & account);

	explicit OscarAccount(const IAccount * iAccount);

	virtual ~OscarAccount();

	virtual OscarAccount * clone() const;

	void setServer(const std::string & oscarServer);
	std::string getServer() const;

	void setServerPort(int oscarServerPort);
	int getServerPort() const;

	void setEncoding(const std::string & encoding);
	std::string getEncoding() const;

protected:

	void copy(const OscarAccount & account);

	/** Connection server. */
	std::string _oscarServer;

	/** Connection server port. */
	int _oscarServerPort;

	/** Encoding */
	std::string _oscarEncoding;
};

#endif	//OWOSCARACCOUNT_H
