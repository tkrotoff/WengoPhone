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

#ifndef CPHONECALL_H
#define CPHONECALL_H

#include <sipwrapper/EnumPhoneCallState.h>
#include <sipwrapper/CodecList.h>

#include <string>

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

	CWengoPhone & getCWengoPhone() const {
		return _cWengoPhone;
	}

	/**
	 * FIXME this breaks the control layer.
	 * Used inside QtContactCallListWidget in order to create a ConferenceCall.
	 */
	PhoneCall & getPhoneCall() const {
		return _phoneCall;
	}

	/**
	 * Gets the current state of this phone call.
	 *
	 * @return phone call state
	 */
	EnumPhoneCallState::PhoneCallState getState() const;

	/**
	 * @see PhoneCall::close()
	 * @see PhoneCall::reject()
	 */
	void hangUp();

	/**
	 * @see PhoneCall::accept()
	 */
	void accept(bool enableVideo);

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
	 * @see PhoneCall::getAudioCodecUsed()
	 */
	CodecList::AudioCodec getAudioCodecUsed();

	/**
	 * @see PhoneCall::getVideoCodecUsed()
	 */
	CodecList::VideoCodec getVideoCodecUsed();

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

	/**
	 * Gets duration of the current call.
	 *
	 * @return duration
	 */
	int getDuration() const;

private:

	void stateChangedEventHandler(PhoneCall & sender, EnumPhoneCallState::PhoneCallState state);

	void videoFrameReceivedEventHandler(PhoneCall & sender, const WebcamVideoFrame & remoteVideoFrame,
		const WebcamVideoFrame & localVideoFrame);

	PhoneCall & _phoneCall;

	PPhoneCall * _pPhoneCall;

	CWengoPhone & _cWengoPhone;
};

#endif	//CPHONECALL_H
