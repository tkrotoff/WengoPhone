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

#include "ui_FileTransferWidget.h"

#include "QtFileTransferDownloadItem.h"
#include "QtFileTransferUploadItem.h"

#include <filesessionmanager/ReceiveFileSession.h>
#include <filesessionmanager/SendFileSession.h>

#include <model/config/ConfigManager.h>
#include <model/config/Config.h>

#include <util/Logger.h>
#include <util/SafeDelete.h>

#include <qtutil/SafeConnect.h>

#include <QtGui/QtGui>

static int DOWNLOAD_TAB_INDEX = 0;
static int UPLOAD_TAB_INDEX = 1;

QtFileTransferWidget::QtFileTransferWidget(QWidget * parent)
	: QWidget(parent) {

	//init main widget
	_ui = new Ui::FileTransferWidget();
	_ui->setupUi(this);
	////

#ifdef OS_LINUX
	setWindowIcon(QIcon(QPixmap(":/pics/avatar_default.png")));
#endif

	Config & config = ConfigManager::getInstance().getCurrentConfig();
	if (!config.getFileTransferDownloadFolder().empty()) {
		setDownloadFolder(QString::fromUtf8(config.getFileTransferDownloadFolder().c_str()));
	}

	//connect signals to slots
	SAFE_CONNECT(_ui->cleanButton, SIGNAL(pressed()), SLOT(cleanButtonClicked()));
	SAFE_CONNECT(_ui->pathButton, SIGNAL(pressed()), SLOT(pathButtonClicked()));
	////
}

QtFileTransferWidget::~QtFileTransferWidget() {
	OWSAFE_DELETE(_ui);
}

void QtFileTransferWidget::cleanButtonClicked() {
	clean(true);
}

void QtFileTransferWidget::clean(bool cleanButton) {

	if (_ui->tabWidget->currentIndex() == DOWNLOAD_TAB_INDEX) {

		for (int i = 0; i < _ui->downloadTransferListWidget->count(); i++) {
			QListWidgetItem * item = _ui->downloadTransferListWidget->item(i);
			QtFileTransferDownloadItem * widgetItem = (QtFileTransferDownloadItem*)_ui->downloadTransferListWidget->itemWidget(item);

			if (cleanButton) {
				if (!widgetItem->isRunning()) {
					_ui->downloadTransferListWidget->takeItem(i);
					OWSAFE_DELETE(widgetItem);
				}
			} else {
				if ((widgetItem->removeHasBeenClicked()) && (!widgetItem->isRunning())) {
					_ui->downloadTransferListWidget->takeItem(i);
					OWSAFE_DELETE(widgetItem);
				}
			}
		}

	} else {

		for (int i = 0; i < _ui->uploadTransferListWidget->count(); i++) {
			QListWidgetItem * item = _ui->uploadTransferListWidget->item(i);
			QtFileTransferUploadItem * widgetItem = (QtFileTransferUploadItem*)_ui->uploadTransferListWidget->itemWidget(item);

			if (cleanButton) {
				if (!widgetItem->isRunning()) {
					_ui->uploadTransferListWidget->takeItem(i);
					OWSAFE_DELETE(widgetItem);
				}
			} else {
				if ((widgetItem->removeHasBeenClicked()) && (!widgetItem->isRunning())) {
					_ui->uploadTransferListWidget->takeItem(i);
					OWSAFE_DELETE(widgetItem);
				}
			}
		}
	}

}

void QtFileTransferWidget::pathButtonClicked() {

	Config & config = ConfigManager::getInstance().getCurrentConfig();
	QString startFolder = QString::fromUtf8(config.getFileTransferDownloadFolder().c_str());
	QString dirName = QFileDialog::getExistingDirectory(this, "Select a directory", startFolder);
	if (!dirName.isEmpty()) {
		setDownloadFolder(dirName);
	}
}

void QtFileTransferWidget::addReceiveItem(ReceiveFileSession * fileSession) {

	QtFileTransferDownloadItem * fileTransferItem = new QtFileTransferDownloadItem(this, fileSession, _downloadFolder);
	SAFE_CONNECT(fileTransferItem, SIGNAL(removeClicked()), SLOT(itemRemoveClicked()));
	QListWidgetItem * item = new QListWidgetItem(/*_ui->downloadTransferListWidget*/);
	item->setSizeHint(fileTransferItem->minimumSizeHint());
	_ui->downloadTransferListWidget->insertItem(0, item);
	_ui->downloadTransferListWidget->setItemWidget(item, fileTransferItem);
	showDownloadTab();
	showAndRaise();
}

void QtFileTransferWidget::addSendItem(SendFileSession * fileSession,
	const std::string & filename, const std::string & contactId, const std::string & contact) {

	QtFileTransferUploadItem * fileTransferItem = new QtFileTransferUploadItem(this, fileSession,
			QString::fromUtf8(filename.c_str()), contactId, contact);
	SAFE_CONNECT(fileTransferItem, SIGNAL(removeClicked()), SLOT(itemRemoveClicked()));
	QListWidgetItem * item = new QListWidgetItem(/*_ui->uploadTransferListWidget*/);
	item->setSizeHint(fileTransferItem->minimumSizeHint());
	_ui->uploadTransferListWidget->insertItem(0, item);
	_ui->uploadTransferListWidget->setItemWidget(item, fileTransferItem);
	showUploadTab();
	showAndRaise();
}

void QtFileTransferWidget::setDownloadFolder(const QString & folder) {
	Config & config = ConfigManager::getInstance().getCurrentConfig();
	//config.set(Config::FILETRANSFER_DOWNLOAD_FOLDER, folder.toUtf8().constData());
	config.set(Config::FILETRANSFER_DOWNLOAD_FOLDER, std::string(folder.toUtf8().constData()));
	QDir dir(folder);
	_ui->pathButton->setText(dir.dirName());
	_downloadFolder = folder;
}

void QtFileTransferWidget::showDownloadTab() {
	_ui->tabWidget->setCurrentIndex(DOWNLOAD_TAB_INDEX);
	_ui->downloadTransferListWidget->scrollToTop();
}

void QtFileTransferWidget::showUploadTab() {
	_ui->tabWidget->setCurrentIndex(UPLOAD_TAB_INDEX);
	_ui->uploadTransferListWidget->scrollToTop();
}

void QtFileTransferWidget::itemRemoveClicked() {
	clean(false);
}

void QtFileTransferWidget::showAndRaise() {
	showMinimized();
	activateWindow();
	showNormal();
}
