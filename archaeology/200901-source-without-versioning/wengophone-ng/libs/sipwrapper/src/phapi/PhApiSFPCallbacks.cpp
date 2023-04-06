/*
 * WengoPhone, a voice over Internet phone
 * Copyright (C) 2004-2007  Wengo
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

#include "PhApiSFPCallbacks.h"

#include "PhApiSFPWrapper.h"

#include <phapi-util/util.h>

#include <cutil/global.h>
#include <util/String.h>

extern "C" {

	static void inviteToTransferHandler(int callId, char * uri, char * fileName, char * fileType, char * fileSize) {
		if (strfilled(uri) && strfilled(fileName) && strfilled(fileSize)) {
			PhApiSFPCallbacks::getInstance().inviteToTransfer(callId,
				std::string((const char *) uri),
				std::string((const char *) fileName),
				std::string((const char *) fileType),
				String(fileSize).toInteger());
		}
	}

	static void newIncomingFileHandler(int callId, char * username, char * fileName, char * fileType, char * fileSize) {
		if (strfilled(username) && strfilled(fileName) && strfilled(fileSize)) {
			PhApiSFPCallbacks::getInstance().newIncomingFile(callId,
				std::string((const char *) username),
				std::string((const char *) fileName),
				std::string((const char *) fileType),
				String(fileSize).toInteger());
		}
	}

	static void waitingForAnswerHandler(int callId, char * uri) {
		if (strfilled(uri)) {
			PhApiSFPCallbacks::getInstance().waitingForAnswer(callId, std::string((const char *) uri));
		}
	}

	static void transferCancelledHandler(int callId, char * fileName, char * fileType, char * fileSize) {
		if (strfilled(fileName) && strfilled(fileSize)) {
			PhApiSFPCallbacks::getInstance().transferCancelled(callId,
				std::string((const char *) fileName),
				std::string((const char *) fileType),
				String(fileSize).toInteger());
		}
	}

	static void transferCancelledByPeerHandler(int callId, char * username, char * fileName, char * fileType, char * fileSize) {
		if (strfilled(username) && strfilled(fileName) && strfilled(fileSize)) {
			PhApiSFPCallbacks::getInstance().transferCancelledByPeer(callId,
				std::string((const char *) username),
				std::string((const char *) fileName),
				std::string((const char *) fileType),
				String(fileSize).toInteger());
		}
	}

	static void sendingFileBeginHandler(int callId, char * username, char * fileName, char * fileType, char * fileSize) {
		if (strfilled(username) && strfilled(fileName) && strfilled(fileSize)) {
			PhApiSFPCallbacks::getInstance().sendingFileBegin(callId,
				std::string((const char *) username),
				std::string((const char *) fileName),
				std::string((const char *) fileType),
				String(fileSize).toInteger());
		}
	}

	static void receivingFileBeginHandler(int callId, char * username, char * fileName, char * fileType, char * fileSize) {
		if (strfilled(username) && strfilled(fileName) && strfilled(fileSize)) {
			PhApiSFPCallbacks::getInstance().receivingFileBegin(callId,
				std::string((const char *) username),
				std::string((const char *) fileName),
				std::string((const char *) fileType),
				String(fileSize).toInteger());
		}
	}

	static void transferClosedHandler(int callId, char * username, char * fileName, char * fileType, char * fileSize) {
	}

	static void transferClosedByPeerHandler(int callId, char * username, char * fileName, char * fileType, char * fileSize) {
	}

	static void transferFromPeerFinishedHandler(int callId, char * username, char * fileName, char * fileType, char * fileSize) {
		if (strfilled(username) && strfilled(fileName) && strfilled(fileSize)) {
			PhApiSFPCallbacks::getInstance().transferFromPeerFinished(callId,
				std::string((const char *) username),
				std::string((const char *) fileName),
				std::string((const char *) fileType),
				String(fileSize).toInteger());
		}
	}

	static void transferToPeerFinishedHandler(int callId, char * username, char * fileName, char * fileType, char * fileSize) {
		if (strfilled(username) && strfilled(fileName) && strfilled(fileSize)) {
			PhApiSFPCallbacks::getInstance().transferToPeerFinished(callId,
				std::string((const char *) username),
				std::string((const char *) fileName),
				std::string((const char *) fileType),
				String(fileSize).toInteger());
		}
	}

	static void transferFromPeerFailedHandler(int callId, char * username, char * fileName, char * fileType, char * fileSize) {
		if (strfilled(username) && strfilled(fileName) && strfilled(fileSize)) {
			PhApiSFPCallbacks::getInstance().transferFromPeerFailed(callId,
				std::string((const char *) username),
				std::string((const char *) fileName),
				std::string((const char *) fileType),
				String(fileSize).toInteger());
		}
	}

	static void transferToPeerFailedHandler(int callId, char * username, char * fileName, char * fileType, char * fileSize) {
		if (strfilled(username) && strfilled(fileName) && strfilled(fileSize)) {
			PhApiSFPCallbacks::getInstance().transferToPeerFailed(callId,
				std::string((const char *) username),
				std::string((const char *) fileName),
				std::string((const char *) fileType),
				String(fileSize).toInteger());
		}
	}

	static void transferFromPeerStoppedHandler(int callId, char * username, char * fileName, char * fileType, char * fileSize) {
		if (strfilled(username) && strfilled(fileName) && strfilled(fileSize)) {
			PhApiSFPCallbacks::getInstance().transferFromPeerFailed(callId,
				std::string((const char *) username),
				std::string((const char *) fileName),
				std::string((const char *) fileType),
				String(fileSize).toInteger());
		}
	}

	static void transferToPeerStoppedHandler(int callId, char * username, char * fileName, char * fileType, char * fileSize) {
		if (strfilled(username) && strfilled(fileName) && strfilled(fileSize)) {
			PhApiSFPCallbacks::getInstance().transferToPeerFailed(callId,
				std::string((const char *) username),
				std::string((const char *) fileName),
				std::string((const char *) fileType),
				String(fileSize).toInteger());
		}
	}

	static void transferProgressionHandler(int callId, int percentage) {
		PhApiSFPCallbacks::getInstance().transferProgression(callId, percentage);
	}

	static void transferPausedByPeerHandler(int callId, char * username, char * fileName, char * fileType, char * fileSize) {
		if (strfilled(username) && strfilled(fileName) && strfilled(fileSize)) {
			PhApiSFPCallbacks::getInstance().transferPausedByPeer(callId,
				std::string((const char *) username),
				std::string((const char *) fileName),
				std::string((const char *) fileType),
				String(fileSize).toInteger());
		}
	}

	static void transferPausedHandler(int callId, char * username, char * fileName, char * fileType, char * fileSize) {
		if (strfilled(username) && strfilled(fileName) && strfilled(fileSize)) {
			PhApiSFPCallbacks::getInstance().transferPaused(callId,
				std::string((const char *) username),
				std::string((const char *) fileName),
				std::string((const char *) fileType),
				String(fileSize).toInteger());
		}
	}

	static void transferResumedByPeerHandler(int callId, char * username, char * fileName, char * fileType, char * fileSize) {
		if (strfilled(username) && strfilled(fileName) && strfilled(fileSize)) {
			PhApiSFPCallbacks::getInstance().transferResumedByPeer(callId,
				std::string((const char *) username),
				std::string((const char *) fileName),
				std::string((const char *) fileType),
				String(fileSize).toInteger());
		}
	}

	static void transferResumedHandler(int callId, char * username, char * fileName, char * fileType, char * fileSize) {
		if (strfilled(username) && strfilled(fileName) && strfilled(fileSize)) {
			PhApiSFPCallbacks::getInstance().transferResumed(callId,
				std::string((const char *) username),
				std::string((const char *) fileName),
				std::string((const char *) fileType),
				String(fileSize).toInteger());
		}
	}

	static void peerNeedsUpgradeHandler(const char * username) {
		if (strfilled(username)) {
			PhApiSFPCallbacks::getInstance().peerNeedsUpgrade(std::string(username));
		}
	}

	static void needUpgradeHandler() {
		PhApiSFPCallbacks::getInstance().needUpgrade();
	}
}

PhApiSFPCallbacks::PhApiSFPCallbacks() {
	_phApiSFPWrapper = NULL;
	setCallbacks();
}

void PhApiSFPCallbacks::setPhApiSFPWrapper(PhApiSFPWrapper * phApiSFPWrapper) {
	_phApiSFPWrapper = phApiSFPWrapper;
}

void PhApiSFPCallbacks::inviteToTransfer(int callId,
	const std::string & uri,
	const std::string & fileName,
	const std::string & fileType,
	int fileSize) {

	_phApiSFPWrapper->emitInviteToTransferSignal(callId,
		uri, fileName, fileType, fileSize);
}

void PhApiSFPCallbacks::newIncomingFile(int callId,
	const std::string & contactId,
	const std::string & fileName,
	const std::string & fileType,
	int fileSize) {

#ifdef OS_WINDOWS
	_phApiSFPWrapper->emitNewIncomingFileSignal(callId, contactId,
		PhApiSFPCallbacks::replaceOddCharacters(fileName), fileType, fileSize);
#else
	_phApiSFPWrapper->emitNewIncomingFileSignal(callId, contactId,
		fileName, fileType, fileSize);
#endif
}

void PhApiSFPCallbacks::waitingForAnswer(int callId, const std::string & uri) {
	_phApiSFPWrapper->emitWaitingForAnswerSignal(callId, uri);
}

void PhApiSFPCallbacks::transferCancelled(int callId,
	const std::string & fileName,
	const std::string & fileType,
	int fileSize) {

	_phApiSFPWrapper->emitTransferCancelledSignal(callId, fileName,
		fileType, fileSize);
}

void PhApiSFPCallbacks::transferCancelledByPeer(int callId,
	const std::string & contactId,
	const std::string & fileName,
	const std::string & fileType,
	int fileSize) {

	_phApiSFPWrapper->emitTransferCancelledByPeerSignal(callId, contactId,
		fileName, fileType, fileSize);
}

void PhApiSFPCallbacks::sendingFileBegin(int callId,
	const std::string & contactId,
	const std::string & fileName,
	const std::string & fileType,
	int fileSize) {

	_phApiSFPWrapper->emitSendingFileBeginSignal(callId, contactId,
		fileName, fileType, fileSize);
}

void PhApiSFPCallbacks::receivingFileBegin(int callId,
	const std::string & contactId,
	const std::string & fileName,
	const std::string & fileType,
	int fileSize) {

	_phApiSFPWrapper->emitReceivingFileBeginSignal(callId, contactId,
		fileName, fileType, fileSize);
}

void PhApiSFPCallbacks::transferFromPeerFinished(int callId,
	const std::string & contactId,
	const std::string & fileName,
	const std::string & fileType,
	int fileSize) {

	_phApiSFPWrapper->emitTransferFromPeerFinishedSignal(callId, contactId,
		fileName, fileType, fileSize);
}

void PhApiSFPCallbacks::transferToPeerFinished(int callId,
	const std::string & contactId,
	const std::string & fileName,
	const std::string & fileType,
	int fileSize) {

	_phApiSFPWrapper->emitTransferToPeerFinishedSignal(callId, contactId,
		fileName, fileType, fileSize);
}

void PhApiSFPCallbacks::transferFromPeerFailed(int callId,
	const std::string & contactId,
	const std::string & fileName,
	const std::string & fileType,
	int fileSize) {

	_phApiSFPWrapper->emitTransferFromPeerFailedSignal(callId, contactId,
		fileName, fileType, fileSize);
}

void PhApiSFPCallbacks::transferToPeerFailed(int callId,
	const std::string & contactId,
	const std::string & fileName,
	const std::string & fileType,
	int fileSize) {

	_phApiSFPWrapper->emitTransferToPeerFailedSignal(callId, contactId,
		fileName, fileType, fileSize);
}

void PhApiSFPCallbacks::transferProgression(int callId, int percentage) {
	_phApiSFPWrapper->emitTransferProgressionSignal(callId, percentage);
}

void PhApiSFPCallbacks::transferPausedByPeer(int callId,
	const std::string & contactId,
	const std::string & fileName,
	const std::string & fileType,
	int fileSize) {

	_phApiSFPWrapper->emitTransferPausedByPeerSignal(callId, contactId,
		fileName, fileType, fileSize);
}

void PhApiSFPCallbacks::transferPaused(int callId,
	const std::string & contactId,
	const std::string & fileName,
	const std::string & fileType,
	int fileSize) {

	_phApiSFPWrapper->emitTransferPausedSignal(callId, contactId,
		fileName, fileType, fileSize);
}

void PhApiSFPCallbacks::transferResumedByPeer(int callId,
	const std::string & contactId,
	const std::string & fileName,
	const std::string & fileType,
	int fileSize) {

	_phApiSFPWrapper->emitTransferResumedByPeerSignal(callId, contactId,
		fileName, fileType, fileSize);
}

void PhApiSFPCallbacks::transferResumed(int callId,
	const std::string & contactId,
	const std::string & fileName,
	const std::string & fileType,
	int fileSize) {

	_phApiSFPWrapper->emitTransferResumedSignal(callId, contactId,
		fileName, fileType, fileSize);
}

void PhApiSFPCallbacks::peerNeedsUpgrade(const std::string & contactId) {
	_phApiSFPWrapper->emitPeerNeedsUpgradeSignal(contactId);
}

void PhApiSFPCallbacks::needUpgrade() {
	_phApiSFPWrapper->emitNeedsUpgradeSignal();
}

void PhApiSFPCallbacks::setCallbacks() {
	owplPluginSetCallback("SFPPlugin", "inviteToTransfer", (owplPS_CommandProc) inviteToTransferHandler);
	owplPluginSetCallback("SFPPlugin", "newIncomingFile", (owplPS_CommandProc) newIncomingFileHandler);
	owplPluginSetCallback("SFPPlugin", "waitingForAnswer", (owplPS_CommandProc) waitingForAnswerHandler);
	owplPluginSetCallback("SFPPlugin", "transferCancelled", (owplPS_CommandProc) transferCancelledHandler);
	owplPluginSetCallback("SFPPlugin", "transferCancelledByPeer", (owplPS_CommandProc) transferCancelledByPeerHandler);
	owplPluginSetCallback("SFPPlugin", "sendingFileBegin", (owplPS_CommandProc) sendingFileBeginHandler);
	owplPluginSetCallback("SFPPlugin", "receivingFileBegin", (owplPS_CommandProc) receivingFileBeginHandler);
	owplPluginSetCallback("SFPPlugin", "transferClosedByPeer", (owplPS_CommandProc) transferClosedByPeerHandler);
	owplPluginSetCallback("SFPPlugin", "transferClosed", (owplPS_CommandProc) transferClosedHandler);
	owplPluginSetCallback("SFPPlugin", "transferFromPeerFinished", (owplPS_CommandProc) transferFromPeerFinishedHandler);
	owplPluginSetCallback("SFPPlugin", "transferToPeerFinished", (owplPS_CommandProc) transferToPeerFinishedHandler);
	owplPluginSetCallback("SFPPlugin", "transferFromPeerFailed", (owplPS_CommandProc) transferFromPeerFailedHandler);
	owplPluginSetCallback("SFPPlugin", "transferToPeerFailed", (owplPS_CommandProc) transferToPeerFailedHandler);
	owplPluginSetCallback("SFPPlugin", "transferFromPeerStopped", (owplPS_CommandProc) transferFromPeerStoppedHandler);
	owplPluginSetCallback("SFPPlugin", "transferToPeerStopped", (owplPS_CommandProc) transferToPeerStoppedHandler);
	owplPluginSetCallback("SFPPlugin", "transferProgression", (owplPS_CommandProc) transferProgressionHandler);
	owplPluginSetCallback("SFPPlugin", "transferPausedByPeer", (owplPS_CommandProc) transferPausedByPeerHandler);
	owplPluginSetCallback("SFPPlugin", "transferPaused", (owplPS_CommandProc) transferPausedHandler);
	owplPluginSetCallback("SFPPlugin", "transferResumedByPeer", (owplPS_CommandProc) transferResumedByPeerHandler);
	owplPluginSetCallback("SFPPlugin", "transferResumed", (owplPS_CommandProc) transferResumedHandler);
	owplPluginSetCallback("SFPPlugin", "peerNeedUpgrade", (owplPS_CommandProc) peerNeedsUpgradeHandler);
	owplPluginSetCallback("SFPPlugin", "needUpgrade", (owplPS_CommandProc) needUpgradeHandler);
}

std::string PhApiSFPCallbacks::replaceOddCharacters(const std::string & str) {
	String newString(str);

	newString.replace("\\", "_", true);
	newString.replace("/", "_", true);
	newString.replace(":", "_", true);
	newString.replace("*", "_", true);
	newString.replace("?", "_", true);
	newString.replace("\"", "_", true);
	newString.replace("<", "_", true);
	newString.replace(">", "_", true);
	newString.replace("|", "_", true);

	return newString;
}
