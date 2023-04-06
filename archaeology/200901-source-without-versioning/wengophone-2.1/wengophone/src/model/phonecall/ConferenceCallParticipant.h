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

#ifndef CONFERENCECALLPARTICIPANT_H
#define CONFERENCECALLPARTICIPANT_H

#include <util/NonCopyable.h>

#include <sipwrapper/EnumPhoneCallState.h>

class ConferenceCall;
class PhoneCall;

/**
 * Participant to a ConferenceCall.
 *
 * @ingroup model
 * @author Tanguy Krotoff
 */
class ConferenceCallParticipant : NonCopyable {
public:

	ConferenceCallParticipant(ConferenceCall & conferenceCall, PhoneCall & phoneCall);

private:

	void joinConference();

	void phoneCallStateChangedEventHandler(PhoneCall & sender, EnumPhoneCallState::PhoneCallState state);

	ConferenceCall & _conferenceCall;

	PhoneCall & _phoneCall;

	bool _waitForHoldState;
};

#endif	//CONFERENCECALLPARTICIPANT_H
