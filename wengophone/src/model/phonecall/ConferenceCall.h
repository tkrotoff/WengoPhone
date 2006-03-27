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

#include <sipwrapper/EnumConferenceCallState.h>
#include <sipwrapper/EnumPhoneCallState.h>

#include <util/NonCopyable.h>
#include <util/Event.h>
#include <util/List.h>

#include <string>
#include <map>

class IPhoneLine;
class PhoneCall;
class ConferenceCallParticipant;

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
class ConferenceCall : NonCopyable {
public:

	/**
	 * The state of the ConferenceCall has changed.
	 *
	 * @param sender this class
	 * @param state new state
	 */
	Event<void (ConferenceCall & sender, EnumConferenceCallState::ConferenceCallState state)> stateChangedEvent;

	/**
	 * Creates a new ConferenceCall given a PhoneLine.
	 */
	ConferenceCall(IPhoneLine & phoneLine);

	~ConferenceCall();

	void addPhoneCall(PhoneCall & phoneCall);

	void addPhoneCall(int callId);

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

	bool isStarted() const {
		return (_confId != -1);
	}

	/** Should only be used by ConferenceCallParticipant. */
	void join(int callId);

private:

	/** Checks if the PhoneCall is not already created. */
	PhoneCall * getPhoneCall(const std::string & phoneNumber) const;

	void phoneCallStateChangedEventHandler(PhoneCall & sender, EnumPhoneCallState::PhoneCallState state);


	/** PhoneLine associated with the ConferenceCall. */
	IPhoneLine & _phoneLine;

	/** Defines the vector of PhoneCall participant. */
	typedef std::map < std::string, PhoneCall * > PhoneCalls;

	/** List of PhoneCall. */
	PhoneCalls _phoneCallMap;

	/**
	 * Conference id of this ConferenceCall.
	 *
	 * -1 means that the conference has not been started yet.
	 */
	int _confId;
};

#endif	//CONFERENCECALL_H
