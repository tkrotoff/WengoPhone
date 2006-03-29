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

#ifndef SOFTUPDATER_H
#define SOFTUPDATER_H

#include <string>

#include <util/NonCopyable.h>
#include <util/http/HttpRequest.h>

/**
 * Downloads a file from an URL.
 *
 * @author Tanguy Krotoff
 */
class SoftUpdater : NonCopyable {
public:

	/**
	 * @see IHttpRequest::dataReadProgressEvent
	 */
	Event<void (int bytesDone, int bytesTotal)> dataReadProgressEvent;

	/**
	 * Downloads a file from an URL to a destination.
	 *
	 * @param url URL of the source file (http://login:password@www.website.com/file.txt)
	 * @param fileName destination file (C:/Program Files/file.txt)
	 */
	SoftUpdater(const std::string & url, const std::string & fileName);

	~SoftUpdater();

private:

	void downloadFile();

	void answerReceivedEventHandler(int requestId, const std::string & answer, HttpRequest::Error error);

	std::string _fileName;
};

#endif	//SOFTUPDATER_H
