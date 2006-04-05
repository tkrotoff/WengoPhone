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

#ifndef QTSOFTUPDATE_H
#define QTSOFTUPDATE_H

#include <presentation/PSoftUpdate.h>

#include <http/HttpRequest.h>

#include <qtutil/QObjectThreadSafe.h>

#include <QProcess>
#include <QString>

class CWengoPhone;
class SoftUpdater;
class CSoftUpdate;
class QtWengoPhone;

class QDialog;
class QLabel;

/**
 * Shows a Qt progress dialog when downloading a WengoPhone update.
 *
 * Starts the update.exe process (the NSIS installer under Windows).
 *
 * @author Tanguy Krotoff
 */
class QtSoftUpdate : public QObjectThreadSafe, public PSoftUpdate {
	Q_OBJECT
public:

	QtSoftUpdate(CSoftUpdate & cSoftUpdate);

	~QtSoftUpdate();

	void updatePresentation() { }

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

	void updatePresentationThreadSafe() { }

	void updateWengoPhoneEventHandler(const std::string & downloadUrl,
				unsigned long long buildId,
				const std::string & version,
				unsigned fileSize);

	void updateWengoPhoneEventHandlerThreadSafe(const std::string & downloadUrl,
				unsigned long long buildId,
				const std::string & version,
				unsigned fileSize);

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

	QtWengoPhone * _qtWengoPhone;

	CSoftUpdate & _cSoftUpdate;

	/** SoftUpdater. */
	SoftUpdater * _softUpdater;

	/** Original progress dialog label text. */
	QString _originalLabelText;

	/**
	 * Progress dialog.
	 */
	QDialog * _softUpdateWindow;

	QLabel * _updateTextLabel;
};

#endif	//QTSOFTUPDATE_H
