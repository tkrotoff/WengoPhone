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

#include "QtFileTransferWidget.h"
#include "QtFileTransferDownloadItem.h"
#include "QtFileTransferUploadItem.h"

#include <filesessionmanager/ReceiveFileSession.h>
#include <filesessionmanager/SendFileSession.h>

#include <model/config/ConfigManager.h>
#include <model/config/Config.h>

#include <qtutil/WidgetBackgroundImage.h>

#include <QtGui/QtGui>

QtFileTransferWidget::QtFileTransferWidget(QWidget * parent) : QWidget(parent) {

	//init main widget
	_ui.setupUi(this);
	////

	Config & config = ConfigManager::getInstance().getCurrentConfig();
	if (!config.getFileTransferDownloadFolder().empty()) {
		setDownloadFolder(QString::fromStdString(config.getFileTransferDownloadFolder()));
	}

	//connect signals to slots
	connect(_ui.cleanButton, SIGNAL(pressed()), this, SLOT(cleanButtonClicked()));
	connect(_ui.pathButton, SIGNAL(pressed()), this, SLOT(pathButtonClicked()));
	////

	//install header image
	//WidgetBackgroundImage::setBackgroundImage(_ui.headerLabel, ":pics/headers/file-tranfer.png", true);
	////
}

void QtFileTransferWidget::cleanButtonClicked() {

	_ui.downloadTransferListWidget->clear();
	//delete items and widgetItem over rows
	for (int i = 0; i < _ui.downloadTransferListWidget->count(); i++) {
		QListWidgetItem * item = _ui.downloadTransferListWidget->item(i);
		QtFileTransferDownloadItem * widgetItem = (QtFileTransferDownloadItem*)_ui.downloadTransferListWidget->itemWidget(item);
		_ui.downloadTransferListWidget->takeItem(i);
		delete widgetItem;
	}
}

void QtFileTransferWidget::pathButtonClicked() {

	Config & config = ConfigManager::getInstance().getCurrentConfig();
	QString startFolder = QString::fromStdString(config.getFileTransferDownloadFolder());
	QString dirName = QFileDialog::getExistingDirectory(this, "Select a directory", startFolder);
	if (!dirName.isEmpty()) {
		setDownloadFolder(dirName);
	}
}

void QtFileTransferWidget::addReceiveItem(ReceiveFileSession * fileSession) {

	QtFileTransferDownloadItem * fileTransferItem = new QtFileTransferDownloadItem(this, fileSession);
	QListWidgetItem * item = new QListWidgetItem(_ui.downloadTransferListWidget);
	item->setSizeHint(fileTransferItem->minimumSizeHint());
	_ui.downloadTransferListWidget->setItemWidget(item, fileTransferItem);
	show();
}

void QtFileTransferWidget::addSendItem(SendFileSession * fileSession) {

	//TODO: put this code in QtFileTransfer

	std::vector<File> fileList =  fileSession->getFileList();
	
	std::vector<File>::iterator it;
	for (it = fileList.begin(); it != fileList.end(); it++) {
		std::string filename = (*it).getFileName();

		StringList contactList = fileSession->getContactList();
		for(int i = 0; i < contactList.size(); i++) {
			QtFileTransferUploadItem * fileTransferItem = new QtFileTransferUploadItem(
				this,
				fileSession,
				filename,
				contactList[i]
			);

			QListWidgetItem * item = new QListWidgetItem(_ui.uploadTransferListWidget);
			item->setSizeHint(fileTransferItem->minimumSizeHint());
			_ui.uploadTransferListWidget->setItemWidget(item, fileTransferItem);
			show();

		}
	}
}

void QtFileTransferWidget::setDownloadFolder(const QString & folder) {
	Config & config = ConfigManager::getInstance().getCurrentConfig();
	config.set(Config::FILETRANSFER_DOWNLOAD_FOLDER, folder.toStdString());
	QDir dir(folder);
	_ui.pathButton->setText(dir.dirName());
}
