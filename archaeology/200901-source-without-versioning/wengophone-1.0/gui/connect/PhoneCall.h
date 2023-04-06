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

#ifndef PHONECALL_H
#define PHONECALL_H

#include <NonCopyable.h>

#include <string>

class SipAddress;

/**
 * A phone call.
 *
 * @author Tanguy Krotoff
 */
class PhoneCall : NonCopyable {
public:

	static const PhoneCall null;

	//Call states.
	enum State {
		/** New outgoing call, not dialed yet. */
		Idle,

		/** Incoming connection from remote party. */
		Incoming,

		/** Dialing, but not yet connected. */
		//Dialing,

		/** Connected to the other party. */
		Connected,

		/** Connected, but currently on hold. */
		//Hold,

		/** Local side hung up the call. */
		//HangupLocal,

		/** Remote side hung up the call, or call lost. */
		//HangupRemote,

		/** Incoming call that was missed. */
		//Missed,

		/** Network has failed in some way. */
		//NetworkFailure,

		/** Something else went wrong. */
		//OtherFailure
	};

	PhoneCall(int phoneLineId);

	~PhoneCall();

	/**
	 * Dials a number on this PhoneCall (must be in the Idle state).
	 *
	 * @param uri SIP address to call
	 */
	bool dial(const SipAddress & uri);

	bool dial(const std::string & number);

	/**
	 * Accepts an incoming call (must be in the Incoming state).
	 *
	 * All active calls are automatically put on hold.
	 */
	void accept();

	/**
	 * Hangups this PhoneCall.
	 *
	 * @return true if success, false otherwise
	 */
	bool hangup();

	/**
	 * Transfers the call to a new number and then discontinue this PhoneCall.
	 *
	 * @param uri new number where to transfer this call.
	 */
	void transfer(const SipAddress & uri) {
	}

	/**
	 * Sends a sequence of DTMF tones over the call.
	 *
	 * Ignored if the call is not connected, or it is on hold.
	 *
	 * @param dtmf DTMF to play over the call
	 */
	void tone(char dtmf);

	/**
	 * Gets the current state of this PhoneCall.
	 *
	 * @return current state
	 */
	State getState() const {
		return _state;
	}

private:

	int _phoneLineId;

	int _phoneCallId;

	/**
	 * Current state of this PhoneCall.
	 */
	State _state;
};

#endif	//PHONECALL_H
