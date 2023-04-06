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

#ifndef AUDIOCALL_H
#define AUDIOCALL_H

#include "AudioCallEvent.h"

#include <observer/Subject.h>


#include <qstring.h>

class PhonePageWidget;
class SessionWindow;
class SipState;
class Contact;
class CallHistory;
class SipAddress;
class QTimer;
class CallHistoryWidget;

/**
 * Event for the state machine SipState.
 *
 * @see phapi
 */
typedef struct phCallStateInfo CallStateInfo;

/**
 * Class that wraps an audio call.
 *
 * Design Pattern Singleton.
 * Design Pattern State.
 *
 * Makes and handles a complete phone/audio call.
 * This class uses phoneapi (phapi) to accomplish this task.
 *
 * TODO Error gestion with exceptions?
 *
 * @author Tanguy Krotoff
 * @author Mathieu Stute
 */
class AudioCall : public Subject {

	/**
	 * To be allocated only by AudioCallManager
	 */
	friend class AudioCallManager;

public:

	/**
	 * If there is no audio conversation, no current call.
	 */
	static const int NOT_CALLING;

	~AudioCall();

	/**
	 * Sets the current SipState.
	 *
	 * @param sipState new SipState
	 */
	void setSipState(SipState * sipState);

	SipState * getSipState() const {
		return _sipState;
	}

	/**
	 * Sets the ID of the current call.
	 *
	 * @param callId call ID
	 */
	void setCallId(int callId) {
		_previousCallId = _callId;
		_callId = callId;
		notify(AudioCallEvent(AudioCallEvent::CallIdChanged));
	}

	/**
	 * Gets the current call ID.
	 *
	 * @return call ID
	 */
	int getCallId() const {
		return _callId;
	}

	int getPreviousCallId() const {
		return _previousCallId;
	}

	/**
	 * @return if the audiocall is used in conversation 
	 */
	bool isCalling() const;

	/**
	 * @return if the state is waiting or dialing
	 */
	bool isWaiting() const;

	/**
	 * Gets the widget that represents a phone.
	 *
	 * @return phone widget component
	 */
	PhonePageWidget & getPhonePageWidget() const;

	/**
	 * Gets the SessionWindow that corresponds to this call.
	 *
	 * This method is the only way to get a SessionWindow.
	 *
	 * @return SessionWindow
	 */
	SessionWindow & getSessionWindow() const;

	/**
	 * Gets the CallHistory class to record calls (incoming, outgoing, missed).
	 *
	 * @return CallHistory
	 */
	CallHistory & getCallHistory() const;

	void enableVideo(bool enableVideo) {
		_enableVideo = enableVideo;
	}

	bool isVideoEnabled() const {
		return _enableVideo;
	}

private:

	AudioCall();

	/**
	 * Callback from phapi in order to check the progress of the AudioCall.
	 *
	 * This method is typically a big switch. This method calls the method handle()
	 * from the current SipState.
	 *
	 * @param callId ID of the call
	 * @param info state info of the call
	 * @see SipState
	 */
	void callProgress(int callId, const CallStateInfo * info);

	/**
	 * Calls a phone number.
	 *
	 * @param sipAddress SIP address to call
	 * @return true if no error, false otherwise
	 */
	bool call(const SipAddress & sipAddress);

	AudioCall(const AudioCall &);
	AudioCall & operator=(const AudioCall &);

	/**
	 * SessionWindow, SipState doesn't deal with it.
	 */
	SessionWindow * _sessionWindow;

	/**
	 * Phone component, SipState deals with it.
	 */
	PhonePageWidget * _phonePageWidget;

	/**
	 * State machine.
	 *
	 * Design Pattern State.
	 */
	SipState * _sipState;

	/**
	 * ID of the current call.
	 *
	 * Initialized to NOT_CALLING since there is no current call.
	 */
	int _callId;

	int _previousCallId;

	bool _enableVideo;
};

#endif	//AUDIOCALL_H
