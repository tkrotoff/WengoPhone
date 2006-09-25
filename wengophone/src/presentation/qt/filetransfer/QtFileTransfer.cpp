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

#include <control/contactlist/CContactList.h>

#include <coipmanager/CoIpManager.h>

#include <imwrapper/IMContactSet.h>

#include <util/Logger.h>
#include <util/SafeDelete.h>

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
	_qtFileTransferWidget->hide();
	delete _qtFileTransferWidget;
}

void QtFileTransfer::newReceiveFileSessionCreatedEventHandler(
	FileSessionManager & sender, ReceiveFileSession fileSession) {

	ReceiveFileSession * newFileSession = new ReceiveFileSession(fileSession);
	newReceiveFileSessionCreatedEventHandlerSignal(newFileSession);
}

void QtFileTransfer::newReceiveFileSessionCreatedEventHandlerSlot(ReceiveFileSession * fileSession) {

	Config & config = ConfigManager::getInstance().getCurrentConfig();
	QString downloadFolder;
	QString filename =  QString::fromStdString(fileSession->getFileName());
	QString contact = QString::fromStdString(fileSession->getIMContact().getContactId());

	LOG_DEBUG("incoming file: " + fileSession->getFileName() + "from: " + fileSession->getIMContact().getContactId());

	QtFileTransferAcceptDialog qtFileTransferAcceptDialog(_qtFileTransferWidget);
	qtFileTransferAcceptDialog.setFileName(filename);
	qtFileTransferAcceptDialog.setContactName(contact);

	// the user accept the file transfer
	if (qtFileTransferAcceptDialog.exec() == QDialog::Accepted) {
	
		QDir dir(QString::fromStdString(config.getFileTransferDownloadFolder()));
		// if no download folder set then choose one
		// or if the choosen folder does not exists anymore.
		if ((config.getFileTransferDownloadFolder().empty()) || (!dir.exists())) {
	
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
			downloadFolder = QString::fromStdString(config.getFileTransferDownloadFolder());
		}

		// here we're sure to have a download folder,
		// but we must check if the file already exists.
		if (isFileInDir(downloadFolder, filename)) {
			
			if (QMessageBox::question(_qtFileTransferWidget, tr("Overwrite File?"),
					tr("A file called %1 already exists."
					"Do you want to overwrite it?").arg(filename),
					tr("&Yes"), tr("&No"), QString(), 0, 1)) {

				fileSession->stop();
				OWSAFE_DELETE(fileSession);
				return;
			}
		}

		_qtFileTransferWidget->setDownloadFolder(downloadFolder);
		_qtFileTransferWidget->addReceiveItem(fileSession);
		fileSession->setFilePath(downloadFolder.toStdString());
		fileSession->start();

	// the user refuse the file transfer.
	} else {
		fileSession->stop();
		OWSAFE_DELETE(fileSession);
		return;
	}
}

void QtFileTransfer::createSendFileSession(IMContactSet imContactSet, const std::string & filename, CContactList & cContactList) {

	SendFileSession * fileSession = _coIpManager->getFileSessionManager().createSendFileSession();
	fileSession->addFile(File(filename));

	for (IMContactSet::const_iterator it = imContactSet.begin(); it != imContactSet.end(); ++it) {
		std::string contactId = cContactList.findContactThatOwns(*it);
		fileSession->addContact(contactId);

		_qtFileTransferWidget->addSendItem(fileSession, filename, contactId, (*it).getContactId());
	}

	fileSession->start();
}

bool QtFileTransfer::isFileInDir(const QString & dirname, const QString & filename) {

	QDir dir(dirname);
	if (dir.exists()) {

		QStringList dirList = dir.entryList(QDir::Files);
		for (int i = 0; i < dirList.size(); i++) {
			LOG_DEBUG("filename: " + dirList[i].toStdString());
			if (dirList[i] == filename) {
				return true;
			}
		}
	}
	return false;
}
