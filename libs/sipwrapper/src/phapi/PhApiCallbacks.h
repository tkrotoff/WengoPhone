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

#ifndef PHAPICALLBACKS_H
#define PHAPICALLBACKS_H

#include <phapi.h>

/**
 * PhApi callbacks.
 *
 * @ingroup model
 * @author Tanguy Krotoff
 * @author Mathieu Stute
 */
class PhApiCallbacks {
public:

	PhApiCallbacks();

	void callProgress(int callId, const phCallStateInfo_t * info);

	void videoFrameReceived(int callId, phVideoFrameReceivedEvent_t * info);

	void transferProgress(int callId, const phTransferStateInfo_t * info);

	void conferenceProgress(int conferenceId, const phConfStateInfo_t * info);

	void registerProgress(int lineId, int status);

	void messageProgress(int messageId, const phMsgStateInfo_t * info);

	void subscriptionProgress(int subscriptionId, const phSubscriptionStateInfo_t * info);

	void onNotify(const char * event, const char * from, const char * content);
};

#endif	//PHAPICALLBACKS_H
