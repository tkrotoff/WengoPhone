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

#ifndef CALLHISTORY_H
#define CALLHISTORY_H

#include "CallLog.h"

#include <observer/Subject.h>
#include <singleton/Singleton.h>

#include <vector>

/**
 * Records all the incoming, outgoing and missed calls.
 *
 * CallHistory is just a vector of CallLog.
 *
 * Design Pattern Observer.
 * Design Pattern Singleton.
 *
 * @see CallLog
 * @author Tanguy Krotoff
 */
class CallHistory : public Subject, public Singleton<CallHistory> {
	friend class Singleton<CallHistory>;
public:

	~CallHistory();

	/**
	 * Appends the specified element to the end of the list.
	 *
	 * @param callLog element to be appended to the CallHistory
	 * @param save saves or not the added CallLog into the CallHistory XML file
	 */
	void addCallLog(CallLog & callLog, bool save = true);

	/**
	 * Removes a single instance of the specified element from this list, if it is present.
	 *
	 * @param contact element to remove from the CallHistory
	 * @return true if the collection contained the specified element
	 */
	bool removeCallLog(const CallLog & callLog);

	/**
	 * Gets the number of CallLog inside the CallHistory.
	 *
	 * @return size of the collection/vector
	 */
	unsigned int size() const;

	/**
	 * Permits to use CallHistory as an array.
	 *
	 * <code>
	 * for (int i = 0; i < callHistory.size(); i++) {
	 *     CallLog & callLog = callHistory[i];
	 * }
	 * </code>
	 *
	 * @param i index inside the array
	 * @return the CallLog that corresponds to the index i inside the CallHistory
	 */
	const CallLog & operator[] (unsigned int i) const;

	/**
	 * @see operator[]
	 */
	CallLog & operator[] (unsigned int i);

	/**
	 * Loads CallHistory from the default XML file.
	 *
	 * @return CallHistory object created from the XML file
	 */
	static CallHistory & loadFromXml();

	void saveAsXml();

	/**
	 * Resets the CallHistory: erases all the CallLog from the list.
	 *
	 * Does not erase the files on the hard disk.
	 */
	void reset();

protected:

	CallHistory();

	CallHistory(const CallHistory &);
	CallHistory & operator=(const CallHistory &);

private:

	void eraseAll();

	/**
	 * Defines the vector of CallLog.
	 */
	typedef std::vector<CallLog *> CallLogList;

	/**
	 * List of CallLog.
	 */
	CallLogList _callLogList;
};

#endif	//CALLHISTORY_H
