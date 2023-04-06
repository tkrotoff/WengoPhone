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

#include "HttpSource.h"
using namespace database;

#include <qhttp.h>

#include <iostream>
using namespace std;


void HttpSource::setVerbose(bool verbose) {
}

bool HttpSource::isVerbose() {
	return false;
}

void HttpSource::setPost(bool post) {
	_post = post;
}

void HttpSource::run() {
}

HttpSource::HttpSource() {
	setPost(false);
}

HttpSource::~HttpSource() {
	//delete _http;
}

void HttpSource::sendHttpRequest(const std::string & hostname,
				/*unsigned int port,*/
				const std::string & path,
				const std::string & data) {

	_http = new QHttp(hostname);

	QObject::connect(_http, SIGNAL(done(bool)),
		this, SLOT(httpTransferDone(bool)));

	string url;
	if (_post) {
		url = path + " " + data;

		QHttpRequestHeader header("POST", path);
		header.setValue("Host", hostname);
		header.setContentType("application/x-www-form-urlencoded");
		QString tmp = data;
		_http->request(header, tmp.utf8());
	} else {
		url = path + "?" + data;

		QHttpRequestHeader header("GET", url);
		header.setValue("Host", hostname);
		_http->request(header);
	}

	cerr << "URL: " << hostname << url << endl;
}

void HttpSource::httpTransferDone(bool transmissionError) {
	if (transmissionError) {
		return;
	}

	httpAnswerReceived();
}

QString HttpSource::read() const {
	/*QString tmp = _http->readAll();
	if (!tmp.isEmpty()) {
		cerr << "SMS " << tmp << endl;
	}
	return tmp;*/
	return _http->readAll();
}
