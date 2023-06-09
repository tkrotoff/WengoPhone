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

#include <owpl_plugin.h>

#include <util/String.h>

//FIXME This line is not the first one otherwise it does not compile under Visual C++ 8.0
//do not know why
#include "PhApiSFPWrapper.h"

PhApiSFPWrapper::PhApiSFPWrapper() {
}

PhApiSFPWrapper::~PhApiSFPWrapper() {
}

int PhApiSFPWrapper::sendFile(int vlineID,
	const std::string & fullIdentity,
	const std::string & contactUri,
	const std::string & filename,
	const std::string & shortFilename,
	const std::string & fileType,
	unsigned fileSize) {
	int retVal = 0;
	owplPluginCallFunction(&retVal, "SFPPlugin", "sfp_send_file", "%d%s%s%s%s%s", vlineID,
		(char *) contactUri.c_str(),
		(char *) filename.c_str(),
		(char *) shortFilename.c_str(),
		(char *) fileType.c_str(),
		(char *) String::fromNumber(fileSize).c_str());
	return retVal;
}

int PhApiSFPWrapper::receiveFile(int callId, const std::string & filename) {
	int retVal = 0;
	return owplPluginCallFunction(&retVal, "SFPPlugin", "sfp_receive_file", "%d%s", callId, (char *) filename.c_str());
}

int PhApiSFPWrapper::cancelTransfer(int callId) {
	int retVal = 0;
	return owplPluginCallFunction(&retVal, "SFPPlugin", "sfp_cancel_transfer", "%d", callId);
}

int PhApiSFPWrapper::pauseTransfer(int callId) {
	int retVal = 0;
	return owplPluginCallFunction(&retVal, "SFPPlugin", "sfp_pause_transfer", "%d", callId);
}

int PhApiSFPWrapper::resumeTransfer(int callId) {
	int retVal = 0;
	return owplPluginCallFunction(&retVal, "SFPPlugin", "sfp_resume_transfer", "%d", callId);
}

void PhApiSFPWrapper::setBasePort(unsigned basePort) {
	const char * port = String::fromNumber(basePort).c_str();
	owplPluginSetParam(port, strlen(port), "SFPPlugin", "sfp_file_transfer_port");
}
