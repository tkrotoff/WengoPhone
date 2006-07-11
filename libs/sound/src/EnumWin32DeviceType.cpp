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

#include "EnumWin32DeviceType.h"

#include <util/Logger.h>

#include <map>

typedef std::map<EnumWin32DeviceType::Win32DeviceType, std::string> DeviceTypeMap;
static DeviceTypeMap _deviceTypeMap;

static void init() {
	_deviceTypeMap[EnumWin32DeviceType::Win32DeviceTypeMasterVolume] = "Win32DeviceTypeMasterVolume";
	_deviceTypeMap[EnumWin32DeviceType::Win32DeviceTypeWaveOut] = "Win32DeviceTypeWaveOut";
	_deviceTypeMap[EnumWin32DeviceType::Win32DeviceTypeWaveIn] = "Win32DeviceTypeWaveIn";
	_deviceTypeMap[EnumWin32DeviceType::Win32DeviceTypeCDOut] = "Win32DeviceTypeCDOut";
	_deviceTypeMap[EnumWin32DeviceType::Win32DeviceTypeMicrophoneOut] = "Win32DeviceTypeMicrophoneOut";
	_deviceTypeMap[EnumWin32DeviceType::Win32DeviceTypeMicrophoneIn] = "Win32DeviceTypeMicrophoneIn";
}

std::string EnumWin32DeviceType::toString(Win32DeviceType deviceType) {
	init();
	std::string tmp = _deviceTypeMap[deviceType];
	if (tmp.empty()) {
		LOG_FATAL("unknown DeviceType=" + String::fromNumber(deviceType));
	}
	return tmp;
}

EnumWin32DeviceType::Win32DeviceType EnumWin32DeviceType::toDeviceType(const std::string & deviceType) {
	init();
	for (DeviceTypeMap::const_iterator it = _deviceTypeMap.begin();
		it != _deviceTypeMap.end();
		++it) {

		if ((*it).second == deviceType) {
			return (*it).first;
		}
	}

	LOG_FATAL("unknown DeviceType=" + deviceType);
	return Win32DeviceTypeMasterVolume;
}
