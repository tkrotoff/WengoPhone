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

#ifndef NULLHTTPREQUESTFACTORY_H
#define NULLHTTPREQUESTFACTORY_H

#include <http/HttpRequestFactory.h>
#include <http/NullHttpRequest.h>

/**
 * Creates NullHttpRequest.
 *
 * @author Tanguy Krotoff
 */
class NullHttpRequestFactory : public HttpRequestFactory {
public:

	/**
	 * Constructs a new NullHttpRequestFactory.
	 */
	NullHttpRequestFactory() { }

	virtual ~NullHttpRequestFactory() { }

	virtual IHttpRequest * create(HttpRequest * httpRequest) const {
		return new NullHttpRequest(httpRequest);
	}
};

#endif	//NULLHTTPREQUESTFACTORY_H
