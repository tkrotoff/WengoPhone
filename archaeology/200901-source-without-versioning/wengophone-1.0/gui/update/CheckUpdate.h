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

#ifndef CHECKUPDATE_H
#define CHECKUPDATE_H

#include <qstring.h>
#include <qobject.h>
#include <qdialog.h>

//#include "database/HttpSource.h"
#include <http/HttpRequest.h>

class Timer;
class Update;
namespace database {
	class HttpSource;
}

class UpdateDialog : public QDialog {
	Q_OBJECT
public:

	UpdateDialog(QWidget * parent, Update * updateInstance);

private slots:

	void update();

	void cancel();

private:

	Update * _updateInstance;
};


/**
 * Checks if an WengoPhone update is available.
 *
 * @author Tanguy Krotoff
 */
class CheckUpdate : public QObject, public HttpRequest {
	Q_OBJECT
public:

	/**
	 * Constructs a CheckUpdate object.
	 *
	 * @param buildId current build id number of WengoPhone
	 * @param os win95, winNT, win98, winMe, win2k, winXP, winCE, linux, mac, solaris (not used)
	 * @param lang language of the application (not used)
	 */
	CheckUpdate(const Q_ULLONG buildId, const QString & os, const QString & lang);

	~CheckUpdate();

	/**
	 * Gets the last build id available, last version of WengoPhone available.
	 *
	 * @return the last build id available or 0 if the last
	 *         build id couldn't be get (for example no Internet connection)
	 */
	static Q_ULLONG getLastBuildIdAvailable() {
		return _lastBuildIdAvailable;
	}

	virtual void answerReceived(const std::string & answer, Error error);

public slots:

	/**
	 * Checks (only once) if there is an update of the softphone on the server
	 * & show a popup with the result
	 */
	void checkUpdate();

protected:

	void customEvent(QCustomEvent *ev);

private:

	int _nbRetry;

	/**
	 * QTimer timeout in milliseconds.
	 *
	 * 24 hours = 24 * 60 * 60 * 1000 = 86400 seconds
	 * int (32bits): from -2147483647 to 2147783647
	 */
	static const int UPDATE_TIMEOUT;

	/**
	 * After 15 seconds, if the update was not received, we start again.
	 */
	static const int UPDATERECEIVED_TIMEOUT;

	/**
	 * Number of limited retry if the update failed to be received.
	 *
	 * Equals 3 times.
	 */
	static const int LIMIT_NB_RETRY;

	/**
	 * Number of update procedure that failed.
	 */
	unsigned int _nbUpdateFailedRetry;

	/**
	 * Timer to update the softphone every 24hours.
	 */
	Timer * _timer24;

	/**
	 * Timer to check if the update was received from the server.
	 *
	 * Checks every UPDATERECEIVED_TIMEOUT seconds.
	 *
	 * @see updateReceived()
	 */
	Timer * _timer;

	/**
	 * In order to send the HTTP request to the server.
	 */
	database::HttpSource * _source;

	/**
	 * Current build id of WengoPhone.
	 */
	Q_ULLONG _buildId;

	/**
	 * Current OS running WengoPhone.
	 */
	QString _os;

	/**
	 * Current language of the application.
	 */
	QString _lang;

	/**
	 * Last build number available from the web service (last version of WengoPhone available).
	 *
	 * Initialized to 0 by default
	 *
	 * Q_ULLONG = unsigned long long
	 * 64bits from 0 to 18446744073709551615
	 * thus perfect for a date like:
	 * YYYYMMDDHHMMSS
	 * 20041215105700
	 */
	static Q_ULLONG _lastBuildIdAvailable;

	Update * _update;

	UpdateDialog * _updateDialog;

	bool _auto;
};

#endif	//CHECKUPDATE_H
