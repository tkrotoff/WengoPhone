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

#include "SoftUpdater.h"

#include <qapplication.h>
#include <qptrlist.h>
#include <qmessagebox.h>
#include <qurloperator.h>
#include <qnetwork.h>
#include <qnetworkprotocol.h>
#include <qprogressdialog.h>

#include <cstdlib>

SoftUpdater::SoftUpdater(const QString & sourceUrl, const QString & destinationUrl) {
	downloadFile(sourceUrl, destinationUrl);
}

SoftUpdater::~SoftUpdater() {
	delete _operationList;
	delete _operator;
	delete _progress;
}

void SoftUpdater::downloadFile(const QString & sourceUrl, const QString & destinationUrl) {
	_progress = new QProgressDialog(tr("From: %1\nTo: %2").arg(sourceUrl).arg(destinationUrl),
						tr("Cancel"),
						0,
						NULL,
						"download progress dialog",
						true);
	_progress->setCaption(tr("SoftUpdater"));

	qInitNetworkProtocols();
	_operator = new QUrlOperator();

	connect(_operator, SIGNAL(dataTransferProgress(int, int, QNetworkOperation *)), SLOT(transferProgress(int, int, QNetworkOperation *)));
	connect(_operator, SIGNAL(finished(QNetworkOperation *)), SLOT(commandFinished(QNetworkOperation *)));
	connect(_progress, SIGNAL(cancelled()), SLOT(stop()));

	_operationList = new OperationList(_operator->copy(sourceUrl, destinationUrl, false, false));

	//Takes a lot of time
	_progress->exec();
}

void SoftUpdater::transferProgress(int bytesDone, int bytesTotal, QNetworkOperation *) {
	_progress->setProgress(bytesDone, bytesTotal);
}

void SoftUpdater::commandFinished(QNetworkOperation * op) {
	//Removes the progress dialog
	_progress->reset();

	int state = op->state();

	switch (state) {
	case QNetworkProtocol::StDone:
		//OK
		if (_operationList->getLast() == op) {
			//Exit only if all this operations are done
			//e.g. the last operation has been completed
			//exit(0);
			QApplication::exit();
		}
		break;

	default:
		//Error
		/*QMessageBox::critical(NULL, tr("Download error"),
				tr("Download failed: %1").arg(op->protocolDetail()));*/
		exit(1);
		//QApplication::exit(1);
	}
}

void SoftUpdater::stop() {
	_operator->stop();
}
