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

#ifndef CURLHTTPREQUEST_H
#define CURLHTTPREQUEST_H

#include <http/HttpRequest.h>

#include <curl/curl.h>

#include <string>
#include <list>

struct POST_DATA {
	const char * readptr;
	size_t sizeleft;
};

/**
 * HttpRequest implementation using libcurl.
 *
 * @see HttpRequest
 * @see http://curl.haxx.se/
 * @author Mathieu Stute
 * @author Tanguy Krotoff
 */
class CurlHttpRequest : public IHttpRequest {
public:

	/**
	 * Constructs a new CurlHttpRequest.
	 *
	 * @param httpRequest callback for answerReceived() and run() methods
	 */
	CurlHttpRequest(HttpRequest * httpRequest);

	int sendRequest(bool sslProtocol,
			const std::string & hostname,
			unsigned int hostPort,
			const std::string & path,
			const std::string & data,
			bool postMethod = false);

	int sendRequest(const std::string & url, const std::string & data, bool postMethod = false);

	void abort() {
		abortTransfer = true;
	}

	/** Libcurl component. */
	CURL * _curl;

	/** Current progress of the download. */
	double downloadDone;

	/** Total amount of data to download. */
	double downloadTotal;

	/** Entire response content. */
	std::string entireResponse;

	/** Abort or not the current HTTP transfer. */
	bool abortTransfer;

protected:

	void run();

private:

	struct Request {
		bool sslProtocol;
		const std::string hostname;
		unsigned int hostPort;
		const std::string path;
		const std::string data;
		bool postMethod;
	};

	/** Sets libcurl global parameters. */
	void setCurlParam();

	/**
	 * Sets libCurl URL.
	 *
	 */
	void setUrl(bool sslProtocol, const std::string & hostname, unsigned int hostPort,
		const std::string & path, const std::string & data, bool postMethod);

	/** Sets libCurl proxy parameters. */
	void setProxyParam();

	/** Sets libCurl user proxy parameters : user id and password. */
	void setProxyUserParam();

	/** Sets libCurl SSL parameters. */
	void setSSLParam();

	/**
	 * Sets curl post options and data.
	 *
	 * @param post data
	 */
	void setPostData(std::string data);

	/**
	 * LibCurl error code to HttpRequest error code
	 *
	 * @param CurlCode
	 * @return the equivalent HttpRequest error code
	 */
	HttpRequest::Error getReturnCode(int curlcode);

	/**
	 * Determines the proxy authentication method.
	 *
	 * @return a long representing one of following authentication method:
	 *         BASIC, DIGEST & NTLM
	 */
	long getProxyAuthenticationType();

	/**
	 * Sets Curl url.
	 *
	 */
	void setUrl(Request request);

	bool useProxy() const;

	bool useProxyAuthentication() const;

	/**
	 * Callback for answerReceived() and run() methods.
	 *
	 * CurlHttpRequest::run() calls _httpRequest->run().
	 * CurlHttpRequest::answerReceived() calls _httpRequest->run().
	 */
	HttpRequest * _httpRequest;

	/** Tells curl to be verbose + additionnal information. */
	static bool _verbose;

	/** Is proxy authentication determined. */
	static bool _proxyAuthenticationDetermine;

	/** Proxy authentication mask. */
	static long _proxyAuthentication;

	typedef std::list < Request > RequestList;

	/** Requests fifo. */
	RequestList _requestList;

	/** Post data structure. */
	struct POST_DATA pooh;

	int _lastRequestId;
};

#endif	//CURLHTTPREQUEST_H
