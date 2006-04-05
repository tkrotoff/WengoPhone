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

#include "QtSoftUpdate.h"

#include <presentation/qt/QtWengoPhone.h>

#include <control/CWengoPhone.h>
#include <control/webservices/softupdate/CSoftUpdate.h>

#include <softupdater/SoftUpdater.h>

#include <QtGui>

#include <util/Logger.h>

static const char * UPDATE_PROGRAM = "update.exe";

QtSoftUpdate::QtSoftUpdate(CSoftUpdate & cSoftUpdate)
	: QObjectThreadSafe(),
	_cSoftUpdate(cSoftUpdate) {

	_progressDialog = NULL;
	_qtWengoPhone = (QtWengoPhone *) _cSoftUpdate.getCWengoPhone().getPresentation();

	updateWengoPhoneEvent += boost::bind(&QtSoftUpdate::updateWengoPhoneEventHandler, this, _1, _2, _3, _4);

	typedef PostEvent0<void ()> MyPostEvent;
	MyPostEvent * event = new MyPostEvent(boost::bind(&QtSoftUpdate::initThreadSafe, this));
	postEvent(event);
}

void QtSoftUpdate::initThreadSafe() {
}

QtSoftUpdate::~QtSoftUpdate() {
	delete _softUpdater;
}

void QtSoftUpdate::updateWengoPhoneEventHandler(const std::string & downloadUrl,
				unsigned long long buildId,
				const std::string & version,
				unsigned fileSize) {

	typedef PostEvent4<void (std::string, unsigned long long, std::string, unsigned),
					std::string, unsigned long long, std::string, unsigned> MyPostEvent;
	MyPostEvent * event = new MyPostEvent(boost::bind(&QtSoftUpdate::updateWengoPhoneEventHandlerThreadSafe,
					this, _1, _2, _3, _4), downloadUrl, buildId, version, fileSize);
	postEvent(event);
}

void QtSoftUpdate::updateWengoPhoneEventHandlerThreadSafe(const std::string & downloadUrl,
				unsigned long long buildId,
				const std::string & version,
				unsigned fileSize) {

	_progressDialog = new QProgressDialog(_qtWengoPhone->getWidget());
	connect(_progressDialog, SIGNAL(canceled()), SLOT(abortDownload()));

	_originalLabelText = tr("Downloading WengoPhone update...\n") +
				tr("\nurl=") + QString::fromStdString(downloadUrl) +
				tr("\nversion=") + QString::fromStdString(version) +
				tr("\nfile size=%1").arg(fileSize);

	QLabel * label = new QLabel();
	label->setAlignment(Qt::AlignLeft);
	label->setPixmap(QPixmap(":pics/update.png"));
	_progressDialog->setLabel(label);
	_progressDialog->setLabelText(_originalLabelText);
	_progressDialog->setWindowTitle(tr("WengoPhone - Downloading update"));
	_progressDialog->setAutoClose(false);
	_progressDialog->setAutoReset(false);

	//Deletes previous update program
	QFile file(UPDATE_PROGRAM);
	file.remove();
	file.close();

	_softUpdater = new SoftUpdater();
	_softUpdater->dataReadProgressEvent += boost::bind(&QtSoftUpdate::dataReadProgressEventHandler, this, _1, _2, _3);
	_softUpdater->downloadFinishedEvent += boost::bind(&QtSoftUpdate::downloadFinishedEventHandler, this, _1);
	_softUpdater->download(downloadUrl, UPDATE_PROGRAM);

	_progressDialog->exec();
}

void QtSoftUpdate::dataReadProgressEventHandler(double bytesDone, double bytesTotal, unsigned downloadSpeed) {
	typedef PostEvent3<void (double, double, unsigned), double, double, unsigned> MyPostEvent;
	MyPostEvent * event = new MyPostEvent(boost::bind(&QtSoftUpdate::dataReadProgressEventHandlerThreadSafe, this, _1, _2, _3), bytesDone, bytesTotal, downloadSpeed);
	postEvent(event);
}

void QtSoftUpdate::dataReadProgressEventHandlerThreadSafe(double bytesDone, double bytesTotal, unsigned downloadSpeed) {
	LOG_DEBUG("progress=" + String::fromNumber(bytesDone));
	if (_progressDialog->wasCanceled()) {
		return;
	}

	_progressDialog->setLabelText(_originalLabelText + QString(" (%1").arg(downloadSpeed) + tr(" kB/s)"));
	_progressDialog->setRange(0, bytesTotal);
	_progressDialog->setValue(bytesDone);
	QApplication::processEvents();
}

void QtSoftUpdate::downloadFinishedEventHandler(HttpRequest::Error error) {
	typedef PostEvent1<void (HttpRequest::Error), HttpRequest::Error> MyPostEvent;
	MyPostEvent * event = new MyPostEvent(boost::bind(&QtSoftUpdate::downloadFinishedEventHandlerThreadSafe, this, _1), error);
	postEvent(event);
}

void QtSoftUpdate::downloadFinishedEventHandlerThreadSafe(HttpRequest::Error error) {
	LOG_DEBUG("download finished");

	if (error == HttpRequest::NoError) {
		//Terminates the thread from the model
		_cSoftUpdate.getCWengoPhone().terminate();

		launchUpdateProcess();
	} else {
		_progressDialog->cancel();
	}
}

void QtSoftUpdate::abortDownload() {
	_softUpdater->abort();
}

void QtSoftUpdate::launchUpdateProcess() {
	QProcess * updateProcess = new QProcess();
	connect(updateProcess, SIGNAL(error(QProcess::ProcessError)), SLOT(updateProcessError(QProcess::ProcessError)));
	updateProcess->start(UPDATE_PROGRAM);
}

void QtSoftUpdate::updateProcessError(QProcess::ProcessError error) {
	QMessageBox::critical(_qtWengoPhone->getWidget(),
			tr("WengoPhone - Update failed"),
			tr("WengoPhone update failed to start: try to update WengoPhone manually"));
}
