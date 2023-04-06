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

#ifndef OWMSNACCOUNT_H
#define OWMSNACCOUNT_H

#include <coipmanager_base/account/IAccount.h>

/**
 * MSN account.
 *
 * @author Philippe Bernery
 */
class COIPMANAGER_BASE_API MSNAccount : public IAccount {
public:

	/**
	 * @name defaults MSN parameters
	 * @{
	 */
	static const std::string DEFAULT_MSN_SERVER;

	static const int DEFAULT_MSN_PORT;
	/** @} */

	MSNAccount();

	MSNAccount(const std::string & login, const std::string & password);

	MSNAccount(const MSNAccount & account);

	explicit MSNAccount(const IAccount * iAccount);

	virtual ~MSNAccount();

	virtual MSNAccount * clone() const;

	void setMailNotification(bool mailNotification);
	bool isMailNotified() const;

	void setServer(const std::string & msnServer);
	std::string getServer() const;

	void setServerPort(int msnServerPort);
	int getServerPort() const;

	void setHttpConnection(bool useHttp);
	bool isHttpUsed() const;

protected:

	void copy(const MSNAccount & account);

	/** Connection server. */
	std::string _msnServer;

	/** Connection server port. */
	int _msnServerPort;

	/** Connection method. */
	bool _useHttp;

	/** Is mail notified? */
	bool _mailNotified;
};

#endif	//OWMSNACCOUNT_H
