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

#include <http/HttpRequest.h>

#include <http/DefaultHttpRequestFactory.h>
#include <http/HttpRequestFactory.h>
#include <StringList.h>

#include <iostream>
using namespace std;
#include <cassert>

const std::string HttpRequest::HTTP_PORT_SEPARATOR = ":";
const std::string HttpRequest::HTTP_PATH_SEPARATOR = "/";
const unsigned int HttpRequest::HTTP_DEFAULT_PORT = 80;
const std::string HttpRequest::HTTPS_PROTOCOL = "https://";
const std::string HttpRequest::HTTP_PROTOCOL = "http://";
const std::string HttpRequest::HTTP_GET_SEPARATOR = "?";

std::string HttpRequest::_proxyHost;
unsigned int HttpRequest::_proxyPort = 80;
std::string HttpRequest::_proxyUsername;
std::string HttpRequest::_proxyPassword;
bool HttpRequest::_useProxy = false;
bool HttpRequest::_useProxyAuthentication = false;

HttpRequestFactory * HttpRequest::_factory = NULL;

void HttpRequest::setFactory(HttpRequestFactory * factory) {
	_factory = factory;
}

HttpRequest::HttpRequest() {
	if (!_factory) {
		_factory = new DefaultHttpRequestFactory();
	}
	_httpRequestPrivate = _factory->create(this);
}

HttpRequest::~HttpRequest() {
	delete _httpRequestPrivate;
}

void HttpRequest::sendRequest(bool sslProtocol,
			const std::string & hostname,
			unsigned int hostPort,
			const std::string & path,
			const std::string & data,
			bool postMethod) {

	_httpRequestPrivate->sendRequest(sslProtocol, hostname, hostPort, path, data, postMethod);
}

bool usesSSLProtocol(const std::string & url) {
	bool sslProtocol = false;
	if (url.find(HttpRequest::HTTPS_PROTOCOL) != string::npos) {
		//https://wengo.fr:8080/softphone-sso/sso.php
		sslProtocol = true;
	} else if (url.find(HttpRequest::HTTP_PROTOCOL) != string::npos) {
		//http://wengo.fr:8080/softphone-sso/sso.php
		sslProtocol = false;
	} else {
		assert(NULL && "HttpRequest: incorrect HTTP URL");
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

unsigned int getHostPort(const std::string & url) {
	unsigned int hostPort = HttpRequest::HTTP_DEFAULT_PORT;
	string::size_type posPortBegin = url.find(HttpRequest::HTTP_PORT_SEPARATOR);
	if (posPortBegin != string::npos) {
		//http://wengo.fr:8080/softphone-sso/sso.php
		posPortBegin = posPortBegin + HttpRequest::HTTP_PATH_SEPARATOR.length();
		string::size_type posPortEnd = url.find(HttpRequest::HTTP_PATH_SEPARATOR, posPortBegin);
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
	string::size_type posHostnameEnd = tmp.find(HttpRequest::HTTP_PATH_SEPARATOR);
	return tmp.substr(0, posHostnameEnd);
	//wengo.fr
}

std::string getPath(const std::string & url) {
	//https://wengo.fr:8080/softphone-sso/sso.php
	String tmp = url;
	eraseProtocol(tmp);

	StringList tmp2 = StringList::split(tmp, HttpRequest::HTTP_PATH_SEPARATOR);
	//tmp2[0] == "wengo.fr:8080"

	if (!tmp2[0].empty()) {
		tmp += HttpRequest::HTTP_PATH_SEPARATOR;
		tmp.replace(tmp2[0], String::null);
	}

	return tmp;
}

void HttpRequest::sendRequest(const std::string & url, const std::string & data, bool postMethod) {
	bool sslProtocol = usesSSLProtocol(url);
	string hostname = getHostname(url);
	unsigned int hostPort = getHostPort(url);
	string path = getPath(url);

	//cerr << "HttpRequest: " << "HTTPS:" << sslProtocol << " " << hostname << ":" << hostPort << path << endl;

	sendRequest(sslProtocol, hostname, hostPort, path, data, postMethod);
}

void HttpRequest::run() {
	_httpRequestPrivate->run();
}
