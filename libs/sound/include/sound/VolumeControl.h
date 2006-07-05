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

#ifndef OWVOLUMECONTROL_H
#define OWVOLUMECONTROL_H

#include <sound/AudioDevice.h>
#include <sound/IVolumeControl.h>

/**
 * Manipulates the volume of an audio device.
 *
 * Gets and sets the volume of an audio device
 * (a microphone or a master/wave out audio device in general).
 *
 * @see http://www.blackberry.com/developers/docs/4.1api/javax/microedition/media/control/VolumeControl.html
 * @author Tanguy Krotoff
 * @author Mathieu Stute
 * @author Philippe Bernery
 */
class VolumeControl :  public IVolumeControl {
public:

	/**
	 * @param audioDevice the AudioDevice we want to set the volume
	 * It it copied internally.
	 */
	VolumeControl(AudioDevice audioDevice);

	/**
	 * Sets the audio device volume level.
	 *
	 * @param level new audio device volume (0 to 100)
	 * @return true if the volume has been changed; false otherwise
	 */
	virtual bool setLevel(unsigned level);

	/**
	 * Gets the audio device volume level.
	 *
	 * @return the audio device volume (0 to 100); -1 if an error occured
	 */
	virtual int getLevel() const;

	/**
	 * Mute or unmute the audio device.
	 *
	 * @param mute mute state of the audio device
	 * @return true if the mute state has been changed; false otherwise
	 */
	virtual bool setMute(bool mute);

	/**
	 * Gets the mute state of the audio device.
	 *
	 * @return true if the audio device is now muted; false otherwise
	 */
	virtual bool isMuted() const;

	/**
	 * @return true if the volume is settable on this device
	 */
	virtual bool isSettable() const;

protected:

	VolumeControl();

	AudioDevice _audioDevice;

private:

	/** System dependent implementation. */
	IVolumeControl * _iVolumeControlPrivate;
};

#endif	//OWVOLUMECONTROL_H
