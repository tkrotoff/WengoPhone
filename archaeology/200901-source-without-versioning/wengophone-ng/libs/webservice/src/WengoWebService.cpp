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

#include <webservice/WengoWebService.h>

#include <coipmanager_base/account/WengoAccount.h>

#include <util/String.h>
#include <util/Logger.h>
#include <util/SafeConnect.h>

WengoWebService::WengoWebService(const std::string & language,
	const std::string & softphoneName, 
	const NetworkProxy & networkProxy,
	WengoAccount * wengoAccount)
	: _wengoAccount(wengoAccount) {

	_language = language;
	_softphoneName = softphoneName;
	_networkProxy = networkProxy;
	_https = false;
	_postMethod = true;
	_auth = false;
	_port = 0;
}

WengoWebService::~WengoWebService() {
}

void WengoWebService::useHttps(bool https) {
	_https = https;
}

void WengoWebService::usePostMethod(bool postMethod) {
	_postMethod = postMethod;
}

void WengoWebService::setHostname(const std::string & hostname) {
	_hostname= hostname;
}

void WengoWebService::setPort(int port) {
	_port = port;
}

void WengoWebService::setServicePath(const std::string & servicePath) {
	_servicePath = servicePath;
}

void WengoWebService::setParameters(const std::string & parameters) {
	_parameters = parameters;
}

void WengoWebService::setWengoAuthentication(bool auth) {
	_auth = auth;
}

void WengoWebService::setWengoAccount(WengoAccount * wengoAccount) {
	_wengoAccount = wengoAccount;
}

int WengoWebService::sendRequest() {
	HttpRequest * httpRequest = new HttpRequest();

	httpRequest->setProxy(_networkProxy.getServer(),
		_networkProxy.getServerPort(),
		_networkProxy.getLogin(),
		_networkProxy.getPassword());
	
	SAFE_CONNECT_TYPE(httpRequest, SIGNAL(answerReceivedSignal(int, std::string, IHttpRequest::Error)),
		SLOT(answerReceivedSlot(int, std::string, IHttpRequest::Error)), Qt::DirectConnection);

	return httpRequest->sendRequest(_https, _hostname, _port, _servicePath, _parameters, _postMethod);
}

int WengoWebService::call(WengoWebService * caller) {
	//Set caller
	_caller = caller;

	//Add wengo parameters
	std::string data = "lang=" + _language;
	data += "&wl=" + _softphoneName;

	//Add authentication parameters
	if ((_auth) && (_wengoAccount)) {
		String login = String::encodeUrl(_wengoAccount->getWengoLogin());
		login.replace("%2e", ".", false);
		String password = String::encodeUrl(_wengoAccount->getWengoPassword());
		password.replace("%2e", ".", false);
		data += "&login=" + login + "&password=" + password;
	}

	if (!_parameters.empty()) {
		_parameters = data + "&" + _parameters;
	} else {
		_parameters = data;
	}

	return sendRequest();
}

void WengoWebService::answerReceivedSlot(int requestId, std::string answer, IHttpRequest::Error error) {
	if (_caller) {
		if (error == IHttpRequest::NoError) {
			_caller->answerReceived(requestId, answer);
		} else {
			_caller->answerReceived(requestId, String::null);
		}
	}
}
