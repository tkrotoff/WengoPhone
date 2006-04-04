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

#ifndef QTSOFTUPDATERWINDOW_H
#define QTSOFTUPDATERWINDOW_H

#include <http/HttpRequest.h>

#include <qtutil/QObjectThreadSafe.h>

#include <QProcess>
#include <QString>

class CWengoPhone;
class SoftUpdater;
class QProgressDialog;
class QWidget;

/**
 * Shows a Qt progress dialog when downloading a WengoPhone update.
 *
 * Starts the update.exe process (the NSIS installer under Windows).
 *
 * @author Tanguy Krotoff
 */
class QtSoftUpdaterWindow : public QObjectThreadSafe {
	Q_OBJECT
public:

	QtSoftUpdaterWindow(CWengoPhone & cWengoPhone, QWidget * parent);

	~QtSoftUpdaterWindow();

private Q_SLOTS:

	void abortDownload();

	/**
	 * An error occured while starting the update process.
	 *
	 * @param error update process launching error
	 */
	void updateProcessError(QProcess::ProcessError error);

private:

	void initThreadSafe();

	/**
	 * @see SoftUpdater::dataReadProgressEvent
	 */
	void dataReadProgressEventHandler(double bytesDone, double bytesTotal, unsigned downloadSpeed);

	void dataReadProgressEventHandlerThreadSafe(double bytesDone, double bytesTotal, unsigned downloadSpeed);

	/**
	 * @see SoftUpdater::downloadFinishedEvent
	 */
	void downloadFinishedEventHandler(HttpRequest::Error error);

	void downloadFinishedEventHandlerThreadSafe(HttpRequest::Error error);

	/**
	 * Launches the update process (the NSIS installer under Windows).
	 */
	void launchUpdateProcess();

	/**
	 * Progress dialog.
	 */
	QProgressDialog * _progressDialog;

	QWidget * _parent;

	/**
	 * So QtSoftUpdaterWindow can terminate the thread from the model.
	 */
	CWengoPhone & _cWengoPhone;

	/** SoftUpdater. */
	SoftUpdater * _softUpdater;

	/** Original progress dialog label text. */
	QString _originalLabelText;
};

#endif	//QTSOFTUPDATERWINDOW_H
