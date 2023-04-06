/*
 * WengoPhone, a voice over Internet phone
 * Copyright (C) 2004-2007  Wengo
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

#ifndef OWSIPWRAPPERCALLSESSION_H
#define OWSIPWRAPPERCALLSESSION_H

#include <sipwrapper/EnumPhoneCallState.h>

#include <coipmanager/callsessionmanager/ICallSessionPlugin.h>

class SipWrapper;

/**
 * SipWrapper implementation of ICallSession.
 *
 * @author Yann Biancheri
 * @author Tanguy Krotoff
 */
class SipWrapperCallSession : public ICallSessionPlugin {
	Q_OBJECT
public:

	SipWrapperCallSession(CoIpManager & coIpManager, SipWrapper * sipWrapper,
		int callId, EnumPhoneCallState::PhoneCallState state, const std::string & from);

	SipWrapperCallSession(CoIpManager & coIpManager, SipWrapper * sipWrapper);

	virtual ~SipWrapperCallSession();

	//Inherited from CoIPModule
	virtual void start() = 0;

	virtual void stop();

	virtual void pause();

	virtual void resume();

	//Inherited from ICallSession
	virtual EnumPhoneCallState::PhoneCallState getState() const;

	virtual void playTone(EnumTone::Tone tone);

	virtual void playSoundFile(const std::string & soundFile);

	virtual CodecList::AudioCodec getAudioCodecUsed() const;

	virtual CodecList::VideoCodec getVideoCodecUsed() const;

	virtual void enableVideo(bool enable);

	virtual bool isVideoEnabled() const;

	virtual int getDuration() const;

private Q_SLOTS:

	void phoneCallStateChangedSlot(int callId,
		EnumPhoneCallState::PhoneCallState state, std::string from);

	void videoFrameReceivedSlot(int callId, piximage * remoteVideoFrame, piximage * localVideoFrame);

private:

	/**
	 * Code factorization.
	 */
	void initialize();

protected:

	SipWrapper * _sipWrapper;

	int _callId;

	EnumPhoneCallState::PhoneCallState _state;

	bool _isIncoming;
};

#endif	//OWSIPWRAPPERCALLSESSION_H
