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

#ifndef ADDCALLLOGEVENT_H
#define ADDCALLLOGEVENT_H

#include "CallLog.h"

#include <observer/Event.h>

/**
 * Event: a CallLog has been added to the CallHistory.
 *
 * @see Observer
 * @see Subject
 * @author Tanguy Krotoff
 */
class AddCallLogEvent : public Event {
public:

	/**
	 * Constructs a AddCallLogEvent.
	 *
	 * @param callLog CallLog that has been added to the CallHistory
	 */
	AddCallLogEvent(CallLog & callLog)
	: Event("AddCallLogEvent"), _callLog(callLog) {
	}

	/**
	 * Gets the CallLog that has been added to the CallHistory.
	 *
	 * @return CallLog added to the CallHistory
	 */
	virtual CallLog & getCallLog() const {
		return _callLog;
	}

	virtual ~AddCallLogEvent() {
	}

private:

	AddCallLogEvent(const AddCallLogEvent &);
	AddCallLogEvent & operator=(const AddCallLogEvent &);

	/**
	 * CallLog that has been added to the CallHistory.
	 */
	CallLog & _callLog;
};

#endif	//ADDCALLLOGEVENT_H
