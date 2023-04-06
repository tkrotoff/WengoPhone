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

#include <http/QtHttpRequest.h>

#include <StringList.h>

#include <qhttp.h>

#include <iostream>
using namespace std;

QtHttpRequest::QtHttpRequest(HttpRequest * httpRequest) {
	_httpRequest = httpRequest;
}

QtHttpRequest::~QtHttpRequest() {
	//Deletes all the QHttp components
	for (unsigned int i = 0; i < _requestList.size(); i++) {
		delete _requestList[i];
	}

	_requestList.clear();
}

void QtHttpRequest::sendRequest(const std::string & url, const std::string & data, bool postMethod) {
	_httpRequest->sendRequest(url, data, postMethod);
}

void QtHttpRequest::sendRequest(bool sslProtocol, const std::string & hostname, unsigned int hostPort,
	const std::string & path, const std::string & data, bool postMethod) {

	QHttp * http = new QHttp(hostname, hostPort);
	_requestList.push_back(http);

	connect(http, SIGNAL(done(bool)),
		this, SLOT(transferDone(bool)));

	string url;
	if (postMethod) {
		url = path + " " + data;

		QHttpRequestHeader header("POST", path);
		header.setValue("Host", hostname);
		header.setContentType("application/x-www-form-urlencoded");
		QString tmp = data;
		http->request(header, tmp.utf8());
	} else {
		if (!data.empty()) {
			url = path + HttpRequest::HTTP_GET_SEPARATOR + data;
		} else {
			url = path;
		}

		QHttpRequestHeader header("GET", url);
		header.setValue("Host", hostname);
		header.setValue("Port", String::fromNumber(hostPort));
		http->request(header);
	}

	cerr << "QtHttpRequest: HTTP URL: " << hostname << url << endl;
}

void QtHttpRequest::transferDone(bool) {
	QHttp * http = (QHttp *) sender();
	Error error;
	switch (http->error()) {
	case QHttp::NoError:
		error = NoError;
		break;

	case QHttp::HostNotFound:
		error = HostNotFound;
		break;

	case QHttp::ConnectionRefused:
		error = ConnectionRefused;
		break;

	case QHttp::UnexpectedClose:
		error = UnexpectedClose;
		break;

	case QHttp::InvalidResponseHeader:
		error = InvalidResponseHeader;
		break;

	case QHttp::WrongContentLength:
		error = WrongContentLength;
		break;

	case QHttp::Aborted:
		error = Aborted;
		break;

	case QHttp::UnknownError:
		error = UnknownError;
		break;

	default:
		error = UnknownError;
		break;
	}

	answerReceived(QString(http->readAll()), error);
}

void QtHttpRequest::run() {
	//Nothing to do, Qt does it for us
	//QHttp is already asynchronous
}

void QtHttpRequest::answerReceived(const std::string & answer, Error error) {
	_httpRequest->answerReceived(answer, error);
}
