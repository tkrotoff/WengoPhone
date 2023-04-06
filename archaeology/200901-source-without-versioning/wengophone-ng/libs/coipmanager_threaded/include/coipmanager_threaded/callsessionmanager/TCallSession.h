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

#ifndef OWTCALLSESSION_H
#define OWTCALLSESSION_H

#include <coipmanager_threaded/session/TSession.h>

#include <sipwrapper/CodecList.h>
#include <sipwrapper/EnumPhoneCallState.h>
#include <sipwrapper/EnumTone.h>

#include <pixertool/pixertool.h>

#include <QtGui/QImage>

class TCoIpManager;
class ICallSession;
class CallSession;

/**
 * Manages a call session between 2 peers.
 *
 * @see CallSession
 * @ingroup TCoIpManager
 * @author Tanguy Krotoff
 */
class COIPMANAGER_THREADED_API TCallSession : public TSession {
	Q_OBJECT
	friend class TCallSessionManager;
public:

	~TCallSession();

	/**
	 * @see CallSession::getState()
	 */
	EnumPhoneCallState::PhoneCallState getState() const;

	/**
	 * @see CallSession::playTone()
	 */
	void playTone(EnumTone::Tone tone);

	/**
	 * @see CallSession::playSoundFile()
	 */
	void playSoundFile(const std::string & soundFile);

	/**
	 * @see CallSession::getAudioCodecUsed()
	 */
	CodecList::AudioCodec getAudioCodecUsed() const;

	/**
	 * @see CallSession::getVideoCodecUsed()
	 */
	CodecList::VideoCodec getVideoCodecUsed() const;

	/**
	 * @see CallSession::enableVideo()
	 */
	void enableVideo(bool enable);

	/**
	 * @see CallSession::isVideoEnabled()
	 */
	bool isVideoEnabled() const;

	/**
	 * @see CallSession::getDuration()
	 */
	int getDuration() const;

Q_SIGNALS:

	/**
	 * @see CallSession::phoneCallStateChangedSignal()
	 */
	void phoneCallStateChangedSignal(EnumPhoneCallState::PhoneCallState state);

	/**
	 * @see CallSession::videoFrameReceivedSignal()
	 */
	void videoFrameReceivedSignal(QImage remoteVideoFrame, QImage localVideoFrame);

private Q_SLOTS:

	void videoFrameReceivedSlot(piximage * remoteVideoFrame, piximage * localVideoFrame);

private:

	/**
	 * Constructor for TChatSession created from outside.
	 */
	TCallSession(TCoIpManager & tCoIpManager, CallSession * callSession);

	/**
	 * Code factorization.
	 */
	CallSession * getCallSession() const;
};

#endif	//OWTCALLSESSION_H
