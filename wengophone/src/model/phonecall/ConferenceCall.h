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

#ifndef OWCONFERENCECALL_H
#define OWCONFERENCECALL_H

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
	 * A PhoneCall has been added to the ConferenceCall.
	 *
	 * @param sender this class
	 * @param phoneCall PhoneCall added to the ConferenceCall
	 */
	Event<void (ConferenceCall & sender, PhoneCall & phoneCall)> phoneCallAddedEvent;

	/**
	 * A PhoneCall has been removed to the ConferenceCall.
	 *
	 * @param sender this class
	 * @param phoneCall PhoneCall removed to the ConferenceCall
	 */
	Event<void (ConferenceCall & sender, PhoneCall & phoneCall)> phoneCallRemovedEvent;

	/**
	 * Creates a new ConferenceCall given a PhoneLine.
	 */
	ConferenceCall(IPhoneLine & phoneLine);

	~ConferenceCall();

	void addPhoneCall(PhoneCall & phoneCall);

	void removePhoneCall(PhoneCall & phoneCall);

	void addPhoneNumber(const std::string & phoneNumber);

	void removePhoneNumber(const std::string & phoneNumber);

	typedef List<PhoneCall *> PhoneCallList;

	/**
	 * Gets the list of PhoneCall associated with this conference.
	 *
	 * @return the list of PhoneCall
	 */
	PhoneCallList getPhoneCallList() const;

	/** Should only be used by ConferenceCallParticipant. */
	void join(int callId);

private:

	/** Waits for a phone call to be in talking state before to create another one. */
	void phoneCallStateChangedEventHandler(PhoneCall & sender, EnumPhoneCallState::PhoneCallState state);

	PhoneCall * getPhoneCall(const std::string & phoneNumber) const;

	/** PhoneLine associated with the ConferenceCall. */
	IPhoneLine & _phoneLine;

	/** Defines the vector of PhoneCall participant. */
	typedef std::map < std::string, PhoneCall * > PhoneCalls;

	/** List of PhoneCall. */
	PhoneCalls _phoneCallMap;

	/**
	 * Conference id of this ConferenceCall.
	 *
	 * SipWrapper::ConfIdError (-1) means that the conference has not been started yet.
	 */
	int _confId;
};

#endif	//OWCONFERENCECALL_H
