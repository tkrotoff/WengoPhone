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

#ifndef XPCOMPHONECALL_H
#define XPCOMPHONECALL_H

#include <presentation/PPhoneCall.h>

#include <NonCopyable.h>

#include <map>

class CPhoneCall;

class XPCOMPhoneCall : public PPhoneCall {
public:

	XPCOMPhoneCall(CPhoneCall & cPhoneCall);

	void close() { }

	void phoneCallStateChangedEvent(PhoneCallState state, int lineId, int callId,
				const std::string & sipAddress, const std::string & userName, const std::string & displayName);

	void videoFrameReceived(const VideoFrame & frame, const LocalWebcam & localWebcam) { }

	void updatePresentation() { }

	void hangUp();

	bool canHangUp() const;

	void pickUp();

	bool canPickUp() const;

private:

	void updatePresentationThreadSafe() { }

	void initThreadSafe() { }

	CPhoneCall & _cPhoneCall;
};

class PhoneCallMap : NonCopyable, public std::map<int, XPCOMPhoneCall *> {
public:

	/** Singleton. */
	static PhoneCallMap & getInstance() {
		static PhoneCallMap phoneCallMap;

		return phoneCallMap;
	}

private:

	PhoneCallMap() { }
};

#endif	//XPCOMPHONECALL_H
