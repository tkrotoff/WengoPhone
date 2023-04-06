/*
 * WengoPhone, a voice over Internet phone
 * Copyright (C) 2004-2007 Wengo
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#include "WengoSMSSession.h"

#include <coipmanager_base/account/Account.h>
#include <coipmanager_base/account/WengoAccount.h>

#include <util/Logger.h>
#include <util/String.h>

WengoSMSSession::WengoSMSSession(CoIpManager & coIpManager, NetworkProxy networkProxy)
	: ISMSSessionPlugin(coIpManager),
	WengoWebService("en", "wengo", networkProxy, NULL) {
}

WengoSMSSession::~WengoSMSSession() {
}

void WengoSMSSession::start() {
	QMutexLocker lock(_mutex);

	//Setup SMS web service
	Account account = getAccount();
	WengoAccount *wengoAccount = static_cast<WengoAccount *>(account.getPrivateAccount());
	setWengoAccount(wengoAccount);
	setHostname(WengoAccount::getWengoServerHostname());
	setServicePath(WengoAccount::getWengoSMSServicePath());
	usePostMethod(true);
	useHttps(true);
	setPort(443);
	setWengoAuthentication(true);
	////

	//Encode the message
	String encodedMessage = String::encodeUrl(_smsMessage);
	encodedMessage.replace("%2e", ".", false);

	for (StringList::const_iterator it = _phoneNumberList.begin();
		it != _phoneNumberList.end();
		it++) {
		String encodedPhoneNumber = String::encodeUrl(*it);
		setParameters("message=" + encodedMessage + "&target=" + encodedPhoneNumber);

		//Call the web service
		int requestId = call(this);
		_requestIdMap[requestId] = *it;
	}
}

void WengoSMSSession::answerReceived(int requestId, const std::string & answer) {
	QMutexLocker lock(_mutex);

	//TODO: replace this ugly "parsing" with a real XML parsing
	static const std::string STATUS_UNAUTHORIZED = "401";
	static const std::string STATUS_OK = "200";

	std::map<int, std::string>::const_iterator it = _requestIdMap.find(requestId);
	if (it == _requestIdMap.end()) {
		// This is not for this Session.
		LOG_DEBUG("not for this session");
		return;
	}

	String tmp(answer);
	if (!tmp.empty()) {
		if (tmp.contains(STATUS_OK) && !tmp.contains(STATUS_UNAUTHORIZED)) {
			//SMS successfully sent.
			smsStateSignal((*it).second, EnumSMSState::SMSStateOk);
			return;
		}
	}

	//SMS unsent
	smsStateSignal((*it).second, EnumSMSState::SMSStateError);
}
