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

#include <imwrapper/IMAccountParameters.h>

#include <util/Logger.h>

using namespace std;

const string IMAccountParameters::MAIL_NOTIFICATION_KEY = "common.mail_notification";
const string IMAccountParameters::REMEMBER_PASSWORD_KEY = "common.remember_password";
const string IMAccountParameters::USE_HTTP_PROXY_KEY = "common.use_http_proxy";
const string IMAccountParameters::HTTP_PROXY_SERVER_KEY = "common.http_proxy_server";
const string IMAccountParameters::HTTP_PROXY_PORT_KEY = "common.http_proxy_port";
const string IMAccountParameters::HTTP_PROXY_LOGIN_KEY = "common.http_proxy_login";
const string IMAccountParameters::HTTP_PROXY_PASSWORD_KEY = "common.http_proxy_password";
const string IMAccountParameters::YAHOO_IS_JAPAN_KEY = "yahoo.yahoojp";
const string IMAccountParameters::YAHOO_SERVER_KEY = "yahoo.server";
const string IMAccountParameters::YAHOO_JAPAN_SERVER_KEY = "yahoo.serverjp";
const string IMAccountParameters::YAHOO_PORT_KEY = "yahoo.port";
const string IMAccountParameters::YAHOO_XFER_HOST_KEY = "yahoo.xfer_host";
const string IMAccountParameters::YAHOO_JAPAN_XFER_HOST_KEY = "yahoo.xferjp_host";
const string IMAccountParameters::YAHOO_XFER_PORT_KEY = "yahoo.xfer_port";
const string IMAccountParameters::YAHOO_ROOM_LIST_LOCALE_KEY = "yahoo.room_list_locale";
const string IMAccountParameters::MSN_SERVER_KEY = "msn.server";
const string IMAccountParameters::MSN_PORT_KEY = "msn.port";
const string IMAccountParameters::MSN_USE_HTTP_KEY = "msn.http_method";
const string IMAccountParameters::OSCAR_SERVER_KEY = "oscar.server";
const string IMAccountParameters::OSCAR_PORT_KEY = "oscar.port";
const string IMAccountParameters::OSCAR_ENCODING_KEY = "oscar.encoding";
const string IMAccountParameters::JABBER_SERVER_KEY = "jabber.server";
const string IMAccountParameters::JABBER_PORT_KEY = "jabber.port";
const string IMAccountParameters::JABBER_RESOURCE_KEY = "jabber.resource";
const string IMAccountParameters::JABBER_USE_TLS_KEY = "jabber.use_tls";
const string IMAccountParameters::JABBER_REQUIRE_TLS_KEY = "jabber.require_tls";
const string IMAccountParameters::JABBER_USE_OLD_SSL_KEY = "jabber.old_ssl";
const string IMAccountParameters::JABBER_AUTH_PLAIN_IN_CLEAR_KEY = "jabber.auth_plain_in_clear";
const string IMAccountParameters::JABBER_CONNECTION_SERVER_KEY = "jabber.connect_server";

IMAccountParameters::IMAccountParameters() : AutomaticSettings() {
	_keyDefaultValueMap[MAIL_NOTIFICATION_KEY] = false;
	_keyDefaultValueMap[REMEMBER_PASSWORD_KEY] = true;
	_keyDefaultValueMap[USE_HTTP_PROXY_KEY] = false;
	_keyDefaultValueMap[HTTP_PROXY_SERVER_KEY] = string("");
	_keyDefaultValueMap[HTTP_PROXY_PORT_KEY] = 0;
	_keyDefaultValueMap[HTTP_PROXY_LOGIN_KEY] = string("");
	_keyDefaultValueMap[HTTP_PROXY_PASSWORD_KEY] = string("");
	_keyDefaultValueMap[YAHOO_IS_JAPAN_KEY] = false;
	_keyDefaultValueMap[YAHOO_SERVER_KEY] = string("scs.msg.yahoo.com");
	_keyDefaultValueMap[YAHOO_JAPAN_SERVER_KEY] = string("cs.yahoo.co.jp");
	_keyDefaultValueMap[YAHOO_PORT_KEY] = 5050;
	_keyDefaultValueMap[YAHOO_XFER_HOST_KEY] = string("filetransfer.msg.yahoo.com");
	_keyDefaultValueMap[YAHOO_JAPAN_XFER_HOST_KEY] = string("filetransfer.msg.yahoo.co.jp");
	_keyDefaultValueMap[YAHOO_XFER_PORT_KEY] = 80;
	_keyDefaultValueMap[YAHOO_ROOM_LIST_LOCALE_KEY] = string("us");
	_keyDefaultValueMap[MSN_SERVER_KEY] = string("messenger.hotmail.com");
	_keyDefaultValueMap[MSN_PORT_KEY] = 1863;
	_keyDefaultValueMap[MSN_USE_HTTP_KEY] = false;
	_keyDefaultValueMap[OSCAR_SERVER_KEY] = string("login.oscar.aol.com");
	_keyDefaultValueMap[OSCAR_PORT_KEY] = 5190;
	_keyDefaultValueMap[OSCAR_ENCODING_KEY] = string("ISO-8859-1");
	_keyDefaultValueMap[JABBER_SERVER_KEY] = string("");
	_keyDefaultValueMap[JABBER_PORT_KEY] = 5222;
	_keyDefaultValueMap[JABBER_RESOURCE_KEY] = string("WengoPhone");
	_keyDefaultValueMap[JABBER_USE_TLS_KEY] = true; /** Mandatory for GTalk */
	_keyDefaultValueMap[JABBER_REQUIRE_TLS_KEY] = true;
	_keyDefaultValueMap[JABBER_USE_OLD_SSL_KEY] = false;
	_keyDefaultValueMap[JABBER_AUTH_PLAIN_IN_CLEAR_KEY] = false;
	_keyDefaultValueMap[JABBER_CONNECTION_SERVER_KEY] = string(""); /** talk.google.com for GTalk */
}

IMAccountParameters::IMAccountParameters(const IMAccountParameters & imAccountParameters) {
	copy(imAccountParameters);
}

IMAccountParameters & IMAccountParameters::operator = (const IMAccountParameters & imAccountParameters) {
	copy(imAccountParameters);
	return *this;
}

void IMAccountParameters::copy(const IMAccountParameters & imAccountParameters) {
	Settings::copy(imAccountParameters);
	_keyDefaultValueMap = imAccountParameters._keyDefaultValueMap;
}

IMAccountParameters::~IMAccountParameters() {
}

bool IMAccountParameters::isMailNotified() const {
	return getBooleanKeyValue(MAIL_NOTIFICATION_KEY);
}

bool IMAccountParameters::isPasswordRemembered() const {
	return getBooleanKeyValue(REMEMBER_PASSWORD_KEY);
}

bool IMAccountParameters::isHttpProxyUsed() const {
	return getBooleanKeyValue(USE_HTTP_PROXY_KEY);
}

string IMAccountParameters::getHttpProxyServer() const {
	return getStringKeyValue(HTTP_PROXY_SERVER_KEY);
}

int IMAccountParameters::getHttpProxyPort() const {
	return getIntegerKeyValue(HTTP_PROXY_PORT_KEY);
}

string IMAccountParameters::getHttpProxyLogin() const {
	return getStringKeyValue(HTTP_PROXY_LOGIN_KEY);
}

string IMAccountParameters::getHttpProxyPassword() const {
	return getStringKeyValue(HTTP_PROXY_PASSWORD_KEY);
}

bool IMAccountParameters::isYahooJapan() const {
	return getBooleanKeyValue(YAHOO_IS_JAPAN_KEY);
}

string IMAccountParameters::getYahooServer() const {
	return getStringKeyValue(YAHOO_SERVER_KEY);
}

string IMAccountParameters::getYahooJapanServer() const {
	return getStringKeyValue(YAHOO_JAPAN_SERVER_KEY);
}

int IMAccountParameters::getYahooServerPort() const {
	return getIntegerKeyValue(YAHOO_PORT_KEY);
}

string IMAccountParameters::getYahooXferHost() const {
	return getStringKeyValue(YAHOO_XFER_HOST_KEY);
}

string IMAccountParameters::getYahooJapanXferHost() const {
	return getStringKeyValue(YAHOO_JAPAN_XFER_HOST_KEY);
}

int IMAccountParameters::getYahooXferPort() const {
	return getIntegerKeyValue(YAHOO_XFER_PORT_KEY);
}

string IMAccountParameters::getYahooRoomListLocale() const {
	return getStringKeyValue(YAHOO_ROOM_LIST_LOCALE_KEY);
}

string IMAccountParameters::getMSNServer() const {
	return getStringKeyValue(MSN_SERVER_KEY);
}

int IMAccountParameters::getMSNServerPort() const {
	return getIntegerKeyValue(MSN_PORT_KEY);
}

bool IMAccountParameters::isMSNHttpUsed() const {
	return getBooleanKeyValue(MSN_USE_HTTP_KEY);
}

string IMAccountParameters::getOscarServer() const {
	return getStringKeyValue(OSCAR_SERVER_KEY);
}

int IMAccountParameters::getOscarServerPort() const {
	return getIntegerKeyValue(OSCAR_PORT_KEY);
}

string IMAccountParameters::getOscarEncoding() const {
	return getStringKeyValue(OSCAR_ENCODING_KEY);
}

string IMAccountParameters::getJabberServer() const {
	return getStringKeyValue(JABBER_SERVER_KEY);
}

int IMAccountParameters::getJabberServerPort() const {
	return getIntegerKeyValue(JABBER_PORT_KEY);
}

string IMAccountParameters::getJabberResource() const {
	return getStringKeyValue(JABBER_RESOURCE_KEY);
}

bool IMAccountParameters::isJabberTLSUsed() const {
	return getBooleanKeyValue(JABBER_USE_TLS_KEY);
}

bool IMAccountParameters::isJabberTLSRequired() const {
	return getBooleanKeyValue(JABBER_REQUIRE_TLS_KEY);
}

bool IMAccountParameters::isJabberOldSSLUsed() const {
	return getBooleanKeyValue(JABBER_USE_OLD_SSL_KEY);
}

bool IMAccountParameters::isJabberAuthPlainInClearUsed() const {
	return getBooleanKeyValue(JABBER_AUTH_PLAIN_IN_CLEAR_KEY);
}

string IMAccountParameters::getJabberConnectionServer() const {
	return getStringKeyValue(JABBER_CONNECTION_SERVER_KEY);
}
