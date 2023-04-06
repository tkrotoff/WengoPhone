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

#ifndef PRESENCESTATUS_H
#define PRESENCESTATUS_H

#include <qobject.h>
#include <qstring.h>
#include <qstringlist.h>
#include <qpixmap.h>

#include <vector>

/**
 * Presence status of a Contact: online, offline, away...
 *
 * A Contact can also personnalized its presence status using a text like
 * "I'm under the shower", "Hacking on WengoPhone", "Having sex, check the webcam ;)"
 *
 * Inherits from QObject for translation purpose.
 *
 * @author Tanguy Krotoff
 */
class PresenceStatus : public QObject {
	Q_OBJECT
public:

	/**
	 * List of available presence status of a Contact.
	 */
	enum Status {
		Online,
		Away,
		DoNotDisturb,
		UserDefine,
		Offline
	};

	/**
	 * Constructs a PresenceStatus object.
	 *
	 * @param status presence status of the Contact, by default Offline
	 */
	PresenceStatus(Status status = Offline);

	/**
	 * Constructs a PresenceStatus object.
	 *
	 * @param statusText user define presence status
	 */
	PresenceStatus(const QString & statusText);

	PresenceStatus(const PresenceStatus & presenceStatus) : QObject() {
		_status = presenceStatus._status;
		_statusText = presenceStatus._statusText;
		_statusIcon = presenceStatus._statusIcon;
	}

	PresenceStatus & operator=(const PresenceStatus & presenceStatus) {
		if (&presenceStatus != this) {
			_status = presenceStatus._status;
			_statusText = presenceStatus._statusText;
			_statusIcon = presenceStatus._statusIcon;
		}
		return *this;
	}

	Status getStatus() const {
		return _status;
	}

	/**
	 * Gets the text of the status ("online", "offline"...).
	 *
	 * @return status string
	 */
	QString getStatusText() const {
		return _statusText;
	}

	void setStatusText(const QString & statusText) {
		_statusText = statusText;
	}

	QString getStatusTextTranslated() const;

	/**
	 * Gets icon related to the status.
	 *
	 * @return status icon
	 */
	QPixmap getStatusIcon() const {
		return _statusIcon;
	}

	/**
	 * List of available PresenceStatus.
	 */
	typedef std::vector<PresenceStatus> PresenceStatusList;

	/**
	 * Gets the list of available PresenceStatus.
	 *
	 * @return PresenceStatusList
	 */
	static PresenceStatusList getPresenceStatusList();

private:

	/**
	 * Contact presence status.
	 */
	Status _status;

	/**
	 * User define status text (e.g. "Under the shower")
	 */
	QString _statusText;

	/**
	 * Status icon.
	 */
	QPixmap _statusIcon;
};

#endif	//PRESENCESTATUS_H
