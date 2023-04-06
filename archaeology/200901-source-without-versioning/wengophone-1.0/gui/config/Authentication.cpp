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

#include "Authentication.h"

#include "database/Database.h"
using namespace database;
#include "database/FileTarget.h"
#include "Softphone.h"
#include "AudioCallManager.h"

#include <observer/Event.h>

#include <qmessagebox.h>
#include <qobject.h>
#include <qdom.h>
#include <qstring.h>

#include <iostream>
using namespace std;

#include <cassert>

const QString Authentication::AUTH_TAG = "sso";
const QString Authentication::AUTO_REGISTER_TAG = "autoregister";
const QString Authentication::FORCE_REGISTER_TAG = "outbound";
const QString Authentication::USERNAME_TAG = "user";
const QString Authentication::USERID_TAG = "userid";
const QString Authentication::PASSWORD_TAG = "password";
const QString Authentication::REALM_TAG = "realm";
const QString Authentication::SERVER_TAG = "server";
const QString Authentication::PROXY_TAG = "proxy";
const QString Authentication::PROXY_HOST_TAG = "host";
const QString Authentication::PORT_TAG = "port";
const QString Authentication::HTTP_TUNNEL_TAG = "httptunnel";
const QString Authentication::SERVER_HOST_TAG = "host";
const QString Authentication::SERVER_PORT_TAG = "port";

/**
 * @brief construct an empty object
 */
Authentication::Authentication() {
	_autoRegister = false;
	_forceRegister = false;
}


void Authentication::setDisplayName(const QString & displayName) {
	_displayName = displayName;
}

QString Authentication::getDisplayName() const {
	return _displayName;
}

/**
 * @param username
 * @brief set username
 */
void Authentication::setUsername(QString username) {
	_username = username;
	notify(Event());
}

/**
 * @return username
 */
QString Authentication::getUsername() const {
	return _username;
}

/**
 * @param userId
 * @brief set userId
 */
void Authentication::setUserId(QString userId) {
	_userId = userId;
	notify(Event());
}

/**
 * @return the user's id
 */
QString Authentication::getUserId() const {
	return _userId;
}

/**
 * @param password
 * @brief set password
 */
void Authentication::setPassword(QString password) {
	_password = password;
	notify(Event());
}

/**
 * @return password
 */
QString Authentication::getPassword() const {
	return _password;
}

/**
 * @param realm
 * @brief set realm
 */
void Authentication::setRealm(QString realm) {
	_realm = realm;
	notify(Event());
}

/**
 * @return realm
 */
QString Authentication::getRealm() const {
	return _realm;
}

/**
 * @param server
 * @brief set server
 */
void Authentication::setServer(QString server) {
	_server = server;
	notify(Event());          
}

/**
 * @return server
 */
QString Authentication::getServer() const {
	return _server;
}


/**
 * @param server port
 * @brief set server port
 */
void Authentication::setServerPort(QString port) {
	_serverPort = port;
	notify(Event());          
}

/**
 * @return server port
 */
QString Authentication::getServerPort() const {
	return _serverPort;
}



/**
 * @param proxy
 * @brief set proxy
 */
void Authentication::setProxy(QString proxy) {
	_proxy = proxy;
	notify(Event());
}

/**
 * @return proxy
 * @brief proxy accessor
 */
QString Authentication::getProxy() const {
#ifdef BETA_TEST_VIDEO_PROFILE
	return QString("213.91.9.210");
#endif
	return _proxy;
}

/**
 * @param port
 * @brief set port
 */
void Authentication::setPort(QString port) {
	_port = port;
	notify(Event());
}

/**
 * @return port
 * @brief port accessor
 */
QString Authentication::getPort() const {
	return _port;
}

void Authentication::setAutoRegister(bool autoRegister){
	_autoRegister = autoRegister;
	notify(Event());
}

bool Authentication::isAutoRegister() const {
	return _autoRegister;
}

void Authentication::setForceRegister(bool forceRegister) {
	_forceRegister = forceRegister;
	notify(Event());
}

bool Authentication::isForceRegister() const {
	return _forceRegister;
}

void Authentication::setStatusCode(int statusCode, const QString & statusMessage) {
	_statusCode = statusCode;
	_statusMessage = statusMessage;
}

int Authentication::getStatusCode() const {
	return _statusCode;
}

QString Authentication::getStatusMessage() const {
	return _statusMessage;
}

void Authentication::changeSettings(QString username,
				QString userId,
				QString password,
				QString realm,
				QString server,
				QString proxy,
				QString port,
				bool forceRegister,
				bool autoRegister) {

	_username = username;
	_userId = userId;
	_password = password;
	_realm = realm;
	_server = server;
	_proxy = proxy;
	_port = port;
	_forceRegister = forceRegister;
	_autoRegister = autoRegister;
	notify(Event());
}

void Authentication::saveAsXml() {
	using namespace database;

	//Does not save the configuration file auth.config anymore
	/*Database & database = Database::getInstance();
	QString xml = database.transformAuthToXml(*this);

	FileTarget * target = NULL;
	try {
		target = new FileTarget(Softphone::getInstance().getAuthFile());
	} catch (const FileNotFoundException & e) {
		cerr << e.what() << endl;
		return;
	}

	database.load(xml, *target);
	delete target;*/
}
