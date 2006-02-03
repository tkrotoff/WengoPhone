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

#ifndef LISTENER_H
#define LISTENER_H

#include <Interface.h>

#include <string>

/**
 * Events notification from WengoPhone.
 *
 * @author Tanguy Krotoff
 */
class Listener : Interface {
public:

	enum LoginState {
		/** Connected to the Wengo platform, login is OK. */
		LoginOk,

		/** login/password is incorrect. */
		LoginPasswordError,

		/** A network error occured. */
		LoginNetworkError,

		/** No Wengo account has been set. */
		LoginNoAccount
	};

	/**
	 * Login procedure is done, event with the procedure result.
	 *
	 * @param state login procedure result
	 * @param login Wengo login used
	 * @param password Wengo password used
	 */
	virtual void wengoLoginStateChangedEvent(LoginState state, const std::string & login, const std::string & password) = 0;

	enum PhoneLineState {
		/** Connection to the SIP proxy failed. */
		LineProxyError,

		/** Connection to the SIP server failed. */
		LineServerError,

		/** Connection to the SIP platform failed due to a timeout. */
		LineTimeout,

		/** Successfull connection to the SIP platform. */
		LineOk,

		/** Line unregistered. */
		LineClosed
	};

	/**
	 * The state of the phone line has changed.
	 *
	 * @param state new phone line state
	 * @param lineId phone line id
	 * @param param for future use
	 */
	virtual void phoneLineStateChangedEvent(PhoneLineState state, int lineId, void * param) = 0;

	enum PhoneCallState {
		/** Incoming phone call. */
		CallIncoming,

		/** Conversation state. */
		CallOk,
		//new API CallTalking,

		/** An error occured. */
		CallError,

		/** Not used for the moment. */
		CallResumeOk,

		/** Outgoing phone call: dialing. */
		CallDialing,

		/** Outgoing phone call: ringing. */
		CallRinging,

		/** Phone call closed (call rejected or call hang up). */
		CallClosed,

		/** Not used for the moment. */
		CallHoldOk
	};

	/**
	 * The state of the phone call has changed.
	 *
	 * @param state new phone call state
	 * @param lineId phone line id associated with this phone call
	 * @param callId phone call id
	 * @param sipAddress caller/callee SIP address (can be empty)
	 * @param userName caller/callee username (can be empty)
	 * @param displayName caller/callee display name (can be empty)
	 * @param param for future use
	 */
	virtual void phoneCallStateChangedEvent(PhoneCallState state, int lineId, int callId,
				const std::string & sipAddress, const std::string & userName, const std::string & displayName,
				void * param) = 0;

	enum PresenceState {
		/** Buddy online. */
		PresenceOnline,

		/** Buddy offline. */
		PresenceOffline,

		/** Buddy away. */
		PresenceAway,

		/** Buddy do not disturb. */
		PresenceDoNotDisturb,

		/** Buddy user defined status by a simple string ("I'm on the phone", "Under the shower"...). */
		PresenceUserDefined,

		/** Not used for the moment. */
		PresenceUnknown
	};

	/**
	 * Presence status of a buddy has changed.
	 *
	 * @param state new buddy presence status
	 * @param note user presence defined (state = PresenceUserDefined)
	 * @param sipAddress buddy SIP address (can be empty)
	 * @param userName buddy username (can be empty)
	 * @param displayName buddy display name (can be empty)
	 * @param param for future use
	 */
	virtual void presenceStateChangedEvent(PresenceState state, const std::string & note,
				const std::string & sipAddress, const std::string & userName, const std::string & displayName,
				void * param) = 0;

	/**
	 * Chat message received callback.
	 *
	 * @see presenceStateChangedEvent
	 * @param message message received
	 */
	virtual void chatMessageReceivedEvent(const std::string & message,
				const std::string & sipAddress, const std::string & userName, const std::string & displayName,
				void * param) = 0;

	/**
	 * Logger message received from WengoPhone.
	 *
	 * @param message message from the WengoPhone logging system
	 */
	virtual void loggerMessageAddedEvent(const std::string & message) = 0;

	enum SmsStatus {
		/** The SMS was not sent. */
		SmsStatusError,

		/** The SMS was sent. */
		SmsStatusOk
	};

	/**
	 * Callback to check if the SMS was received or not.
	 *
	 * @param status SMS status (ok or error)
	 */
	virtual void smsStatusEvent(SmsStatus status)/* = 0*/ { }

	/**
	 * A login/password is needed for the HTTP proxy.
	 *
	 * @param hostname HTTP proxy server hostname
	 * @param port HTTP proxy server port
	 */
	virtual void proxySettingsNeededEvent(const std::string & hostname, unsigned port)/* = 0*/ { }
};

#endif	//LISTENER_H
