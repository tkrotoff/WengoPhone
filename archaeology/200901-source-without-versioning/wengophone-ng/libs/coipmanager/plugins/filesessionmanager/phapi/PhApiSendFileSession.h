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

#ifndef OWPHAPISENDFILESESSION_H
#define OWPHAPISENDFILESESSION_H

#include <coipmanager/filesessionmanager/ISendFileSessionPlugin.h>

#include <string>

class PhApiSFPWrapper;

/**
 * TODO
 *
 * @author Nicolas Couturier
 */
class PhApiSendFileSession : public ISendFileSessionPlugin {
	Q_OBJECT
public:

	PhApiSendFileSession(CoIpManager & coIpManager);

	PhApiSendFileSession(const PhApiSendFileSession & phApiSendFileSession);

	virtual Identifiable * clone() const;

	virtual ~PhApiSendFileSession();

	void start();

	void pause();

	void resume();

	void stop();

private Q_SLOTS:

	void inviteToTransferSlot(int callId, const std::string & uri,
		const std::string & fileName, const std::string & fileType, int fileSize);

	void waitingForAnswerSlot(int callId, const std::string & uri);

	void transferCancelledSlot(int callId, const std::string & fileName,
		const std::string & fileType, int fileSize);

	void transferCancelledByPeerSlot(int callId, const std::string & contactId,
		const std::string & fileName, const std::string & fileType, int fileSize);

	void sendingFileBeginSlot(int callId, const std::string & contactId,
		const std::string & fileName, const std::string & fileType, int fileSize);

	void transferFromPeerFinishedSlot(int callId, const std::string & contactId,
		const std::string & fileName, const std::string & fileType, int fileSize);

	void transferToPeerFinishedSlot(int callId, const std::string & contactId,
		const std::string & fileName, const std::string & fileType, int fileSize);

	void transferFromPeerFailedSlot(int callId, const std::string & contactId,
		const std::string & fileName, const std::string & fileType, int fileSize);

	void transferToPeerFailedSlot(int callId, const std::string & contactId,
		const std::string & fileName, const std::string & fileType, int fileSize);

	void transferProgressionSlot(int callId, int percentage);

	void transferPausedByPeerSlot(int callId, const std::string & contactId,
		const std::string & fileName, const std::string & fileType, int fileSize);

	void transferPausedSlot(int callId, const std::string & contactId,
		const std::string & fileName, const std::string & fileType, int fileSize);

	void transferResumedByPeerSlot(int callId, const std::string & contactId,
		const std::string & fileName, const std::string & fileType, int fileSize);

	void transferResumedSlot(int callId, const std::string & contactId,
		const std::string & fileName, const std::string & fileType, int fileSize);
		
private:

	void registerToEvents();

	std::string extractContactIdFromURI(const std::string & uri) const;

	/**
	 * Call id of current file transfer.
	 */
	int _currentCallId;
};

#endif	//OWPHAPISENDFILESESSION_H
