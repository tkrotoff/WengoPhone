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

#ifndef DOWNLOAD_H
#define DOWNLOAD_H

#include <qobject.h>
#include <qnetworkprotocol.h>
#include <qptrlist.h>
#include <qprogressdialog.h>
#include <qdialog.h>

class QString;
class QUrlOperator;

/**
 * Downloads a file from an URL to another and shows a progress dialog.
 *
 * @author Tanguy Krotoff
 */
class Download : public QObject {
	Q_OBJECT
public:

	/**
	 * Downloads a file from an URL to a destination.
	 *
	 * @param sourceUrl absolute URL of the source file (ex: http://login:password@www.website.com/file.txt)
	 * @param destinationUrl absolute URL of the destination file (ex: file:/C:/Program Files/file.txt) 
	 */
	Download(const QString & sourceUrl, const QString & destinationUrl);

	~Download();

	QDialog * getWidget() const {
		return _progress;
	}

private slots:

	void transferProgress(int bytesDone, int bytesTotal, QNetworkOperation * op);

	void commandFinished(QNetworkOperation * op);

	void stop();

private:

	void downloadFile(const QString & url, const QString & filename);

	QUrlOperator * _operator;

	QProgressDialog * _progress;

	typedef QPtrList<QNetworkOperation> OperationList;
	OperationList * _operationList;
};

#endif	//DOWNLOAD_H
