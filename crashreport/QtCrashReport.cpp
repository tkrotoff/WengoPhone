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

#include <cutil/global.h>
#include <util/Date.h>
#include <util/Time.h>
#include <util/Path.h>
#include <thread/Thread.h>

#ifdef OS_WINDOWS
	#include <system/WindowsVersion.h>
#endif

#include <QtGui>

#include <fstream>
#include <stdio.h>

QtCrashReport::QtCrashReport(std::string dumpfile, std::string applicationName, std::string lang, std::string info)
	: QObjectThreadSafe(NULL), _dumpfile(dumpfile), _lang(lang), _info(info) {

	_progressTotal = 0;
	_progressNow = 0;
	_status = FtpUpload::None;
	_firstFileUploaded = false;
	_descfile = _dumpfile + ".txt";

	_ftpUpload = new FtpUpload("ftp.wengo.fr", "wengophone_ng", _dumpfile);
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
	ui.progressBar->setMaximum(_progressTotal);
	ui.progressBar->setValue(_progressNow);

	//sleep: only for the feeling
	if(( _status == FtpUpload::Ok ) || (_status == FtpUpload::Error) ) {

		//TODO: upload the description file
		if( !_firstFileUploaded  ) {

			_firstFileUploaded  = true;
			ui.progressBar->setValue(0);

			//delete _ftpUpload;
			_ftpUpload2 = new FtpUpload("ftp.wengo.fr", "wengophone_ng", _descfile);
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

std::string readLogFile() {
	std::string content;
	std::string line;

	std::ifstream myfile (getApplicationDirPath() + "log.txt");
	if (myfile.is_open()) {
		while (! myfile.eof() ) {
			getline(myfile,line);
			content += line + "\n";
		}
		myfile.close();
	} else {
		printf("Failed to open log.txt\n");
	}
	return content;
}

void QtCrashReport::createDescriptionFile() {

	std::fstream file;
	file.open(_descfile.c_str(), std::fstream::out);

	file << "Date: " << Date().toString() << std::endl;
	file << "Time: " << Time().toString() << std::endl;

#ifdef OS_WINDOWS
	file << "Windows version: " << std::string(getWindowsVersion()) << std::endl;
#endif

	std::string tmp = "";
	if( ui.mailLineEdit ) {

		tmp = ui.mailLineEdit->text().toStdString();
		file << "From: " << tmp << std::endl;
	}

	if( ui.descTextEdit ) {

		tmp = ui.descTextEdit->toPlainText().toStdString();
		file << "Description: " << tmp << std::endl;
	}

	file << "User info:\n" << _info << std::endl;

	file << "log.txt content:\n" << readLogFile() << std::endl;

	file.close();
}
