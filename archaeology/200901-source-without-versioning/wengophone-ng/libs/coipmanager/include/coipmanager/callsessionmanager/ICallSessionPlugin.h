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

#ifndef OWICALLSESSIONPLUGIN_H
#define OWICALLSESSIONPLUGIN_H

#include <coipmanager/callsessionmanager/ICallSession.h>
#include <coipmanager/session/ISession.h>

#include <pixertool/pixertool.h>

/**
 * Interface for Call Session Pluginementation.
 *
 * @author Philippe Bernery
 * @author Tanguy Krotoff
 */
class COIPMANAGER_API ICallSessionPlugin : public ISession, public ICallSession {
	Q_OBJECT
public:

	ICallSessionPlugin(CoIpManager & coIpManager);

	virtual CodecList::AudioCodec getAudioCodecUsed() const = 0;

	virtual CodecList::VideoCodec getVideoCodecUsed() const = 0;

	virtual bool isVideoEnabled() const = 0;

	virtual int getDuration() const = 0;

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

protected:

	/** Duration in seconds. */
	int _duration;

	/** True if video is enabled. */
	bool _videoEnabled;

	/** Audio codec currently used. */
	CodecList::AudioCodec _audioCodec;

	/** Video codec currently used. */
	CodecList::VideoCodec _videoCodec;
};

#endif	//OWICALLSESSIONPLUGIN_H
