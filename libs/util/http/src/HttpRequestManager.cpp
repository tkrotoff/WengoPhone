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

#include <http/HttpRequestManager.h>

#include <thread/ThreadEvent.h>

#include <util/SafeDelete.h>

HttpRequestManager::HttpRequestManager() {
	start();
}

HttpRequestManager::~HttpRequestManager() {
}

void HttpRequestManager::addAndStart(IHttpRequest *httpRequest) {
	httpRequest->answerReceivedEvent +=
		boost::bind(&HttpRequestManager::answerReceivedEventHandler, this, _1, _2, _3, _4);

	httpRequest->start();
}

void HttpRequestManager::run() {
	runEvents();
}

void HttpRequestManager::answerReceivedEventHandler(IHttpRequest * sender, int requestId,
	const std::string & answer, IHttpRequest::Error error) {

	typedef ThreadEvent1<void (IHttpRequest *), IHttpRequest *> MyThreadEvent;
	MyThreadEvent *event = new MyThreadEvent(boost::bind(&HttpRequestManager::joinAndDelete, this, _1), sender);

	this->postEvent(event);
}

void HttpRequestManager::joinAndDelete(IHttpRequest *httpRequest) {
	//Delete will join the Thread. @see Thread::~Thread
	OWSAFE_DELETE(httpRequest);
}
