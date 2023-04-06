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

#ifndef PRESENCE_H
#define PRESENCE_H

#include <observer/Observer.h>

#include <qobject.h>
#include <qstring.h>

class ContactList;
class SipAddress;

/**
 * Handles presence.
 *
 * Design Pattern Observer.
 * Presence is updated each time there is a modification to ContactList
 * or to one of the Contact (for example if the name of the Contact has been changed).
 *
 * @author Tanguy Krotoff
 */
class Presence : public Observer {
public:

	friend class WaitingForConnectedSignal;

	Presence();

	~Presence();

	/**
	 * Publish my online SIP presence status.
	 *
	 * @param note can be something like "online", "away", "do not disturb"...
	 *        if note is null then uses the previous status saved
	 */
	static void publishOnlineStatus(const QString & note = QString::null);

	/**
	 * Publish my offline SIP presence status.
	 */
	static void publishOfflineStatus();

	/**
	 * Asks to the SIP presence server for the list of all my watchers.
	 */
	static void askForWatcherList();

	/**
	 * Subscribes to the presence of all the contacts from the ContactList.
	 */
	static void subscribeToEverybodyPresence();

	/**
	 * On each SIP presence message received.
	 *
	 * @param event kind of SIP presence event: presence of presence.winfo
	 * @param from SIP address the SIP presence message is coming from
	 * @param content XML content of the SIP presence message received
	 */
	static void onNotify(const QString & event, const SipAddress & from, const QString & content);

	static void subscriptionProgress(int subscriptionId, const SipAddress & to, bool success);

	/**
	 * Callback, called each time a Subject has changed.
	 *
	 * @param subject Subject that has been changed
	 * @param event Event
	 * @see Observer::update();
	 */
	virtual void update(const Subject & subject, const Event & event);

private:

	Presence(const Presence &);
	Presence & operator=(const Presence &);

	static bool askAllowWatcher(const QString & sipAddress);

	static void subscribeToPresence(const QString & sipAddress);

	static void allowWatcher(const QString & sipAddress);

	static void forbidWatcher(const QString & sipAddress);
};

/**
 * Wait for phApi to finish its register.
 *
 * This is quite an ugly trick.
 *
 * @author Tanguy Krotoff
 */
class WaitingForConnectedSignal : public QObject {
	Q_OBJECT;
public:

	WaitingForConnectedSignal(const QString & param = QString::null) {
		_param = param;
	}

public slots:

	void publishOnlineStatus();

	void subscribeToPresence();

	void allowWatcher();

	void forbidWatcher();

	void askForWatcherList();

private:

	QString _param;
};

#endif	//PRESENCE_H
