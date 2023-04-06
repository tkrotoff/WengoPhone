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

#ifndef OWPHAPISFPCALLBACKS_H
#define OWPHAPISFPCALLBACKS_H

// Fix a build problem on Windows with MSVC 8
#include <owpl_plugin.h>

#include <util/Singleton.h>

#include <string>

class PhApiSFPWrapper;

/**
 * Class implementing the callbacks for the SFP plugin and throwing the corresponding events
 *
 * @author Nicolas Couturier
 */
class PhApiSFPCallbacks : public Singleton<PhApiSFPCallbacks> {
	friend class Singleton<PhApiSFPCallbacks>;
public:

	/**
	 * Must be called just after construction.
	 */
	void setPhApiSFPWrapper(PhApiSFPWrapper * phApiSFPWrapper);

	void inviteToTransfer(int callId,
		const std::string & uri,
		const std::string & fileName,
		const std::string & fileType,
		int fileSize);

	void newIncomingFile(int callId,
		const std::string & contactId,
		const std::string & fileName,
		const std::string & fileType,
		int fileSize);

	void waitingForAnswer(int callId, const std::string & uri);

	void transferCancelled(int callId,
		const std::string & fileName,
		const std::string & fileType,
		int fileSize);

	void transferCancelledByPeer(int callId,
		const std::string & contactId,
		const std::string & fileName,
		const std::string & fileType,
		int fileSize);

	void sendingFileBegin(int callId,
		const std::string & contactId,
		const std::string & fileName,
		const std::string & fileType,
		int fileSize);

	void receivingFileBegin(int callId,
		const std::string & contactId,
		const std::string & fileName,
		const std::string & fileType,
		int fileSize);

	void transferFromPeerFinished(int callId,
		const std::string & contactId,
		const std::string & fileName,
		const std::string & fileType,
		int fileSize);

	void transferToPeerFinished(int callId,
		const std::string & contactId,
		const std::string & fileName,
		const std::string & fileType,
		int fileSize);

	void transferFromPeerFailed(int callId,
		const std::string & contactId,
		const std::string & fileName,
		const std::string & fileType,
		int fileSize);

	void transferToPeerFailed(int callId,
		const std::string & contactId,
		const std::string & fileName,
		const std::string & fileType,
		int fileSize);

	void transferProgression(int callId, int percentage);

	void transferPausedByPeer(int callId,
		const std::string & contactId,
		const std::string & fileName,
		const std::string & fileType,
		int fileSize);

	void transferPaused(int callId,
		const std::string & contactId,
		const std::string & fileName,
		const std::string & fileType,
		int fileSize);

	void transferResumedByPeer(int callId,
		const std::string & contactId,
		const std::string & fileName,
		const std::string & fileType,
		int fileSize);

	void transferResumed(int callId,
		const std::string & contactId,
		const std::string & fileName,
		const std::string & fileType,
		int fileSize);

	void peerNeedsUpgrade(const std::string & contactId);

	void needUpgrade();

	std::string replaceOddCharacters(const std::string & str);

private:

	PhApiSFPCallbacks();

	void setCallbacks();

	PhApiSFPWrapper * _phApiSFPWrapper;

};

#endif //OWPHAPISFPCALLBACKS_H
