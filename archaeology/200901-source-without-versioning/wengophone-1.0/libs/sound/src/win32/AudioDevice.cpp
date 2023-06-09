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

#include <AudioDevice.h>

#include <windows.h>

/**
 * Key registry for the default playback audio device.
 */
static const char * DEFAULT_PLAYBACK_DEVICE_REGISTRY_KEY = "Software\\Microsoft\\Multimedia\\Sound Mapper\\";

/**
 * Sets the SetupPreferredAudioDevicesCount registry key.
 *
 * Part of setDefaultDevice()
 *
 * @return true if the registry key was changed, false otherwise
 */
bool setSetupPreferredAudioDevicesCount() {
	static const char * SETUPPREFERREDAUDIODEVICESCOUNT_REGISTRY_KEY = "SYSTEM\\CurrentControlSet\\Control\\MediaResources\\SetupPreferredAudioDevices\\";
	HKEY hKey;

	if (ERROR_SUCCESS == ::RegOpenKeyExA(HKEY_LOCAL_MACHINE, SETUPPREFERREDAUDIODEVICESCOUNT_REGISTRY_KEY,
						0, KEY_ALL_ACCESS, &hKey)) {

		DWORD dwDataType = REG_DWORD;
		DWORD dwSize = 4;
		DWORD setupPreferredAudioDevicesCountKeyValue = 0;

		if (ERROR_SUCCESS == ::RegQueryValueExA(hKey, "SetupPreferredAudioDevicesCount", 0, &dwDataType,
							(BYTE *) &setupPreferredAudioDevicesCountKeyValue, &dwSize)) {

			::RegCloseKey(hKey);

			::RegOpenKeyExA(HKEY_CURRENT_USER, DEFAULT_PLAYBACK_DEVICE_REGISTRY_KEY,
					0, KEY_ALL_ACCESS, &hKey);

			DWORD dwDisposition;
			::RegCreateKeyExA(HKEY_CURRENT_USER, DEFAULT_PLAYBACK_DEVICE_REGISTRY_KEY,
					0, 0, REG_OPTION_NON_VOLATILE,
					KEY_WRITE, 0, &hKey, &dwDisposition);

			if (ERROR_SUCCESS == ::RegSetValueExA(hKey, "SetupPreferredAudioDevicesCount", 0,
					dwDataType, (BYTE *) &setupPreferredAudioDevicesCountKeyValue, dwSize)) {

				::RegCloseKey(hKey);
				return true;
			}
		}
	}

	::RegCloseKey(hKey);
	return false;
}

/**
 * Sets the default audio device.
 *
 * Uses the Windows registry.
 *
 * There is no documentation at all about how to set a default audio device.
 * This is an ugly trick but it seems to work...
 *
 * @param deviceName name of the default audio device
 * @param deviceType can be "playback" or "record"
 * @return true if the default audio device was changed, false if an error occured
 */
bool setDefaultDevice(const std::string & deviceName, const std::string & deviceType) {
	HKEY hKey;
	DWORD dwDisposition;
	bool ret = false;	//Return value, false by default

	::RegOpenKeyExA(HKEY_CURRENT_USER, DEFAULT_PLAYBACK_DEVICE_REGISTRY_KEY,
			0, KEY_ALL_ACCESS, &hKey);
	::RegCreateKeyExA(HKEY_CURRENT_USER, DEFAULT_PLAYBACK_DEVICE_REGISTRY_KEY,
			0, 0, REG_OPTION_NON_VOLATILE,
			KEY_WRITE, 0, &hKey, &dwDisposition);

	if (ERROR_SUCCESS == ::RegSetValueExA(hKey, deviceType.c_str(), 0, REG_SZ,
				(const BYTE *) deviceName.c_str(), deviceName.length())) {
		ret = ret && true;
	}
	if (ERROR_SUCCESS == ::RegSetValueExA(hKey, "UserPlayback", 0, REG_SZ,
				(const BYTE *) deviceName.c_str(), deviceName.length())) {
		ret = ret && true;
	}
	if (ERROR_SUCCESS == ::RegSetValueExA(hKey, "UserRecord", 0, REG_SZ,
				(const BYTE *) deviceName.c_str(), deviceName.length())) {
		ret = ret && true;
	}

	/*DWORD valuePreferredOnly = 0;	//Why 0? I have no idea
	if (ERROR_SUCCESS == ::RegSetValueExA(hKey, "PreferredOnly", 0,
				REG_DWORD, (BYTE *) &valuePreferredOnly, sizeof(DWORD))) {
		ret = true && ret;
	}*/

	::RegCloseKey(hKey);

	/*ret = ret && */setSetupPreferredAudioDevicesCount();

	return ret;
}

std::string AudioDevice::getDefaultPlaybackDevice() {
	HKEY hKey;

	//Try to find the default playback audio device using the registry
	if (ERROR_SUCCESS == ::RegOpenKeyExA(HKEY_CURRENT_USER, DEFAULT_PLAYBACK_DEVICE_REGISTRY_KEY,
					0, KEY_QUERY_VALUE, &hKey)) {

		DWORD dwDataType = REG_SZ;
		DWORD dwSize = 255;
		char * playbackKeyValue = (char *) malloc(dwSize * sizeof(char));

		if (ERROR_SUCCESS == ::RegQueryValueExA(hKey, "Playback", 0, &dwDataType,
					(BYTE *) playbackKeyValue, &dwSize)) {

			::RegCloseKey(hKey);
			return playbackKeyValue;
		}
		free(playbackKeyValue);
	}

	WAVEOUTCAPSA outcaps;

	//We didn't find the default playback audio device using the registry
	//Try using the device id = 0 => should be the default playback audio device
	if (MMSYSERR_NOERROR == ::waveOutGetDevCapsA(0, &outcaps, sizeof(WAVEOUTCAPSA))) {
		char * deviceName = strdup(outcaps.szPname);
		return deviceName;
	}

	//Cannot determine the default playback audio device
	return "";
}

bool AudioDevice::setDefaultPlaybackDevice(const std::string & deviceName) {
	return setDefaultDevice(deviceName, "Playback");
}

bool AudioDevice::setDefaultRecordDevice(const std::string & deviceName) {
	return setDefaultDevice(deviceName, "Record");
}

StringList getMixerDeviceList(DWORD targetType) {
	unsigned int nbDevices = ::mixerGetNumDevs();
	StringList listDevices;
	MIXERCAPSA mixercaps;

	//For all the mixer devices
	for (unsigned int deviceId = 0; deviceId < nbDevices; deviceId++) {

		//Gets the capacities of the mixer device
		if (MMSYSERR_NOERROR == ::mixerGetDevCapsA(deviceId, &mixercaps, sizeof(MIXERCAPSA))) {

			//For all the destinations available through the mixer device
			for (unsigned int i = 0; i < mixercaps.cDestinations; i++) {
				MIXERLINEA mixerline;
				mixerline.cbStruct = sizeof(MIXERLINEA);
				mixerline.dwDestination = i;

				//Handle identifying the opened mixer device
				HMIXER hMixer;

				//Opens the mixer in order to get the handle identifying the opened mixer device
				if (MMSYSERR_NOERROR == ::mixerOpen(&hMixer, deviceId, NULL, NULL, MIXER_OBJECTF_MIXER)) {

					//Gets all information about the mixer line
					if (MMSYSERR_NOERROR == ::mixerGetLineInfoA((HMIXEROBJ) hMixer, &mixerline, MIXER_GETLINEINFOF_DESTINATION)) {

						//Checks if the mixer line is of type targetType
						if (mixerline.Target.dwType == targetType) {
							listDevices.push_back(mixercaps.szPname);
						}

						//Closes the opened mixer device
						::mixerClose(hMixer);
					}
				}
			}
		}
	}

	return listDevices;
}

StringList AudioDevice::getOutputMixerDeviceList() {
	return getMixerDeviceList(MIXERLINE_TARGETTYPE_WAVEOUT);
}

StringList AudioDevice::getInputMixerDeviceList() {
	return getMixerDeviceList(MIXERLINE_TARGETTYPE_WAVEIN);
}

int AudioDevice::getWaveOutDeviceId(const std::string & deviceName) {
	unsigned int nbDevices = ::waveOutGetNumDevs();
	if (nbDevices == 0) {
		//No audio device are present
		return -1;
	}

	WAVEOUTCAPSA outcaps;

	for (unsigned int deviceId = 0; deviceId < nbDevices; deviceId++) {
		if (MMSYSERR_NOERROR == ::waveOutGetDevCapsA(deviceId, &outcaps, sizeof(WAVEOUTCAPSA))) {
			if (deviceName == outcaps.szPname) {
				return deviceId;
			}
		}
	}

	//Default deviceId is 0
	return 0;
}

int AudioDevice::getWaveInDeviceId(const std::string & deviceName) {
	unsigned int nbDevices = ::waveInGetNumDevs();
	if (nbDevices == 0) {
		//No audio device are present
		return -1;
	}

	WAVEINCAPSA incaps;

	for (unsigned int deviceId = 0; deviceId < nbDevices; deviceId++) {
		if (MMSYSERR_NOERROR == ::waveInGetDevCapsA(deviceId, &incaps, sizeof(WAVEINCAPSA))) {
			if (deviceName == incaps.szPname) {
				return deviceId;
			}
		}
	}

	//Default deviceId is 0
	return 0;
}

int AudioDevice::getMixerDeviceId(const std::string & mixerName) {
	unsigned int nbMixers = ::mixerGetNumDevs();
	if (nbMixers == 0) {
		//No audio mixer device are present
		return -1;
	}

	MIXERCAPSA mixcaps;

	for (unsigned int mixerId = 0; mixerId < nbMixers; mixerId++) {
		if (MMSYSERR_NOERROR == ::mixerGetDevCapsA(mixerId, &mixcaps, sizeof(MIXERCAPSA))) {
			if (mixerName == mixcaps.szPname) {
				return mixerId;
			}
		}
	}

	//Default deviceId is 0
	return 0;
}

std::string AudioDevice::getDefaultRecordDevice() {
	return "";
}

