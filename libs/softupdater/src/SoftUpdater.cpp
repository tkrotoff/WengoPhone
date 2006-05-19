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

#include <softupdater/SoftUpdater.h>

#include <util/Logger.h>
#include <util/File.h>

#include <ctime>

SoftUpdater::SoftUpdater(const std::string & url, const std::string & fileName) {
	_url = url;
	_fileName = fileName;
	_httpRequest = NULL;
}

SoftUpdater::~SoftUpdater() {
	if (_httpRequest) {
		delete _httpRequest;
	}
}

void SoftUpdater::start() {
	if (_httpRequest) {
		LOG_FATAL("file transfer already started");
		return;
	}

	_httpRequest = new HttpRequest();
	_httpRequest->dataReadProgressEvent += boost::bind(&SoftUpdater::dataReadProgressEventHandler, this, _1, _2, _3);
	_httpRequest->answerReceivedEvent += boost::bind(&SoftUpdater::answerReceivedEventHandler, this, _1, _2, _3, _4);

	_httpRequest->sendRequest(_url, String::null);
}

void SoftUpdater::abort() {
	if (_httpRequest) {
		_httpRequest->abort();
	}
}

void SoftUpdater::dataReadProgressEventHandler(int requestId, double bytesDone, double bytesTotal) {
	static const int startTime = time(NULL);

	int currentTime = time(NULL);

	unsigned downloadSpeed = bytesDone / (currentTime - startTime) / 1000;
	dataReadProgressEvent(bytesDone, bytesTotal, downloadSpeed);
}

void SoftUpdater::answerReceivedEventHandler(IHttpRequest * sender, int requestId, const std::string & answer, HttpRequest::Error error) {
	LOG_DEBUG("requestId=" + String::fromNumber(requestId) + " error=" + String::fromNumber(error));
	if (error == HttpRequest::NoError && !answer.empty()) {
		FileWriter file(_fileName);
		file.write(answer);
	}
	downloadFinishedEvent(error);
	//sender == _httpRequest
	/*delete sender;
	sender = NULL;*/
	delete _httpRequest;
	_httpRequest = NULL;
}
