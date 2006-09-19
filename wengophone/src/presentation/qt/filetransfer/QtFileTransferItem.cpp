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

#include <util/Logger.h>

#include <qtutil/SafeConnect.h>

#include <QtGui/QtGui>

QtFileTransferItem::QtFileTransferItem(QWidget * parent) : QWidget(parent) {

	//init main widget
	_ui.setupUi(this);
	_ui.progressBar->setMaximum(100);
	setProgress(0);
	setState(tr("Starting"));
	updateButtonsDownloading();
	////

	// SIGNAL/SLOT for thread safe
	SAFE_CONNECT(this, SIGNAL(progressChangeEvent(int)),
		SLOT(setProgress(int)));
	SAFE_CONNECT(this, SIGNAL(stateChangeEvent(const QString &)),
		SLOT(setState(const QString &)));
	SAFE_CONNECT(this, SIGNAL(updateStateEvent(int)),
		SLOT(updateState(int)));
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
	_ui.removePauseResumeButton->setEnabled(true);
	_ui.cancelOpenButton->setEnabled(true);
	//FIXME: remove the status bar from its layout
	//_ui.progressBar->hide();
}

void QtFileTransferItem::updateButtonsPaused() {
	disconnectButtons();
	SAFE_CONNECT(_ui.cancelOpenButton, SIGNAL(clicked()), SLOT(stop()));
	SAFE_CONNECT(_ui.removePauseResumeButton, SIGNAL(clicked()), SLOT(resume()));
	_ui.cancelOpenButton->setText(tr("Cancel"));
	_ui.removePauseResumeButton->setText(tr("Resume"));
	_ui.removePauseResumeButton->setEnabled(true);
	_ui.cancelOpenButton->setEnabled(false);
}

void QtFileTransferItem::updateButtonsPausedByPeer() {
	disconnectButtons();
	SAFE_CONNECT(_ui.cancelOpenButton, SIGNAL(clicked()), SLOT(stop()));
	SAFE_CONNECT(_ui.removePauseResumeButton, SIGNAL(clicked()), SLOT(pause()));
	_ui.cancelOpenButton->setText(tr("Cancel"));
	_ui.removePauseResumeButton->setText(tr("Pause"));
	_ui.removePauseResumeButton->setEnabled(false);
	_ui.cancelOpenButton->setEnabled(true);
}

void QtFileTransferItem::updateButtonsDownloading() {
	disconnectButtons();
	SAFE_CONNECT(_ui.cancelOpenButton, SIGNAL(clicked()), SLOT(stop()));
	SAFE_CONNECT(_ui.removePauseResumeButton, SIGNAL(clicked()), SLOT(pause()));
	_ui.cancelOpenButton->setText(tr("Cancel"));
	_ui.removePauseResumeButton->setText(tr("Pause"));
	_ui.removePauseResumeButton->setEnabled(true);
	_ui.cancelOpenButton->setEnabled(true);
}

void QtFileTransferItem::disconnectButtons() {
	_ui.cancelOpenButton->disconnect(SIGNAL(clicked()));
	_ui.removePauseResumeButton->disconnect(SIGNAL(clicked()));
}

void QtFileTransferItem::remove() {
	//TODO:
}

void QtFileTransferItem::open() {
	//TODO:
}

void QtFileTransferItem::updateState(int e) {

	IFileSession::IFileSessionEvent event = (IFileSession::IFileSessionEvent)e;

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
			stateChangeEvent(tr("Done"));
			updateButtonsFinished();
			break;
		case IFileSession::IFileSessionEventFileTransferFailed:
			stateChangeEvent(tr("Failed"));
			updateButtonsFinished();
			break;
		case IFileSession::IFileSessionEventFileTransferPaused:
			stateChangeEvent(tr("Paused"));
			updateButtonsPaused();
			break;
		case IFileSession::IFileSessionEventFileTransferPausedByPeer:
			stateChangeEvent(tr("Paused by peer"));
			updateButtonsPausedByPeer();
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
			stateChangeEvent(tr("Cancelled"));
			updateButtonsFinished();
			break;
		case IFileSession::IFileSessionEventFileTransferCancelledByPeer:
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
