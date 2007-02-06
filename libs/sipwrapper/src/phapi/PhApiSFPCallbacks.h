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

#ifndef OWPHAPISFPCALLBACKS_H
#define OWPHAPISFPCALLBACKS_H

#include <string>

/**
 * Class implementing the callbacks for the SFP plugin and throwing the corresponding events
 *
 * @author Nicolas Couturier
 */
class PhApiSFPCallbacks {
public:

	static void inviteToTransfer(int cid,
			const std::string & uri,
			const std::string & short_filename,
			const std::string & file_type,
			int file_size);

	static void newIncomingFile(int cid,
			const std::string & contactID,
			const std::string & short_filename,
			const std::string & file_type,
			int file_size);

	static void waitingForAnswer(int cid, const std::string & uri);

	static void transferCancelled(int cid,
			const std::string & short_filename,
			const std::string & file_type,
			int file_size);

	static void transferCancelledByPeer(int cid,
			const std::string & contactID,
			const std::string & short_filename,
			const std::string & file_type,
			int file_size);

	static void sendingFileBegin(int cid,
			const std::string & contactID,
			const std::string & short_filename,
			const std::string & file_type,
			int file_size);

	static void receivingFileBegin(int cid,
			const std::string & contactID,
			const std::string & short_filename,
			const std::string & file_type,
			int file_size);

	static void transferFromPeerFinished(int cid,
			const std::string & contactID,
			const std::string & short_filename,
			const std::string & file_type,
			int file_size);

	static void transferToPeerFinished(int cid,
			const std::string & contactID,
			const std::string & short_filename,
			const std::string & file_type,
			int file_size);

	static void transferFromPeerFailed(int cid,
			const std::string & contactID,
			const std::string & short_filename,
			const std::string & file_type,
			int file_size);

	static void transferToPeerFailed(int cid,
			const std::string & contactID,
			const std::string & short_filename,
			const std::string & file_type,
			int file_size);

	static void transferProgression(int cid, int percentage);

	static void transferPausedByPeer(int cid,
			const std::string & contactID,
			const std::string & short_filename,
			const std::string & file_type,
			int file_size);

	static void transferPaused(int cid,
			const std::string & contactID,
			const std::string & short_filename,
			const std::string & file_type,
			int file_size);

	static void transferResumedByPeer(int cid,
			const std::string & contactID,
			const std::string & short_filename,
			const std::string & file_type,
			int file_size);

	static void transferResumed(int cid,
			const std::string & contactID,
			const std::string & short_filename,
			const std::string & file_type,
			int file_size);

	static void peerNeedsUpgrade(const std::string & contactID);

	static void needUpgrade();

	static void setCallbacks();

	static std::string replaceOddCharacters(const std::string & str);
};

#endif	//OWPHAPISFPCALLBACKS_H
