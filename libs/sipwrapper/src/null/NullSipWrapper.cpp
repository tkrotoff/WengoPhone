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

#include "NullSipWrapper.h"

#include <Thread.h>
#include <StringList.h>
#include <Logger.h>

NullSipWrapper::NullSipWrapper() {
	LOG_DEBUG("NullSip initialization");
}

NullSipWrapper::~NullSipWrapper() { }

void NullSipWrapper::init() {
}

void NullSipWrapper::terminate() {
}

int NullSipWrapper::addVirtualLine(const std::string &,
	const std::string &,
	const std::string &,
	const std::string &,
	const std::string &,
	const std::string &,
	const std::string &) {

	return 1;
}

void NullSipWrapper::removeVirtualLine(int) {
}

int NullSipWrapper::makeCall(int lineId, const std::string & phoneNumber) {
	LOG_DEBUG("call=" + phoneNumber);
	return 1;
}

void NullSipWrapper::sendRingingNotification(int) {
}

void NullSipWrapper::acceptCall(int) {
}

void NullSipWrapper::rejectCall(int) {
}

void NullSipWrapper::closeCall(int) {
}

void NullSipWrapper::holdCall(int) {
}

void NullSipWrapper::resumeCall(int) {
}

void NullSipWrapper::playTone(int callId, EnumTone::Tone tone) {
}

void NullSipWrapper::playSoundFile(int callId, const std::string & soundFile) {
}

void NullSipWrapper::setProxy(const std::string & address, unsigned port,
	const std::string & login, const std::string & password) {
}

void NullSipWrapper::setTunnel(const std::string & address, unsigned port, bool ssl) {
}

void NullSipWrapper::setNatType(EnumNatType::NatType natType) {
}

void NullSipWrapper::setSIP(const std::string & server, unsigned serverPort, unsigned localPort) {
}

bool NullSipWrapper::setCallInputAudioDevice(const std::string &) {
	return false;
}

bool NullSipWrapper::setRingerOutputAudioDevice(const std::string &) {
	return false;
}

bool NullSipWrapper::setCallOutputAudioDevice(const std::string &) {
	return false;
}

void NullSipWrapper::enableAEC(bool enable) {
}

void NullSipWrapper::enableHalfDuplex(bool enable) {
}

void NullSipWrapper::setPluginPath(const std::string & path) {
}
