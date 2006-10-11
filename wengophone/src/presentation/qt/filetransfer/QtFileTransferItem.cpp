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

#include "QtFileTransferUpgradeDialog.h"

#include "ui_FileTransferItem.h"

#include <util/Logger.h>
#include <util/SafeDelete.h>
#include <util/WebBrowser.h>

#include <qtutil/SafeConnect.h>

#include <QtGui/QtGui>

QtFileTransferItem::QtFileTransferItem(QWidget * parent, Type type)
	: QWidget(parent),
	_type(type) {

	//init main widget
	_ui = new Ui::FileTransferItem();
	_ui->setupUi(this);
	_ui->progressBar->setMaximum(100);
	setProgress(0);
	setState(tr("Starting"));
	updateButtonsInProgress();
	////

	_removeClicked = false;

	// SIGNAL/SLOT for thread safe
	SAFE_CONNECT(this, SIGNAL(progressChangeEvent(int)),
		SLOT(setProgress(int)));
	SAFE_CONNECT(this, SIGNAL(stateChangeEvent(const QString &)),
		SLOT(setState(const QString &)));
	SAFE_CONNECT(this, SIGNAL(updateStateEvent(int)),
		SLOT(updateState(int)));
	////
}

QtFileTransferItem::~QtFileTransferItem() {
	OWSAFE_DELETE(_ui);
}

void QtFileTransferItem::setFilename(const QString & filename) {
	_ui->filenameLabel->setText(filename);
}

void QtFileTransferItem::setState(const QString & state) {
	_ui->statusLabel->setText(state);
}

void QtFileTransferItem::setContact(const QString & contact) {
	if (_type == Download) {
		_ui->contactLabel->setText("  " + tr("From: ") + contact);
	} else {
		_ui->contactLabel->setText("  " + tr("To: ") + contact);
	}
}

void QtFileTransferItem::setFileSize(int size) {
}

void QtFileTransferItem::setProgress(int progress) {
	_ui->progressBar->setValue(progress);
	stateChangeEventDownUp();
}

void QtFileTransferItem::updateButtonsFinished() {
	disconnectButtons();
	SAFE_CONNECT(_ui->cancelOpenButton, SIGNAL(clicked()), SLOT(open()));
	SAFE_CONNECT(_ui->removePauseResumeButton, SIGNAL(clicked()), SLOT(remove()));
	_ui->cancelOpenButton->setText(tr("Open"));
	_ui->removePauseResumeButton->setText(tr("Remove"));
	_ui->removePauseResumeButton->setEnabled(true);
	_ui->cancelOpenButton->setEnabled(true);
	//FIXME: remove the status bar from its layout
	//_ui->progressBar->hide();
}

void QtFileTransferItem::updateButtonsPaused() {
	disconnectButtons();
	SAFE_CONNECT(_ui->cancelOpenButton, SIGNAL(clicked()), SLOT(stop()));
	SAFE_CONNECT(_ui->removePauseResumeButton, SIGNAL(clicked()), SLOT(resume()));
	_ui->cancelOpenButton->setText(tr("Cancel"));
	_ui->removePauseResumeButton->setText(tr("Resume"));
	_ui->removePauseResumeButton->setEnabled(true);
	_ui->cancelOpenButton->setEnabled(false);
}

void QtFileTransferItem::updateButtonsPausedByPeer() {
	disconnectButtons();
	SAFE_CONNECT(_ui->cancelOpenButton, SIGNAL(clicked()), SLOT(stop()));
	SAFE_CONNECT(_ui->removePauseResumeButton, SIGNAL(clicked()), SLOT(pause()));
	_ui->cancelOpenButton->setText(tr("Cancel"));
	_ui->removePauseResumeButton->setText(tr("Pause"));
	_ui->removePauseResumeButton->setEnabled(false);
	_ui->cancelOpenButton->setEnabled(true);
}

void QtFileTransferItem::updateButtonsInProgress() {
	disconnectButtons();
	SAFE_CONNECT(_ui->cancelOpenButton, SIGNAL(clicked()), SLOT(stop()));
	SAFE_CONNECT(_ui->removePauseResumeButton, SIGNAL(clicked()), SLOT(pause()));
	_ui->cancelOpenButton->setText(tr("Cancel"));
	_ui->removePauseResumeButton->setText(tr("Pause"));
	_ui->removePauseResumeButton->setEnabled(true);
	_ui->cancelOpenButton->setEnabled(true);
}

void QtFileTransferItem::disconnectButtons() {
	_ui->cancelOpenButton->disconnect(SIGNAL(clicked()));
	_ui->removePauseResumeButton->disconnect(SIGNAL(clicked()));
}

void QtFileTransferItem::remove() {
	//TODO:
	_removeClicked = true;
	removeClicked();
}

void QtFileTransferItem::open() {
	LOG_DEBUG("open from file transfer manager: " + _filename.toStdString());
#ifdef OS_WINDOWS
	WebBrowser::openUrl(_filename.toStdString());
#endif
}

void QtFileTransferItem::updateState(int e) {

	IFileSession::IFileSessionEvent event = (IFileSession::IFileSessionEvent) e;
	QtFileTransferUpgradeDialog  * qtFileTransferUpgradeDialog;

	switch(event) {
	case IFileSession::IFileSessionEventInviteToTransfer:
		stateChangeEvent(tr("Starting"));
		updateButtonsInProgress();
		break;
	case IFileSession::IFileSessionEventWaitingForAnswer:
		stateChangeEvent(tr("Waiting for anwser..."));
		updateButtonsInProgress();
		break;
	case IFileSession::IFileSessionEventFileTransferFinished:
		stateChangeEvent(tr("Done"));
		updateButtonsFinished();
		break;
	case IFileSession::IFileSessionEventFileTransferFailed:
		stateChangeEvent(tr("Failed"));
		updateButtonsFinished();
		qtFileTransferUpgradeDialog = new QtFileTransferUpgradeDialog(this);
		qtFileTransferUpgradeDialog->setHeader("<html><head><meta name=\"qrichtext\" content=\"1\" />"
			"</head><body style=\" white-space: pre-wrap; font-family:MS Shell Dlg; font-size:8.25pt;"
			"font-weight:400; font-style:normal; text-decoration:none;\"><p style=\" margin-top:0px;"
			"margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">"
			"<span style=\" font-size:18pt; font-weight:600; color:#ffffff;\">An error<br> occured during<br>"
			"the file transfer</span></p></body></html>");
		qtFileTransferUpgradeDialog->setStatus("<html><head><meta name=\"qrichtext\" content=\"1\" />"
			"</head><body style=\" white-space: pre-wrap; font-family:MS Shell Dlg; font-size:8.25pt;"
			"font-weight:400; font-style:normal; text-decoration:none;\"><p style=\" margin-top:0px;"
			"margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;"
			"font-size:8pt;\"><span style=\" font-weight:600;\">This may be caused by:"
			"</span> <br>- Your WengoPhone is not up to date. Please download the latest version on"
			"www.wengo.com<br>- The Wengo network may be temporarily unavailable. Please try later.</p></body></html>");

		if (qtFileTransferUpgradeDialog->exec() == QDialog::Accepted) {

		} else {

		}

		OWSAFE_DELETE(qtFileTransferUpgradeDialog);
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
		stateChangeEventDownUp();
		updateButtonsInProgress();
		break;
	case IFileSession::IFileSessionEventFileTransferResumedByPeer:
		stateChangeEventDownUp();
		updateButtonsInProgress();
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
		stateChangeEventDownUp();
		updateButtonsInProgress();
		break;
	default:
		LOG_FATAL("unknonw IFileSessionEvent: " + String::fromNumber(event));
	}
}

void QtFileTransferItem::stateChangeEventDownUp() {
	if (_type == Download) {
		stateChangeEvent(tr("Downloading..."));
	} else {
		stateChangeEvent(tr("Uploading..."));
	}
}
