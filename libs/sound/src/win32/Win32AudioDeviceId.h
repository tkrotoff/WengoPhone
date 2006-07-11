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

#ifndef OWWIN32AUDIODEVICEID_H
#define OWWIN32AUDIODEVICEID_H

#include <util/NonCopyable.h>

#include <string>

/**
 * Converts a audio device name to a device id using the Win32 API.
 *
 * @author Tanguy Krotoff
 */
class Win32AudioDeviceId : NonCopyable {
public:

	/**
	 * Gets the wave out of the given device name.
	 *
	 * @param deviceName the name of the device we want the id
	 * @return the id of the device
	 */
	static int getWaveOutDeviceId(const std::string & deviceName);

	/**
	 * Gets the wave in of the given device name.
	 *
	 * @param deviceName the name of the device we want the id
	 * @return the id of the device
	 */
	static int getWaveInDeviceId(const std::string & deviceName);
};

#endif	//OWWIN32AUDIODEVICEID_H
