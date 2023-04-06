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

#ifndef OWPHAPISFPWRAPPER_H
#define OWPHAPISFPWRAPPER_H

// Fix a build problem on Windows with MSVC 8
#include <owpl_plugin.h>

#include <phapiwrapperdll.h>

#include <util/Singleton.h>

#include <QtCore/QObject>

#include <string>

/**
 * Class that wraps the calls to the SFP plugin
 *
 * @author Nicolas Couturier
 */
class PHAPIWRAPPER_API PhApiSFPWrapper : public QObject, public Singleton<PhApiSFPWrapper> {
	Q_OBJECT
	friend class Singleton<PhApiSFPWrapper>;
public:

	/**
	 * Initializes file transfer of PhApi.
	 * This method will use the plugin path set in PhApiWrapper.
	 */
	void initialize();

	/**
	 * @return the call id if the invitation to transfer could be sent
	 */
	int sendFile(int lineId,
		const std::string & fullIdentity,
		const std::string & contactUri,
		const std::string & filename,
		const std::string & shortFilename,
		const std::string & fileType,
		unsigned fileSize);

	int receiveFile(int callId, const std::string & filename);

	int cancelTransfer(int callId);

	int pauseTransfer(int callId);

	int resumeTransfer(int callId);

	void emitInviteToTransferSignal(int callId, std::string uri,
		std::string fileName, std::string fileType, int fileSize);

	void emitNewIncomingFileSignal(int callId, std::string contactId,
		std::string fileName, std::string fileType, int fileSize);

	void emitWaitingForAnswerSignal(int callId, std::string uri);

	void emitTransferCancelledSignal(int callId, std::string fileName,
		std::string fileType, int fileSize);

	void emitTransferCancelledByPeerSignal(int callId, std::string contactId,
		std::string fileName, std::string fileType, int fileSize);

	void emitSendingFileBeginSignal(int callId, std::string contactId,
		std::string fileName, std::string fileType, int fileSize);

	void emitReceivingFileBeginSignal(int callId, std::string contactId,
		std::string fileName, std::string fileType, int fileSize);

	void emitTransferFromPeerFinishedSignal(int callId, std::string contactId,
		std::string fileName, std::string fileType, int fileSize);

	void emitTransferToPeerFinishedSignal(int callId, std::string contactId,
		std::string fileName, std::string fileType, int fileSize);

	void emitTransferFromPeerFailedSignal(int callId, std::string contactId,
		std::string fileName, std::string fileType, int fileSize);

	void emitTransferToPeerFailedSignal(int callId, std::string contactId,
		std::string fileName, std::string fileType, int fileSize);

	void emitTransferProgressionSignal(int callId, int percentage);

	void emitTransferPausedByPeerSignal(int callId, std::string contactId,
		std::string fileName, std::string fileType, int fileSize);

	void emitTransferPausedSignal(int callId, std::string contactId,
		std::string fileName, std::string fileType, int fileSize);

	void emitTransferResumedByPeerSignal(int callId, std::string contactId,
		std::string fileName, std::string fileType, int fileSize);

	void emitTransferResumedSignal(int callId, std::string contactId,
		std::string fileName, std::string fileType, int fileSize);

	void emitPeerNeedsUpgradeSignal(std::string contactId);

	void emitNeedsUpgradeSignal();
	
Q_SIGNALS:

	void inviteToTransferSignal(int callId, std::string uri,
		std::string fileName, std::string fileType, int fileSize);

	void newIncomingFileSignal(int callId, std::string contactId,
		std::string fileName, std::string fileType, int fileSize);

	void waitingForAnswerSignal(int callId, std::string uri);

	void transferCancelledSignal(int callId, std::string fileName,
		std::string fileType, int fileSize);

	void transferCancelledByPeerSignal(int callId, std::string contactId,
		std::string fileName, std::string fileType, int fileSize);

	void sendingFileBeginSignal(int callId, std::string contactId,
		std::string fileName, std::string fileType, int fileSize);

	void receivingFileBeginSignal(int callId, std::string contactId,
		std::string fileName, std::string fileType, int fileSize);

	void transferFromPeerFinishedSignal(int callId, std::string contactId,
		std::string fileName, std::string fileType, int fileSize);

	void transferToPeerFinishedSignal(int callId, std::string contactId,
		std::string fileName, std::string fileType, int fileSize);

	void transferFromPeerFailedSignal(int callId, std::string contactId,
		std::string fileName, std::string fileType, int fileSize);

	void transferToPeerFailedSignal(int callId, std::string contactId,
		std::string fileName, std::string fileType, int fileSize);

	void transferProgressionSignal(int callId, int percentage);

	void transferPausedByPeerSignal(int callId, std::string contactId,
		std::string fileName, std::string fileType, int fileSize);

	void transferPausedSignal(int callId, std::string contactId,
		std::string fileName, std::string fileType, int fileSize);

	void transferResumedByPeerSignal(int callId, std::string contactId,
		std::string fileName, std::string fileType, int fileSize);

	void transferResumedSignal(int callId, std::string contactId,
		std::string fileName, std::string fileType, int fileSize);

	void peerNeedsUpgradeSignal(std::string contactId);

	void needsUpgradeSignal();

private:

	PhApiSFPWrapper();

	void setBasePort(unsigned basePort);
};

#endif	//OWPHAPISFPWRAPPER_H
