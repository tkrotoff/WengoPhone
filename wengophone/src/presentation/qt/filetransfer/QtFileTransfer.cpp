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

#include <coipmanager/CoIpManager.h>
#include <filesessionmanager/ReceiveFileSession.h>

#include <util/Macro.h>

#include <QtGui/QtGui>

QtFileTransfer::QtFileTransfer(QObject * parent, CoIpManager * coIpManager)
	: QObject(parent), _coIpManager(coIpManager) {


	connect(this, SIGNAL(newReceiveFileSessionCreatedEventHandlerSignal(ReceiveFileSession *)),
		SLOT(newReceiveFileSessionCreatedEventHandlerSlot(ReceiveFileSession *)));
	_coIpManager->getFileSessionManager().newReceiveFileSessionCreatedEvent +=
		boost::bind(&QtFileTransfer::newReceiveFileSessionCreatedEventHandler, this, _1, _2);
}

QtFileTransfer::~QtFileTransfer() {
}

void QtFileTransfer::newReceiveFileSessionCreatedEventHandler(FileSessionManager & sender, 
	ReceiveFileSession * fileSession) {

	newReceiveFileSessionCreatedEventHandlerSignal(fileSession);
}

void QtFileTransfer::newReceiveFileSessionCreatedEventHandlerSlot(ReceiveFileSession * fileSession) {
	QtFileTransferAcceptDialog qtFileTransferAcceptDialog(0);
	qtFileTransferAcceptDialog.setFileName(fileSession->getFileName());
	qtFileTransferAcceptDialog.setContactName(fileSession->getIMContact().getContactId());

	if (qtFileTransferAcceptDialog.exec() == QDialog::Accepted) {
		QString dir = QFileDialog::getExistingDirectory(
			0,
			tr("Choose a directory"), 
			"",
			QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks
		);

		if (!dir.isEmpty()) {
			fileSession->setFilePath(dir.toStdString());
			fileSession->start();
		} else {
			fileSession->stop();
			SAFE_DELETE(fileSession);
		}
	}
}
