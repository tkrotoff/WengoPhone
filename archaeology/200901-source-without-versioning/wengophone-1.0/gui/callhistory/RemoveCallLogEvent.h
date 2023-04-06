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

#ifndef REMOVECALLLOGEVENT_H
#define REMOVECALLLOGEVENT_H

#include "CallLog.h"

#include <observer/Event.h>

/**
 * Event: a CallLog has been removed from the CallHistory.
 *
 * The CallLog will be removed just after this Event is created.
 *
 * @see Observer
 * @see Subject
 * @author Tanguy Krotoff
 */
class RemoveCallLogEvent : public Event {
public:

	/**
	 * Constructs a RemoveCallLogEvent.
	 *
	 * @param CallLog CallLog removed from the list
	 */
	RemoveCallLogEvent(const CallLog & CallLog)
	: Event("RemoveCallLogEvent"), _callLog(CallLog) {
	}

	/**
	 * Gets the CallLog that will be removed from the CallHistory.
	 *
	 * @return the CallLog removed
	 */
	virtual const CallLog & getCallLog() const {
		return _callLog;
	}

	virtual ~RemoveCallLogEvent() {
	}

private:

	RemoveCallLogEvent(const RemoveCallLogEvent &);
	RemoveCallLogEvent & operator=(const RemoveCallLogEvent &);

	/**
	 * The CallLog to be removed from the CallHistory.
	 */
	const CallLog & _callLog;
};

#endif	//REMOVECALLLOGEVENT_H
