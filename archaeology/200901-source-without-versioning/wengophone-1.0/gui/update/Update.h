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

#ifndef UPDATE_H
#define UPDATE_H

#include <qobject.h>
#include <qstring.h>

class QProcess;
class UpdateDialog;

/**
 * Updates WengoPhone if necessary.
 *
 * Launches the download external process in order to get the
 * update (a setup.exe under Windows).
 *
 * TODO rather than to download a setup.exe under Windows,
 * a better solution would be to download a simple .zip
 * and uncompress it inside the WengoPhone directory.
 *
 * @author Tanguy Krotoff
 */
class Update : public QObject {
	Q_OBJECT
public:

	friend class UpdateDialog;

	/**
	 * Constructs an Update object.
	 *
	 * @param updateUrl URL where to download the WengoPhone update
	 * @param updateCompulsery is the update compulsery or not?
	 * @param updateName name of the update
	 */
	Update();

	void setUpdate(const QString & updateUrl, bool updateCompulsery, const QString & updateName);

	~Update();

private slots:

	/**
	 * The WengoPhone update has been downloaded.
	 */
	void updateDownloadFinished();

private:

	void update();

	void cancel();

	/**
	 * QProcess that launches the download process.
	 */
	QProcess * _downloadProc;

	bool _updateCompulsery;

	QString _updateUrl;
};

#endif	//UPDATE_H
