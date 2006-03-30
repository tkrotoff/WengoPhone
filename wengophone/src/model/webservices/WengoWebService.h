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

#ifndef OW_WENGOWEBSERVICE_H
#define OW_WENGOWEBSERVICE_H

#include <http/HttpRequest.h>

class WengoAccount;

/**
 * @class WengoWebService
 *
 * @author Mathieu Stute
 */
class WengoWebService {

public:

	/**
	 * Callback to check if the SMS was received or not.
	 *
	 * @param sender this class
	 * @param requestId request unique identifier
	 * @param response the response returned by the web service
	 */
	Event<void (WengoWebService & sender, int requestId, std::string response)> answerReceivedEvent;

	/**
	 * default constructor
	 *
	 * @param wengoAccount the WengoAccount used for web services
	 */
	WengoWebService(WengoAccount & wengoAccount);

	/**
	 * Set/unset https mode
	 * @param https if true https else http
	 */
	void setHttps(bool https);
	
	/**
	 * Set/unset get mode
	 * @param get if true GET else POST
	 */
	void setGet(bool get);
	
	/**
	 * Set hostname
	 * @param hostname hostname
	 */
	void setHostname(std::string hostname);
	
	/**
	 * Set port
	 * @param port port
	 */
	void setPort(int port);
	
	/**
	 * Set service path
	 * @param servicePath the path to the service
	 */
	void setServicePath(std::string servicePath);
	
	/**
	 * Set additionnal parameter
	 * a string like : "name=toto&city=paris",
	 * no '?' at the begining of the string
	 * 
	 * @param servicePath the path to the service
	 */
	void setParameters(std::string param);
	
	/**
	 * Set/unset wengi authentication
	 * @param auth activate authentication if true
	 */
	void setWengoAuthentication(bool auth);
	
	/**
	 * call the web service
	 * @param caller caller
	 */
	int call(WengoWebService * caller);

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

	/**
	 * Answer received callback
	 * @param answer the answer to the request
	 * @param id the request id
	 */
	void answerReceivedEventHandler(int requestId, const std::string & answer, HttpRequest::Error error);

private:
	
	/**
	 * Answer received callback
	 * @param answer the answer to the request
	 * @param id the request id
	 */
	virtual void answerReceived(std::string answer, int id) = 0;

	/** WengoAccount: to get login & password */
	WengoAccount & _wengoAccount;
	
	/** https/http mode */
	bool _https;
	
	/** GET/POST mode */
	bool _get;
	
	/** use authentication mode */
	bool _auth;
	
	/** hostname of the web service */
	std::string _hostname;
	
	/** port of the web service */
	int _port;
	
	/** path of the web service */
	std::string _servicePath;
	
	/** parameters of the web service */
	std::string _parameters;
	
	/** caller */
	WengoWebService * _caller;
};

#endif //OW_WENGOWEBSERVICE_H
