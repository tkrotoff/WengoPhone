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

#include "XPCOMPhoneCall.h"

#include <Listener.h>
#include "ListenerList.h"

#include <control/phonecall/CPhoneCall.h>

#include <util/Logger.h>

XPCOMPhoneCall::XPCOMPhoneCall(CPhoneCall & cPhoneCall)
	: _cPhoneCall(cPhoneCall) {
}

void XPCOMPhoneCall::phoneCallStateChangedEvent(PhoneCallState state, int lineId, int callId,
				const std::string & sipAddress, const std::string & userName, const std::string & displayName) {

	PhoneCallMap & phoneCallMap = PhoneCallMap::getInstance();
	phoneCallMap[callId] = this;

	ListenerList & listenerList = ListenerList::getInstance();
	for (unsigned i = 0; i < listenerList.size(); i++) {
		Listener * listener = listenerList[i];

		switch (state) {
		case CallIncoming:
			listener->phoneCallStateChangedEvent(Listener::CallIncoming, lineId, callId, sipAddress, userName, displayName, NULL);
			break;

		case CallTalking:
			listener->phoneCallStateChangedEvent(Listener::CallOk, lineId, callId, sipAddress, userName, displayName, NULL);
			break;

		case CallError:
			listener->phoneCallStateChangedEvent(Listener::CallError, lineId, callId, sipAddress, userName, displayName, NULL);
			break;

		case CallResumeOk:
			listener->phoneCallStateChangedEvent(Listener::CallResumeOk, lineId, callId, sipAddress, userName, displayName, NULL);
			break;

		case CallDialing:
			listener->phoneCallStateChangedEvent(Listener::CallDialing, lineId, callId, sipAddress, userName, displayName, NULL);
			break;

		case CallRinging:
			listener->phoneCallStateChangedEvent(Listener::CallRinging, lineId, callId, sipAddress, userName, displayName, NULL);
			break;

		case CallClosed:
			listener->phoneCallStateChangedEvent(Listener::CallClosed, lineId, callId, sipAddress, userName, displayName, NULL);
			break;

		case CallHoldOk:
			listener->phoneCallStateChangedEvent(Listener::CallHoldOk, lineId, callId, sipAddress, userName, displayName, NULL);
			break;

		default:
			LOG_FATAL("Unknown event");
		}
	}
}

void XPCOMPhoneCall::hangUp() {
	_cPhoneCall.hangUp();
}

bool XPCOMPhoneCall::canHangUp() const {
	return _cPhoneCall.canHangUp();
}

void XPCOMPhoneCall::pickUp() {
	_cPhoneCall.pickUp();
}

bool XPCOMPhoneCall::canPickUp() const {
	return _cPhoneCall.canPickUp();
}
