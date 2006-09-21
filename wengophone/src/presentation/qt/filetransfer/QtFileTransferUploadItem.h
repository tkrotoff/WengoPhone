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

#ifndef OWQTFILETRANSFERUPLOADITEM_H
#define OWQTFILETRANSFERUPLOADITEM_H

#include "QtFileTransferItem.h"

#include <filesessionmanager/IFileSession.h>

class SendFileSession;
class File;
class IMContact;
class CoIpModule;

/**
 *
 * @author Mathieu Stute
 */
class QtFileTransferUploadItem : public QtFileTransferItem {
	Q_OBJECT
public:

	/**
	 * Default constructor.
	 */
	QtFileTransferUploadItem(QWidget * parent, SendFileSession * fileSession, 
		const std::string & filename, const std::string & contactId, const std::string & contact);

private Q_SLOTS:

	/**
	 * @see ReceiveFileSession::pause().
	 */
	void pause();

	/**
	 * @see ReceiveFileSession::resume().
	 */
	void resume();

	/**
	 * @see ReceiveFileSession::stop().
	 */
	void stop();

private:

	void fileTransferProgressionEventHandler(SendFileSession & sender, IMContact imContact,
		File sentFile, int percentage);

	void fileTransferEventHandler(SendFileSession & sender, IFileSession::IFileSessionEvent event,
		IMContact imContact, File sentFile);

	void moduleFinishedEventHandler(CoIpModule & sender);

	SendFileSession * _sendFileSession;

	std::string _filename;

	std::string _contactId;
};

#endif	//OWQTFILETRANSFERUPLOADITEM_H
