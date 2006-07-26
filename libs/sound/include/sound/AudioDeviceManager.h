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

#ifndef OWAUDIODEVICEMANAGER_H
#define OWAUDIODEVICEMANAGER_H

#include <sound/AudioDevice.h>

#include <util/NonCopyable.h>
#include <thread/Mutex.h>

#include <list>

/**
 * Sound managing: gets and sets the default input/output audio device and
 * available audio devices on the platform.
 *
 * Manage sound routines.
 *
 * @author Philippe Kajmar
 * @author Tanguy Krotoff
 * @author Mathieu Stute
 * @author David Ferlier
 * @author Philippe Bernery
 */
class AudioDeviceManager : NonCopyable {
public:

	/**
	 * Gets the list of input (recording) audio devices.
	 *
	 * @return list of input audio devices
	 */
	static std::list<AudioDevice> getInputDeviceList();

	/**
	 * Gets the list of output audio devices.
	 *
	 * @return list of output audio devices
	 */
	static std::list<AudioDevice> getOutputDeviceList();

	/**
	 * Gets the default output device.
	 *
	 * @return the default output device
	 */
	static AudioDevice getDefaultOutputDevice();

	/**
	 * Sets the default output audio device.
	 *
	 * @param device the desired device
	 * @return true if the default output audio device has been changed,
	 * false if an error occured
	 */
	static bool setDefaultOutputDevice(const AudioDevice & audioDevice);

	/**
	 * Gets the default input audio device.
	 *
	 * @return the default input device
	 */
	static AudioDevice getDefaultInputDevice();

	/**
	 * Sets the default input audio device.
	 *
	 * @param device the desired device
	 * @return true if the default input audio device has been changed
	 * false if an error occured
	 */
	static bool setDefaultInputDevice(const AudioDevice & audioDevice);

private:

	static Mutex _mutex;
};

#endif	//OWAUDIODEVICEMANAGER_H
