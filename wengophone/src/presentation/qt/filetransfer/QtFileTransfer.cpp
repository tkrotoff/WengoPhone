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
#include "QtFileTransferUpgradeDialog.h"
#include "QtFileTransferWidget.h"

#include <model/config/ConfigManager.h>
#include <model/config/Config.h>

#include <control/contactlist/CContactList.h>

#include <coipmanager/CoIpManager.h>

#include <imwrapper/IMContactSet.h>

#include <util/Logger.h>
#include <util/SafeDelete.h>

#include <qtutil/SafeConnect.h>

#include <QtGui/QtGui>

#include <limits.h>

QtFileTransfer::QtFileTransfer(QObject * parent, CoIpManager * coIpManager)
	: QObject(parent),
	_coIpManager(coIpManager) {

	_qtFileTransferWidget = new QtFileTransferWidget(NULL);
	SAFE_CONNECT(this, SIGNAL(newReceiveFileSessionCreatedEventHandlerSignal(ReceiveFileSession *)),
		SLOT(newReceiveFileSessionCreatedEventHandlerSlot(ReceiveFileSession *)));
	_coIpManager->getFileSessionManager().newReceiveFileSessionCreatedEvent +=
		boost::bind(&QtFileTransfer::newReceiveFileSessionCreatedEventHandler, this, _1, _2);

	SAFE_CONNECT(this, SIGNAL(needUpgradeEventHandlerSignal()),
		SLOT(needUpgradeEventHandlerSlot()));
	_coIpManager->getFileSessionManager().needUpgradeEvent +=
		boost::bind(&QtFileTransfer::needUpgradeEventHandler, this, _1);

	SAFE_CONNECT(this, SIGNAL(peerNeedsUpgradeEventHandlerSignal()),
		SLOT(peerNeedsUpgradeEventHandlerSlot()));
	_coIpManager->getFileSessionManager().peerNeedsUpgradeEvent +=
		boost::bind(&QtFileTransfer::peerNeedsUpgradeEventHandler, this, _1);
}

QtFileTransfer::~QtFileTransfer() {
	_qtFileTransferWidget->hide();
	OWSAFE_DELETE(_qtFileTransferWidget);
}

void QtFileTransfer::newReceiveFileSessionCreatedEventHandler(
	FileSessionManager & sender, ReceiveFileSession fileSession) {

	ReceiveFileSession * newFileSession = new ReceiveFileSession(fileSession);
	newReceiveFileSessionCreatedEventHandlerSignal(newFileSession);
}

void QtFileTransfer::newReceiveFileSessionCreatedEventHandlerSlot(ReceiveFileSession * fileSession) {

	Config & config = ConfigManager::getInstance().getCurrentConfig();
	QString downloadFolder;
	QString filename =  QString::fromUtf8(fileSession->getFileName().c_str());
	QString contact = QString::fromStdString(fileSession->getIMContact().getContactId());

	LOG_DEBUG("incoming file: " + fileSession->getFileName() + "from: " + fileSession->getIMContact().getContactId());

	_qtFileTransferWidget->showAndRaise();
	QtFileTransferAcceptDialog qtFileTransferAcceptDialog(_qtFileTransferWidget);
	qtFileTransferAcceptDialog.setFileName(filename);
	qtFileTransferAcceptDialog.setContactName(contact);

	// the user accept the file transfer
	if (qtFileTransferAcceptDialog.exec() == QDialog::Accepted) {

		QDir dir(QString::fromUtf8(config.getFileTransferDownloadFolder().c_str()));
		// if no download folder set then choose one
		// or if the choosen folder does not exists anymore.
		if ((config.getFileTransferDownloadFolder().empty()) || (!dir.exists())) {

			downloadFolder = QFileDialog::getExistingDirectory(
				_qtFileTransferWidget,
				tr("Choose a directory"),
				QString::null,
				QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks
			);

			if (!downloadFolder.isEmpty()) {
				config.set(Config::FILETRANSFER_DOWNLOAD_FOLDER_KEY, std::string(downloadFolder.toUtf8().constData()));
			} else {
				//TODO: warn the user is has set no download folder.
				fileSession->stop();
				OWSAFE_DELETE(fileSession);
				return;
			}
		} else {
			downloadFolder = QString::fromUtf8(config.getFileTransferDownloadFolder().c_str());
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
		fileSession->setFilePath(std::string(downloadFolder.toUtf8().constData()));
		fileSession->start();

	// the user refuse the file transfer.
	} else {
		fileSession->stop();
		OWSAFE_DELETE(fileSession);
		return;
	}
}

void QtFileTransfer::createSendFileSession(IMContactSet imContactSet, const std::string & filename, CContactList & cContactList) {

	File file(filename);

	// check the file size
	if (file.getSize() < INT_MAX) {

		SendFileSession * fileSession = _coIpManager->getFileSessionManager().createSendFileSession();
		fileSession->addFile(file);

		for (IMContactSet::const_iterator it = imContactSet.begin(); it != imContactSet.end(); ++it) {
			std::string contactId = cContactList.findContactThatOwns(*it);
			fileSession->addContact(contactId);

			_qtFileTransferWidget->addSendItem(fileSession, filename, contactId, (*it).getContactId());
		}
		fileSession->start();

	} else {

		QMessageBox::warning(_qtFileTransferWidget, tr("File size error"),
			tr("%1 exceeds the maximum authorized size.").arg(QString::fromStdString(file.getFileName())),
			QMessageBox::Ok, 0, 0
		);
	}
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

void QtFileTransfer::needUpgradeEventHandler(FileSessionManager & sender) {
	needUpgradeEventHandlerSignal();
}

void QtFileTransfer::peerNeedsUpgradeEventHandler(FileSessionManager & sender) {
	peerNeedsUpgradeEventHandlerSignal();
}

void QtFileTransfer::needUpgradeEventHandlerSlot() {
	QtFileTransferUpgradeDialog qtFileTransferUpgradeDialog(_qtFileTransferWidget);
	qtFileTransferUpgradeDialog.setHeader(tr("<html><head><meta name=\"qrichtext\" content=\"1\" />"
		"</head><body style=\" white-space: pre-wrap; font-family:MS Shell Dlg; font-size:8.25pt;"
		"font-weight:400; font-style:normal; text-decoration:none;\"><p style=\" margin-top:0px;"
		"margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">"
		"<span style=\" font-size:18pt; font-weight:600; color:#ffffff;\">Please upgrade<br> your"
		"WengoPhone</span></p></body></html>"));
	qtFileTransferUpgradeDialog.setStatus(tr("<html><head><meta name=\"qrichtext\" content=\"1\" />"
		"</head><body style=\" white-space: pre-wrap; font-family:MS Shell Dlg; font-size:8.25pt;"
		"font-weight:400; font-style:normal; text-decoration:none;\"><p style=\" margin-top:0px;"
		"margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;"
		"font-size:8pt;\"><span style=\" font-weight:600;\">The file cannot be received:"
		"</span> you must upgrade your WengoPhone in order to receive it.</p></body></html>"));

	if (qtFileTransferUpgradeDialog.exec() == QDialog::Accepted) {

	} else {

	}
}

void QtFileTransfer::peerNeedsUpgradeEventHandlerSlot() {
	QtFileTransferUpgradeDialog qtFileTransferUpgradeDialog(_qtFileTransferWidget);
	qtFileTransferUpgradeDialog.setHeader(tr("<html><head><meta name=\"qrichtext\" content=\"1\" />"
		"</head><body style=\" white-space: pre-wrap; font-family:MS Shell Dlg; font-size:8.25pt;"
		"font-weight:400; font-style:normal; text-decoration:none;\"><p style=\" margin-top:0px;"
		"margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">"
		"<span style=\" font-size:18pt; font-weight:600; color:#ffffff;\">Tell your contact<br> to"
		"upgrade<br> his WengoPhone</span></p></body></html>"));
	qtFileTransferUpgradeDialog.setStatus(tr("<html><head><meta name=\"qrichtext\" content=\"1\" />"
		"</head><body style=\" white-space: pre-wrap; font-family:MS Shell Dlg; font-size:8.25pt;"
		"font-weight:400; font-style:normal; text-decoration:none;\"><p style=\" margin-top:0px;"
		"margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;"
		"font-size:8pt;\"><span style=\" font-weight:600;\">A contact is trying to send you a file:"
		"</span> but his WengoPhone must be upgraded in order to receive it. Tell him to download the latest version.</p></body></html>"));


	if (qtFileTransferUpgradeDialog.exec() == QDialog::Accepted) {

	} else {

	}
}

const QString QtFileTransfer::getChosenFile() const {

	Config & config = ConfigManager::getInstance().getCurrentConfig();
	QString startDir = QString::fromStdString(config.getLastUploadedFileFolder());

	QString filename = QFileDialog::getOpenFileName(
		_qtFileTransferWidget,
		"Choose a file",
		startDir,
		"All files (*)"
	);
	return filename;
}

