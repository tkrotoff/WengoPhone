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

#include "ui_CrashReport.h"

#include <cutil/global.h>
#include <util/Date.h>
#include <util/Time.h>
#include <util/Path.h>
#include <util/File.h>
#include <util/SafeDelete.h>
#include <util/Logger.h>

#include <thread/Thread.h>

#ifdef OS_WINDOWS
	#include <system/WindowsVersion.h>
#endif

#include <QtGui/QtGui>

#include <iostream>
#include <cstdio>

static const std::string LOG_FILE = "log-main.txt";
static const std::string FTP_SERVER = "ftp.wengo.fr";
static const std::string FTP_PATH = "wengophone_ng";

QtCrashReport::QtCrashReport(const std::string & dumpfile, const std::string & applicationName,
			const std::string & lang, const std::string & info)
	: QObjectThreadSafe(NULL),
	_info(info) {

	_progressTotal = 0;
	_progressNow = 0;
	_status = FtpUpload::None;
	_firstFileUploaded = false;
	_descfile = dumpfile + ".txt";

	_ftpUpload = new FtpUpload(FTP_SERVER, FTP_PATH, dumpfile);
	_ftpUpload->progressionEvent += boost::bind(&QtCrashReport::ftpProgressEventHandler, this, _1, _2, _3);
	_ftpUpload->statusEvent += boost::bind(&QtCrashReport::ftpStatusEventHandler, this, _1, _2);

	_dialog = new QDialog(NULL);

	_ui = new Ui::CrashReport();
	_ui->setupUi(_dialog);

	connect(_ui->sendButton, SIGNAL(clicked()), SLOT(sendButtonClicked()));

	_dialog->setWindowTitle(QString::fromStdString(applicationName));

	_ui->progressBar->setValue(0);
}

QtCrashReport::~QtCrashReport() {
	OWSAFE_DELETE(_ui);
}

void QtCrashReport::sendButtonClicked() {
	//start the upload
	createDescriptionFile();
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
	_ui->progressBar->setMaximum(_progressTotal);
	_ui->progressBar->setValue(_progressNow);

	//sleep: only for the feeling
	if ((_status == FtpUpload::Ok) || (_status == FtpUpload::Error)) {

		//TODO: upload the description file
		if (!_firstFileUploaded) {

			_firstFileUploaded  = true;
			_ui->progressBar->setValue(0);

			//delete _ftpUpload;
			_ftpUpload2 = new FtpUpload(FTP_SERVER, FTP_PATH, _descfile);
			_ftpUpload2->progressionEvent += boost::bind(&QtCrashReport::ftpProgressEventHandler, this, _1, _2, _3);
			_ftpUpload2->statusEvent += boost::bind(&QtCrashReport::ftpStatusEventHandler, this, _1, _2);
			_ftpUpload2->start();

		} else {
			Thread::sleep(1);
			_dialog->close();
		}
	} else {

	}
}

static std::string readLogFile() {
	std::string data;

	FileReader file(LOG_FILE);
	if (file.open()) {
		data = file.read();
		file.close();
	} else {
		LOG_ERROR("failed to open log file=" + LOG_FILE);
	}
	return data;
}

void QtCrashReport::createDescriptionFile() const {
	FileWriter file(_descfile, false);

	file.write("Date: " + Date().toString() + String::EOL);
	file.write("Time: " + Time().toString() + String::EOL);

#ifdef OS_WINDOWS
	file.write("Windows version: " + std::string(WindowsVersion::getVersion()) + String::EOL);
#endif

	if (_ui->mailLineEdit) {
		file.write("From: " + _ui->mailLineEdit->text().toStdString() + String::EOL);
	}

	if (_ui->descTextEdit) {
		file.write("Description: " + _ui->descTextEdit->toPlainText().toStdString() + String::EOL);
	}

	file.write("User info:\n" + _info + String::EOL);

	file.write(LOG_FILE + " content:\n" + readLogFile() + String::EOL);

	file.close();
}
