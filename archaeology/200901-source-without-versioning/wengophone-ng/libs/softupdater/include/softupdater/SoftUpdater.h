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

#ifndef OWSOFTUPDATER_H
#define OWSOFTUPDATER_H

#include <http/HttpRequest.h>

#include <util/NonCopyable.h>

#include <string>

/**
 * Downloads a file from an URL.
 *
 * @author Tanguy Krotoff
 */
class SoftUpdater : public QObject, NonCopyable {
	Q_OBJECT
public:

Q_SIGNALS:

	/**
	 * @see IHttpRequest::dataReadProgressSignal
	 *
	 * @param downloadSpeed download speed in kilo bytes per second
	 */
	void dataReadProgressSignal(double bytesDone, double bytesTotal, unsigned downloadSpeed);

	/**
	 * @see IHttpRequest::answerReceivedSignal
	 */
	void downloadFinishedSignal(IHttpRequest::Error error);

	/**
	 * Downloads a file from an URL to a destination file.
	 *
	 * @param url URL of the source file (http://login:password@www.website.com/file.txt)
	 * @param fileName destination file (C:/Program Files/file.txt)
	 */
	SoftUpdater(const std::string & url, const std::string & fileName);

	~SoftUpdater();

	/**
	 * Starts the file transfer.
	 *
	 * Does nothing if the file transfer has been already started.
	 */
	void start();

	/**
	 * @see IHttpRequest::abort()
	 */
	void abort();

private Q_SLOTS:

	void dataReadProgressSlot(int requestId, double bytesDone, double bytesTotal);

	void answerReceivedSlot(int requestId, std::string answer, IHttpRequest::Error error);

private:

	void downloadFile();

	std::string _fileName;

	std::string _url;

	HttpRequest * _httpRequest;
};

#endif	//OWSOFTUPDATER_H
