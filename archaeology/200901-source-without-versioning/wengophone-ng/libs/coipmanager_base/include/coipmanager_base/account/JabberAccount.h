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

#ifndef OWJABBERACCOUNT_H
#define OWJABBERACCOUNT_H

#include <coipmanager_base/account/IAccount.h>

/**
 * Jabber account.
 *
 * @author Philippe Bernery
 */
class COIPMANAGER_BASE_API JabberAccount : public IAccount {
public:

	/**
	 * @name defaults Jabber parameters
	 * @{
	 */
	static const std::string DEFAULT_JABBER_SERVER;

	static const int DEFAULT_JABBER_PORT;

	static const std::string DEFAULT_JABBER_RESOURCE;

	static const bool DEFAULT_JABBER_USE_TLS;

	static const bool DEFAULT_JABBER_REQUIRE_TLS;

	static const bool DEFAULT_JABBER_USE_OLD_SSL;

	static const bool DEFAULT_JABBER_AUTH_PLAIN_IN_CLEAR;

	static const std::string DEFAULT_JABBER_CONNECTION_SERVER;
	/** @} */

	JabberAccount();

	JabberAccount(const std::string & login, const std::string & password);

	JabberAccount(const JabberAccount & account);

	explicit JabberAccount(const IAccount * iAccount);

	virtual ~JabberAccount();

	virtual JabberAccount * clone() const;

	void setServer(const std::string & jabberServer);
	std::string getServer() const;

	void setServerPort(int jabberServerPort);
	int getServerPort() const;

	void setResource(const std::string & resource);
	std::string getResource() const;

	void setTLS(bool useTLS);
	bool isTLSUsed() const;

	void setTLSRequired(bool requireTLS);
	bool isTLSRequired() const;

	void setOldSSLUsed(bool oldSSLUsed);
	bool isOldSSLUsed() const;

	void setAuthPlainInClearUsed(bool useAuthPlainInClear);
	bool isAuthPlainInClearUsed() const;

	void setConnectionServer(const std::string & connectionServer);
	std::string getConnectionServer() const;

protected:

	void copy(const JabberAccount & account);

	/** Jabber server. */
	std::string _jabberServer;

	/** Jabber server port. */
	int _jabberServerPort;

	/** Resource. */
	std::string _resource;

	/** Is TLS used? */
	bool _tlsUsed;

	/** Is TLS required? */
	bool _tlsRequired;

	/** Is old SSL used? */
	bool _oldSSLUsed;

	/** Is auth plain in clear used? */
	bool _authPlainInClearUsed;

	/** Connection server. */
	std::string _connectionServer;
};

#endif	//OWJABBERACCOUNT_H
