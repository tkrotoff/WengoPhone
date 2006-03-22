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

#ifndef VOLUME_H
#define VOLUME_H

#include <SoundMixerException.h>

#include <util/NonCopyable.h>

#include <windows.h>

/**
 * Gets and changes the volume of a Windows sound mixer.
 *
 * @author Tanguy Krotoff
 */
class Volume : NonCopyable {
public:

	/**
	 * Kind of audio mixer device to deal with.
	 */
	enum DeviceType {
		//MIXERLINE_COMPONENTTYPE_DST_SPEAKERS
		MasterVolume,

		//MIXERLINE_COMPONENTTYPE_SRC_WAVEOUT
		WaveOut,

		//MIXERLINE_COMPONENTTYPE_DST_WAVEIN
		WaveIn,

		//MIXERLINE_COMPONENTTYPE_SRC_COMPACTDISC
		CDOut,

		//MIXERLINE_COMPONENTTYPE_SRC_MICROPHONE
		MicrophoneOut,

		//MIXERLINE_COMPONENTTYPE_DST_WAVEIN + MIXERLINE_COMPONENTTYPE_SRC_MICROPHONE
		MicrophoneIn
	};

	/**
	 * Constructs a Volume object.
	 *
	 * @param deviceId Windows audio device id, id of a sound card
	 * @param deviceType the kind of audio mixer device to manipulate
	 * @throw SoundMixerException if a error occured while trying to configure the audio mixer device
	 */
	Volume(unsigned int deviceId, DeviceType deviceType) throw (SoundMixerException);

	virtual ~Volume();

	/**
	 * Gets the current volume of the audio mixer.
	 *
	 * @return volume of the audio mixer, or -1 if it failed
	 */
	int getVolume();

	/**
	 * Sets the current volume of the audio mixer.
	 *
	 * @param volume sound volume of the audio mixer
	 * @return true if the volume has been changed, false otherwise
	 */
	bool setVolume(unsigned int volume);

	/**
	 * Closes the audio mixer.
	 */
	void closeMixer();

private:

	/**
	 * Inits the audio mixer device.
	 *
	 * @param deviceId Windows audio device id, id of a sound card
	 * @param deviceType the kind of audio mixer device to manipulate
	 * @return the win32 error code
	 */
	MMRESULT initVolumeControl(unsigned int deviceId, DeviceType deviceType);

	/**
	 * Creates the mixer line.
	 *
	 * @param dwComponentType type of mixer controller
	 * @return the win32 error code
	 */
	MMRESULT createMixerLine(DWORD dwComponentType);

	/**
	 * Creates the mixer controller.
	 *
	 * @return the win32 error code
	 */
	MMRESULT createMixerControl(DWORD dwControlType);

	/**
	 * Creates the mixer controller.
	 *
	 * @param dwComponentType type of mixer controller
	 * @return the win32 error code
	 */
	MMRESULT createSecondMixerLine(DWORD dwComponentType);

	/**
	 * Handle to the mixer device.
	 */
	HMIXER _hMixer;

	/**
	 * The MIXERLINE structure describes the state and metrics of an audio line.
	 */
	MIXERLINEA _mxl;

	/**
	 * The MIXERCONTROL structure describes the state and metrics of a single control for an audio line.
	 */
	MIXERCONTROLA _mxc;

	/**
	 * The MIXERLINECONTROLS structure contains information about the controls of an audio line.
	 */
	MIXERLINECONTROLSA _mxlc;

	/**
	 * Minimum volume of the audio mixer device.
	 */
	DWORD _dwMinimum;

	/**
	 * Maximum volume of the audio mixer device.
	 */
	DWORD _dwMaximum;

	/**
	 * Audio mixer-defined identifier that uniquely refers to the control
	 * described by the MIXERCONTROL structure.
	 */
	DWORD _dwVolumeControlID;
};

#endif	//VOLUME_H
