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

Command::Command(const std::string & configFilesPath) { }
Command::~Command() { }
void Command::start() { }
void Command::setHttpProxySettings(const std::string & hostname, unsigned port,
			const std::string & login, const std::string & password) { }
void Command::terminate() { }
void Command::addWengoAccount(const std::string & login, const std::string & password, bool autoLogin) { }
void Command::addListener(Listener * listener) { }
bool Command::removeListener(Listener * listener) { }
void Command::removeAllListeners() { }
int Command::makeCall(const std::string & phoneNumber, int lineId) { }
void Command::hangUp(int callId) { }
bool Command::canHangUp(int callId) const { }
void Command::pickUp(int callId) { }
bool Command::canPickUp(int callId) const { }
void Command::subscribeToPresenceOf(const std::string & contactId) { }
void Command::publishMyPresence(Listener::PresenceState state, const std::string & note) { }
int Command::sendChatMessage(const std::string & contactId, const std::string & message) { }
void Command::setInputVolume(int volume) { }
int Command::getInputVolume() { }
int Command::getOutputVolume() { }
void Command::setOutputVolume(int volume) { }
