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

#include "QtFileTransfer.h"
#include "QtFileTransferAcceptDialog.h"
#include "QtFileTransferWidget.h"

#include <model/config/ConfigManager.h>
#include <model/config/Config.h>

#include <coipmanager/CoIpManager.h>

#include <util/SafeDelete.h>
#include <util/Logger.h>

#include <QtGui/QtGui>

QtFileTransfer::QtFileTransfer(QObject * parent, CoIpManager * coIpManager)
	: QObject(parent), _coIpManager(coIpManager) {

	_qtFileTransferWidget = new QtFileTransferWidget();

	connect(this, SIGNAL(newReceiveFileSessionCreatedEventHandlerSignal(ReceiveFileSession *)),
		SLOT(newReceiveFileSessionCreatedEventHandlerSlot(ReceiveFileSession *)));
	_coIpManager->getFileSessionManager().newReceiveFileSessionCreatedEvent +=
		boost::bind(&QtFileTransfer::newReceiveFileSessionCreatedEventHandler, this, _1, _2);
}

QtFileTransfer::~QtFileTransfer() {
}

void QtFileTransfer::newReceiveFileSessionCreatedEventHandler(FileSessionManager & sender,
	ReceiveFileSession fileSession) {

	ReceiveFileSession * newFileSession = new ReceiveFileSession(fileSession);
	newReceiveFileSessionCreatedEventHandlerSignal(newFileSession);
}

void QtFileTransfer::newReceiveFileSessionCreatedEventHandlerSlot(ReceiveFileSession * fileSession) {

	Config & config = ConfigManager::getInstance().getCurrentConfig();
	QString downloadFolder;

	LOG_DEBUG("incoming file: " + fileSession->getFileName() + "from: " + fileSession->getIMContact().getContactId());

	QtFileTransferAcceptDialog qtFileTransferAcceptDialog(0);
	qtFileTransferAcceptDialog.setFileName(fileSession->getFileName());
	qtFileTransferAcceptDialog.setContactName(fileSession->getIMContact().getContactId());

	// the user accept the file transfer
	if (qtFileTransferAcceptDialog.exec() == QDialog::Accepted) {
	
		// if no download folder set then choose one
		if (config.getFileTransferDownloadFolder().empty()) {
	
			downloadFolder = QFileDialog::getExistingDirectory(
				0,
				tr("Choose a directory"),
				"",
				QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks
			);

			if (!downloadFolder.isEmpty()) {
				config.set(Config::FILETRANSFER_DOWNLOAD_FOLDER, downloadFolder.toStdString());
			} else {
				//TODO: warn the user is has set no download folder.
				fileSession->stop();
				OWSAFE_DELETE(fileSession);
				return;
			}
		} else {
			//TODO: check if the folder exists
			downloadFolder = QString::fromStdString(config.getFileTransferDownloadFolder());
		}

		// here we're sure to have a download folder.
		_qtFileTransferWidget->setDownloadFolder(downloadFolder);
		_qtFileTransferWidget->addReceiveItem(fileSession);
		fileSession->setFilePath(downloadFolder.toStdString());
		fileSession->start();

	// the user refuse the file transfer.
	} else {
		fileSession->stop();
		OWSAFE_DELETE(fileSession);
	}
}

void QtFileTransfer::addSendFileSession(SendFileSession * fileSession) {
	_qtFileTransferWidget->addSendItem(fileSession);
}

void QtFileTransfer::sendFileToPeer(const std::string contactId, const std::string filename) {
	SendFileSession * fileSession = _coIpManager->getFileSessionManager().createSendFileSession();
	fileSession->addContact(contactId);
	fileSession->addFile(filename);
	_qtFileTransferWidget->addSendItem(fileSession);
	fileSession->start();
}

void QtFileTransfer::sendFileToPeers(List<std::string> contactIdList, const std::string filename) {
	SendFileSession * fileSession = _coIpManager->getFileSessionManager().createSendFileSession();
	for(unsigned int i = 0; i < contactIdList.size(); i++) {
		fileSession->addContact(contactIdList[i]);
	}
	fileSession->addFile(filename);
	_qtFileTransferWidget->addSendItem(fileSession);
	fileSession->start();
}
