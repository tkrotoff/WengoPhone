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

#ifndef PPHONECALL_H
#define PPHONECALL_H

#include "Presentation.h"

#include <string>

class WebcamVideoFrame;

/**
 *
 * @ingroup presentation
 * @author Tanguy Krotoff
 */
class PPhoneCall : public Presentation {
public:

	virtual ~PPhoneCall() {
	}

	enum PhoneCallState {
		CallIncoming,
		CallTalking,
		CallError,
		CallResumeOk,
		CallDialing,
		CallRinging,
		CallClosed,
		CallHoldOk
	};

	virtual void phoneCallStateChangedEvent(PhoneCallState state, int lineId, int callId,
				const std::string & sipAddress, const std::string & userName, const std::string & displayName) = 0;

	virtual void close() = 0;

	virtual void videoFrameReceived(const WebcamVideoFrame & remoteVideoFrame, const WebcamVideoFrame & localVideoFrame) = 0;
};

#endif	//PPHONECALL_H
