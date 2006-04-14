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

#ifndef VOLUMECONTROL_H
#define VOLUMECONTROL_H

#include <sound/IVolumeControl.h>

#include <sound/SoundMixerException.h>
#include <sound/NoSoundCardException.h>

#include <string>

/**
 * Manipulates the volume of an audio device.
 *
 * @see IVolumeControl
 * @author Tanguy Krotoff
 * @author Mathieu Stute
 */
class VolumeControl : public IVolumeControl {
public:

	/**
	 * Constructs a VolumeControl object.
	 *
	 * @param deviceName audio device name
	 * @param deviceType type of audio device (input or output)
	 * @exception NoSoundCardException no sound card available on the system
	 * @exception SoundMixerException an error occured while manipulating an audio mixer
	 */
	VolumeControl(const std::string & deviceName, DeviceType deviceType) throw (NoSoundCardException, SoundMixerException);

	virtual ~VolumeControl() {
		close();
	}

	bool setLevel(unsigned level);

	int getLevel();

	bool setMute(bool mute);

	bool isMuted();

	bool selectAsRecordDevice();

	bool close();

private:

	/** System-dependant implementation. */
	IVolumeControl * _volumeControlPrivate;
};

#endif	//VOLUMECONTROL_H
