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

#ifndef OWIRECEIVEFILESESSIONPLUGIN_H
#define OWIRECEIVEFILESESSIONPLUGIN_H

#include <coipmanager/filesessionmanager/IReceiveFileSession.h>
#include <coipmanager/session/ISession.h>

/**
 * Interface for receive file session implementations.
 *
 * @author Philippe Bernery
 */
class COIPMANAGER_API IReceiveFileSessionPlugin : public ISession, public IReceiveFileSession {
	Q_OBJECT
public:

	IReceiveFileSessionPlugin(CoIpManager & coIpManager);

	IReceiveFileSessionPlugin(const IReceiveFileSessionPlugin & iReceiveFileSession);

	virtual ~IReceiveFileSessionPlugin();

	/**
	 * Gets the IMContact who sends the File.
	 */
	IMContact getIMContact() const;

Q_SIGNALS:

	/**
	 * @see ISendFileSessionPlugin::fileTransferSignal
	 */
	void fileTransferSignal(IFileSession::IFileSessionEvent event);

	/**
	 * @see ISendFileSessionPlugin::fileTransferProgressionSignal
	 */
	void fileTransferProgressionSignal(int percentage);

};

#endif	//OWIRECEIVEFILESESSIONPLUGIN_H
