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

#ifndef PHONECALL_H
#define PHONECALL_H

#include "SipAddress.h"

#include <sipwrapper/EnumTone.h>
#include <sipwrapper/EnumPhoneCallState.h>

#include <Event.h>
#include <List.h>

#include <string>

class WebcamVideoFrame;
class PhoneCallState;
class IPhoneLine;
class WenboxPlugin;

/**
 * Class that holds informations about a current phone call.
 *
 * A phone call is associated with a PhoneLine.
 *
 * @ingroup model
 * @author Tanguy Krotoff
 */
class PhoneCall {
public:

	/**
	 * The state of the PhoneCall has changed.
	 *
	 * @param sender this class
	 * @param status new status
	 */
	Event<void (PhoneCall & sender, EnumPhoneCallState::PhoneCallState status)> stateChangedEvent;

	/**
	 * A video frame has been received from the network.
	 *
	 * @param sender this class
	 * @param remoteVideoFrame remote video frame
	 * @param localVideoFrame local video frame from the webcam
	 */
	Event<void (PhoneCall & sender, const WebcamVideoFrame & remoteVideoFrame, const WebcamVideoFrame & localVideoFrame)> videoFrameReceivedEvent;

	/**
	 * Creates a new PhoneCall given a PhoneLine.
	 *
	 * @param phoneLine PhoneLine associated with this PhoneCall
	 * @param sipAddress caller/callee/peer SIP address
	 */
	PhoneCall(IPhoneLine & phoneLine, const SipAddress & sipAddress);

	~PhoneCall();

	/**
	 * @see SipWrapper::acceptCall()
	 */
	void accept();

	/**
	 * @see SipWrapper::resumeCall()
	 */
	void resume();

	/**
	 * @see SipWrapper::holdCall()
	 */
	void hold();

	/**
	 * @see IPhoneLine::closeCall()
	 */
	void close();

	/**
	 * @see SipWrapper::mute()
	 */
	void mute();

	/**
	 * @see SipWrapper::playTone()
	 */
	void playTone(EnumTone::Tone tone);

	/**
	 * @see SipWrapper::playSoundFile()
	 */
	void playSoundFile(const std::string & soundFile);

	/**
	 * Video frame received.
	 *
	 * @param remoteVideoFrame received
	 * @param localWebcam local webcam
	 */
	void videoFrameReceived(const WebcamVideoFrame & remoteVideoFrame, const WebcamVideoFrame & localVideoFrame);

	/**
	 * Changes the state of this PhoneCall.
	 *
	 * @param status status code corresponding to the new PhoneCall state
	 */
	void setState(EnumPhoneCallState::PhoneCallState status);

	/**
	 * Gets the current state of this PhoneCall.
	 *
	 * @return state of this PhoneCall
	 */
	const PhoneCallState & getState() const {
		return *_state;
	}

	/**
	 * Sets the call id associated with this PhoneCall.
	 *
	 * @param callId call id of this PhoneCall
	 */
	void setCallId(int callId) {
		_callId = callId;
	}

	/**
	 * Gets the call id of this PhoneCall.
	 *
	 * @return call id of this PhoneCall
	 */
	int getCallId() const {
		return _callId;
	}

	/**
	 * Gets the caller/callee/peer SIP address associated with this PhoneCall.
	 *
	 * @return caller/callee/peer SIP address
	 */
	const SipAddress & getPeerSipAddress() const {
		return _sipAddress;
	}

	/**
	 * Gets the PhoneLine associated with this PhoneCall.
	 *
	 * @return phone line
	 */
	IPhoneLine & getPhoneLine() const {
		return _phoneLine;
	}

	WenboxPlugin & getWenboxPlugin() const;

	/**
	 * Gets the duration of the call in seconds
	 *
	 * @return duration of the call
	 */
	int getDuration() {
		return _duration;
	}
	
private:

	/** PhoneLine associated with this PhoneCall. */
	IPhoneLine & _phoneLine;

	/** Call id of this PhoneCall. */
	int _callId;

	/** Caller/callee/peer SIP address. */
	SipAddress _sipAddress;

	/** Current state of this PhoneCall. */
	PhoneCallState * _state;

	/** Defines the vector of PhoneCallState. */
	typedef List < PhoneCallState * > PhoneCallStates;

	/** List of PhoneCallState. */
	PhoneCallStates _phoneCallStateList;

	/** The duration of the call in second */
	int _duration;

	/** store temporary the time */
	int _temp;
};

#endif	//PHONECALL_H
