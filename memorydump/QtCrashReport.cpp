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

#include "QtCrashReport.h"

#include <QtGui>

#include <stdio.h>

QtCrashReport::QtCrashReport(std::string dumpfile, std::string applicationName, std::string lang) 
	: QObjectThreadSafe(NULL)/*, QDialog(0)*/, _dumpfile(dumpfile), _lang(lang)  {

	_progressTotal = 0;
	_progressNow = 0;
	_status = FtpUpload::None;
	
	_ftpUpload = new FtpUpload("ftp.wengo.fr", "wengophone_ng", dumpfile);
	_ftpUpload->progressionEvent += boost::bind(&QtCrashReport::ftpProgressEventHandler, this, _1, _2, _3);
	_ftpUpload->statusEvent += boost::bind(&QtCrashReport::ftpStatusEventHandler, this, _1, _2);

	_dialog = new QDialog(0);
	ui.setupUi(_dialog);
	
	connect(ui.sendButton, SIGNAL(clicked()), SLOT(sendButtonClicked()));
	
	_dialog->setWindowTitle(QString::fromStdString(applicationName));
	
	ui.progressBar->setValue(0);
}

QtCrashReport::~QtCrashReport() {
}

void QtCrashReport::sendButtonClicked() {
	//start the upload
	_ftpUpload->start();
}

void QtCrashReport::initThreadSafe() {
}

void QtCrashReport::show() {
	_dialog->show();
}

void QtCrashReport::ftpProgressEventHandler(FtpUpload * sender, double ultotal, double ulnow) {
	_progressNow = ulnow;
	_progressTotal = ultotal;

	typedef PostEvent0<void ()> MyPostEvent;
	MyPostEvent * event = new MyPostEvent(boost::bind(&QtCrashReport::updatePresentationThreadSafe, this));
	postEvent(event);
}

void QtCrashReport::ftpStatusEventHandler(FtpUpload * sender, FtpUpload::Status status) {
	_status = status;
	
	typedef PostEvent0<void ()> MyPostEvent;
	MyPostEvent * event = new MyPostEvent(boost::bind(&QtCrashReport::updatePresentationThreadSafe, this));
	postEvent(event);
}

void QtCrashReport::updatePresentationThreadSafe() {
	ui.progressBar->setMaximum(_progressTotal);
	ui.progressBar->setValue(_progressNow);
	
	if( _status == FtpUpload::Ok ) {
		sleep(1);
		_dialog->close();
	} else if (_status == FtpUpload::Error) {
		sleep(1);
		_dialog->close();
	} else {
	}
}
