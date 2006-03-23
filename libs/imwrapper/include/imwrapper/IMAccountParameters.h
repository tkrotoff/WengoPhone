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

#ifndef IMACCOUNTPARAMETER_H
#define IMACCOUNTPARAMETER_H

#include <settings/Settings.h>

#include <util/String.h>

/**
 * Contains specific parameters needed by some protocols for connection.
 *
 * @author Philippe Bernery
 */
class IMAccountParameters : public Settings {
public:
	IMAccountParameters();

	IMAccountParameters(const IMAccountParameters & imAccountParameters);

	IMAccountParameters & operator=(const IMAccountParameters & imAccountParameters);

	virtual ~IMAccountParameters();

	/**
	 * @name Common parameters
	 * @{
	 */
	static const std::string MAIL_NOTIFICATION_KEY;
	bool isMailNotified() const;

	static const std::string REMEMBER_PASSWORD_KEY;
	bool isPasswordRemembered() const;

	/** @} */

	/**
	 * @name Yahoo specific parameters
	 * @{
	 */
	static const std::string YAHOO_IS_JAPAN_KEY;
	bool isYahooJapan() const;

	static const std::string YAHOO_SERVER_KEY;
	std::string getYahooServer() const;

	static const std::string YAHOO_JAPAN_SERVER_KEY;
	std::string getYahooJapanServer() const;

	static const std::string YAHOO_PORT_KEY;
	int getYahooServerPort() const;

	static const std::string YAHOO_XFER_HOST_KEY;
	std::string getYahooXferHost() const;

	static const std::string YAHOO_JAPAN_XFER_HOST_KEY;
	std::string getYahooJapanXferHost() const;

	static const std::string YAHOO_XFER_PORT_KEY;
	std::string getYahooXferPort() const;

	static const std::string YAHOO_ROOM_LIST_LOCALE_KEY;
	std::string getYahooRoomListLocale() const;
	/** @} */

	/**
	 * @name MSN specific parameters
	 * @{
	 */
	static const std::string MSN_SERVER_KEY;
	std::string getMSNServer() const;

	static const std::string MSN_PORT_KEY;
	int getMSNServerPort() const;

	static const std::string MSN_USE_HTTP_KEY;
	bool isMSNHttpUsed() const;
	/** @} */

	/**
	 * @name Oscar (AIM/ICQ) specific parameters
	 * @{
	 */
	static const std::string OSCAR_SERVER_KEY;
	std::string getOscarServer() const;

	static const std::string OSCAR_PORT_KEY;
	int getOscarServerPort() const;

	static const std::string OSCAR_ENCODING_KEY;
	std::string getOscarEncoding() const;
	/** @} */

	/**
	 * @name Jabber specific parameters
	 * @{
	 */
	static const std::string JABBER_SERVER_KEY;
	std::string getJabberServer() const;

	static const std::string JABBER_PORT_KEY;
	int getJabberServerPort() const;

	static const std::string JABBER_RESOURCE_KEY;
	std::string getJabberResource() const;

	static const std::string JABBER_USE_TLS_KEY;
	bool isJabberTLSUsed() const;

	static const std::string JABBER_REQUIRE_TLS_KEY;
	bool isJabberTLSRequired() const;

	static const std::string JABBER_USE_OLD_SSL_KEY;
	bool isJabberOldSSLUsed() const;

	static const std::string JABBER_AUTH_PLAIN_IN_CLEAR_KEY;
	bool isJabberAuthPlainInClearUsed() const;

	static const std::string JABBER_CONNECTION_SERVER_KEY;
	std::string getJabberConnectionServer() const;
	/** @} */

private:

	virtual void copy(const IMAccountParameters & imAccountParameters);

	/**
	 * Makes it impossible to use get() directly.
	 *
	 * @see Settings::get()
	 */
	std::string get(const std::string &, const std::string &) const { return String::null; }

	/**
	 * @see get()
	 */
	StringList get(const std::string &, const StringList &) const {
		static const StringList empty;
		return empty;
	}

	/**
	 * @see get()
	 */
	bool get(const std::string &, bool) const { return false; }

	/**
	 * @see get()
	 */
	int get(const std::string &, int) const { return 0; }

	/**
	 * @see get()
	 */
	boost::any getAny(const std::string &, const boost::any &) const {
		static const boost::any empty;
		return empty;
	}

	/*
	 * @see Settings::getAny()
	 */
	boost::any getAny(const std::string & key) const;

	/**
	 * @see get()
	 */
	bool getBooleanKeyValue(const std::string & key) const;

	/**
	 * @see get()
	 */
	int getIntegerKeyValue(const std::string & key) const;

	/**
	 * @see get()
	 */
	std::string getStringKeyValue(const std::string & key) const;

	/** Associates a key to a default value. */
	Keys _keyDefaultValueMap;

};

#endif //IMACCOUNTPARAMETER_H
