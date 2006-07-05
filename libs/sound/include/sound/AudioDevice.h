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

#ifndef OWAUDIODEVICE_H
#define OWAUDIODEVICE_H

#include <string>

/**
 * Identifies a part of an audio device.
 * e.g: the microphone input of device #1
 *
 * @author Philippe Bernery
 */
class AudioDevice {
public:

	/**
	 * Constructs an AudioDevice from a string identifying uniquely a 
	 * sound device.
	 *
	 * @param deviceId the device ID. e.g: on MacOS X this will a list of 
	 * integer with the following scheme: "deviceId:dataSource:isInput"
	 * @see getId for more information.
	 */
	AudioDevice(const std::string & deviceId);

	AudioDevice();

	AudioDevice(const AudioDevice & audioDevice);

	AudioDevice & operator = (const AudioDevice & audioDevice);

	virtual ~AudioDevice();

	virtual std::string getName() const;

	virtual std::string getId() const;

	/** Gets the system dependent implementation. */
	AudioDevice * getAudioDevicePrivate() const { return _audioDevicePrivate; }

private:

	/** System-dependent implementation. */
	AudioDevice * _audioDevicePrivate;
};

#endif //OWAUDIODEVICE_H
