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

#ifndef CPHONECALL_H
#define CPHONECALL_H

#include <string>

#include <sipwrapper/EnumPhoneCallState.h>

class PhoneCall;
class PPhoneCall;
class WengoPhone;
class CWengoPhone;
class WebcamVideoFrame;

/**
 *
 * @ingroup control
 * @author Tanguy Krotoff
 */
class CPhoneCall {
public:

	CPhoneCall(PhoneCall & phoneCall, CWengoPhone & cWengoPhone);

	PPhoneCall * getPresentation() const {
		return _pPhoneCall;
	}

	void hangUp();

	/**
	 * S'il est possible de raccrocher le telephone.
	 */
	bool canHangUp() const;

	void pickUp();

	/**
	 * S'il est possible de decrocher le telephone.
	 */
	bool canPickUp() const;

	/**
	 * @see PhoneCall::mute()
	 */
	void mute();

	/**
	 * @see PhoneCall::hold()
	 */
	void hold();

	/**
	 * @see PhoneCall::resume()
	 */
	void resume();

	/**
	 * @see PhoneCall::blindTransfer()
	 */
	void blindTransfer(const std::string & phoneNumber);

	/**
	 * Gets the peer SIP address (phone number) of the current call.
	 *
	 * @return SIP address
	 */
	std::string getPeerSipAddress() const;

	/**
	 * Gets the peer username part of the SIP address.
	 *
	 * @return peer username
	 */
	std::string getPeerUserName() const;

	/**
	 * Gets the peer display name part of the SIP address.
	 *
	 * @return peer display name
	 */
	std::string getPeerDisplayName() const;

	CWengoPhone & getCWengoPhone() const {
		return _cWengoPhone;
	}

private:

	void stateChangedEventHandler(PhoneCall & sender, EnumPhoneCallState::PhoneCallState state);

	void videoFrameReceivedEventHandler(PhoneCall & sender, const WebcamVideoFrame & remoteVideoFrame,
		const WebcamVideoFrame & localVideoFrame);

	PhoneCall & _phoneCall;

	PPhoneCall * _pPhoneCall;

	CWengoPhone & _cWengoPhone;
};

#endif	//CPHONECALL_H
