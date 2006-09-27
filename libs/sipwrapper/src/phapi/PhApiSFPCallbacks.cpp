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
 * along with *(PhApiSFPWrapper::getInstance()) program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "PhApiSFPCallbacks.h"
#include "PhApiSFPEvent.h"
#include "PhApiSFPWrapper.h"

#include <util/String.h>

extern "C" {

	static void inviteToTransfer(int cid, char * uri, char * short_filename, char * file_type, char * file_size){
		PhApiSFPCallbacks::inviteToTransfer(cid, std::string((const char *) uri), std::string((const char *) short_filename), std::string((const char *) file_type), String(file_size).toInteger());
	}

	static void newIncomingFile(int cid, char * username, char * short_filename, char * file_type, char * file_size){
		PhApiSFPCallbacks::newIncomingFile(cid, std::string((const char *) username), std::string((const char *) short_filename), std::string((const char *) file_type), String(file_size).toInteger());
	}

	static void waitingForAnswer(int cid, char * uri){
		PhApiSFPCallbacks::waitingForAnswer(cid, std::string((const char *) uri));
	}

	static void transferCancelled(int cid, char * short_filename, char * file_type, char * file_size){
		PhApiSFPCallbacks::transferCancelled(cid, std::string((const char *) short_filename), std::string((const char *) file_type), String(file_size).toInteger());
	}

	static void transferCancelledByPeer(int cid, char * username, char * short_filename, char * file_type, char * file_size){
		PhApiSFPCallbacks::transferCancelledByPeer(cid, std::string((const char *) username), std::string((const char *) short_filename), std::string((const char *) file_type), String(file_size).toInteger());
	}

	static void sendingFileBegin(int cid, char * username, char * short_filename, char * file_type, char * file_size){
		PhApiSFPCallbacks::sendingFileBegin(cid, std::string((const char *) username), std::string((const char *) short_filename), std::string((const char *) file_type), String(file_size).toInteger());
	}
	
	static void receivingFileBegin(int cid, char * username, char * short_filename, char * file_type, char * file_size){
		PhApiSFPCallbacks::receivingFileBegin(cid, std::string((const char *) username), std::string((const char *) short_filename), std::string((const char *) file_type), String(file_size).toInteger());
	}

	static void transferClosed(int cid, char * username, char * short_filename, char * file_type, char * file_size){
	}

	static void transferClosedByPeer(int cid, char * username, char * short_filename, char * file_type, char * file_size){
	}

	static void transferFromPeerFinished(int cid, char * username, char * short_filename, char * file_type, char * file_size){
		PhApiSFPCallbacks::transferFromPeerFinished(cid, std::string((const char *) username), std::string((const char *) short_filename), std::string((const char *) file_type), String(file_size).toInteger());
	}

	static void transferToPeerFinished(int cid, char * username, char * short_filename, char * file_type, char * file_size){
		PhApiSFPCallbacks::transferToPeerFinished(cid, std::string((const char *) username), std::string((const char *) short_filename), std::string((const char *) file_type), String(file_size).toInteger());
	}

	static void transferFromPeerFailed(int cid, char * username, char * short_filename, char * file_type, char * file_size){
		PhApiSFPCallbacks::transferFromPeerFailed(cid, std::string((const char *) username), std::string((const char *) short_filename), std::string((const char *) file_type), String(file_size).toInteger());
	}

	static void transferToPeerFailed(int cid, char * username, char * short_filename, char * file_type, char * file_size){
		PhApiSFPCallbacks::transferToPeerFailed(cid, std::string((const char *) username), std::string((const char *) short_filename), std::string((const char *) file_type), String(file_size).toInteger());
	}

	static void transferFromPeerStopped(int cid, char * username, char * short_filename, char * file_type, char * file_size){
		PhApiSFPCallbacks::transferFromPeerFailed(cid, std::string((const char *) username), std::string((const char *) short_filename), std::string((const char *) file_type), String(file_size).toInteger());
	}

	static void transferToPeerStopped(int cid, char * username, char * short_filename, char * file_type, char * file_size){
		PhApiSFPCallbacks::transferToPeerFailed(cid, std::string((const char *) username), std::string((const char *) short_filename), std::string((const char *) file_type), String(file_size).toInteger());
	}

	static void transferProgression(int cid, int percentage){
		PhApiSFPCallbacks::transferProgression(cid, percentage);
	}

	static void transferPausedByPeer(int cid, char * username, char * short_filename, char * file_type, char * file_size){
		PhApiSFPCallbacks::transferPausedByPeer(cid, std::string((const char *) username), std::string((const char *) short_filename), std::string((const char *) file_type), String(file_size).toInteger());
	}

	static void transferPaused(int cid, char * username, char * short_filename, char * file_type, char * file_size){
		PhApiSFPCallbacks::transferPaused(cid, std::string((const char *) username), std::string((const char *) short_filename), std::string((const char *) file_type), String(file_size).toInteger());
	}

	static void transferResumedByPeer(int cid, char * username, char * short_filename, char * file_type, char * file_size){
		PhApiSFPCallbacks::transferResumedByPeer(cid, std::string((const char *) username), std::string((const char *) short_filename), std::string((const char *) file_type), String(file_size).toInteger());
	}

	static void transferResumed(int cid, char * username, char * short_filename, char * file_type, char * file_size){
		PhApiSFPCallbacks::transferResumed(cid, std::string((const char *) username), std::string((const char *) short_filename), std::string((const char *) file_type), String(file_size).toInteger());
	}
	
	static sfp_callbacks_t sfp_cbks = {
		inviteToTransfer,
		newIncomingFile,
		waitingForAnswer,
		transferCancelled,
		transferCancelledByPeer,
		sendingFileBegin,
		receivingFileBegin,
		transferClosedByPeer,
		transferClosed,
		transferFromPeerFinished,
		transferToPeerFinished,
		transferFromPeerFailed,
		transferToPeerFailed,
		transferFromPeerStopped,
		transferToPeerStopped,
		transferProgression,
		transferPausedByPeer,
		transferPaused,
		transferResumedByPeer,
		transferResumed
	};

}


void PhApiSFPCallbacks::inviteToTransfer(int callID, std::string uri, std::string fileName, std::string fileType, int fileSize){
	PhApiSFPEvent::inviteToTransferEvent(PhApiSFPWrapper::getInstance(), callID, uri, fileName, fileType, fileSize);
}

void PhApiSFPCallbacks::newIncomingFile(int callID, std::string contactID, std::string fileName, std::string fileType, int fileSize){
	PhApiSFPEvent::newIncomingFileEvent(PhApiSFPWrapper::getInstance(), callID, contactID, fileName, fileType, fileSize);
}

void PhApiSFPCallbacks::waitingForAnswer(int callID, std::string uri){
	PhApiSFPEvent::waitingForAnswerEvent(PhApiSFPWrapper::getInstance(), callID, uri);
}

void PhApiSFPCallbacks::transferCancelled(int callID, std::string fileName, std::string fileType, int fileSize){
	PhApiSFPEvent::transferCancelledEvent(PhApiSFPWrapper::getInstance(), callID, fileName, fileType, fileSize);
}

void PhApiSFPCallbacks::transferCancelledByPeer(int callID, std::string contactID, std::string fileName, std::string fileType, int fileSize){
	PhApiSFPEvent::transferCancelledByPeerEvent(PhApiSFPWrapper::getInstance(), callID, contactID, fileName, fileType, fileSize);
}

void PhApiSFPCallbacks::sendingFileBegin(int callID, std::string contactID, std::string fileName, std::string fileType, int fileSize){
	PhApiSFPEvent::sendingFileBeginEvent(PhApiSFPWrapper::getInstance(), callID, contactID, fileName, fileType, fileSize);
}

void PhApiSFPCallbacks::receivingFileBegin(int callID, std::string contactID, std::string fileName, std::string fileType, int fileSize){
	PhApiSFPEvent::receivingFileBeginEvent(PhApiSFPWrapper::getInstance(), callID, contactID, fileName, fileType, fileSize);
}

void PhApiSFPCallbacks::transferFromPeerFinished(int callID, std::string contactID, std::string fileName, std::string fileType, int fileSize){
	PhApiSFPEvent::transferFromPeerFinishedEvent(PhApiSFPWrapper::getInstance(), callID, contactID, fileName, fileType, fileSize);
}

void PhApiSFPCallbacks::transferToPeerFinished(int callID, std::string contactID, std::string fileName, std::string fileType, int fileSize){
	PhApiSFPEvent::transferToPeerFinishedEvent(PhApiSFPWrapper::getInstance(), callID, contactID, fileName, fileType, fileSize);
}

void PhApiSFPCallbacks::transferFromPeerFailed(int callID, std::string contactID, std::string fileName, std::string fileType, int fileSize){
	PhApiSFPEvent::transferFromPeerFailedEvent(PhApiSFPWrapper::getInstance(), callID, contactID, fileName, fileType, fileSize);
}

void PhApiSFPCallbacks::transferToPeerFailed(int callID, std::string contactID, std::string fileName, std::string fileType, int fileSize){
	PhApiSFPEvent::transferToPeerFailedEvent(PhApiSFPWrapper::getInstance(), callID, contactID, fileName, fileType, fileSize);
}

void PhApiSFPCallbacks::transferProgression(int callID, int percentage){
	PhApiSFPEvent::transferProgressionEvent(PhApiSFPWrapper::getInstance(), callID, percentage);
}

void PhApiSFPCallbacks::transferPausedByPeer(int callID, std::string contactID, std::string fileName, std::string fileType, int fileSize){
	PhApiSFPEvent::transferPausedByPeerEvent(PhApiSFPWrapper::getInstance(), callID, contactID, fileName, fileType, fileSize);
}

void PhApiSFPCallbacks::transferPaused(int callID, std::string contactID, std::string fileName, std::string fileType, int fileSize){
	PhApiSFPEvent::transferPausedEvent(PhApiSFPWrapper::getInstance(), callID, contactID, fileName, fileType, fileSize);
}

void PhApiSFPCallbacks::transferResumedByPeer(int callID, std::string contactID, std::string fileName, std::string fileType, int fileSize){
	PhApiSFPEvent::transferResumedByPeerEvent(PhApiSFPWrapper::getInstance(), callID, contactID, fileName, fileType, fileSize);
}

void PhApiSFPCallbacks::transferResumed(int callID, std::string contactID, std::string fileName, std::string fileType, int fileSize){
	PhApiSFPEvent::transferResumedEvent(PhApiSFPWrapper::getInstance(), callID, contactID, fileName, fileType, fileSize);
}

const sfp_callbacks_t * PhApiSFPCallbacks::getCallbacks(){
	return &sfp_cbks;
}
