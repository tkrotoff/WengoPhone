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

#include "Sms.h"

#include <model/history/History.h>
#include <model/config/ConfigManager.h>
#include <model/config/Config.h>
#include <model/profile/UserProfile.h>

#include <util/Logger.h>

Sms::Sms(WengoAccount * wengoAccount, UserProfile & userProfile)
	: WengoWebService(wengoAccount), _userProfile(userProfile) {

	Config & config = ConfigManager::getInstance().getCurrentConfig();

	//setup SMS web service
	setHostname(config.getWengoServerHostname());
	setGet(true);
	setHttps(true);
	setServicePath(config.getWengoSMSPath());
	setPort(443);
	setWengoAuthentication(true);
}

int Sms::sendSMS(const std::string & phoneNumber, const std::string & message) {

	//encode the message
	String message2 = String::encodeUrl(message);
	message2.replace("%2e", ".", false);
	
	setParameters("message=" + message2 + "&target=" + phoneNumber);

	//call the web service
	int requestId = call(this);

	//History: create a History Memento for this outgoing SMS
	HistoryMemento * memento = new HistoryMemento(
		HistoryMemento::OutgoingSmsNok, phoneNumber, requestId, message2);
	_userProfile.getHistory().addMemento(memento);
	
	return requestId;
}

void Sms::answerReceived(const std::string & answer, int requestId) {
	//TODO: replace this ugly "parsing" with a real Xml parsing
	static const std::string STATUS_UNAUTHORIZED = "401";
	static const std::string STATUS_OK = "200";

	String tmp(answer);

	if (!tmp.empty()) {
		if (tmp.contains(STATUS_OK) && !tmp.contains(STATUS_UNAUTHORIZED)) {

			//SMS sent
			smsStatusEvent(*this, requestId, SmsStatusOk);

			//History: retrieve the HistoryMemento & update its state to Ok
			_userProfile.getHistory().updateSMSState(requestId, HistoryMemento::OutgoingSmsOk);
			
			return;
		}
	}

	//SMS unsent
	smsStatusEvent(*this, requestId, SmsStatusError);

	//History: retrieve the HistoryMemento & update its state to Nok
	_userProfile.getHistory().updateSMSState(requestId, HistoryMemento::OutgoingSmsNok);
}
