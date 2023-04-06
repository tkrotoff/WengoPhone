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

#ifndef OWPHAPIRECEIVEFILESESSION_H
#define OWPHAPIRECEIVEFILESESSION_H

#include <coipmanager/filesessionmanager/IReceiveFileSessionPlugin.h>

class PhApiSFPWrapper;

/**
 * TODO
 *
 * @author Nicolas Couturier
 */
class PhApiReceiveFileSession : public IReceiveFileSessionPlugin {
	Q_OBJECT
public:

	PhApiReceiveFileSession(CoIpManager & coIpManager);

	PhApiReceiveFileSession(CoIpManager & coIpManager,
		int callId, const std::string & contactId, const std::string & fileName, unsigned fileSize);

	PhApiReceiveFileSession(const PhApiReceiveFileSession & phApiReceiveFileSession);

	virtual Identifiable * clone() const;

	virtual ~PhApiReceiveFileSession();

	void start();

	void pause();

	void resume();

	void stop();

private Q_SLOTS:

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

	int _currentCallId;
};

#endif	//OWPHAPIRECEIVEFILESESSION_H
