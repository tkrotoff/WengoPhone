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

#ifndef AUTHENTICATION_H
#define AUTHENTICATION_H

#include <observer/Subject.h>
#include <singleton/Singleton.h>

#include <qstring.h>

/**
 * Singleton class used to record all fields need for authentification.
 *
 * @author Philippe Kajmar
 */
class Authentication : public Subject, public Singleton<Authentication> {
	friend class Singleton<Authentication>;
public:

	/// authentification tag
	static const QString AUTH_TAG;

	/// number of tags
	static const int NB_TAGS;

	/// auto register tag
	static const QString AUTO_REGISTER_TAG;

	/// force register tag
	static const QString FORCE_REGISTER_TAG;

	/// username tag
	static const QString USERNAME_TAG;

	/// user id tag
	static const QString USERID_TAG;

	/// password tag
	static const QString PASSWORD_TAG;

	/// realm tag
	static const QString REALM_TAG;

	/// server tag
	static const QString SERVER_TAG;

	/// server host tag
	static const QString SERVER_HOST_TAG;

	/// server port tag
	static const QString SERVER_PORT_TAG;
	
	/// proxy tag
	static const QString PROXY_TAG;
	
	/// proxy host tag
	static const QString PROXY_HOST_TAG;
	
	/// port tag
	static const QString PORT_TAG;

	/// HTTP tunnel tag
	static const QString HTTP_TUNNEL_TAG;

	/// set displayname
	void setDisplayName(const QString & displayName);

	/// get displayname
	QString getDisplayName() const;

	/// set username
	void setUsername(QString username);

	/// get username
	QString getUsername() const;

	/// set user id
	void setUserId(QString userId);

	/// get user id
	QString getUserId() const;

	/// set password
	void setPassword(QString password);

	/// get password
	QString getPassword() const;

	/// set realm
	void setRealm(QString realm);

	/// get realm
	QString getRealm() const;

	/// set server
	void setServer(QString server);

	/// get server
	QString getServer() const;

	/// set server port
	void setServerPort(QString port);

	/// get server port
	QString getServerPort() const;

	void setProxy(QString proxy);

	QString getProxy() const;

	void setPort(QString port);

	void setStatusCode(int statusCode, const QString & statusMessage);
	int getStatusCode() const;
	QString getStatusMessage() const;

	QString getPort() const;

	/**
	 * @param autoRegister
	 * @brief set autoRegister
	 */
	void setAutoRegister(bool autoRegister);

	/**
	 * @return if the user will authentifiate when the session starts
	 */
	bool isAutoRegister() const;

	/**
	 * @param forceRegister
	 * @brief set ForceRegister
	 */
	void setForceRegister(bool forceRegister);

	/**
	 * @return if registration will be done by the proxy
	 */
	bool isForceRegister() const;

	void setHttpTunnelHost(const QString & host) {
		_httpTunnelHost = host;
	}

	QString getHttpTunnelHost() const {
		return _httpTunnelHost;
	}

	/**
	 * Changes all attributes.
	 *
	 * @param username
	 * @param userId
	 * @param password
	 * @param realm
	 * @param server
	 * @param proxy
	 * @param port
	 * @param autoRegister
	 */
	void changeSettings(QString username,
			QString userId,
			QString password,
			QString realm,
			QString server,
			QString proxy,
			QString port,
			bool forceRegister,
			bool autoRegister);

	void saveAsXml();

private:

	/// build an empty object
	Authentication();

	Authentication(const Authentication &);
	Authentication & operator=(const Authentication &);

	/// displayname
	QString _displayName;

	/// username
	QString _username;

	/// user id
	QString _userId;

	/// password
	QString _password;

	/// realm
	QString _realm;

	/// server
	QString _server;

	/// server port
	QString _serverPort;

	/// proxy
	QString _proxy;

	/// port of proxy
	QString _port;

	/// http tunnel host
	QString _httpTunnelHost;

	/// if an authentification was asked
	bool _autoRegister;

	/// if registration is forced towards proxy
	bool _forceRegister;

	int _statusCode;

	QString _statusMessage;
};

#endif	//AUTHENTICATION_H 
