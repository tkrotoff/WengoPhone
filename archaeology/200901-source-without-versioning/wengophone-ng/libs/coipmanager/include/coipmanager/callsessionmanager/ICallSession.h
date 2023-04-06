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

#ifndef OWICALLSESSION_H
#define OWICALLSESSION_H

#include <sipwrapper/CodecList.h>
#include <sipwrapper/EnumPhoneCallState.h>
#include <sipwrapper/EnumTone.h>

#include <util/Interface.h>

/**
 * Interface for CallSession.
 *
 * @author Philippe Bernery
 * @author Tanguy Krotoff
 */
class ICallSession : Interface {
public:

	/**
	 * Gets the current state of this CallSession.
	 *
	 * @return state of this CallSession
	 */
	virtual EnumPhoneCallState::PhoneCallState getState() const = 0;

	/**
	 * Plays a tone during the call.
	 *
	 * @param tone the tone to play
	 */
	virtual void playTone(EnumTone::Tone tone) = 0;

	/**
	 * Plays a sound file. Supported format depends on the implementation.
	 *
	 * @param soundFile the path to the file to play
	 */
	virtual void playSoundFile(const std::string & soundFile) = 0;

	/**
	 * Gets the audio codec used for the call.
	 */
	virtual CodecList::AudioCodec getAudioCodecUsed() const = 0;

	/**
	 * Gets the video codec used for the call.
	 */
	virtual CodecList::VideoCodec getVideoCodecUsed() const = 0;

	/**
	 * Enables or disables video usage.
	 *
	 * @param enable true if video should be activated; false otherwise
	 */
	virtual void enableVideo(bool enable) = 0;

	/**
	 * @return true video is enabled for this CallSession
	 */
	virtual bool isVideoEnabled() const = 0;

	/**
	 * @return duration of this CallSession in seconds
	 */
	virtual int getDuration() const = 0;

protected:

	/** True if video is enabled. */
	bool _videoEnabled;

	/** Audio codec currently used. */
	CodecList::AudioCodec _audioCodec;

	/** Video codec currently used. */
	CodecList::VideoCodec _videoCodec;

};

#endif	//OWICALLSESSION_H
