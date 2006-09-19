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

#include "QtFileTransferUploadItem.h"

#include <filesessionmanager/SendFileSession.h>

#include <util/Logger.h>
#include <qtutil/SafeConnect.h>
#include <QtGui/QtGui>

QtFileTransferUploadItem::QtFileTransferUploadItem(QWidget * parent, SendFileSession * fileSession, std::string filename, std::string contactId)
	:  QtFileTransferItem(parent), _sendFileSession(fileSession), _filename(filename), _contactId(contactId) {

	setFilename(QString::fromStdString(fileSession->getFileList()[0].getFileName()));

	// bind to fileSession events
	_sendFileSession->fileTransferProgressionEvent +=
		boost::bind(&QtFileTransferUploadItem::fileTransferProgressionEventHandler, this, _1, _2, _3, _4);
	_sendFileSession->fileTransferEvent +=
		boost::bind(&QtFileTransferUploadItem::fileTransferEventHandler, this, _1, _2, _3, _4);
	////
}

void QtFileTransferUploadItem::pause() {
	_sendFileSession->pause();
}

void QtFileTransferUploadItem::resume() {
	_sendFileSession->resume();
}

void QtFileTransferUploadItem::stop() {
	_sendFileSession->stop();
}

void QtFileTransferUploadItem::fileTransferProgressionEventHandler(
	SendFileSession & sender, IMContact imContact, File sentFile, int percentage) {

	LOG_DEBUG("fileTransferProgressionEventHandler: " + String::fromNumber(percentage));

	if ((sentFile.getFileName() == _filename) && (imContact.getContactId() == _contactId)) {
		progressChangeEvent(percentage);
	}
}

void QtFileTransferUploadItem::fileTransferEventHandler(
	SendFileSession & sender, IFileSession::IFileSessionEvent event, IMContact imContact, File sentFile) {

	LOG_DEBUG("fileTransferProgressionEventHandler: " + String::fromNumber((int)event));

	if ((sentFile.getFileName() == _filename) && (imContact.getContactId() == _contactId)) {
		updateStateEvent((int)event);
	}
}
