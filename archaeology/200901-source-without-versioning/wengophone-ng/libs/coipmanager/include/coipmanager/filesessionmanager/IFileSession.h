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

#ifndef OWIFILESESSION_H
#define OWIFILESESSION_H

#include <coipmanager/coipmanagerdll.h>

#include <util/File.h>

/**
 * Interface for File session.
 *
 * @author Philippe Bernery
 */
class COIPMANAGER_API IFileSession {
public:

	enum IFileSessionEvent {
		/** No error. */
		IFileSessionEventNoError,

		/** Emitted when a ask for transferring a file has been sent. */
		IFileSessionEventInviteToTransfer,

		/**
		 * Emitted when the invite to transfer has been received by the peer but
		 * he has not yet accept the tranfer.
		 */
		IFileSessionEventWaitingForAnswer,

		/** Emitted when a file transfer is finished. */
		IFileSessionEventFileTransferFinished,

		/** Emitted when a file transfer failed. */
		IFileSessionEventFileTransferFailed,

		/** Emitted when a File transfer is paused by the user. */
		IFileSessionEventFileTransferPaused,

		/** Emitted when a File transfer is paused by peer. */
		IFileSessionEventFileTransferPausedByPeer,

		/** Emitted when a File transfer is resumed by the user. */
		IFileSessionEventFileTransferResumed,

		/** Emitted when a File transfer is resumed by peer. */
		IFileSessionEventFileTransferResumedByPeer,

		/** Emitted when a File transfer has been cancelled by the user. */
		IFileSessionEventFileTransferCancelled,

		/** Emitted when a File transfer has been cancelled by peer. */
		IFileSessionEventFileTransferCancelledByPeer,

		/** Emitted when the transfer of a File starts. */
		IFileSessionEventFileTransferBegan,

	};

	IFileSession();

	IFileSession(const IFileSession & iFileSession);

	virtual ~IFileSession();

	/**
	 * Sets the File object of this IFileSession.
	 */
	void setFile(const File & file);

	/**
	 * Gets the File of this IFileSession.
	 */
	File getFile() const;

	/**
	 * Gets the last happened event.
	 */
	IFileSession::IFileSessionEvent getLastEvent() const;

protected:

	/** File used for this IFileSession. */
	File _file;

	/** Last happened event. */
	IFileSession::IFileSessionEvent _lastEvent;
};

#endif	//OWIFILESESSION_H
