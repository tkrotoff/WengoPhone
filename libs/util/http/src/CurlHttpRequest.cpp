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

#include <http/CurlHttpRequest.h>
#include <http/HttpRequest.h>

#include <util/Logger.h>
#include <util/IdGenerator.h>

#include <iostream>
using namespace std;

size_t curlHttpHeaderWrite(void * ptr, size_t size, size_t nmemb, void * curlHttpRequestInstance);
int curlHTTPProgress(void * curlHttpRequestInstance, double dltotal, double dlnow, double ultotal, double ulnow);
size_t curlHTTPWrite(void * ptr, size_t size, size_t nmemb, void * curlHttpRequestInstance);
size_t curlHTTPRead(void * ptr, size_t size, size_t nmemb, void * curlHttpRequestInstance);

char * getstr(std::string str) {
	return strdup(str.c_str());
}

bool CurlHttpRequest::_verbose;
bool CurlHttpRequest::_proxyAuthenticationDetermine;
long CurlHttpRequest::_proxyAuthentication;

CurlHttpRequest::CurlHttpRequest(HttpRequest * httpRequest) {
	_curl = NULL;
	_proxyAuthenticationDetermine = false;
	_verbose = true;
	_httpRequest = httpRequest;
	abortTransfer = false;
	downloadDone = 0;
	downloadTotal = 0;
}

int CurlHttpRequest::sendRequest(bool sslProtocol, const std::string & hostname, unsigned int hostPort,
	const std::string & path, const std::string & data, bool postMethod) {

	if (!_proxyAuthenticationDetermine) {
		_proxyAuthenticationDetermine = true;
		_proxyAuthentication = getProxyAuthenticationType();
	}

	Request r = {sslProtocol, hostname, hostPort, path, data, postMethod};
	_requestList.push_back(r);

	_lastRequestId = IdGenerator::generate();
	if (_requestList.size() <= 1) {
		start();
	}

	return _lastRequestId;
}

void CurlHttpRequest::run() {
	CURLcode res;
	long response;
	_curl = curl_easy_init();
	do {
		if (_curl) {
			setCurlParam();
			setProxyParam();
			setProxyUserParam();
			setSSLParam();
			setUrl(_requestList.front());
			curl_easy_setopt(_curl, CURLOPT_PRIVATE, _lastRequestId);

			res = curl_easy_perform(_curl);
			if (res != CURLE_OK) {
				cerr << curl_easy_strerror(res) << endl;
				answerReceivedEvent(NULL, _lastRequestId, String::null, getReturnCode(res));
			}

			curl_easy_getinfo(_curl, CURLINFO_RESPONSE_CODE, & response);
			if (!response) {
				LOG_DEBUG("no server response code has been received");
			}
			else {
				LOG_DEBUG("server response code=" + String::fromNumber(response));
			}
		}
		else {
			LOG_DEBUG("cURL initialization failed");
			return;
		}
		_requestList.pop_front();
	}
	while (!_requestList.empty());
	curl_easy_cleanup(_curl);
}

int CurlHttpRequest::sendRequest(const std::string & url, const std::string & data, bool postMethod) {
	return _httpRequest->sendRequest(url, data, postMethod);
}

void CurlHttpRequest::setUrl(Request request) {
	setUrl(request.sslProtocol, request.hostname,
		request.hostPort, request.path,
		request.data, request.postMethod);
}

void CurlHttpRequest::setUrl(bool sslProtocol, const std::string & hostname, unsigned int hostPort,
	const std::string & path, const std::string & data, bool postMethod) {

	string url;

	if (sslProtocol) {
		url = HttpRequest::HTTPS_PROTOCOL;
	} else {
		url = HttpRequest::HTTP_PROTOCOL;
	}

	if (!hostname.empty()) {
		url.append(hostname);
	}

	if (hostPort > 0) {
		url.append(HttpRequest::HTTP_PORT_SEPARATOR);
		url.append(String::fromNumber(hostPort));
	}

	if (!path.empty()) {
		url.append(path);
	}

	if (!data.empty()) {
		if (!postMethod) {
			curl_easy_setopt(_curl, CURLOPT_POST, 0);
			url.append("?");
			url.append(data);
		} else {
			curl_easy_setopt(_curl, CURLOPT_POST, 1);
			if (!data.empty()) {
				setPostData(data);
			}
		}
	}

	curl_easy_setopt(_curl, CURLOPT_URL, getstr(url));
}

void CurlHttpRequest::setCurlParam() {
	curl_easy_setopt(_curl, CURLOPT_VERBOSE, _verbose);
	curl_easy_setopt(_curl, CURLOPT_NOPROGRESS, false);
	curl_easy_setopt(_curl, CURLOPT_READFUNCTION, curlHTTPRead);
	curl_easy_setopt(_curl, CURLOPT_WRITEFUNCTION, curlHTTPWrite);
	curl_easy_setopt(_curl, CURLOPT_PROGRESSFUNCTION, curlHTTPProgress);
	//curl_easy_setopt(_curl, CURLOPT_HEADERFUNCTION, curlHttpHeaderWrite);
	//curl_easy_setopt(_curl, CURLOPT_WRITEHEADER, this);
	curl_easy_setopt(_curl, CURLOPT_WRITEDATA, this);
	curl_easy_setopt(_curl, CURLOPT_PROGRESSDATA, this);
	curl_easy_setopt(_curl, CURLOPT_NOSIGNAL, 0);
	curl_easy_setopt(_curl, CURLOPT_TIMEOUT, 15);
	curl_easy_setopt(_curl, CURLOPT_USERAGENT, HttpRequest::getUserAgent().c_str());
}

void CurlHttpRequest::setSSLParam() {
	curl_easy_setopt(_curl, CURLOPT_SSL_VERIFYPEER, 0);
	curl_easy_setopt(_curl, CURLOPT_SSL_VERIFYHOST, 0);
	curl_easy_setopt(_curl, CURLOPT_SSLVERSION, 3);
}

void CurlHttpRequest::setPostData(std::string _data) {
	char * data = getstr(_data);

	if (data) {
		//struct POST_DATA pooh;
		pooh.readptr = data;
		pooh.sizeleft = strlen(data);

		curl_easy_setopt(_curl, CURLOPT_POSTFIELDSIZE, (long)pooh.sizeleft);
		curl_easy_setopt(_curl, CURLOPT_INFILE, & pooh);
	}
}

void CurlHttpRequest::setProxyUserParam() {
	if (useProxyAuthentication() && _proxyAuthentication) {
		if ((_proxyAuthentication | CURLAUTH_BASIC) == CURLAUTH_BASIC) {
			LOG_DEBUG("set proxy authentication: BASIC");
			curl_easy_setopt(_curl, CURLOPT_PROXYAUTH, CURLAUTH_BASIC);
		}
		else if ((_proxyAuthentication | CURLAUTH_DIGEST) == CURLAUTH_DIGEST) {
			LOG_DEBUG("set proxy authentication: DIGEST");
			curl_easy_setopt(_curl, CURLOPT_PROXYAUTH, CURLAUTH_DIGEST);
		}
		else if ((_proxyAuthentication | CURLAUTH_NTLM) == CURLAUTH_NTLM) {
			LOG_DEBUG("set proxy authentication: NTLM");
			curl_easy_setopt(_curl, CURLOPT_PROXYAUTH, CURLAUTH_NTLM);
		}
		string proxyUserIDPassword = HttpRequest::getProxyUsername() + ":" + HttpRequest::getProxyPassword();
		curl_easy_setopt(_curl, CURLOPT_PROXYUSERPWD, getstr(proxyUserIDPassword));
	}
}

void CurlHttpRequest::setProxyParam() {
	if (useProxy() && _proxyAuthentication) {
		string proxy = HttpRequest::getProxyHost();
		proxy += ":" + String::fromNumber(HttpRequest::getProxyPort());
		curl_easy_setopt(_curl, CURLOPT_PROXY, getstr(proxy));
		curl_easy_setopt(_curl, CURLOPT_HTTPPROXYTUNNEL, 0);
	}
}

/**
 * Finds the proxy authentication method (BASIC, DIGEST, NTLM).
 *
 * Thanks to Julien Bossart
 */
long CurlHttpRequest::getProxyAuthenticationType() {
	CURL * curl_tmp;
	long AuthMask;
	string testUrl = "www.wengo.fr";

	if (useProxy()) {
		string proxy = HttpRequest::getProxyHost();
		proxy += ":" + String::fromNumber(HttpRequest::getProxyPort());

		curl_tmp = curl_easy_init();
		curl_easy_setopt(curl_tmp, CURLOPT_URL, getstr(testUrl));
		curl_easy_setopt(curl_tmp, CURLOPT_VERBOSE, _verbose);
		curl_easy_setopt(curl_tmp, CURLOPT_NOBODY, 1);
		curl_easy_setopt(curl_tmp, CURLOPT_PROXY, getstr(proxy));
		curl_easy_perform(curl_tmp);
		curl_easy_getinfo(curl_tmp, CURLINFO_PROXYAUTH_AVAIL, & AuthMask);
		curl_easy_cleanup(curl_tmp);

		if ((AuthMask | CURLAUTH_BASIC) == CURLAUTH_BASIC) {
			LOG_DEBUG("proxy authentication find: BASIC");
		}
		else if ((AuthMask | CURLAUTH_DIGEST) == CURLAUTH_DIGEST) {
			LOG_DEBUG("proxy authentication find: DIGEST");
		}
		else if ((AuthMask | CURLAUTH_NTLM) == CURLAUTH_NTLM) {
			LOG_DEBUG("proxy authentication find: NTLM");
		}
	}
	return AuthMask;
}

bool CurlHttpRequest::useProxy() const {
	if (!HttpRequest::getProxyHost().empty() && HttpRequest::getProxyPort() != 0) {
		return true;
	}
	return false;
}

bool CurlHttpRequest::useProxyAuthentication() const {
	if (!HttpRequest::getProxyUsername().empty() && !HttpRequest::getProxyPassword().empty()) {
		return true;
	}
	return false;
}

HttpRequest::Error CurlHttpRequest::getReturnCode(int curlcode) {
	Error error;

	switch (curlcode) {
	case CURLE_OK:
		error = HttpRequest::NoError;
		break;
	case CURLE_COULDNT_RESOLVE_HOST:
		error = HttpRequest::HostNotFound;
		break;
	case CURLE_COULDNT_CONNECT:
		error = HttpRequest::ConnectionRefused;
		break;
	case CURLE_COULDNT_RESOLVE_PROXY:
		error = HttpRequest::ProxyConnectionError;
		break;
	case CURLE_LOGIN_DENIED:
		error = HttpRequest::ProxyAuthenticationError;
		break;
	case CURLE_OPERATION_TIMEOUTED:
		error = HttpRequest::TimeOut;
		break;
	default:
		error = HttpRequest::UnknownError;
		break;
	}

	return error;
}

size_t curlHTTPWrite(void * ptr, size_t size, size_t nmemb, void * curlHttpRequestInstance) {
	if (curlHttpRequestInstance && ptr) {
		CurlHttpRequest * instance = (CurlHttpRequest *) curlHttpRequestInstance;

		int requestId;
		curl_easy_getinfo(instance->_curl, CURLINFO_PRIVATE, &requestId);

		//Appends the data received to the entire response content
		instance->entireResponse.append((const char *) ptr, nmemb);
		/*LOG_DEBUG("download done=" + String::fromNumber(instance->downloadDone) +
			" download total=" + String::fromNumber(instance->downloadTotal));*/

		if (instance->abortTransfer) {
			return 0;
		} else {
			return nmemb;
		}
	}
	else {
		return 0;
	}
}

size_t curlHTTPRead(void * ptr, size_t size, size_t nmemb, void * userp) {
	struct POST_DATA * pooh = (struct POST_DATA *) userp;

	if (size * nmemb < 1) {
		return 0;
	}

	if (pooh->sizeleft) {
		//Copy one single byte
		* (char *) ptr = pooh->readptr[0];

		//Advance pointer
		pooh->readptr++;

		//Less data left
		pooh->sizeleft--;

		//We return 1 byte at a time!
		return 1;
	}

	//No more data left to deliver
	return -1;
}

size_t curlHttpHeaderWrite(void * ptr, size_t size, size_t nmemb, void * curlHttpRequestInstance) {
	if (curlHttpRequestInstance && ptr) {
		CurlHttpRequest * instance = (CurlHttpRequest *) curlHttpRequestInstance;
		if (instance->abortTransfer) {
			return 0;
		} else {
			return nmemb;
		}
	} else {
		return 0;
	}
}

int curlHTTPProgress(void * curlHttpRequestInstance, double dltotal, double dlnow, double ultotal, double ulnow) {
	if (curlHttpRequestInstance) {
		CurlHttpRequest * instance = (CurlHttpRequest *) curlHttpRequestInstance;
		instance->downloadDone = dlnow;
		instance->downloadTotal = dltotal;
		int requestId;
		curl_easy_getinfo(instance->_curl, CURLINFO_PRIVATE, &requestId);
		instance->dataReadProgressEvent(requestId, dlnow, dltotal);
		instance->dataSendProgressEvent(requestId, ulnow, ultotal);

		//Launches answerReceivedEvent only if the entire response content has been received
		if ((dlnow >= dltotal) && (dlnow != 0 && dltotal != 0)) {
			instance->answerReceivedEvent(NULL, requestId, instance->entireResponse, HttpRequest::NoError);
		}

		if (instance->abortTransfer) {
			return 1;
		} else {
			return 0;
		}

	} else {
		return 1;
	}
}
