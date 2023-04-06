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

#include <ssorequest/SSORequest.h>

#include <networkdiscovery/NetworkProxyDiscovery.h>
#include <ssorequest/WengoAccountParser.h>

#include <util/Logger.h>

SSORequest::SSORequest(const NetworkProxy & networkProxy,
	WengoAccount & wengoAccount)
	: WengoWebService("en", "wengo", networkProxy, &wengoAccount) {
	_lastError = SSORequestErrorOk;
}

SSORequest::~SSORequest() {
}

SSORequest::SSORequestError SSORequest::launch() {
	QMutexLocker locker(&_mutex);

	_lastError = SSORequestErrorOk;

	setHostname(WengoAccount::getWengoServerHostname());
	setServicePath(WengoAccount::getWengoSSOServicePath());
	usePostMethod(true);
	useHttps(true);
	setPort(443);
	setWengoAuthentication(true);

	call(this);

	_condition.wait(&_mutex);

	return _lastError;
}

void SSORequest::answerReceived(int requestId, const std::string & answer) {
	if (!answer.empty()) {
		LOG_DEBUG("SSO request processed successfully");
		WengoAccountParser parser(*_wengoAccount, answer);
		if (parser.isLoginPasswordOk()) {
			LOG_DEBUG("valid login/password");
		} else {
			_lastError = SSORequestErrorInvalidLoginPassword;
			LOG_DEBUG("invalid login/password");
		}
	} else {
		_lastError = SSORequestErrorCannotConnect;
	}

	_condition.wakeAll();
}
