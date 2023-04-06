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

#ifndef MEMORYDUMPWINDOW_H
#define MEMORYDUMPWINDOW_H

#include <qobject.h>
#include <qstring.h>

class QWidget;
class QDialog;
class QTextEdit;
class QFile;
class QFtp;
class QLineEdit;

/**
 * Shows a window to complete a mini memory dump and to send it.
 *
 * MemoryDumpWindow is a separate executable file otherwise
 * some crashes do not open MemoryDumpWindow (probably because
 * of the dependency with the Qt dll).
 *
 * TODO merge with download since it can upload a file.
 * TODO send through email
 *
 * @author Tanguy Krotoff
 */
class MemoryDumpWindow : public QObject {
	Q_OBJECT
public:

	/**
	 * Shows a window to send a memory dump.
	 *
	 * @param parent parent widget
	 * @param title window title
	 * @param memoryDumpPath location of the memory dump (aka mini dump, core dump)
	 */
	MemoryDumpWindow(QWidget * parent, const QString & title, const QString & memoryDumpPath, const QString &userData);

	~MemoryDumpWindow();

	/**
	 * Gets the low-level widget of this class.
	 *
	 * @return low-level widget
	 */
	QDialog * getWidget() const {
		return _memoryDumpWindow;
	}

private slots:

	/**
	 * Sends the memory dump plus its description on the FTP server.
	 */
	void sendMemoryDump();

private:

	MemoryDumpWindow(const MemoryDumpWindow &);
	MemoryDumpWindow & operator=(const MemoryDumpWindow &);

	/**
	 * Creates the memory dump QFile from its location.
	 *
	 * @return the memory dump file
	 */
	QFile * getMemoryDumpFile();

	/**
	 * Creates the description of the memory dump QFile from its location.
	 *
	 * @return the memory dump description file
	 */
	QFile * getMemoryDumpDescriptionFile();

	/**
	 * Uploads the memory dump on a FTP server.
	 *
	 * @param ftp FTP session with the FTP server
	 * @param file memory dump file to upload
	 * @return unique identifier which is passed by QFtp::commandStarted()
	 *         and QFtp::commandFinished()
	 */
	int uploadFile(QFtp * ftp, QFile * file);

	/**
	 * Low-level widget component of this class.
	 */
	QDialog * _memoryDumpWindow;

	/**
	 * Widget where the user describes the problem.
	 */
	QTextEdit * _problemDescriptionText;

	/**
	 * Location of the memory dump (aka mini dump, core dump).
	 */
	QString _memoryDumpPath;
	
	/**
	 * Reporter email
	 */
	QLineEdit * _mailLineEdit;
	
	QString _userData;
};

#endif	//MEMORYDUMPWINDOW_H
