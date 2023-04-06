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

#include "QtHttpRequest.h"

#include <http/HttpRequest.h>

#include <util/Logger.h>
#include <util/SafeDelete.h>
#include <util/SafeConnect.h>
#include <util/String.h>

#include <QtNetwork/QHttp>

#include <iostream>

QtHttpRequest::QtHttpRequest() {
}

QtHttpRequest::~QtHttpRequest() {
	//Deletes all the QHttp components
	for (unsigned i = 0; i < _requestList.size(); i++) {
		OWSAFE_DELETE(_requestList[i]);
	}

	_requestList.clear();
}

int QtHttpRequest::sendRequest(bool sslProtocol, const std::string & hostname, unsigned hostPort,
	const std::string & path, const std::string & data, bool postMethod) {

	QHttp * http = NULL;
	if (sslProtocol) {
		http = new QHttp(hostname.c_str(), QHttp::ConnectionModeHttps, hostPort);
	} else {
		http = new QHttp(hostname.c_str(), hostPort);
	}

	SAFE_CONNECT(http, SIGNAL(done(bool)), SLOT(transferDone(bool)));

	_requestList.push_back(http);

	std::string url;
	if (postMethod) {
		url = path + " " + data;

		QHttpRequestHeader header("POST", path.c_str());
		header.setValue("Host", hostname.c_str());
		header.setContentType("application/x-www-form-urlencoded");
		QString tmp = data.c_str();
		http->request(header, tmp.toUtf8());
	} else {
		if (!data.empty()) {
			url = path + HttpRequest::HTTP_GET_SEPARATOR + data;
		} else {
			url = path;
		}

		QHttpRequestHeader header("GET", url.c_str());
		header.setValue("Host", hostname.c_str());
		header.setValue("Port", QString::number(hostPort).toStdString().c_str());
		http->request(header);
	}

	LOG_DEBUG("url=" + hostname + url);

	return http->currentId();
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

	case QHttp::ProxyAuthenticationRequiredError:
		error = ProxyAuthenticationError;
		break;

	case QHttp::AuthenticationRequiredError:
		error = ProxyAuthenticationError;
		break;

	case QHttp::UnknownError:
		error = UnknownError;
		break;

	default:
		LOG_FATAL("unknown error=" + QString::number(http->error()).toStdString());
	}

	LOG_DEBUG("error=" + http->errorString().toStdString());
	QByteArray byteArray = http->readAll();

	answerReceivedSignal(http->currentId(), std::string(byteArray.constData(), byteArray.size()), error);
}

void QtHttpRequest::abort() {
	for (unsigned i = 0; i < _requestList.size(); i++) {
		_requestList[i]->abort();
	}
}
