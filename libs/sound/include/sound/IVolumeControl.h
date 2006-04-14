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

#ifndef IVOLUMECONTROL_H
#define IVOLUMECONTROL_H

#include <util/Interface.h>

#include <string>

/**
 * Manipulates the volume of an audio device.
 *
 * Gets and sets the volume of an audio device
 * (a microphone or a master/wave out audio device in general).
 *
 * @see VolumeControl
 * @see http://www.blackberry.com/developers/docs/4.1api/javax/microedition/media/control/VolumeControl.html
 * @author Tanguy Krotoff
 * @author Mathieu Stute
 */
class IVolumeControl : Interface {
public:

	enum DeviceType {
		/** Input audio device (record device like microphone). */
		DeviceTypeInput,

		/** Output audio device (master volume). */
		DeviceTypeOutput,
	};

	/**
	 * Sets the audio device volume level.
	 *
	 * @param level new audio device volume (0 to 100)
	 * @return true if the volume has been changed; false otherwise
	 */
	virtual bool setLevel(unsigned level) = 0;

	/**
	 * Gets the audio device volume level.
	 *
	 * @return the audio device volume (0 to 100); -1 if an error occured
	 */
	virtual int getLevel() = 0;

	/**
	 * Mute or unmute the audio device.
	 *
	 * @param mute mute state of the audio device
	 * @return true if the mute state has been changed; false otherwise
	 */
	virtual bool setMute(bool mute) = 0;

	/**
	 * Gets the mute state of the audio device.
	 *
	 * @return true if the audio device is now muted; false otherwise
	 */
	virtual bool isMuted() = 0;

	/**
	 * Selects this audio device to be the record device.
	 *
	 * Works only the audio device is a DeviceTypeInput (input device/record device).
	 *
	 * @return true if the audio device is now selected; false otherwise
	 */
	virtual bool selectAsRecordDevice() = 0;

	/**
	 * Gets the selected state of the audio device as a record device.
	 *
	 * Works only the audio device is a DeviceTypeInput (input device/record device).
	 *
	 * @return true if the audio device is a selected record device; false otherwise
	 */
	//virtual bool isSelectedAsRecordDevice() = 0;

	//MIXERCONTROL_CONTROLTYPE_PEAKMETER
	//virtual unsigned getPeakMeterLevel() = 0;

	/**
	 * Closes the audio device mixer.
	 *
	 * @return true if the audio device has been closed; false otherwise
	 */
	virtual bool close() = 0;
};

#endif	//IVOLUMECONTROL_H
