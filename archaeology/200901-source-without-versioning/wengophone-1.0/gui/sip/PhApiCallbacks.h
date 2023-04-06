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

#ifndef PHAPICALLBACKS_H
#define PHAPICALLBACKS_H

#include <phapi.h>

class AudioCallManager;
class Presence;

/**
 * PhApi function callbacks.
 *
 * Configures the callbacks for phapi.
 *
 * @author Tanguy Krotoff
 */
class PhApiCallbacks {
public:

	static void setAudioCallManager(AudioCallManager & audioCallManager);

	/**
	 * Initializes phApi.
	 *
	 * @return -1 if initPhApi failed
	 */
	static int initPhApi();

	/**
	 * Call progress callback routine.
	 *
	 * @param callId call id
	 * @param info call state information
	 */
	static void callProgress(int callId, const struct phCallStateInfo * info);

	/**
	 * Transfer progress callback routine.
	 *
	 * @param cid transfer id
	 * @param info transfer state information
	 */
	static void transferProgress(int callId, const struct phCallStateInfo * info);

	/**
	 * Conference progress callback routine.
	 *
	 * @param conferenceId conference id
	 * @param info conference state information
	 */
	static void conferenceProgress(int conferenceId, const struct phCallStateInfo * info);

	/**
	 * A new chat message has been received.
	 *
	 * @param from user who is sending the message
	 * @param to address of the receiver (me)
	 * @param contentType "text"
	 * @param subType "plain"
	 * @param content the content of the chat message
	 */
	static void messageProgress(int messageId,  const struct phMsgStateInfo * info);

	/**
	 * Subscription progress callback routine.
	 *
	 * @param sid  id of the subscription  // Not being used actually	 
	 * @param subscription state infomation
	 */
	static void subscriptionProgress(int sid,  const struct phSubscriptionStateInfo *info);
	/**
	 * On each SIP presence message received.
	 *
	 * @param event kind of SIP presence event: presence of presence.winfo
	 * @param from SIP address the SIP presence message is coming from
	 * @param content XML content of the SIP presence message received
	 */
	static void onNotify(const char * event, const char * from, const char * content);
	static void onFrameReady(int cid, struct phVideoFrameReceivedEvent *ev);
	
	/**
	 * On sound card open / close problem
	 *
	 * @param message the error message
	 */
	static void errorNotify(enum phErrors error);

private:

	PhApiCallbacks();
	PhApiCallbacks(const PhApiCallbacks &);
	PhApiCallbacks & operator=(const PhApiCallbacks &);
	~PhApiCallbacks();

	/**
	 * For the AudioCall callback.
	 */
	static AudioCallManager * _audioCallManager;

	static Presence * _presence;
};

#endif	//PHAPICALLBACKS_H
