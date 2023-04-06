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

#ifndef OWTRECEIVEFILESESSION_H
#define OWTRECEIVEFILESESSION_H

#include <coipmanager_threaded/tcoipmanagerdll.h>

#include <coipmanager_threaded/filesessionmanager/TIFileSession.h>

#include <coipmanager/filesessionmanager/ReceiveFileSession.h>

class TCoIpManager;

/**
 * Threaded ReceiveFileSession.
 * 
 * @see ReceiveFileSession
 * @author Philippe Bernery
 */
class COIPMANAGER_THREADED_API TReceiveFileSession : public TIFileSession {
	Q_OBJECT
	friend class TFileSessionManager;
public:

	virtual ~TReceiveFileSession();

	/**
	 * Gets the Contact who sends the File.
	 */
	Contact getContact() const;

	/**
	 * Sets the path for saving the File.
	 */
	void setFilePath(const std::string & path);

	/**
	 * Gets the path to the File to be saved.
	 */
	std::string getFilePath() const;

	/**
	 * Gets the name of the file to receive.
	 */
	std::string getFileName() const;

	/**
	 * Gets the size of the file to receive.
	 */
	unsigned getFileSize() const;

Q_SIGNALS:

	/**
	 * @see ReceiveFileSession::fileTransferSignal
	 */
	void fileTransferSignal(IFileSession::IFileSessionEvent event);

	/**
	 * @see ReceiveFileSession::fileTransferProgressionSignal
	 */
	void fileTransferProgressionSignal(int percentage);

private:

	TReceiveFileSession(TCoIpManager & tCoIpManager, ReceiveFileSession * receiveFileSession);

	ReceiveFileSession * getReceiveFileSession() const;
};

#endif //OWTRECEIVEFILESESSION_H
