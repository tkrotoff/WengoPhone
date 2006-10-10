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

#ifndef OWHTTPREQUESTMANAGER_H
#define OWHTTPREQUESTMANAGER_H

#include <http/IHttpRequest.h>
#include <thread/Thread.h>
#include <util/Singleton.h>
#include <util/Trackable.h>

/**
 * Deletes HttpRequests when they are finished.
 *
 * @author Philippe Bernery
 */
class HttpRequestManager : public Singleton<HttpRequestManager>, public Thread, public Trackable {

	friend class Singleton<HttpRequestManager>;

public:

	/**
	 * Adds an IHttpRequest to the manager and starts it.
	 */
	void addAndStart(IHttpRequest *httpRequest);

private:

	HttpRequestManager();

	virtual ~HttpRequestManager();

	/**
	 * @see IHttpRequest::answerReceivedEvent
	 */
	void answerReceivedEventHandler(IHttpRequest * sender, int requestId,
		const std::string & answer, IHttpRequest::Error error);

	/**
	 * Joins and deletes the HttpTequest.
	 */
	void joinAndDelete(IHttpRequest *httpRequest);

	virtual void run();
};

#endif //OWHTTPREQUESTMANAGER_H
