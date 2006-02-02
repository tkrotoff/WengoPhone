/*
 * WengoPhone, a voice over Internet phone
 * Copyright (C) 2004-2005  Wengo
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

#include <Command.h>
#include <Listener.h>

#include <string>
#include <iostream>

void LOG_DEBUG(const std::string & message = "") {
	std::string tmp = "(debug)\t" + std::string(__FUNCTION__) + ": " + message;
	std::cerr << tmp << std::endl;
}


static Listener * _listener = NULL;
static std::string _login;
static std::string _password;

Command::Command(const std::string & configFilesPath) {
	LOG_DEBUG();
}

Command::~Command() {
	LOG_DEBUG();
}

void Command::start() {
	LOG_DEBUG();
	if (_listener) {
		_listener->wengoLoginStateChangedEvent(Listener::LoginOk, _login, _password);
		_listener->phoneLineStateChangedEvent(Listener::LineOk, 1, NULL);
	}
}

void Command::setHttpProxySettings(const std::string & hostname, unsigned port,
			const std::string & login, const std::string & password) {

	LOG_DEBUG();
	_login = login;
	_password = password;
}

void Command::terminate() {
	LOG_DEBUG();
}

void Command::addWengoAccount(const std::string & login, const std::string & password, bool autoLogin) {
	LOG_DEBUG();
}

void Command::addListener(Listener * listener) {
	LOG_DEBUG();
	_listener = listener;
}

bool Command::removeListener(Listener * listener) {
	LOG_DEBUG();
	return true;
}

void Command::removeAllListeners() {
	LOG_DEBUG();
}

int Command::makeCall(const std::string & phoneNumber, int lineId) {
	LOG_DEBUG();
	if (_listener) {
		_listener->phoneCallStateChangedEvent(Listener::CallOk, 1, 1,
					"sip:toto@toto.com", "toto", "toto", NULL);
	}

	return 1;
}

void Command::hangUp(int callId) {
	LOG_DEBUG();
	if (_listener) {
		_listener->phoneCallStateChangedEvent(Listener::CallClosed, 1, 1,
					"sip:toto@toto.com", "toto", "toto", NULL);
	}
}

bool Command::canHangUp(int callId) const {
	LOG_DEBUG();
	return true;
}

void Command::pickUp(int callId) {
	LOG_DEBUG();
}

bool Command::canPickUp(int callId) const {
	LOG_DEBUG();
	return true;
}

void Command::subscribeToPresenceOf(const std::string & contactId) {
	LOG_DEBUG();
}

void Command::publishMyPresence(Listener::PresenceState state, const std::string & note) {
	LOG_DEBUG();
}

int Command::sendChatMessage(const std::string & contactId, const std::string & message) {
	LOG_DEBUG();
	return 1;
}

void Command::setInputVolume(int volume) {
	LOG_DEBUG();
}

int Command::getInputVolume() {
	LOG_DEBUG();
	return 0;
}

int Command::getOutputVolume() {
	LOG_DEBUG();
	return 0;
}

void Command::setOutputVolume(int volume) {
	LOG_DEBUG();
}
