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

#ifndef OWSENDFILESESSION_H
#define OWSENDFILESESSION_H

#include <coipmanager/filesessionmanager/ISendFileSession.h>
#include <coipmanager/session/Session.h>

class ISendFileSessionPlugin;

/**
 * File Session.
 *
 * Interface to File transfer services.
 *
 *  - Send mode: 'start' starts sending files by creating a FileSession per
 * Contact and per file. 'stop' stops the transfer.
 *
 * @see TestFileSession for an example
 *
 * @ingroup CoIpManager
 * @author Philippe Bernery
 */
class COIPMANAGER_API SendFileSession : public Session, public ISendFileSession {
	Q_OBJECT
	friend class FileSessionManager;
public:

	virtual ~SendFileSession();

	// Inherited from Session
	virtual void start();

	virtual void pause();

	virtual void resume();

	virtual void stop();
	////

Q_SIGNALS:

	/**
	 * @see ISendFileSessionPlugin::fileTransferSignal
	 */
	void fileTransferSignal(IFileSession::IFileSessionEvent event);

	/**
	 * @see ISendFileSessionPlugin::fileTransferProgressionSignal
	 */
	void fileTransferProgressionSignal(int percentage);

private Q_SLOTS:

	/**
	 * @see ISendFileSessionPlugin::fileTransferSignal
	 */
	void fileTransferSlot(IFileSession::IFileSessionEvent event);

	/**
	 * @see ISendFileSessionPlugin::fileTransferProgressionSignal
	 */
	void fileTransferProgressionSlot(int percentage);

private:

	virtual bool canCreateISession(const Account & account, const ContactList & contactList) const;

	/**
	 * Creates a SendFileSession.
	 *
	 * @param userProfile link to UserProfile
	 */
	SendFileSession(CoIpManager & coIpManager);

	/** Pointer to running ISendFileSessionPlugin. */
	ISendFileSessionPlugin * _currentFileSessionPlugin;

};

#endif	//OWSENDFILESESSION_H
