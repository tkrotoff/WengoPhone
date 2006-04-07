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

#include "Subscribe.h"

#include <model/config/ConfigManager.h>
#include <model/config/Config.h>

#include <util/Logger.h>

#include <tinyxml.h>

WsWengoSubscribe::WsWengoSubscribe(WengoAccount & wengoAccount) : WengoWebService(wengoAccount) {
	
	Config & config = ConfigManager::getInstance().getCurrentConfig();
	
	//setup subscribe web service
	setHostname(config.getWengoServerHostname());
	setGet(true);
	setHttps(true);
	setServicePath(config.getWengoSubscribePath());
	setPort(80);
	setWengoAuthentication(false);
}

int WsWengoSubscribe::subscribe(const std::string & email, const std::string & nickname,
	const std::string & lang, const std::string & password) {
	
	std::string query = "action=activate";

	query += "&email=" + email;
	query += "&pseudo=" + nickname;
	query += "&lang=" + lang;
	query += "&password=" + password;

	setParameters(query);
	
	return call(this);
}

void WsWengoSubscribe::answerReceived(const std::string & answer, int id) {

	std::string statusCode = "";
	std::string statusMessage = "";
	std::string password = "";
	SubscriptionStatus status = SubscriptionUnknownError;

	TiXmlDocument doc;
	doc.Parse(answer.c_str());
	
	TiXmlHandle docHandle(& doc);
	TiXmlHandle response = docHandle.FirstChild("response");
	
	//retrieve the version of the ws
	TiXmlText * text = response.FirstChild("answer").FirstChild("version").FirstChild().Text();
	if( text ) {
	}
	
	//retrieve the status code
	text = response.FirstChild("answer").FirstChild("status").FirstChild("code").FirstChild().Text();
	if( text ) {
		LOG_DEBUG("\n status code:" + std::string(text->Value()));
		statusCode = std::string(text->Value());
		if( statusCode == "OK" ) {
			status = SubscriptioOk;
		} else if( statusCode == "BAD_ACTIVATE" ) {
			status = SubscriptionFailed;
		} else if( statusCode == "UNIQUE_PSEUDO" ) {
			status = SubscriptionNicknameError;
		} else if( statusCode == "UNIQUE_EMAIL" ) {
			status = SubscriptionMailError;
		} else if( statusCode == "ERROR" ) {
			status = SubscriptionUnknownError;
		} else if( statusCode == "BAD_VERSION" ) {
			status = SubscriptionBadVersion;
		} else if( statusCode == "BAD_QUERY" ) {
			status = SubscriptionBadQuery;
		} else {
			wengoSubscriptionEvent(*this, id, SubscriptionUnknownError, "", "");
			return;
		}
	}
	
	//retrieve the status message
	text = response.FirstChild("answer").FirstChild("status").FirstChild("message").FirstChild().Text();
	if( text ) {
		statusMessage = std::string(text->Value());
	}
	
	//retrieve the password
	text = response.FirstChild("answer").FirstChild("data").FirstChild("password").FirstChild().Text();
	if( text ) {
		password = std::string(text->Value());
	}
	
	wengoSubscriptionEvent(*this, id, status, statusMessage, password);
}
