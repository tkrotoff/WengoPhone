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

#ifndef OWENUMWIN32DEVICETYPE_H
#define OWENUMWIN32DEVICETYPE_H

#include <util/NonCopyable.h>

#include <string>

/**
 * Kind of audio mixer device to deal with.
 *
 * @author Tanguy Krotoff
 */
class EnumWin32DeviceType : NonCopyable {
public:

	enum Win32DeviceType {
		//MIXERLINE_COMPONENTTYPE_DST_SPEAKERS
		Win32DeviceTypeMasterVolume,

		//MIXERLINE_COMPONENTTYPE_SRC_WAVEOUT
		Win32DeviceTypeWaveOut,

		//MIXERLINE_COMPONENTTYPE_DST_WAVEIN
		Win32DeviceTypeWaveIn,

		//MIXERLINE_COMPONENTTYPE_SRC_COMPACTDISC
		Win32DeviceTypeCDOut,

		//MIXERLINE_COMPONENTTYPE_SRC_MICROPHONE
		Win32DeviceTypeMicrophoneOut,

		//MIXERLINE_COMPONENTTYPE_DST_WAVEIN + MIXERLINE_COMPONENTTYPE_SRC_MICROPHONE
		Win32DeviceTypeMicrophoneIn
	};

	static std::string toString(Win32DeviceType deviceType);

	static Win32DeviceType toDeviceType(const std::string & deviceType);
};

#endif	//OWENUMWIN32DEVICETYPE_H
