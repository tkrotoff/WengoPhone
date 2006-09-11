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

#include "QtFileTransferDownloadItem.h"

#include <filesessionmanager/ReceiveFileSession.h>

#include <util/Logger.h>

#include <qtutil/SafeConnect.h>

#include <QtGui/QtGui>

QtFileTransferDownloadItem::QtFileTransferDownloadItem(QWidget * parent, ReceiveFileSession * fileSession)
	: QtFileTransferItem(parent), _receiveFileSession(fileSession) {

	setFilename(QString::fromStdString(fileSession->getFileName()));

	// bind to fileSession events
	_receiveFileSession->fileTransferProgressionEvent +=
		boost::bind(&QtFileTransferDownloadItem::fileTransferProgressionEventHandler, this, _1, _2, _3, _4);
	_receiveFileSession->fileTransferEvent +=
		boost::bind(&QtFileTransferDownloadItem::fileTransferEventHandler, this, _1, _2, _3, _4);
	////
}

void QtFileTransferDownloadItem::pause() {
	_receiveFileSession->pause();
}

void QtFileTransferDownloadItem::resume() {
	_receiveFileSession->resume();
}

void QtFileTransferDownloadItem::stop() {
	_receiveFileSession->stop();
}

void QtFileTransferDownloadItem::fileTransferProgressionEventHandler(
	ReceiveFileSession & sender, IMContact imContact, File sentFile, int percentage) {

	LOG_DEBUG("progress: " + String::fromNumber(percentage));
	progressChangeEvent(percentage);
}

void QtFileTransferDownloadItem::fileTransferEventHandler(ReceiveFileSession & sender,
	IFileSession::IFileSessionEvent event, IMContact imContact, File sentFile) {

	LOG_DEBUG("filetransfer event: " + String::fromNumber(event));

	updateStateEvent((int)event);
}