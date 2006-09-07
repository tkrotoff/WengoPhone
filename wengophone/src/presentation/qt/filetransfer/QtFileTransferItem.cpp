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

#include "QtFileTransferItem.h"

#include <filesessionmanager/ReceiveFileSession.h>

#include <util/Logger.h>

#include <qtutil/SafeConnect.h>

#include <QtGui/QtGui>

QtFileTransferItem::QtFileTransferItem(QWidget * parent, ReceiveFileSession * fileSession)
	: QWidget(parent), _fileSession(fileSession) {

	_isFinished = false;
	_upload = false;

	//init main widget
	_ui.setupUi(this);
	_ui.progressBar->setMaximum(100);
	setProgress(0);
	setFilename(QString::fromStdString(fileSession->getFileName()));
	setState(tr("Starting"));
	updateButtonsDownloading();
	////

	// bind to fileSession events
	_fileSession->fileTransferProgressionEvent +=
		boost::bind(&QtFileTransferItem::fileTransferProgressionEventHandler, this, _1, _2, _3, _4);
	_fileSession->fileTransferEvent +=
		boost::bind(&QtFileTransferItem::fileTransferEventHandler, this, _1, _2, _3, _4);
	////

	// SIGNAL/SLOT for thread safe
	SAFE_CONNECT(this, SIGNAL(progressChangeEvent(int)), SLOT(setProgress(int)));
	SAFE_CONNECT(this, SIGNAL(stateChangeEvent(const QString &)), SLOT(setState(const QString &)));
	////
}

void QtFileTransferItem::setFilename(const QString & filename) {
	_ui.filenameLabel->setText(filename);
}

void QtFileTransferItem::setState(const QString & state) {
	_ui.statusLabel->setText(state);
}

void QtFileTransferItem::setFileSize(int size) {
}

void QtFileTransferItem::setProgress(int progress) {
	_ui.progressBar->setValue(progress);
}

void QtFileTransferItem::updateButtonsFinished() {
	disconnectButtons();
	SAFE_CONNECT(_ui.cancelOpenButton, SIGNAL(clicked()), SLOT(open()));
	SAFE_CONNECT(_ui.removePauseResumeButton, SIGNAL(clicked()), SLOT(remove()));
	_ui.cancelOpenButton->setText(tr("Open"));
	_ui.removePauseResumeButton->setText(tr("Remove"));
}

void QtFileTransferItem::updateButtonsPaused() {
	disconnectButtons();
	SAFE_CONNECT(_ui.cancelOpenButton, SIGNAL(clicked()), SLOT(stop()));
	SAFE_CONNECT(_ui.removePauseResumeButton, SIGNAL(clicked()), SLOT(resume()));
	_ui.cancelOpenButton->setText(tr("Cancel"));
	_ui.removePauseResumeButton->setText(tr("Resume"));
}

void QtFileTransferItem::updateButtonsDownloading() {
	disconnectButtons();
	SAFE_CONNECT(_ui.cancelOpenButton, SIGNAL(clicked()), SLOT(stop()));
	SAFE_CONNECT(_ui.removePauseResumeButton, SIGNAL(clicked()), SLOT(pause()));
	_ui.cancelOpenButton->setText(tr("Cancel"));
	_ui.removePauseResumeButton->setText(tr("Pause"));
}
	
void QtFileTransferItem::disconnectButtons() {
	_ui.cancelOpenButton->disconnect(SIGNAL(clicked()));
	_ui.removePauseResumeButton->disconnect(SIGNAL(clicked()));
}

void QtFileTransferItem::pause() {
	_fileSession->pause();
}

void QtFileTransferItem::resume() {
	_fileSession->resume();
}

void QtFileTransferItem::stop() {
	_fileSession->stop();
}

void QtFileTransferItem::remove() {
	//TODO:
}

void QtFileTransferItem::open() {
	//TODO:
}

void QtFileTransferItem::fileTransferProgressionEventHandler(
	ReceiveFileSession & sender, IMContact imContact, File sentFile, int percentage) {

	LOG_DEBUG("progress: " + String::fromNumber(percentage));
	progressChangeEvent(percentage);
}

void QtFileTransferItem::fileTransferEventHandler(ReceiveFileSession & sender,
	IFileSession::IFileSessionEvent event, IMContact imContact, File sentFile) {

	LOG_DEBUG("filetransfer event: " + String::fromNumber(event));

	switch(event) {

		case IFileSession::IFileSessionEventInviteToTransfer:
			stateChangeEvent(tr("Starting"));
			updateButtonsDownloading();
			break;
		case IFileSession::IFileSessionEventWaitingForAnswer:
			stateChangeEvent(tr("Waiting for anwser..."));
			updateButtonsDownloading();
			break;
		case IFileSession::IFileSessionEventFileTransferFinished:
			_isFinished = true;
			stateChangeEvent(tr("Done"));
			updateButtonsFinished();
			break;
		case IFileSession::IFileSessionEventFileTransferFailed:
			_isFinished = true;
			stateChangeEvent(tr("Failed"));
			updateButtonsFinished();
			break;
		case IFileSession::IFileSessionEventFileTransferPaused:
			stateChangeEvent(tr("Paused"));
			updateButtonsPaused();
			break;
		case IFileSession::IFileSessionEventFileTransferPausedByPeer:
			stateChangeEvent(tr("Paused by peer"));
			updateButtonsPaused();
			break;
		case IFileSession::IFileSessionEventFileTransferResumed:
			stateChangeEvent(tr("Downloading..."));
			updateButtonsDownloading();
			break;
		case IFileSession::IFileSessionEventFileTransferResumedByPeer:
			stateChangeEvent(tr("Downloading..."));
			updateButtonsDownloading();
			break;
		case IFileSession::IFileSessionEventFileTransferCancelled:
			_isFinished = true;
			stateChangeEvent(tr("Cancelled"));
			updateButtonsFinished();
			break;
		case IFileSession::IFileSessionEventFileTransferCancelledByPeer:
			_isFinished = true;
			stateChangeEvent(tr("Cancelled by peer"));
			updateButtonsFinished();
			break;
		case IFileSession::IFileSessionEventFileTransferBegan:
			stateChangeEvent(tr("Downloading..."));
			updateButtonsDownloading();
			break;
		default:
			LOG_FATAL("unknonw IFileSessionEvent: " + String::fromNumber(event));
	}
}
