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

#ifndef PHONELINE_H
#define PHONELINE_H

#include "IPhoneLine.h"

#include <model/account/SipAccount.h>

#include <util/List.h>

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

	void blindTransfer(int callId, const std::string & sipAddress);

	void playTone(int callId, EnumTone::Tone tone);

	void playSoundFile(int callId, const std::string & soundFile);

	SipWrapper & getSipWrapper() const {
		return *_sipWrapper;
	}

	SipAccount & getSipAccount() const {
		return _sipAccount;
	}

	void connect();

	void disconnect();

	void setPhoneCallState(int callId, EnumPhoneCallState::PhoneCallState state, const SipAddress & sipAddress);

	int getLineId() const {
		return _lineId;
	}

	void setState(EnumPhoneLineState::PhoneLineState state);

	const PhoneLineState & getState() const {
		return *_state;
	}

	WengoPhone & getWengoPhone() const {
		return _wengoPhone;
	}

	PhoneCall * getPhoneCall(int callId) /*const*/;

	PhoneCall * getActivePhoneCall() const {
		return _activePhoneCall;
	}

	List<PhoneCall *> getPhoneCallList() const;

private:

	/**
	 * Creates and sets the SIP stack.
	 */
	void initSipWrapper();

	/**
	 * Puts all the PhoneCall in the hold state except one.
	 *
	 * Code factorization.
	 *
	 * @param callId phone call that won't be put in hold state
	 */
	void holdCallsExcept(int callId);

	/**
	 * A call has been closed.
	 *
	 * Code factorization.
	 *
	 * @param callId phone call being closed
	 */
	void callClosed(int callId);

	/**
	 * Checks if a callId is valid.
	 *
	 * Makes an assertion if the callId is not valid.
	 *
	 * @param callId to check
	 */
	void checkCallId(int callId);

	/** SIP implementation. */
	SipWrapper * _sipWrapper;

	typedef std::map < int, PhoneCall * > PhoneCalls;

	/** Map of PhoneCall. */
	PhoneCalls _phoneCallMap;

	/** Active PhoneCall. */
	PhoneCall * _activePhoneCall;

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
