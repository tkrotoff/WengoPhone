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

#ifndef AUDIODEVICE_H
#define AUDIODEVICE_H

#include <NonCopyable.h>
#include <StringList.h>

/**
 * Sound managing: get and set the wave out/in and default devices for Windows.
 *
 * Manage sound Windows routines.
 *
 * @author Philippe Kajmar
 * @author Tanguy Krotoff
 */
class AudioDevice : NonCopyable {
public:

	/**
	 * Gets the list of input (recording) mixer devices.
	 *
	 * @return list of input mixer devices
	 */
	static StringList getInputMixerDeviceList();

	/**
	 * Gets the list of output mixer devices.
	 *
	 * @return list of output mixer devices
	 */
	static StringList getOutputMixerDeviceList();

	/**
	 * Gets the default playback device.
	 *
	 * @return the default playback device or an empty string if could not be found
	 */
	static std::string getDefaultPlaybackDevice();

	/**
	 * Sets the default playback audio device.
	 *
	 * @param deviceName name of the default playback audio device
	 * @return true if the default playback audio device was changed, false if an error occured
	 */
	static bool setDefaultPlaybackDevice(const std::string & deviceName);

	/**
	 * Sets the default record audio device.
	 *
	 * @param deviceName name of the default record audio device
	 * @return true if the default record audio device was changed, false if an error occured
	 */
	static bool setDefaultRecordDevice(const std::string & deviceName);

	/**
	 * Gets the default record audio device.
	 *
	 * @return the default record device or an empty string if could not be found
	 */
	static std::string getDefaultRecordDevice();

	/**
	 * Gets the wave out audio device id given its name.
	 *
	 * @param deviceName name of the wave out audio device
	 * @return the device id corresponding to the audio device, or 0 (the default one)
	 *         or -1 if no audio device (sound card) are present on the system
	 */
	static int getWaveOutDeviceId(const std::string & deviceName);

	/**
	 * Gets the wave in audio device id given its name.
	 *
	 * @param deviceName name of the wave in audio device
	 * @return the device id corresponding to the audio device, or 0 (the default one)
	 *         or -1 if no audio device (sound card) are present on the system
	 */
	static int getWaveInDeviceId(const std::string & deviceName);

	/**
	 * Gets the mixer audio device id given its name.
	 *
	 * @param mixerName name of the mixer audio device
	 * @return the device id corresponding to the audio device, or 0 (the default one)
	 *         or -1 if no audio device (sound card) are present on the system
	 */
	static int getMixerDeviceId(const std::string & mixerName);
};

#endif	//AUDIODEVICE_H
