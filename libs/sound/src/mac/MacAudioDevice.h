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

#ifndef OWMACAUDIODEVICE_H
#define OWMACAUDIODEVICE_H

#include <sound/AudioDevice.h>

#include <CoreAudio/CoreAudio.h>

/**
 * MacOS X implementation of AudioDevice.
 *
 * @author Philippe Bernery
 */
class MacAudioDevice : public AudioDevice {
public:

	static const std::string SEPARATOR;

	/**
	 * @see AudioDevice::AudioDevice
	 */
	MacAudioDevice(const std::string & deviceId);

	MacAudioDevice(AudioDeviceID audioDeviceID, UInt32 dataSourceID, bool isInput);

	MacAudioDevice(AudioDeviceID audioDeviceID, bool isInput);

	~MacAudioDevice();

	std::string getName() const;

	std::string getId() const;

	AudioDeviceID getAudioDeviceID() const { return _audioDeviceID; }

	UInt32 getDataSourceID() const { return _dataSourceID; }

	Boolean isInput() const { return _isInput; }

private:

	/** Unserialize a MacAudioDevice from a string in this instance. */
	void unserialize(const std::string & data);

	/** Serialize this MacAudioDevice to a string.*/
	std::string serialize() const;

	/** ID of the audio device. */
	AudioDeviceID _audioDeviceID;

	/** Data source ID. */
	UInt32 _dataSourceID;

	/** Is input? */
	Boolean _isInput;
};

#endif	//OWMACAUDIODEVICE_H
