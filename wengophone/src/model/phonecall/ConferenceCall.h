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

#ifndef CONFERENCECALL_H
#define CONFERENCECALL_H

#include <Event.h>
#include <List.h>
#include <Mutex.h>

#include <string>
#include <vector>

class IPhoneLine;
class PhoneCall;

/**
 * Handles a conference call.
 *
 * Conferences are an association of calls
 * where the audio media is mixed.
 *
 * The number of participant is unlimited in this model but probably
 * limited by the SIP stack that uses ConferenceCall.
 *
 * @ingroup model
 * @author Tanguy Krotoff
 */
class ConferenceCall {
public:

	/**
	 * The state of the ConferenceCall has changed.
	 *
	 * @param sender this class
	 * @param status new status
	 */
	Event<void (ConferenceCall & sender, int status)> stateChangedEvent;

	/**
	 * Creates a new ConferenceCall given a PhoneLine.
	 */
	ConferenceCall(IPhoneLine & phoneLine);

	~ConferenceCall();

	void addPhoneCall(PhoneCall & phoneCall);

	void removePhoneCall(PhoneCall & phoneCall);

	void addPhoneNumber(const std::string & phoneNumber);

	void removePhoneNumber(const std::string & phoneNumber);

	/**
	 * Starts the conference call using the added phone calls and phone numbers.
	 */
	void start();

	/**
	 * Stops the conference call.
	 */
	void stop();

private:

	/** Checks if the PhoneCall is not already created. */
	PhoneCall * getPhoneCall(const std::string & phoneNumber) const;

	/** PhoneLine associated with the ConferenceCall. */
	IPhoneLine & _phoneLine;

	/** Defines the vector of PhoneCall. */
	typedef std::vector < PhoneCall * > PhoneCalls;

	/** List of PhoneCall. */
	PhoneCalls _phoneCallList;

	/** Conference id of this ConferenceCall. */
	int _confId;

	static Mutex _mutex;
};

#endif	//CONFERENCECALL_H
