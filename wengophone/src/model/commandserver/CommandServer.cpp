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

#include "CommandServer.h"

#include "ContactInfo.h"

#include <model/chat/ChatHandler.h>
#include <model/config/ConfigManager.h>
#include <model/config/Config.h>
#include <model/phoneline/IPhoneLine.h>
#include <model/profile/UserProfile.h>
#include <model/profile/UserProfileHandler.h>
#include <model/WengoPhone.h>

#include <imwrapper/EnumIMProtocol.h>
#include <imwrapper/IMAccountHandler.h>
#include <imwrapper/IMContact.h>
#include <imwrapper/IMContactSet.h>

#include <util/Logger.h>
#include <util/SafeDelete.h>

CommandServer * CommandServer::_commandServerInstance = NULL;
const std::string CommandServer::_queryStatus = "1|s";
const std::string CommandServer::_queryCall = "1|o|call/";
const std::string CommandServer::_querySms = "1|o|sms/";
const std::string CommandServer::_queryChat = "1|o|chat/";
const std::string CommandServer::_queryAddContact = "1|o|addc/";

const std::string NICKNAME_STR = "pseudo=";
const std::string SIP_STR = "sip=";
const std::string FIRSTNAME_STR = "firstname=";
const std::string LASTNAME_STR = "lastname=";
const std::string COUNTRY_STR = "country=";
const std::string CITY_STR = "city=";
const std::string STATE_STR = "state=";
const std::string GROUP_STR = "group=";
const std::string WDEALSERVICETITLE_STR = "title=";
const std::string URL_STR = "url=";

CommandServer::CommandServer(WengoPhone & wengoPhone)
	: _wengoPhone(wengoPhone) {

	_serverSocket = new ServerSocket("127.0.0.1", _port);
	_serverSocket->serverStatusEvent += boost::bind(&CommandServer::serverStatusEventHandler, this, _1, _2);
	_serverSocket->connectionEvent += boost::bind(&CommandServer::connectionEventHandler, this, _1, _2);
	_serverSocket->incomingRequestEvent += boost::bind(&CommandServer::incomingRequestEventHandler, this, _1, _2, _3);
	_serverSocket->writeStatusEvent += boost::bind(&CommandServer::writeStatusEventHandler, this, _1, _2, _3);
	_serverSocket->init();
}

CommandServer::~CommandServer() {
	OWSAFE_DELETE(_serverSocket);
}

CommandServer & CommandServer::getInstance(WengoPhone & wengoPhone) {
	if (!_commandServerInstance) {
		_commandServerInstance = new CommandServer(wengoPhone);
	}

	return *_commandServerInstance;
}

void CommandServer::serverStatusEventHandler(ServerSocket & sender, ServerSocket::Error error) {
	if (error == ServerSocket::NoError) {
		LOG_DEBUG("connected");
	} else {
		LOG_WARN("not connected");
	}
}

void CommandServer::connectionEventHandler(ServerSocket & sender, const std::string & connectionId) {
	LOG_DEBUG("client connection=" + connectionId);
}

void CommandServer::incomingRequestEventHandler(ServerSocket & sender, const std::string & connectionId, const std::string & data) {
	LOG_DEBUG("incoming request connectionId=" + connectionId + " data=" + data);
	String query = String(data);
	if (query == _queryStatus) {

		//Find the phoneline status and answer
		UserProfile * userprofile = _wengoPhone.getUserProfileHandler().getCurrentUserProfile();
		if (userprofile) {
			IPhoneLine * phoneLine = userprofile->getActivePhoneLine();
			if (phoneLine && phoneLine->isConnected()) {
				_serverSocket->writeToClient(connectionId, _queryStatus + "|1");
			} else {
				_serverSocket->writeToClient(connectionId, _queryStatus + "|0");
			}
		}

	} else if (query.contains(_queryCall)) {

		//Extract the number from query & place the call
		StringList l = query.split("/");
		if (l.size() == 2) {
			LOG_DEBUG("call peer=" + l[1]);
			UserProfile * userprofile = _wengoPhone.getUserProfileHandler().getCurrentUserProfile();
			if (userprofile) {
				IPhoneLine * phoneLine = userprofile->getActivePhoneLine();
				if (phoneLine && phoneLine->isConnected()) {
					phoneLine->makeCall(l[1]);
					_serverSocket->writeToClient(connectionId, data + "|1");
					return;
				}
			}
		}
		_serverSocket->writeToClient(connectionId, data + "|0");

	} else if (query.contains(_querySms)) {

		LOG_WARN("not yet implemented");

	} else if (query.contains(_queryChat)) {

		UserProfile * userProfile = _wengoPhone.getUserProfileHandler().getCurrentUserProfile();
		if (userProfile) {
			IPhoneLine * phoneLine = userProfile->getActivePhoneLine();
			if (phoneLine && phoneLine->isConnected()) {

				// extract the nickname from 1|o|chat/pseudo=value&sip=value
				StringList l = query.split("/");
				std::string nickname;
				if (l.size() == 2) {
					int sepPos = l[1].find("&");
					nickname = l[1].substr(7, sepPos - 7);
				}
				////

				// get THE Wengo account
				IMAccountHandler imAccountHandler(userProfile->getIMAccountHandler());
				std::set<IMAccount *> imAccountSet = imAccountHandler.getIMAccountsOfProtocol(EnumIMProtocol::IMProtocolWengo);
				////

				// create the IMContactSet
				IMAccount * imAccount = *imAccountSet.begin();
				IMContact imContact(*imAccount, nickname);
				IMContactSet imContactSet;
				imContactSet.insert(imContact);
				////

				// create the chat session
				ChatHandler & chatHandler = userProfile->getChatHandler();
				chatHandler.createSession(*imAccount, imContactSet);
				////
			}
		}
		// failed
		_serverSocket->writeToClient(connectionId, _queryChat + "|0");

	} else if (query.contains(_queryAddContact)) {

		UserProfile * userProfile = _wengoPhone.getUserProfileHandler().getCurrentUserProfile();
		if (userProfile) {

			// extract the information
			StringList l = query.split("/");

			ContactInfo contactInfo;

			if (l.size() == 2) {
				String infos = String(l[1]);
				StringList contactInfoList = infos.split("&");

				for (unsigned i = 0; i < contactInfoList.size(); i++) {

					String info = contactInfoList[i];

					// TODO: remove name= when the new protocol will be respected
					if (info.beginsWith("name=")) {
						contactInfo.wengoName = info.substr(5, info.size() - 5);
					} else if (info.beginsWith(NICKNAME_STR)) {
						contactInfo.wengoName = info.substr(NICKNAME_STR.size(), info.size() - NICKNAME_STR.size());
					} else if (info.beginsWith(SIP_STR)) {
						contactInfo.sip = info.substr(SIP_STR.size(), info.size() - SIP_STR.size());
					} else if (info.beginsWith(FIRSTNAME_STR)) {
						contactInfo.firstname = info.substr(FIRSTNAME_STR.size(), info.size() - FIRSTNAME_STR.size());
					} else if (info.beginsWith(LASTNAME_STR)) {
						contactInfo.lastname = info.substr(LASTNAME_STR.size(), info.size() - LASTNAME_STR.size());
					} else if (info.beginsWith(COUNTRY_STR)) {
						contactInfo.country = info.substr(COUNTRY_STR.size(), info.size() - COUNTRY_STR.size());
					} else if (info.beginsWith(CITY_STR)) {
						contactInfo.city = info.substr(CITY_STR.size(), info.size() - CITY_STR.size());
					} else if (info.beginsWith(STATE_STR)) {
						contactInfo.state = info.substr(STATE_STR.size(), info.size() - STATE_STR.size());
					} else if (info.beginsWith(GROUP_STR)) {
						contactInfo.group = info.substr(GROUP_STR.size(), info.size() - GROUP_STR.size());
					} else if (info.beginsWith(WDEALSERVICETITLE_STR)) {
						contactInfo.wdealServiceTitle = info.substr(WDEALSERVICETITLE_STR.size(), info.size() - WDEALSERVICETITLE_STR.size());
					} else if (info.beginsWith(URL_STR)) {
						contactInfo.website = info.substr(URL_STR.size(), info.size() - URL_STR.size());
					}
				}
			}
			////

			showAddContactEvent(contactInfo);
		}

	} else {

		Config & config = ConfigManager::getInstance().getCurrentConfig();

		//"emulate" a http server. Needed for Flash sockets
		std::string temp = "<?xml version=\"1.0\"?>\n"
			"<!DOCTYPE cross-domain-policy SYSTEM \"http://www.macromedia.com/xml/dtds/cross-domain-policy.dtd\">\n"
			"<cross-domain-policy>\n"
			"<allow-access-from domain=\"button.wdeal.com\" to-ports=\"*\" />\n"
			"<allow-access-from domain=\"button.wengo.com\" to-ports=\"*\" />\n";
		if (!config.getCmdServerAuthorized().empty()) {
			temp += "<allow-access-from domain=\"" + config.getCmdServerAuthorized() + "\" to-ports=\"*\" />\n";
		}
		temp += "<allow-access-from domain=\"localhost\" to-ports=\"*\" />\n"
				"</cross-domain-policy>";
		_serverSocket->writeToClient(connectionId, buildHttpForFlash(temp));
	}
}

void CommandServer::writeStatusEventHandler(ServerSocket & sender, const std::string & writeId, ServerSocket::Error error) {
	if (error == ServerSocket::NoError) {
		LOG_DEBUG("writeId=" + writeId + " write success");
	} else {
		LOG_WARN("writeId=" + writeId + " write failed");
	}
}

std::string CommandServer::buildHttpForFlash(const std::string & xml) {

	std::string httpHeader =
		"HTTP/1.1 200 OK\n"
		"Date: Thu, 29 Jun 2006 11:02:50 GMT\n"
		"Server: Apache/2.0.46 (CentOS)\n"
		"Last-Modified: Thu, 29 Jun 2006 10:41:12 GMT\n"
		"Accept-Ranges: bytes\n"
		"Connection: close\n"
		"Content-Type: text/xml";

	//Add the xml Content-Length and the header separator
	httpHeader += "Content-Length: " + String::fromNumber(xml.size()) + "\n\n";

	return httpHeader + xml;
}
