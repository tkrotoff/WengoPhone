/*
 * WengoPhone, a voice over Internet phone
 * Copyright (C) 2004-2005  Wengo
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

#include <sound/AudioDevice.h>

#include <util/StringList.h>

std::list<std::string> AudioDevice::getInputMixerDeviceList() {
	return StringList();
}

std::list<std::string> AudioDevice::getOutputMixerDeviceList() {
	return StringList();
}

std::string AudioDevice::getDefaultPlaybackDevice() {
	return String::null;
}

bool AudioDevice::setDefaultPlaybackDevice(const std::string & /*deviceName*/) {
	return false;
}

bool AudioDevice::setDefaultRecordDevice(const std::string & /*deviceName*/) {
	return false;
}

std::string AudioDevice::getDefaultRecordDevice() {
	return std::string("");
}

int AudioDevice::getWaveOutDeviceId(const std::string & /*deviceName*/) {
	return 0;
}

int AudioDevice::getWaveInDeviceId(const std::string & /*deviceName*/) {
	return 0;
}

int AudioDevice::getMixerDeviceId(const std::string & /*mixerName*/) {
	return 0;
}
