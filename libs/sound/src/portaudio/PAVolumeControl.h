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

#ifndef OWPAVOLUMECONTROL_H
#define OWPAVOLUMECONTROL_H

#include <sound/VolumeControl.h>

/**
 * @see VolumeControl
 */
class PAVolumeControl :  public VolumeControl {
public:

	/**
	 * @see VolumeControl::VolumeControl
	 */
	PAVolumeControl(AudioDevice audioDevice);

	bool setLevel(unsigned level);

	int getLevel() const;

	bool setMute(bool mute);

	bool isMuted() const;

	bool isSettable() const;
};

#endif	//OWPAVOLUMECONTROL_H