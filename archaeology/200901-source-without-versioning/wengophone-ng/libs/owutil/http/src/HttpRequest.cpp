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

#include <http/HttpRequest.h>

#include "HttpRequestFactory.h"

#include <util/StringList.h>
#include <util/String.h>
#include <util/Logger.h>
#include <util/SafeDelete.h>
#include <util/SafeConnect.h>

#include <QtCore/QMetaType>

#include <iostream>

const std::string HttpRequest::HTTP_PORT_SEPARATOR = ":";
const std::string HttpRequest::HTTP_PATH_SEPARATOR = "/";
const unsigned HttpRequest::HTTP_DEFAULT_PORT = 80;
const std::string HttpRequest::HTTPS_PROTOCOL = "https://";
const std::string HttpRequest::HTTP_PROTOCOL = "http://";
const std::string HttpRequest::HTTP_GET_SEPARATOR = "?";

std::string HttpRequest::_proxyHost;
unsigned HttpRequest::_proxyPort = 80;
std::string HttpRequest::_proxyUsername;
std::string HttpRequest::_proxyPassword;
std::string HttpRequest::_userAgent;

HttpRequest::HttpRequest() {
	qRegisterMetaType<IHttpRequest::Error>("IHttpRequest::Error");

	_httpRequestPrivate = HttpRequestFactory::create();

	SAFE_CONNECT_TYPE(_httpRequestPrivate, SIGNAL(answerReceivedSignal(int, std::string, IHttpRequest::Error)),
		SLOT(answerReceivedSlot(int, std::string, IHttpRequest::Error)), Qt::DirectConnection);
	SAFE_CONNECT(_httpRequestPrivate, SIGNAL(dataReadProgressSignal(int, double, double)),
		SIGNAL(dataReadProgressSignal(int, double, double)));
	SAFE_CONNECT(_httpRequestPrivate, SIGNAL(dataSendProgressSignal(int, double, double)),
		SIGNAL(dataSendProgressSignal(int, double, double)));
}

void HttpRequest::answerReceivedSlot(int requestId, std::string answer, IHttpRequest::Error error) {
	answerReceivedSignal(requestId, answer, error);
}

HttpRequest::~HttpRequest() {
	OWSAFE_DELETE(_httpRequestPrivate);
}

int HttpRequest::sendRequest(bool sslProtocol,
	const std::string & hostname,
	unsigned hostPort,
	const std::string & path,
	const std::string & data,
	bool postMethod) {

	std::string protocol = "http://";
	if (sslProtocol) {
		protocol = "https://";
	}

	LOG_DEBUG(protocol + hostname + ":" + String::fromNumber(hostPort) + path);

	return _httpRequestPrivate->sendRequest(sslProtocol, hostname, hostPort, path, data, postMethod);
}

void HttpRequest::setProxy(const std::string & host,
	unsigned port,
	const std::string & username,
	const std::string & password) {

	_proxyHost = host;
	_proxyPort = port;
	_proxyUsername = username;
	_proxyPassword = password;
}

const std::string & HttpRequest::getProxyHost() {
	return _proxyHost;
}

unsigned HttpRequest::getProxyPort() {
	return _proxyPort;
}

const std::string & HttpRequest::getProxyUsername() {
	return _proxyUsername;
}

const std::string & HttpRequest::getProxyPassword() {
	return _proxyPassword;
}

void HttpRequest::setUserAgent(const std::string & userAgent) {
	_userAgent = userAgent;
}

const std::string & HttpRequest::getUserAgent() {
	return _userAgent;
}

bool usesSSLProtocol(const std::string & url) {
	bool sslProtocol = false;
	if (url.find(HttpRequest::HTTPS_PROTOCOL) != std::string::npos) {
		//https://wengo.fr:8080/softphone-sso/sso.php
		sslProtocol = true;
	} else if (url.find(HttpRequest::HTTP_PROTOCOL) != std::string::npos) {
		//http://wengo.fr:8080/softphone-sso/sso.php
		sslProtocol = false;
	} else {
		LOG_FATAL("incorrect HTTP URL=" + url);
	}

	return sslProtocol;
}

void eraseProtocol(std::string & url) {
	if (usesSSLProtocol(url)) {
		//https://wengo.fr:8080/softphone-sso/sso.php
		url.erase(0, HttpRequest::HTTPS_PROTOCOL.length());
	} else {
		//http://wengo.fr:8080/softphone-sso/sso.php
		url.erase(0, HttpRequest::HTTP_PROTOCOL.length());
	}
}

unsigned getHostPort(const std::string & url) {
	unsigned hostPort = HttpRequest::HTTP_DEFAULT_PORT;
	std::string::size_type posPortBegin = url.find(HttpRequest::HTTP_PORT_SEPARATOR);
	if (posPortBegin != std::string::npos) {
		//http://wengo.fr:8080/softphone-sso/sso.php
		posPortBegin = posPortBegin + HttpRequest::HTTP_PATH_SEPARATOR.length();
		std::string::size_type posPortEnd = url.find(HttpRequest::HTTP_PATH_SEPARATOR, posPortBegin);
		String tmp = url.substr(posPortBegin, posPortEnd - posPortBegin);
		if (!tmp.empty()) {
			hostPort = tmp.toInteger();
		}
	}

	return hostPort;
}

std::string getHostname(const std::string & url) {
	String tmp = url;
	eraseProtocol(tmp);

	//wengo.fr:8080/softphone-sso/sso.php
	unsigned hostPort = getHostPort(tmp);
	tmp.replace(HttpRequest::HTTP_PORT_SEPARATOR + String::fromNumber(hostPort), String::null);
	//wengo.fr/softphone-sso/sso.php
	std::string::size_type posHostnameEnd = tmp.find(HttpRequest::HTTP_PATH_SEPARATOR);
	return tmp.substr(0, posHostnameEnd);
	//wengo.fr
}

std::string getPath(const std::string & url) {
	//https://wengo.fr:8080/softphone-sso/sso.php
	String tmp = url;
	eraseProtocol(tmp);

	StringList tmp2 = tmp.split(HttpRequest::HTTP_PATH_SEPARATOR);
	//tmp2[0] == "wengo.fr:8080"

	if (!tmp2[0].empty()) {
		//tmp += HttpRequest::HTTP_PATH_SEPARATOR;
		tmp.replace(tmp2[0], String::null);
	}

	return tmp;
}

int HttpRequest::sendRequest(const std::string & url, const std::string & data, bool postMethod) {
	bool sslProtocol = usesSSLProtocol(url);
	std::string hostname = getHostname(url);
	unsigned hostPort = getHostPort(url);
	std::string path = getPath(url);

	return sendRequest(sslProtocol, hostname, hostPort, path, data, postMethod);
}

void HttpRequest::abort() {
	LOG_DEBUG("HTTP request aborted");
	_httpRequestPrivate->abort();
}
