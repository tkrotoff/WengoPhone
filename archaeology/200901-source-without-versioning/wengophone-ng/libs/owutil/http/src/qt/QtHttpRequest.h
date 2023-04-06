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

#ifndef OWQTHTTPREQUEST_H
#define OWQTHTTPREQUEST_H

#include <http/IHttpRequest.h>

#include <QtCore/QObject>
#include <QtNetwork/QHttp>

#include <vector>

class QHttp;

/**
 * HttpRequest implementation using the Qt library.
 *
 * BE CAREFULE: SSL request works only with Qt 4.3!!
 *
 * @see HttpRequest
 * @author Tanguy Krotoff
 */
class QtHttpRequest : public IHttpRequest {
	Q_OBJECT
public:

	QtHttpRequest();

	virtual ~QtHttpRequest();

	int sendRequest(bool sslProtocol,
			const std::string & hostname,
			unsigned hostPort,
			const std::string & path,
			const std::string & data,
			bool postMethod = false);

	void abort();

private Q_SLOTS:

	/**
	 * The HTTP transfer is done.
	 *
	 * @param transmissionError true if an error of transmission occured or not
	 */
	void transferDone(bool transmissionError);

private:

	typedef std::vector<QHttp *> Requests;

	/**
	 * List of QHttp components.
	 */
	Requests _requestList;
};

#endif	//OWQTHTTPREQUEST_H
