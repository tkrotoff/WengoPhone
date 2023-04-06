/*
 * WengoPhone, a voice over Internet phone
 * Copyright (C) 2004-2005  Wengo
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

#include "MemoryDumpWindow.h"
#include "MemoryDumpWindowForm.h"

#include "WindowsVersion.h"

#include <qlabel.h>
#include <qtextedit.h>
#include <qpushbutton.h>
#include <qmessagebox.h>
#include <qftp.h>
#include <qfile.h>
#include <qfileinfo.h>
#include <qprogressbar.h>
#include <qlineedit.h>
#include <qdatetime.h>

#include <iostream>
using namespace std;

QWidget * create(const QString & uiFile, QObject * connector = 0, QWidget * parent = 0, const char * name = 0) {
	//To suppress the warning from the compiler
	connector = 0;

	if (uiFile == "MemoryDumpWindowForm.ui") return new MemoryDumpWindowForm(parent, name);

	//To suppress the warning from the compiler
	return 0;
}

MemoryDumpWindow::MemoryDumpWindow(QWidget * parent, const QString & title, const QString & memoryDumpPath, const QString &userData) {
	_memoryDumpWindow = (QDialog *) create("MemoryDumpWindowForm.ui", this, parent);
	_memoryDumpWindow->setCaption(title);
	
	_memoryDumpPath = memoryDumpPath;
	_userData = userData;
	
	QLabel * pathDiagnosticFileLabel = (QLabel *) _memoryDumpWindow->child("pathDiagnosticFileLabel", "QLabel");
	pathDiagnosticFileLabel->setText(_memoryDumpPath);
	
	//Send button
	QPushButton * sendButton = (QPushButton *) _memoryDumpWindow->child("sendButton", "QPushButton");
	connect(sendButton, SIGNAL(clicked()),
		this, SLOT(sendMemoryDump()));
	
	//Description text edit
	_problemDescriptionText = (QTextEdit *) _memoryDumpWindow->child("problemDescriptionText", "QTextEdit");
	
	//reporter email edit
	_mailLineEdit = (QLineEdit *) _memoryDumpWindow->child("mailLineEdit", "QLineEdit");
}

MemoryDumpWindow::~MemoryDumpWindow() {
}

void MemoryDumpWindow::sendMemoryDump() {
	static const QString FTP_HOSTNAME = "ftp.wengo.fr";
	static const QString FTP_USERNAME = "wengophone";
	static const QString FTP_PASSWORD = "coredump";
	
	QFile * memoryDumpFile = getMemoryDumpFile();
	if (memoryDumpFile == NULL) {
		return;
	}
	
	QFile * memoryDumpDescriptionFile = getMemoryDumpDescriptionFile();
	if (memoryDumpDescriptionFile == NULL) {
		return;
	}
	
	QFtp * ftp = new QFtp(this);
	ftp->connectToHost(FTP_HOSTNAME);
	ftp->login(FTP_USERNAME, FTP_PASSWORD);
	
	uploadFile(ftp, memoryDumpDescriptionFile);
	uploadFile(ftp, memoryDumpFile);

	QProgressBar * uploadProgressBar = (QProgressBar *) _memoryDumpWindow->child("uploadProgressBar", "QProgressBar");
	connect(ftp, SIGNAL(dataTransferProgress(int, int)),
		uploadProgressBar, SLOT(setProgress(int, int)));

	connect(ftp, SIGNAL(done(bool)),
		_memoryDumpWindow, SLOT(close()));
}

QFile * MemoryDumpWindow::getMemoryDumpFile() {
	QFile * file = new QFile(_memoryDumpPath);
	if (file->open(IO_ReadOnly)) {
		return file;
	}
	
	delete file;
	return NULL;
}

QFile * MemoryDumpWindow::getMemoryDumpDescriptionFile() {
	QFile  * file = new QFile(_memoryDumpPath + ".txt");
	if (file->open(IO_WriteOnly)) {
		QTextStream stream(file);
		stream << "Date: "  + QDateTime::currentDateTime().toString();
		stream << "    from: " + _mailLineEdit->text() + "\n";
		
		char * winver = getWindowsVersion();
		QString winVersion = QString(winver);
		stream << "Windows version: " + winVersion + "\n";
		stream << "UserData: " + _userData + "\n";
		stream << "Description: " + _problemDescriptionText->text() + "\n";
		cout << _problemDescriptionText->text() << endl;
		file->close();
	}

	if (file->open(IO_ReadOnly)) {
		return file;
	}

	delete file;
	return NULL;
}

int MemoryDumpWindow::uploadFile(QFtp * ftp, QFile * file) {
	QFileInfo fileInfo(*file);
	return ftp->put(file, fileInfo.fileName());
}
