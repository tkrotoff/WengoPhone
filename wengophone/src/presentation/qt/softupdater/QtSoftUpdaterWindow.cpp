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

#include "QtSoftUpdaterWindow.h"

#include <control/CWengoPhone.h>

#include <softupdater/SoftUpdater.h>

#include <QtGui>

#include <util/Logger.h>

static const char * UPDATE_PROGRAM = "update.exe";

QtSoftUpdaterWindow::QtSoftUpdaterWindow(CWengoPhone & cWengoPhone, QWidget * parent)
	: QObjectThreadSafe(),
	_cWengoPhone(cWengoPhone) {

	_parent = parent;
	_progressDialog = new QProgressDialog(_parent);
	connect(_progressDialog, SIGNAL(canceled()), SLOT(abortDownload()));
	_originalLabelText = tr("Downloading WengoPhone update...");
	_progressDialog->setLabelText(_originalLabelText);
	_progressDialog->setWindowTitle(tr("WengoPhone - Downloading update"));
	_progressDialog->setAutoClose(false);
	_progressDialog->setAutoReset(false);

	//Deletes previous update program
	QFile file(UPDATE_PROGRAM);
	file.remove();
	file.close();

	_softUpdater = new SoftUpdater();
	_softUpdater->dataReadProgressEvent += boost::bind(&QtSoftUpdaterWindow::dataReadProgressEventHandler, this, _1, _2, _3);
	_softUpdater->downloadFinishedEvent += boost::bind(&QtSoftUpdaterWindow::downloadFinishedEventHandler, this, _1);
	/*_softUpdater->download(
			"http://wengofiles.wengo.fr/nightlybuilds/installer/NG/win32/WengoPhoneNG-setup-release-symbols-20060326160934.exe",
			UPDATE_PROGRAM);

	_progressDialog->exec();*/
}

void QtSoftUpdaterWindow::initThreadSafe() {
}

QtSoftUpdaterWindow::~QtSoftUpdaterWindow() {
	delete _softUpdater;
}

void QtSoftUpdaterWindow::dataReadProgressEventHandler(double bytesDone, double bytesTotal, unsigned downloadSpeed) {
	typedef PostEvent3<void (double, double, unsigned), double, double, unsigned> MyPostEvent;
	MyPostEvent * event = new MyPostEvent(boost::bind(&QtSoftUpdaterWindow::dataReadProgressEventHandlerThreadSafe, this, _1, _2, _3), bytesDone, bytesTotal, downloadSpeed);
	postEvent(event);
}

void QtSoftUpdaterWindow::dataReadProgressEventHandlerThreadSafe(double bytesDone, double bytesTotal, unsigned downloadSpeed) {
	LOG_DEBUG("progress=" + String::fromNumber(bytesDone));
	if (_progressDialog->wasCanceled()) {
		return;
	}

	_progressDialog->setLabelText(_originalLabelText + QString(" (%1").arg(downloadSpeed) + tr(" kB/s)"));
	_progressDialog->setRange(0, bytesTotal);
	_progressDialog->setValue(bytesDone);
	QApplication::processEvents();
}

void QtSoftUpdaterWindow::downloadFinishedEventHandler(HttpRequest::Error error) {
	typedef PostEvent1<void (HttpRequest::Error), HttpRequest::Error> MyPostEvent;
	MyPostEvent * event = new MyPostEvent(boost::bind(&QtSoftUpdaterWindow::downloadFinishedEventHandlerThreadSafe, this, _1), error);
	postEvent(event);
}

void QtSoftUpdaterWindow::downloadFinishedEventHandlerThreadSafe(HttpRequest::Error error) {
	LOG_DEBUG("download finished");

	if (error == HttpRequest::NoError) {
		//Terminates the thread from the model
		_cWengoPhone.terminate();

		launchUpdateProcess();
	}
}

void QtSoftUpdaterWindow::abortDownload() {
	_softUpdater->abort();
}

void QtSoftUpdaterWindow::launchUpdateProcess() {
	QProcess * updateProcess = new QProcess(_parent);
	connect(updateProcess, SIGNAL(error(QProcess::ProcessError)), SLOT(updateProcessError(QProcess::ProcessError)));
	updateProcess->start(UPDATE_PROGRAM);
}

void QtSoftUpdaterWindow::updateProcessError(QProcess::ProcessError error) {
	QMessageBox::critical(_parent,
			tr("WengoPhone - Update failed"),
			tr("WengoPhone update failed to start: try to update WengoPhone manually"));
}
