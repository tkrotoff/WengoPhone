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
#include "PhApiWrapper.h"

#include <cutil/global.h>
#include <util/String.h>

PhApiSFPWrapper::PhApiSFPWrapper() {
}

void PhApiSFPWrapper::initialize() {
	// FIXME: we should not have to give a full path to the plugin.
	// PhApi has a plugin path in its config (currently used for codecs),
	// and it should be used for other plugins too.
	std::string prefix = PhApiWrapper::getInstance()->getPluginPath();

#if defined(OS_WINDOWS)
	owplPluginLoad((prefix + "sfp-plugin.dll").c_str());
#else
	owplPluginLoad((prefix + "libsfp-plugin.so").c_str());
#endif
	owplPluginSetParam("11000", 5, "SFPPlugin", "sfp_file_transfer_port");
	owplPluginSetParam("tcp", 3, "SFPPlugin", "sfp_default_ip_protocol");

	PhApiSFPCallbacks::getInstance().setPhApiSFPWrapper(this);	
}

int PhApiSFPWrapper::sendFile(int lineId,
	const std::string & fullIdentity,
	const std::string & contactUri,
	const std::string & filename,
	const std::string & shortFilename,
	const std::string & fileType,
	unsigned fileSize) {

	int retVal = 0;
	owplPluginCallFunction(&retVal, "SFPPlugin", "sfp_send_file", "%d%s%s%s%s%s", lineId,
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

void PhApiSFPWrapper::emitInviteToTransferSignal(int callId, std::string uri,
	std::string fileName, std::string fileType, int fileSize) {

	inviteToTransferSignal(callId, uri, fileName, fileType, fileSize);
}

void PhApiSFPWrapper::emitNewIncomingFileSignal(int callId, std::string contactId,
	std::string fileName, std::string fileType, int fileSize) {

	newIncomingFileSignal(callId, contactId, fileName, fileType, fileSize);
}

void PhApiSFPWrapper::emitWaitingForAnswerSignal(int callId, std::string uri) {
	waitingForAnswerSignal(callId, uri);
}

void PhApiSFPWrapper::emitTransferCancelledSignal(int callId, std::string fileName,
	std::string fileType, int fileSize) {

	transferCancelledSignal(callId, fileName, fileType, fileSize);
}

void PhApiSFPWrapper::emitTransferCancelledByPeerSignal(int callId, std::string contactId,
	std::string fileName, std::string fileType, int fileSize) {

	transferCancelledByPeerSignal(callId, contactId, fileName, fileType, fileSize);
}

void PhApiSFPWrapper::emitSendingFileBeginSignal(int callId, std::string contactId,
	std::string fileName, std::string fileType, int fileSize) {

	sendingFileBeginSignal(callId, contactId, fileName, fileType, fileSize);
}

void PhApiSFPWrapper::emitReceivingFileBeginSignal(int callId, std::string contactId,
	std::string fileName, std::string fileType, int fileSize) {

	receivingFileBeginSignal(callId, contactId, fileName, fileType, fileSize);
}

void PhApiSFPWrapper::emitTransferFromPeerFinishedSignal(int callId, std::string contactId,
	std::string fileName, std::string fileType, int fileSize) {

	transferFromPeerFinishedSignal(callId, contactId, fileName, fileType, fileSize);
}

void PhApiSFPWrapper::emitTransferToPeerFinishedSignal(int callId, std::string contactId,
	std::string fileName, std::string fileType, int fileSize) {

	transferToPeerFinishedSignal(callId, contactId, fileName, fileType, fileSize);
}

void PhApiSFPWrapper::emitTransferFromPeerFailedSignal(int callId, std::string contactId,
	std::string fileName, std::string fileType, int fileSize) {

	transferFromPeerFailedSignal(callId, contactId, fileName, fileType, fileSize);
}

void PhApiSFPWrapper::emitTransferToPeerFailedSignal(int callId, std::string contactId,
	std::string fileName, std::string fileType, int fileSize) {

	transferToPeerFailedSignal(callId, contactId, fileName, fileType, fileSize);
}

void PhApiSFPWrapper::emitTransferProgressionSignal(int callId, int percentage) {
	transferProgressionSignal(callId, percentage);
}

void PhApiSFPWrapper::emitTransferPausedByPeerSignal(int callId, std::string contactId,
	std::string fileName, std::string fileType, int fileSize) {

	transferPausedByPeerSignal(callId, contactId, fileName, fileType, fileSize);
}

void PhApiSFPWrapper::emitTransferPausedSignal(int callId, std::string contactId,
	std::string fileName, std::string fileType, int fileSize) {

	transferPausedSignal(callId, contactId, fileName, fileType, fileSize);
}

void PhApiSFPWrapper::emitTransferResumedByPeerSignal(int callId, std::string contactId,
	std::string fileName, std::string fileType, int fileSize) {

	transferResumedByPeerSignal(callId, contactId, fileName,
		fileType, fileSize);
}

void PhApiSFPWrapper::emitTransferResumedSignal(int callId, std::string contactId,
	std::string fileName, std::string fileType, int fileSize) {
	
	transferResumedSignal(callId, contactId, fileName, fileType, fileSize);
}

void PhApiSFPWrapper::emitPeerNeedsUpgradeSignal(std::string contactId) {
	peerNeedsUpgradeSignal(contactId);
}

void PhApiSFPWrapper::emitNeedsUpgradeSignal() {
	needsUpgradeSignal();
}
