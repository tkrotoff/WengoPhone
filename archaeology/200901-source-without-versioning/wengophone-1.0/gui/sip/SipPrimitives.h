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

#ifndef SIPPRIMITIVES_H
#define SIPPRIMITIVES_H

#include "SipAddress.h"

#include <qobject.h>

class QString;

/**
 * SIP phone primitives.
 *
 * High level component above phApi.
 *
 * @author Tanguy Krotoff
 * @author Mathieu Stute
 */
class SipPrimitives {
public:

	/**
	 * Places a video phone call.
	 *
	 * @param sipAddress call destination address
	 * @return the call Id or -1 if failed
	 */
	static int placeVideoCall(const SipAddress & sipAddress);

	/**
	 * Places an audio phone call.
	 *
	 * @param sipAddress call destination address
	 * @return the call Id or -1 if failed
	 */
	static int placeAudioCall(const SipAddress & sipAddress);

	/**
	 * Accepts an incoming video call.
	 *
	 * @param callId call identifier
	 *
	 * @brief accept a call
	 */
	static void acceptVideoCall(int callId);

	/**
	 * Accepts an incoming audio call.
	 *
	 * @param callId call identifier
	 *
	 * @brief accept a call
	 */
	static void acceptAudioCall(int callId);

	/**
	 * Rejects an incoming call.
	 *
	 * @param callId call identifier
	 * @param reason SIP reason code for the rejection
	 */
	static void rejectCall(int callId, int reason);

	/**
	 * Close a call given its id.
	 *
	 * @param callId call identifier
	 */
	static void closeCall(int callId);

	/**
	 * Sends the ringing event.
	 *
	 * @param callId call identifier
	 */
	static void ringingCall(int callId);

	/**
	 * Holds a call given its id.
	 *
	 * @param callId call identifier
	 */
	static void holdCall(int callId);

	/**
	 * Resumes a call given its id.
	 *
	 * @param callId call identifier
	 */
	static void resumeCall(int callId);

	/**
	 * Plays a DTMF into a call conversation.
	 *
	 * If the user is not calling, stock the DTMF and play it when the user calls.
	 *
	 * @param callId call identifier
	 * @param dtmf DTMF character
	 */
	static void playDTMF(int callId, char dmtf);

	/**
	 * Sends an instant message to a SIP URI.
	 *
	 * @param sipAddress SIP URI of the receiver of the message
	 * @param message message to send
	 * @return true if the message was sent, false otherwise
	 */
	static bool sendMessage(const SipAddress & sipAddress, const char * message);

	/**
	 * Asks for the list of all the persons that wants to know
	 * my presence status.
	 */
	static bool askForWatcherList();

	/**
	 * Subscribes for the presence status of somebody.
	 *
	 * @param sipAddress person I want to get its presence status
	 */
	static bool subscribeToPresence(const SipAddress & sipAddress);

	/**
	 * Allows somebody to see my presence status.
	 *
	 * @param sipAddress person allowed to see my presence status
	 */
	static bool allowWatcher(const SipAddress & sipAddress);

	/**
	 * Forbids somebody to see my presence status.
	 *
	 * @param sipAddress person forbidden to see my presence status
	 */
	static bool forbidWatcher(const SipAddress & sipAddress);

	/**
	 * Sets my presence status to online.
	 *
	 * @param note personnalized status string ("I'm under the shower")
	 */
	static bool publishOnline(const QString & note);

	/**
	 * Sets my presence to offline.
	 */
	static bool publishOffline();

private:

	SipPrimitives();
	SipPrimitives(const SipPrimitives &);
	SipPrimitives & operator=(const SipPrimitives &);
	~SipPrimitives();
};

#endif	//SIPPRIMITIVES_H
