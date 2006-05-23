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

#ifndef AUDIODEVICE_H
#define AUDIODEVICE_H

#include <util/NonCopyable.h>

#include <list>
#include <string>

/**
 * Sound managing: gets and sets the wave out/in and default devices.
 *
 * Manage sound routines.
 * Some of the methods are Windows oriented even if the goal of this API is to be multiplatform.
 *
 * @author Philippe Kajmar
 * @author Tanguy Krotoff
 * @author Mathieu Stute
 * @author David Ferlier
 */
class AudioDevice : NonCopyable {
public:

	/**
	 * Gets the list of input (recording) mixer devices.
	 *
	 * Depending on the platform, can be the list of sound cards.
	 *
	 * @return list of input mixer devices
	 */
	static std::list<std::string> getInputMixerDeviceList();

	/**
	 * Gets the list of output mixer devices.
	 *
	 * Depending on the platform, can be the list of sound cards.
	 *
	 * @return list of output mixer devices
	 */
	static std::list<std::string> getOutputMixerDeviceList();

	/**
	 * Gets the default playback device (sound card).
	 *
	 * @return the default playback device or an empty string if could not be found
	 */
	static std::string getDefaultPlaybackDevice();

	/**
	 * Sets the default playback audio device.
	 *
	 * This is for Windows.
	 *
	 * @param deviceName name of the default playback audio device (sound card)
	 * @return true if the default playback audio device was changed; false if an error occured
	 */
	static bool setDefaultPlaybackDevice(const std::string & deviceName);

	/**
	 * Sets the default record audio device.
	 *
	 * This is for Windows.
	 *
	 * @param deviceName name of the default record audio device (sound card)
	 * @return true if the default record audio device was changed; false if an error occured
	 */
	static bool setDefaultRecordDevice(const std::string & deviceName);

	/**
	 * Gets the default record audio device.
	 *
	 * @return the default record device or an empty string if could not be found
	 */
	static std::string getDefaultRecordDevice();

	enum TypeInput {

		/** An error occured. */
		TypeInputError,

		/** Microphone. */
		TypeInputMicrophone,

		/** Line in. */
		TypeInputLineIn
	};

	/**
	 * Selects an input from an audio device to be the record input.
	 *
	 * A sound card (=audio device) can have different inputs, one should be selected as the record one.
	 *
	 * FIXME does not work currently, should be totally reworked
	 *
	 * @param deviceName name of the audio device (sound card)
	 * @param typeInput input to be selected as the record input
	 * @return true if the input is now selected; false otherwise
	 */
	static bool selectAsRecordDevice(const std::string & deviceName, TypeInput typeInput);

	/**
	 * Gets the selected record device type.
	 *
	 * FIXME does not work currently, should be totally reworked
	 *
	 * @param deviceName name of the device to be selected as the record audio device
	 * @return the input selected selected as the record one for this audio device
	 */
	static TypeInput getSelectedRecordDevice(const std::string & deviceName);

	/**
	 * Gets the wave out audio device id given its name.
	 *
	 * @param deviceName name of the wave out audio device (sound card)
	 * @return the device id corresponding to the audio device, or 0 (the default one)
	 *         or -1 if no audio device (sound card) are present on the system
	 */
	static int getWaveOutDeviceId(const std::string & deviceName);

	/**
	 * Gets the wave in audio device id given its name.
	 *
	 * @param deviceName name of the wave in audio device (sound card)
	 * @return the device id corresponding to the audio device, or 0 (the default one)
	 *         or -1 if no audio device (sound card) are present on the system
	 */
	static int getWaveInDeviceId(const std::string & deviceName);

	/**
	 * Gets the mixer audio device id given its name.
	 *
	 * @param mixerName name of the mixer audio device, can be a sound card, depends on the platform
	 * @return the device id corresponding to the audio device, or 0 (the default one)
	 *         or -1 if no audio device (sound card) are present on the system
	 */
	static int getMixerDeviceId(const std::string & mixerName);
};

#endif	//AUDIODEVICE_H
