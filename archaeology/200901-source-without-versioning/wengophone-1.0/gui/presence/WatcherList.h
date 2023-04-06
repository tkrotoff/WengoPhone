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

#ifndef WATCHERLIST_H
#define WATCHERLIST_H

#include "WatcherList.h"
#include "sip/SipAddress.h"

#include <string>
#include <vector>

/**
 * Watcher: a user is looking for my presence status.
 *
 * @author Tanguy Krotoff
 */
class Watcher {
public:

	/**
	 * Constructs a Watcher.
	 *
	 * @param sipAddress Watcher SIP address
	 * @param pending if the Watcher is pending for my authorization to see my presence status
	 */
	Watcher(const SipAddress & sipAddress, bool pending) {
		_pending = pending;
		_sipAddress = sipAddress;
	}

	/**
	 * Gets the SipAddress of the Watcher.
	 *
	 * @return Watcher SIP address
	 */
	SipAddress getSipAddress() const {
		return _sipAddress;
	}

	/**
	 * Gets if the Watcher is pending for my authorization to see my presence status.
	 *
	 * @return if the Watcher is in pending state
	 */
	bool isPending() const {
		return _pending;
	}

private:

	/**
	 * If the Watcher is pending for my authorization to see my presence status.
	 */
	bool _pending;

	/**
	 * Watcher SIP address.
	 */
	SipAddress _sipAddress;
};


/**
 * List of Watcher.
 *
 * From the Presence Event Package for SIP.
 * A WatcherList is a list of all the users that are looking at my presence status.
 * A User can be:
 * - pending: user is waiting for my authorization to see my presence status
 * - active: user is OK to see my presence status
 *
 * @author Tanguy Krotoff
 */
class WatcherList {
public:

	WatcherList();

	~WatcherList();

	/**
	 * Appends the specified element to the end of the list.
	 *
	 * @param watcher element to be appended to the WatcherList
	 */
	void addWatcher(Watcher * watcher);

	/**
	 * Gets the number of Watcher inside the WatcherList.
	 *
	 * @return size of the collection/vector
	 */
	unsigned int size() const;

	/**
	 * Permits to use WatcherList as an array.
	 *
	 * <code>
	 * WatcherList watcherList;
	 * watcherList.addContact(...);
	 * watcherList.addContact(...);
	 * for (unsigned int i = 0; i  watcherList.size(); i++) {
	 *     Watcher & watcher = watcherList[i];
	 * }
	 * </code>
	 *
	 * @param i index inside the array
	 * @return the Contact that corresponds to the index i inside the ContactList
	 */
	const Watcher & operator[] (unsigned int i) const;

private:

	void eraseAll();

	/**
	 * Defines the vector of Watcher.
	 */
	typedef std::vector<Watcher *> Watchers;

	/**
	 * List of Watcher.
	 */
	Watchers _watcherList;
};

#endif	//WATCHERLIST_H
