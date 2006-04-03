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

#ifndef HTTPREQUEST_H
#define HTTPREQUEST_H

#include <util/NonCopyable.h>
#include <util/Event.h>

#include <thread/Thread.h>

#include <http/HttpRequestFactory.h>

#include <string>

/**
 * Sends a HTTP request on a HTTP server then gets and reads the answer from the server.
 *
 * A proxy can be set and an error code is returned.
 *
 * Example:
 * <pre>
 * class MyHttpRequest : public HttpRequest {
 * public:
 *    //Callback called when the server answer to the HTTP request.
 *    virtual void answerReceived(const std::string & answer, Error error) {
 *        if (error == NoError && !answer.empty()) {
 *            std::cout << "HTTP server anwser: " << answer << std::endl;
 *        }
 *    }
 * };
 *
 * HttpRequest::setFactory(new CurlHttpRequestFactory());
 * HttpRequest::setProxy("proxy.wengo.fr", 8080, "myProxyUsername", "myProxyPassword");
 * MyHttpRequest * http = new MyHttpRequest();
 * http->sendRequest("https://wengo.fr:8080/login.php", "login=mylogin&password=mypassword");
 * </pre>
 *
 * @author Tanguy Krotoff
 */
class IHttpRequest : public Thread {
public:

	enum Error {
		/** No error. */
		NoError,

		/** The hostname lookup failed. */
		HostNotFound,

		/** The server refused the connection. */
		ConnectionRefused,

		/** The server closed the connection unexpectedly. */
		UnexpectedClose,

		/** The server sent an invalid response header. */
		InvalidResponseHeader,

		/**
		 * The client could not read the content correctly because
		 * an error with respect to the content length occurred.
		 */
		WrongContentLength,

		/** The request was aborted. */
		Aborted,

		/** The proxy authentication failed. */
		ProxyAuthenticationError,

		/** The connection with the proxy failed. */
		ProxyConnectionError,

		/** operation timeouted. */
		TimeOut,

		/** Unknow error. */
		UnknownError
	};

	/**
	 * The HTTP answer to the request has been received.
	 *
	 * @param requestId HTTP request ID
	 * @param answer HTTP answer (std::string is used as a byte array)
	 * @param error Error code
	 */
	Event<void (int requestId, const std::string & answer, Error error)> answerReceivedEvent;

	/**
	 * Indicates the current progress of the download.
	 *
	 * @see QHttp::dataReadProgress()
	 * @param requestId HTTP request ID
	 * @param bytesDone specifies how many bytes have been transfered
	 * @param bytesTotal total size of the HTTP transfer, if 0 then the total number of bytes is not known
	 */
	Event<void (int requestId, double bytesDone, double bytesTotal)> dataReadProgressEvent;

	/**
	 * Indicates the current progress of the upload.
	 *
	 * @see QHttp::dataSendProgress()
	 * @param requestId HTTP request ID
	 * @param bytesDone specifies how many bytes have been transfered
	 * @param bytesTotal total size of the HTTP transfer, if 0 then the total number of bytes is not known
	 */
	Event<void (int requestId, double bytesDone, double bytesTotal)> dataSendProgressEvent;

	virtual ~IHttpRequest() { }

	/**
	 * Sends a HTTP request given a HTTP URL.
	 *
	 * A complete HTTP URL is something like:
	 * https://wengo.fr:8080/login.php?login=mylogin&password=mypassword
	 *
	 * @param sslProtocol true if HTTPS protocol (https://), false if HTTP protocol (http://)
	 * @param hostname HTTP server hostname (e.g wengo.fr, yahoo.fr)
	 * @param hostPort HTTP server port number (e.g 80, 8080)
	 * @param path path on the server (e.g login.php)
	 * @param data HTTP data (e.g login=mylogin&password=mypassword)
	 * @param postMethod HTTP POST method if true, HTTP GET method if false
	 * @return HTTP request ID
	 */
	virtual int sendRequest(bool sslProtocol,
			const std::string & hostname,
			unsigned int hostPort,
			const std::string & path,
			const std::string & data,
			bool postMethod = false) = 0;

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
	 * @see sendRequest(bool, const std::string &, unsigned int, const std::string &, const std::string &, bool)
	 * @return HTTP request ID
	 */
	virtual int sendRequest(const std::string & url, const std::string & data, bool postMethod = false) = 0;

	virtual void run() = 0;
};

/**
 * HttpRequest implementation.
 *
 * @author Tanguy Krotoff
 */
class HttpRequest : public IHttpRequest {
public:

	/** HTTP port separator tag (e.g : in wengo.fr:8080). */
	static const std::string HTTP_PORT_SEPARATOR;

	/** HTTP path separator tag (e.g / in wengo.fr/home/index.html). */
	static const std::string HTTP_PATH_SEPARATOR;

	/** HTTP default port number (e.g 80). */
	static const unsigned int HTTP_DEFAULT_PORT;

	/** HTTPS protocol tag (e.g https://). */
	static const std::string HTTPS_PROTOCOL;

	/** HTTP protocol tag (e.g http://). */
	static const std::string HTTP_PROTOCOL;

	/** HTTP GET method separator tag (e.g ? in wengo.fr/login.php?login=mylogin&password=mypassword). */
	static const std::string HTTP_GET_SEPARATOR;

	static void setFactory(HttpRequestFactory * factory);

	HttpRequest();

	virtual ~HttpRequest();

	virtual int sendRequest(bool sslProtocol,
			const std::string & hostname,
			unsigned int hostPort,
			const std::string & path,
			const std::string & data,
			bool postMethod = false);

	virtual int sendRequest(const std::string & url, const std::string & data, bool postMethod = false);

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
		unsigned int port,
		const std::string & username,
		const std::string & password) {

		_proxyHost = host;
		_proxyPort = port;
		_proxyUsername = username;
		_proxyPassword = password;
	}

	/**
	 * Gets the HTTP proxy hostname.
	 *
	 * @return HTTP proxy hostname
	 */
	static const std::string & getProxyHost() {
		return _proxyHost;
	}

	/**
	 * Gets the HTTP proxy port number.
	 *
	 * @return HTTP proxy port number
	 */
	static unsigned int getProxyPort() {
		return _proxyPort;
	}

	/**
	 * Gets the HTTP proxy username/login.
	 *
	 * @return HTTP proxy username
	 */
	static const std::string & getProxyUsername() {
		return _proxyUsername;
	}

	/**
	 * Gets the HTTP proxy password.
	 *
	 * @return HTTP proxy password
	 */
	static const std::string & getProxyPassword() {
		return _proxyPassword;
	}

	virtual void run();

private:

	static HttpRequestFactory * _factory;

	/** System-dependant implementation. */
	IHttpRequest * _httpRequestPrivate;

	/** HTTP proxy hostname. */
	static std::string _proxyHost;

	/** HTTP proxy port number. */
	static unsigned int _proxyPort;

	/** HTTP proxy username. */
	static std::string _proxyUsername;

	/** HTTP proxy password. */
	static std::string _proxyPassword;
};

#endif	//HTTPREQUEST_H
