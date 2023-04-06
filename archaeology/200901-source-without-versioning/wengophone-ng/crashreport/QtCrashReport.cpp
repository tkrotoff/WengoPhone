/*
 * WengoPhone, a voice over Internet phone
 * Copyright (C) 2004-2007  Wengo
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
#include <util/File.h>
#include <util/Logger.h>
#include <util/Path.h>
#include <util/SafeDelete.h>
#include <util/Time.h>

#ifdef OS_WINDOWS
	#include <system/WindowsVersion.h>
#endif

#include <util/SafeConnect.h>

#include <QtGui/QtGui>

#include <cstdio>
#include <iostream>

static const std::string LOG_FILE = "log-main.txt";
static const std::string FTP_SERVER = "ftp.openwengo.com";
static const std::string FTP_LOGIN = "wengophone";
static const std::string FTP_PASSWORD = "coredump";

QtCrashReport::QtCrashReport(const std::string & dumpfile, const std::string & applicationName,
	const std::string & lang, const std::string & info)
	: _info(info) {

	_progressTotal = 0;
	_progressNow = 0;
	_status = FtpUpload::None;
	_firstFileUploaded = false;
	_descfile = dumpfile + ".txt";

	_ftpUpload = new FtpUpload(FTP_SERVER, FTP_LOGIN, FTP_PASSWORD, dumpfile);
	SAFE_CONNECT(_ftpUpload, SIGNAL(progressionSignal(double, double)),
		SLOT(ftpProgressSlot));
	SAFE_CONNECT(_ftpUpload, SIGNAL(statusSignal(FtpUpload::Status)),
		SLOT(ftpStatusSlot));

	_ui = new Ui::CrashReport();
	_ui->setupUi(this);
	SAFE_CONNECT(_ui->sendButton, SIGNAL(clicked()),
		SLOT(sendButtonClicked()));
	setWindowTitle(QString::fromStdString(applicationName));
	_ui->progressBar->setValue(0);
}

QtCrashReport::~QtCrashReport() {
	OWSAFE_DELETE(_ui);
}

void QtCrashReport::sendButtonClicked() {
	//Start the upload
	createDescriptionFile();
	_ftpUpload->start();
}

void QtCrashReport::ftpProgressSlot(double bytesDone, double bytesTotal) {
	_progressNow = bytesDone;
	_progressTotal = bytesTotal;
	updateProgressBar();
}

void QtCrashReport::ftpStatusSlot(FtpUpload::Status status) {
	_status = status;
	updateProgressBar();
}

void QtCrashReport::updateProgressBar() {
	_ui->progressBar->setMaximum(_progressTotal);
	_ui->progressBar->setValue(_progressNow);

	if ((_status == FtpUpload::Ok) || (_status == FtpUpload::Error)) {
		if (!_firstFileUploaded) {

			_firstFileUploaded  = true;
			_ui->progressBar->setValue(0);

			//Delete _ftpUpload;
			_ftpUpload2 = new FtpUpload(FTP_SERVER, FTP_LOGIN, FTP_PASSWORD, _descfile);
			SAFE_CONNECT(_ftpUpload2,
				SIGNAL(progressionSignal(double, double)),
				SLOT(ftpProgressSlot));
			SAFE_CONNECT(_ftpUpload2,
				SIGNAL(statusSignal(FtpUpload::Status)),
				SLOT(ftpStatusSlot));
			_ftpUpload2->start();

		} else {
			close();
		}
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
	std::ofstream file;
	file.open(_descfile.c_str(), std::ios::out);

	file
		<< "Date: " << Date().toString() << std::endl
		<< "Time: " << Time().toString() << std::endl;

#ifdef OS_WINDOWS
	file << "Windows version: " << WindowsVersion::getVersion() << std::endl;
#endif

	if (_ui->mailLineEdit) {
		file << "From: " << _ui->mailLineEdit->text().toStdString() << std::endl;
	}

	if (_ui->descTextEdit) {
		file << "Description: " << _ui->descTextEdit->toPlainText().toStdString() << std::endl;
	}

	file << "User info:" << std::endl << _info << std::endl;
	file << LOG_FILE << " content:" << std::endl << readLogFile() << std::endl;
}
