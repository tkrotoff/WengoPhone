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

#ifndef OW_QTCRASHREPORT_H
#define OW_QTCRASHREPORT_H

#include "FtpUpload.h"

#include "ui_CrashReport.h"

#include <QDialog>

#include <qtutil/QObjectThreadSafe.h>

/**
 * Crash report widget
 *
 * @author Mathieu Stute
 */
class QtCrashReport : public QObjectThreadSafe {
	Q_OBJECT
public:

	QtCrashReport(std::string dumpfile, std::string applicationName, std::string lang = "");

	~QtCrashReport();

	void show();

private Q_SLOTS:

	void sendButtonClicked();

private:

	void ftpProgressEventHandler(FtpUpload * sender, double ultotal, double ulnow);

	void ftpStatusEventHandler(FtpUpload * sender, FtpUpload::Status status);

	void updatePresentationThreadSafe();

	void initThreadSafe();

	void createDescriptionFile();

	FtpUpload * _ftpUpload;

	FtpUpload * _ftpUpload2;

	FtpUpload::Status _status;

	double _progressNow;

	double _progressTotal;

	std::string _dumpfile;

	std::string _descfile;

	std::string _lang;

	QDialog * _dialog;

	Ui::CrashReportForm ui;

	bool _firstFileUploaded;
};

#endif	//OW_QTCRASHREPORT_H
