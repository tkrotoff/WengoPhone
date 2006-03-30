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

#include <util/Logger.h>

std::string hostname = "ws.wengo.fr";
std::string servicePath = "/sms/sendsms.php";
int port = 443;
bool https = true;
bool get = true;

Sms::Sms(WengoAccount & wengoAccount) : WengoWebService(wengoAccount) {
	//setup sms web service
	setHostname(hostname);
	setGet(get);
	setHttps(https);
	setServicePath(servicePath);
	setPort(port);
	setWengoAuthentication(true);
}

int Sms::sendSMS(const std::string & phoneNumber, const std::string & message) {
	setParameters("message=" + message + "&target=" + phoneNumber);
	
	//encode the message
	String message2 = String::encodeUrl(message);
	message2.replace("%2e", ".", false);
	
	//call the web service
	int id = call(this);

	//History: create a History Memento for this outgoing SMS
	HistoryMemento * memento = new HistoryMemento(
		HistoryMemento::OutgoingSmsNok, phoneNumber, id, message2);
	History::getInstance().addMemento(memento);
}

void Sms::answerReceived(std::string answer, int id) {
	//TODO: replace this ugly "parsing" with a real Xml parsing
	static const std::string STATUS_UNAUTHORIZED = "401";
	static const std::string STATUS_OK = "200";
	
	String tmp(answer);

	if (!tmp.empty()) {
		if (tmp.contains(STATUS_OK) && !tmp.contains(STATUS_UNAUTHORIZED)) {
			
			//smssent
			smsStatusEvent(*this, id, SmsStatusOk);
			
			//History: retrieve the HistoryMemento & update its state to Ok
			History::getInstance().updateSMSState(id, HistoryMemento::OutgoingSmsOk);
			return;
		}
	}
	
	//sms unsent
	smsStatusEvent(*this, id, SmsStatusError);
	
	//History: retrieve the HistoryMemento & update its state to Nok
	History::getInstance().updateSMSState(id, HistoryMemento::OutgoingSmsNok);
}
