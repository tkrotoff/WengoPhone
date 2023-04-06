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

#ifndef OWHTTPREQUEST_H
#define OWHTTPREQUEST_H

#include <http/IHttpRequest.h>

/**
 * HttpRequest implementation.
 *
 * @author Tanguy Krotoff
 */
class OWHTTP_API HttpRequest : public IHttpRequest {
	Q_OBJECT
public:

	/** HTTP port separator tag (e.g : in wengo.fr:8080). */
	static const std::string HTTP_PORT_SEPARATOR;

	/** HTTP path separator tag (e.g / in wengo.fr/home/index.html). */
	static const std::string HTTP_PATH_SEPARATOR;

	/** HTTP default port number (e.g 80). */
	static const unsigned HTTP_DEFAULT_PORT;

	/** HTTPS protocol tag (e.g https://). */
	static const std::string HTTPS_PROTOCOL;

	/** HTTP protocol tag (e.g http://). */
	static const std::string HTTP_PROTOCOL;

	/** HTTP GET method separator tag (e.g ? in wengo.fr/login.php?login=mylogin&password=mypassword). */
	static const std::string HTTP_GET_SEPARATOR;

	HttpRequest();

	virtual ~HttpRequest();

	int sendRequest(bool sslProtocol,
			const std::string & hostname,
			unsigned hostPort,
			const std::string & path,
			const std::string & data,
			bool postMethod = false);

	/**
	 * Sends a HTTP request given a HTTP URL.
	 *
	 * A complete HTTP URL is something like:
	 * https://wengo.fr:8080/login.php?login=mylogin&password=mypassword
	 * This is an overloaded member function, provided for convenience.
	 * It behaves essentially like the above function.
	 *
	 * @param url HTTP URL (e.g https://wengo.fr:8080/login.php)
	 * @param data HTTP data (e.g login=mylogin&password=mypassword)
	 * @param postMethod HTTP POST method if true, HTTP GET method if false
	 * @return HTTP request Id
	 * @see sendRequest(bool, const std::string &, unsigned, const std::string &, const std::string &, bool)
	 */
	int sendRequest(const std::string & url, const std::string & data, bool postMethod = false);

	/**
	 * Sets the local proxy settings.
	 * The proxy can be a SOCKS5 proxy,
	 *
	 * Static method so that it keeps the proxy informations once it is set.
	 *
	 * @param host HTTP proxy hostname
	 * @param port HTTP proxy port number
	 * @param username HTTP proxy username
	 * @param password HTTP proxy password
	 */
	static void setProxy(const std::string & host,
		unsigned port,
		const std::string & username,
		const std::string & password);

	/**
	 * Gets the HTTP proxy hostname.
	 *
	 * @return HTTP proxy hostname
	 */
	static const std::string & getProxyHost();

	/**
	 * Gets the HTTP proxy port number.
	 *
	 * @return HTTP proxy port number
	 */
	static unsigned getProxyPort();

	/**
	 * Gets the HTTP proxy username/login.
	 *
	 * @return HTTP proxy username
	 */
	static const std::string & getProxyUsername();

	/**
	 * Gets the HTTP proxy password.
	 *
	 * @return HTTP proxy password
	 */
	static const std::string & getProxyPassword();

	/**
	 * Sets the User Agent.
	 *
	 * @param userAgent the user agent
	 */
	static void setUserAgent(const std::string & userAgent);

	/**
	 * Sets the User Agent.
	 *
	 * @param userAgent the user agent
	 */
	static const std::string & getUserAgent();

	void abort();

private Q_SLOTS:

	void answerReceivedSlot(int requestId, std::string answer, IHttpRequest::Error error);

private:

	/** System-dependant implementation. */
	IHttpRequest * _httpRequestPrivate;

	/** HTTP proxy hostname. */
	static std::string _proxyHost;

	/** HTTP proxy port number. */
	static unsigned _proxyPort;

	/** HTTP proxy username. */
	static std::string _proxyUsername;

	/** HTTP proxy password. */
	static std::string _proxyPassword;

	/** HTTP user agent. */
	static std::string _userAgent;
};

#endif	//OWHTTPREQUEST_H
