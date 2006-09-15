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

#include "PhApiSFPWrapper.h"
#include "PhApiSFPCallbacks.h"

#include <sfp-plugin/sfp-plugin.h> // TODO remove once we have a real plugin architecture

#include <util/String.h>

PhApiSFPWrapper::PhApiSFPWrapper(){
	sfp_register_plugin(); // TODO remove once we have a real plugin architecture
	sfp_set_file_transfer_port("11000"); // TODO remove once we have a real plugin architecture
	setCallbacks(PhApiSFPCallbacks::getCallbacks());
}

PhApiSFPWrapper::~PhApiSFPWrapper(){}

int PhApiSFPWrapper::sendFile(int vlineID, std::string fullIdentity, std::string contactUri, std::string filename, std::string shortFilename, std::string fileType, unsigned int fileSize){
	return sfp_send_file(vlineID, (void *)fullIdentity.c_str(), (char *)contactUri.c_str(), (char *)filename.c_str(), (char *)shortFilename.c_str(), (char *)fileType.c_str(), (char *)String::fromNumber(fileSize).c_str());
}

int PhApiSFPWrapper::receiveFile(int callId, std::string filename){
	return sfp_receive_file(callId, (char *)filename.c_str());
}

int PhApiSFPWrapper::cancelTransfer(int callId){
	return sfp_cancel_transfer(callId);
}

int PhApiSFPWrapper::pauseTransfer(int callId){
	return sfp_pause_transfer(callId);
}

int PhApiSFPWrapper::resumeTransfer(int callId){
	return sfp_resume_transfer(callId);
}

void PhApiSFPWrapper::setBasePort(const unsigned int basePort){
	sfp_set_file_transfer_port(String::fromNumber(basePort).c_str());
}

void PhApiSFPWrapper::setCallbacks(const sfp_callbacks_t * callbacks){
	sfp_set_plugin_callbacks(callbacks);
}