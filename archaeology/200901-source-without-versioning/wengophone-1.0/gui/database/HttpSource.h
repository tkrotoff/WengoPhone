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

#ifndef DATABASE_HTTPSOURCE_H
#define DATABASE_HTTPSOURCE_H

#include "database/Source.h"
#include <NonCopyable.h>

#include <thread/Thread.h>
#include <qstring.h>
#include <qobject.h>

#include <curl/curl.h>

#include <string>

class QHttp;

struct POST_DATA {
  char *readptr;
  size_t sizeleft;
};

namespace database {

/**
 * Source from an HTTP webserver, using libcurl with SSL support
 *
 * @author Tanguy Krotoff, Mathieu Stute
 */
class HttpSource : public QObject, public Source , public Thread {
	Q_OBJECT
public:

	HttpSource();
	virtual ~HttpSource();

	/**
	 * Set verbose mode
	 * @param verbose or not
	 */
	void setVerbose(bool verbose);

	/**
	 * Return the verbose mode
	 */
	bool isVerbose();

	/**
	 * Set POST method
	 * @param POST method or not
	 */
	void setPost(bool post);

	/**
	 * Read the result of the HTTP request
	 * @return return a string representing the request result
	 */
	virtual QString read() const;

	/**
	 * Send the HTTP request : start the thread
	 */
	void sendHttpRequest(const std::string & hostname,
				/*unsigned int port,*/
				const std::string & path,
				const std::string & data);

	/**
	 * Signal emitted when the answer of the HTTP server has arrived.
	 */
	virtual void httpAnswerReceived() = 0;

protected:

	/**
	 * Thread run method implementation
	 */
	virtual void run();

private slots:

	void httpTransferDone(bool transmissionError);

private:

	QHttp * _http;

	CURL *_curl;
	QString _url;
	bool _verbose;
	bool _post;
	QString _postdata;
	//long _authMask;		//determine the type of authentification
	QString _proxyUrlPort;
	QString _proxyUserIDPassword;
	struct POST_DATA pooh;

	const static QString testUrl;
	
	char * getstr(QString s);
	long getProxyAuthenticationType();
	void setCurlParam();
	void setUrl();
	void setGetPostParam();
	void setSslParam();
	void setProxyParam();
};

}

/**
 * Curl callback function that is called when data is available
 */
size_t curlHTTPWrite(void *ptr, size_t size, size_t nmemb, void *stream);

/**
 * Curl callback read function that send data to the url
 */
size_t curlHTTPRead(void *ptr, size_t size, size_t nmemb, void *userp);


#endif	//DATABASE_HTTPSOURCE_H
