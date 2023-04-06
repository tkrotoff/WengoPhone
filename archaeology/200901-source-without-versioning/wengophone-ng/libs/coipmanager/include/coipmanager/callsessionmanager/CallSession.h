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

#ifndef OWCALLSESSION_H
#define OWCALLSESSION_H

#include <coipmanager/callsessionmanager/ICallSession.h>
#include <coipmanager/session/Session.h>

#include <pixertool/pixertool.h>

class ICallSessionPlugin;

/**
 * Manages a call session between 2 peers.
 *
 * @ingroup CoIpManager
 * @author Philippe Bernery
 * @author Tanguy Krotoff
 */
class COIPMANAGER_API CallSession : public Session, public ICallSession {
	Q_OBJECT
	friend class CallSessionManager;
public:

	virtual ~CallSession();

	// Inherited from Session
	virtual void start();

	virtual void stop();

	virtual void pause();

	virtual void resume();
	////

	virtual EnumPhoneCallState::PhoneCallState getState() const;

	virtual void playTone(EnumTone::Tone tone);

	virtual void playSoundFile(const std::string & soundFile);

	virtual CodecList::AudioCodec getAudioCodecUsed() const;

	virtual CodecList::VideoCodec getVideoCodecUsed() const;

	virtual void enableVideo(bool enable);

	virtual bool isVideoEnabled() const;

	/**
	 * TODO
	 * This should be put inside Session since a duration can be computed
	 * for file transfert, chat session...
	 * A start time + end time should be computed too with methods like:
	 * Session::getStartedTime() Session::getEndedTime()
	 * This can be usefull for the history part of the graphical interface.
	 *
	 * @return duration of this CallSession in seconds
	 */
	virtual int getDuration() const;

Q_SIGNALS:

	/**
	 * Emitted when the state of the CallSession has changed.
	 *
	 * @param state new state
	 */
	void phoneCallStateChangedSignal(EnumPhoneCallState::PhoneCallState state);

	/**
	 * A video frame has been received from the network.
	 *
	 * @param remoteVideoFrame remote video frame
	 * @param localVideoFrame local video frame from the webcam
	 */
	void videoFrameReceivedSignal(piximage * remoteVideoFrame, piximage * localVideoFrame);

private Q_SLOTS:

	/**
	 * Slot from signal emitted by ICallSession.
	 */
	void phoneCallStateChangedSlot(EnumPhoneCallState::PhoneCallState state);

private:

	virtual bool canCreateISession(const Account & account, const ContactList & contactList) const;

	/**
	 * Constructor used for user created CallSession
	 */
	CallSession(CoIpManager & coIpManager);

	/**
	 * Constructor used when receiving a call session from outside.
	 */
	CallSession(CoIpManager & coIpManager, ICallSessionPlugin * currentCallSession);

	/** Current ICallSession. */
	ICallSessionPlugin * _currentCallSession;

	/** True if video is enabled. */
	bool _videoEnabled;
};

#endif	//OWCALLSESSION_H
