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

#ifndef NULLHTTPREQUEST_H
#define NULLHTTPREQUEST_H

#include <http/HttpRequest.h>

/**
 * Empty (null) HttpRequest implementation.
 *
 * @see HttpRequest
 * @author Tanguy Krotoff
 */
class NullHttpRequest : public IHttpRequest {
public:

	NullHttpRequest(HttpRequest * httpRequest);

	virtual void sendRequest(bool sslProtocol,
				const std::string & hostname,
				unsigned int hostPort,
				const std::string & path,
				const std::string & data,
				bool postMethod = false);

	virtual void sendRequest(const std::string & url, const std::string & data, bool postMethod = false);

protected:

	virtual void run();

private:

	/**
	 * Callback for answerReceived() and run() methods.
	 *
	 * NullHttpRequest::run() calls _httpRequest->run().
	 * NullHttpRequest::answerReceived() calls _httpRequest->run().
	 */
	HttpRequest * _httpRequest;
};

#endif	//NULLHTTPREQUEST_H
