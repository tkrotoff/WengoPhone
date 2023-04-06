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

#ifndef OWWENGOWEBSERVICE_H
#define OWWENGOWEBSERVICE_H

#include <webservice/webservicedll.h>

#include <networkdiscovery/NetworkProxyDiscovery.h>

#include <http/HttpRequest.h>
#include <util/NonCopyable.h>

class WengoAccount;

/**
 * WengoWebService.
 *
 * Class helper to create web services easily.
 *
 * @author Mathieu Stute
 */
class WEBSERVICE_API WengoWebService : public QObject, NonCopyable {
	Q_OBJECT
public:

	/**
	 * Default constructor.
	 *
	 * @param networkProxy NetworkProxy information
	 * @param wengoAccount the WengoAccount used for web services
	 */
	WengoWebService(const std::string & language, const std::string & softphoneName,
		const NetworkProxy & networkProxy, WengoAccount * wengoAccount = NULL);

	virtual ~WengoWebService();

	/**
	 * Uses or not https mode.
	 *
	 * @param https if true uses https else http
	 */
	void useHttps(bool https);

	/**
	 * Uses or not http post method (HTTP POST mode).
	 *
	 * @param postMethod if true uses HTTP POST else HTTP GET
	 */
	void usePostMethod(bool postMethod);

	/**
	 * Sets hostname.
	 *
	 * @param hostname hostname
	 */
	void setHostname(const std::string & hostname);

	/**
	 * Sets port.
	 *
	 * @param port port
	 */
	void setPort(int port);

	/**
	 * Sets service path.
	 *
	 * @param servicePath the path to the service
	 */
	void setServicePath(const std::string & servicePath);

	/**
	 * Sets additionnal parameter.
	 *
	 * A string like : "name=toto&city=paris",
	 * no '?' at the begining of the string
	 *
	 * @param servicePath the path to the service
	 */
	void setParameters(const std::string & param);

	/**
	 * Sets/unsets Wengo authentication.
	 *
	 * @param auth activate authentication if true
	 */
	void setWengoAuthentication(bool auth);

	/**
	 * Sets the WengoAccount to use.
	 */
	void setWengoAccount(WengoAccount * wengoAccount);

	/**
	 * Calls the web service.
	 *
	 * @param caller caller
	 */
	int call(WengoWebService * caller);

Q_SIGNALS:

	/**
	 * Callback to check if the SMS was received or not.
	 *
	 * @param requestId request unique identifier
	 * @param answer the response returned by the web service
	 */
	void answerReceivedSignal(int requestId, std::string answer);

protected:

	/**
	 * Sends a request to a web service.
	 *
	 * @param https if true this is a https request
	 * @param get if true this is a get request
	 * @param hostname hostname of the webservice
	 * @param port port
	 * @param servicePath path to the service
	 * @param additionalParameters additional parameters to the request
	 * @return unique request ID
	 */
	int sendRequest();

	/** WengoAccount: to get login & password. */
	WengoAccount * _wengoAccount;

protected Q_SLOTS:

	/**
	 * @see IHttpRequest::answerReceivedSignal
	 */
	void answerReceivedSlot(int requestId, std::string answer, IHttpRequest::Error error);

private:

	/**
	 * Answer received callback.
	 *
	 * @param answer the answer to the request
	 * @param requestId the request id
	 */
	virtual void answerReceived(int requestId, const std::string & answer) = 0;

	/** https/http mode. */
	bool _https;

	/** HTTP GET/POST mode. */
	bool _postMethod;

	/** Use authentication mode. */
	bool _auth;

	/** Web service hostname. */
	std::string _hostname;

	/** Web service port number. */
	int _port;

	/** Web service path. */
	std::string _servicePath;

	/** Web service parameters. */
	std::string _parameters;

	/** Caller */
	WengoWebService * _caller;

	/** Language parameter. */
	std::string _language;

	/** Softphone name parameter. */
	std::string _softphoneName;

	/** NetworkProxy information. */
	NetworkProxy _networkProxy;

};

#endif	//OWWENGOWEBSERVICE_H
