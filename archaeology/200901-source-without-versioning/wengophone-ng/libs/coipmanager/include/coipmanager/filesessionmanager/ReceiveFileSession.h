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

#ifndef OWRECEIVEFILESESSION_H
#define OWRECEIVEFILESESSION_H

#include <coipmanager/filesessionmanager/IReceiveFileSession.h>
#include <coipmanager/session/Session.h>

class FileSessionManager;
class IReceiveFileSessionPlugin;

/**
 * File Session.
 *
 * Interface to File transfer services.
 *
 *  - Receive mode: 'start' accepts the transfer and 'stop' refuses it.
 *
 * @see TestFileSession for an example
 *
 * @ingroup CoIpManager
 * @author Philippe Bernery
 */
class COIPMANAGER_API ReceiveFileSession : public Session, public IReceiveFileSession {
	Q_OBJECT
public:

	/**
	 * Creates a ReceiveFileSession.
	 *
	 * @param coIpManager CoIpManager instance
	 * @param fileSessionPlugin the IFileSessionPlugin to use for receiving the file
	 */
	ReceiveFileSession(CoIpManager & coIpManager, IReceiveFileSessionPlugin * fileSessionPlugin);

	ReceiveFileSession(const ReceiveFileSession & receiveFileSession);

	virtual ~ReceiveFileSession();

	// Inherited from Session
	virtual void start();

	virtual void pause();

	virtual void resume();

	virtual void stop();
	////

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
	 * @see IReceiveFileSessionPlugin::fileTransferSignal
	 */
	void fileTransferSignal(IFileSession::IFileSessionEvent event);

	/**
	 * @see IReceiveFileSessionPlugin::fileTransferProgressionSignal
	 */
	void fileTransferProgressionSignal(int percentage);

private Q_SLOTS:

	/**
	 * @see IFileSessionPlugin::fileTransferEvent
	 */
	void fileTransferSlot(IFileSession::IFileSessionEvent event);

	/**
	 * @see IFileSessionPlugin::fileTransferProgressionEvent
	 */
	void fileTransferProgressionSlot(int percentage);

private:

	virtual bool canCreateISession(const Account & account, const ContactList & contactList) const;

	// Inherited from Session
	// Cannot add/remove on a ReceiveFileSession
	virtual EnumSessionError::SessionError addContact(const Contact & /*contact*/);

	virtual void removeContact(const Contact & /*contact*/);
	////

	/** Pointer to running IFileSessionPlugin. */
	IReceiveFileSessionPlugin * _currentFileSessionPlugin;
};

#endif	//OWRECEIVEFILESESSION_H
