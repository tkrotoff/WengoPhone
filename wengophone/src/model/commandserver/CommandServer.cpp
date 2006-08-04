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

#include <model/WengoPhone.h>
#include <model/phoneline/IPhoneLine.h>
#include <model/profile/UserProfileHandler.h>
#include <model/profile/UserProfile.h>
#include <model/config/ConfigManager.h>
#include <model/config/Config.h>

#include <util/Logger.h>

CommandServer * CommandServer::_commandServerInstance = NULL;
const std::string CommandServer::_queryStatus = "1|s";
const std::string CommandServer::_queryCall = "1|o|call/";
const std::string CommandServer::_querySms = "1|o|sms/";

CommandServer::CommandServer(WengoPhone & wengoPhone)
	: _wengoPhone(wengoPhone) {

	_serverSocket = new OWServerSocket("127.0.0.1", _port);
	_serverSocket->serverStatusEvent += boost::bind(&CommandServer::serverStatusEventHandler, this, _1, _2);
	_serverSocket->connectionEvent += boost::bind(&CommandServer::connectionEventHandler, this, _1, _2);
	_serverSocket->incomingRequestEvent += boost::bind(&CommandServer::incomingRequestEventHandler, this, _1, _2, _3);
	_serverSocket->writeStatusEvent += boost::bind(&CommandServer::writeStatusEventHandler, this, _1, _2, _3);
	_serverSocket->init();
}

CommandServer::~CommandServer() {
	delete _serverSocket;
}

CommandServer & CommandServer::getInstance(WengoPhone & wengoPhone) {
	if (!_commandServerInstance) {
		_commandServerInstance = new CommandServer(wengoPhone);
	}

	return *_commandServerInstance;
}

void CommandServer::serverStatusEventHandler(OWServerSocket * sender, OWServerSocket::Error error) {
	if (error == OWServerSocket::NoError) {
		LOG_DEBUG("CommandServer: connected");
	} else {
		LOG_WARN("CommandServer: not connected");
	}
}

void CommandServer::connectionEventHandler(OWServerSocket * sender, const std::string & connectionId) {
	LOG_DEBUG("CommandServer: client connection: " + connectionId);
}

void CommandServer::incomingRequestEventHandler(OWServerSocket * sender, const std::string & connectionId, const std::string & data) {
	LOG_DEBUG("CommandServer: incoming request, connectionId: " + connectionId + " data: " + data);
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
			LOG_DEBUG("call peer: " + l[1]);
			UserProfile * userprofile = _wengoPhone.getUserProfileHandler().getCurrentUserProfile();
			if (userprofile) {
				IPhoneLine * phoneLine = userprofile->getActivePhoneLine();
				if (phoneLine && phoneLine->isConnected()) {
					phoneLine->makeCall(l[1]);
					_serverSocket->writeToClient(connectionId, data + "|1");
				}
			}
		}
		_serverSocket->writeToClient(connectionId, data + "|0");

	} else if (query.contains(_querySms)) {
		LOG_WARN("not yet implemented");
	} else {

		Config & config = ConfigManager::getInstance().getCurrentConfig();

		//"emulate" a http server. Needed for Flash sockets
		_serverSocket->writeToClient(connectionId,
			buildHttpForFlash(
				"<?xml version=\"1.0\"?>\n"
				"<!DOCTYPE cross-domain-policy SYSTEM \"http://www.macromedia.com/xml/dtds/cross-domain-policy.dtd\">\n"
				"<cross-domain-policy>\n"
				"<allow-access-from domain=\"" + config.getCmdServerAuthorized() + "\" to-ports=\"*\" />\n"
				"<allow-access-from domain=\"localhost\" to-ports=\"*\" />\n"
				"</cross-domain-policy>"
			)
		);
	}
}

void CommandServer::writeStatusEventHandler(OWServerSocket * sender, const std::string & writeId, OWServerSocket::Error error) {
	if (error == OWServerSocket::NoError) {
		LOG_DEBUG("CommandServer: writeId: " + writeId + ", write success");
	} else {
		LOG_WARN("CommandServer: writeId: " + writeId + ", write failed");
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
