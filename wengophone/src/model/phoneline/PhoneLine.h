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

#ifndef PHONELINE_H
#define PHONELINE_H

#include "IPhoneLine.h"

#include <model/account/SipAccount.h>

#include <List.h>

#include <map>

class PhoneLineState;
class PhoneCall;
class WengoPhone;
class SipAddress;
class SipWrapper;
class SipPresenceState;

class PhoneLine : public IPhoneLine {
public:

	PhoneLine(SipAccount & sipAccount, WengoPhone & wengoPhone);

	~PhoneLine();

	std::string getMySipAddress() const;

	int makeCall(const std::string & phoneNumber);

	void acceptCall(int callId);

	void rejectCall(int callId);

	void closeCall(int callId);

	void holdCall(int callId);

	void resumeCall(int callId);

	SipWrapper & getSipWrapper() const {
		return *_sipWrapper;
	}

	SipAccount & getSipAccount() const {
		return _sipAccount;
	}

	void connect();

	void disconnect();

	void PhoneLine::setPhoneCallState(int callId, int status, const SipAddress & sipAddress);

	int getLineId() const {
		return _lineId;
	}

	void setState(int status);

	const PhoneLineState & getState() const {
		return *_state;
	}

	WengoPhone & getWengoPhone() const {
		return _wengoPhone;
	}

	PhoneCall * getPhoneCall(int callId) /*const*/;

private:

	/**
	 * Create and set the SIP stack.
	 */
	void initSipWrapper();

	/**
	 * Creates a new PhoneCall waiting to receive callbacks.
	 *
	 * @return PhoneCall created
	 */
	PhoneCall * createWaitingPhoneCall();

	/** SIP implementation. */
	SipWrapper * _sipWrapper;

	typedef std::map < int, PhoneCall * > PhoneCalls;

	/** Map of PhoneCall. */
	PhoneCalls _phoneCallHash;

	/** Active PhoneCall. */
	PhoneCall * _activePhoneCall;

	/**
	 * PhoneCall waiting for incoming calls.
	 *
	 * At least they should be always a PhoneCall waiting
	 * for incoming calls.
	 */
	PhoneCall * _waitingPhoneCall;

	/** SipAccount associated with this PhoneLine. */
	SipAccount & _sipAccount;

	/** Line id of this PhoneLine. */
	int _lineId;

	/** State of the PhoneLine. */
	PhoneLineState * _state;

	/** Defines the vector of PhoneLineState. */
	typedef List < PhoneLineState * > PhoneLineStates;

	/** List of PhoneLineState. */
	PhoneLineStates _phoneLineStateList;

	WengoPhone & _wengoPhone;
};

#endif	//PHONELINE_H
