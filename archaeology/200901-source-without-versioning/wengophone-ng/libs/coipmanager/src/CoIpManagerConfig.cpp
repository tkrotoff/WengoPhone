/*
 * WengoPhone, a voice over Internet phone
 * Copyright (C) 2004-2007  Wengo
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

#include <coipmanager/CoIpManagerConfig.h>

#include <networkdiscovery/NetworkProxyDiscovery.h>

#include <sound/AudioDeviceManager.h>

#include <webcam/WebcamDriver.h>

#include <sipwrapper/EnumVideoQuality.h>

#include <util/File.h>
#include <util/Path.h>

#include <cstdlib>

const std::string CoIpManagerConfig::CONFIG_PATH_KEY = "path.profiles";
const std::string CoIpManagerConfig::COIP_PLUGINS_PATH_KEY = "path.coip-plugins";
const std::string CoIpManagerConfig::AUTO_SAVE_KEY = "autosave";
const std::string CoIpManagerConfig::HTTP_PROXY_AUTO_DETECT_KEY = "httpproxy.autodetect";
const std::string CoIpManagerConfig::HTTP_PROXY_SERVER_KEY = "httpproxy.server";
const std::string CoIpManagerConfig::HTTP_PROXY_SERVER_PORT_KEY = "httpproxy.port";
const std::string CoIpManagerConfig::HTTP_PROXY_LOGIN_KEY = "httpproxy.login";
const std::string CoIpManagerConfig::HTTP_PROXY_PASSWORD_KEY = "httpproxy.password";
const std::string CoIpManagerConfig::HTTP_PROXY_AUTH_TYPE_KEY = "httpproxy.authtype";

const std::string CoIpManagerConfig::AUDIO_OUTPUT_DEVICEID_KEY = "audio.output.deviceid";
const std::string CoIpManagerConfig::AUDIO_INPUT_DEVICEID_KEY = "audio.input.deviceid";
const std::string CoIpManagerConfig::AUDIO_RINGER_DEVICEID_KEY = "audio.ringer.deviceid";

const std::string CoIpManagerConfig::VIDEO_WEBCAM_DEVICE_KEY = "video.webcam.device";
const std::string CoIpManagerConfig::VIDEO_QUALITY_KEY = "video.quality";

#define stringize2(x) #x
#define stringize(x) stringize2(x)

CoIpManagerConfig::CoIpManagerConfig() {
	_keyDefaultValueMap[CONFIG_PATH_KEY] = Path::getHomeDirPath();

	StringList coipPluginsPaths;

	//Get the COIPMANAGER_PLUGIN_PATH
	char * envPath = getenv("COIPMANAGER_PLUGIN_PATH");
	if (envPath) {
		coipPluginsPaths += File::normalize(std::string(envPath), true);
	}

#if defined(OS_MACOSX)
	coipPluginsPaths += Path::getBundlePathFromId("com.openwengo.CoIpManager") + "Libraries/";
#elif defined(OS_LINUX)
	coipPluginsPaths += Path::getApplicationDirPath();
	coipPluginsPaths += "/usr/local/CoIpManager-0.1/lib/";
	coipPluginsPaths += stringize(COIPMANAGER_PLUGIN_PATH); //Defined in CMakeLists.txt
#else
	coipPluginsPaths += Path::getApplicationDirPath();
	coipPluginsPaths += "C:/CoIpManager-0.1/lib/";
#endif

	_keyDefaultValueMap[COIP_PLUGINS_PATH_KEY] = coipPluginsPaths;

	_keyDefaultValueMap[AUTO_SAVE_KEY] = true;

	_keyDefaultValueMap[HTTP_PROXY_AUTO_DETECT_KEY] = true;
	_keyDefaultValueMap[HTTP_PROXY_SERVER_KEY] = std::string();
	_keyDefaultValueMap[HTTP_PROXY_SERVER_PORT_KEY] = 0;
	_keyDefaultValueMap[HTTP_PROXY_LOGIN_KEY] = std::string();
	_keyDefaultValueMap[HTTP_PROXY_PASSWORD_KEY] = std::string();
	_keyDefaultValueMap[HTTP_PROXY_AUTH_TYPE_KEY] = EnumProxyAuthType::toString(EnumProxyAuthType::ProxyAuthTypeUnknown);

	_keyDefaultValueMap[AUDIO_OUTPUT_DEVICEID_KEY] = AudioDeviceManager::getInstance().getDefaultOutputDevice().getData();
	_keyDefaultValueMap[AUDIO_INPUT_DEVICEID_KEY] = AudioDeviceManager::getInstance().getDefaultInputDevice().getData();
	_keyDefaultValueMap[AUDIO_RINGER_DEVICEID_KEY] = AudioDeviceManager::getInstance().getDefaultOutputDevice().getData();

	_keyDefaultValueMap[VIDEO_WEBCAM_DEVICE_KEY] = WebcamDriver::getInstance()->getDefaultDevice();
	_keyDefaultValueMap[VIDEO_QUALITY_KEY] = EnumVideoQuality::toString(EnumVideoQuality::VideoQualityNormal);
}

CoIpManagerConfig::~CoIpManagerConfig() {
}

std::string CoIpManagerConfig::getConfigPath() const {
	return getStringKeyValue(CONFIG_PATH_KEY);
}

void CoIpManagerConfig::setConfigPath(const std::string & path) {
	set(CONFIG_PATH_KEY, path);
}

StringList CoIpManagerConfig::getCoIpPluginsPath() const {
	return getStringListKeyValue(COIP_PLUGINS_PATH_KEY);
}

void CoIpManagerConfig::addCoIpPluginsPath(const std::string & path) {
	StringList list = getCoIpPluginsPath();
	list.push_back(File::normalize(path));
	set(COIP_PLUGINS_PATH_KEY, list);
}

bool CoIpManagerConfig::autoSave() const {
	return getBooleanKeyValue(AUTO_SAVE_KEY);
}

bool CoIpManagerConfig::autoDetectHttpProxy() const {
	return getBooleanKeyValue(HTTP_PROXY_AUTO_DETECT_KEY);
}

std::string CoIpManagerConfig::getHttpProxyServer() const {
	return getStringKeyValue(HTTP_PROXY_SERVER_KEY);
}

int CoIpManagerConfig::getHttpProxyServerPort() const {
	return getIntegerKeyValue(HTTP_PROXY_SERVER_PORT_KEY);
}

std::string CoIpManagerConfig::getHttpProxyLogin() const {
	return getStringKeyValue(HTTP_PROXY_LOGIN_KEY);
}

std::string CoIpManagerConfig::getHttpProxyPassword() const {
	return getStringKeyValue(HTTP_PROXY_PASSWORD_KEY);
}

EnumProxyAuthType::ProxyAuthType CoIpManagerConfig::getHttpProxyAuthType() const {
	return EnumProxyAuthType::toProxyAuthType(getStringKeyValue(HTTP_PROXY_AUTH_TYPE_KEY));
}

/**
 * Code factorization.
 *
 * @see Config::getAudioRingerDeviceId()
 * @see Config::getAudioInputDeviceId()
 * @see Config::getAudioOutputDeviceId()
 * @param storedDeviceData the stored audio device
 * @param currentdeviceList the actual audio devices list
 * @param currentDefaultDevice the actual default device
 */
static StringList getProperAudioDeviceId(
	const StringList &storedDeviceData,
	const std::list<AudioDevice> &currentdeviceList,
	const StringList &currentDefaultDevice) {

	StringList result;

	// if no device stored use the default one
	if (storedDeviceData.empty()) {
		result = currentDefaultDevice;

	// else check if the stored device is currently available
	// do not exec this check under Linux because when devices
	// are busy they are not in currentdeviceList
	} else {
#ifndef OS_LINUX
		bool found = false;

		std::list<AudioDevice>::const_iterator it;
		for (it = currentdeviceList.begin(); it != currentdeviceList.end(); ++it) {

			if ((*it).getData() == storedDeviceData) {
				found = true;
			}
		}

		// if the stored device has been found we use it
		if (found) {
			result = storedDeviceData;
		} else {
			result = currentDefaultDevice;
		}
#else
		result = storedDeviceData;
#endif
	}
	return result;
}

StringList CoIpManagerConfig::getAudioOutputDeviceId() const {
	return getProperAudioDeviceId(
		getStringListKeyValue(AUDIO_OUTPUT_DEVICEID_KEY),
		AudioDeviceManager::getInstance().getOutputDeviceList(),
		AudioDeviceManager::getInstance().getDefaultOutputDevice().getData()
		);
}

StringList CoIpManagerConfig::getAudioInputDeviceId() const {
	return getProperAudioDeviceId(
		getStringListKeyValue(AUDIO_INPUT_DEVICEID_KEY),
		AudioDeviceManager::getInstance().getInputDeviceList(),
		AudioDeviceManager::getInstance().getDefaultInputDevice().getData()
		);
}

StringList CoIpManagerConfig::getAudioRingerDeviceId() const {
	return getProperAudioDeviceId(
		getStringListKeyValue(AUDIO_RINGER_DEVICEID_KEY),
		AudioDeviceManager::getInstance().getOutputDeviceList(),
		AudioDeviceManager::getInstance().getDefaultOutputDevice().getData()
	);
}

std::string CoIpManagerConfig::getVideoWebcamDevice() const {
	WebcamDriver *webcam = WebcamDriver::getInstance();

	std::string deviceName = getStringKeyValue(VIDEO_WEBCAM_DEVICE_KEY);

	//FIXME see fixme DirectXWebcamDriver.h
	//string defaultDevice = webcam->getDefaultDevice();
	std::string defaultDevice = deviceName;

	if (defaultDevice == WEBCAM_NULL) {
		defaultDevice = "";
	}
	StringList deviceList = webcam->getDeviceList();

	if (deviceName.empty()) {
		return defaultDevice;
	} else if (!deviceList.contains(deviceName)) {
		return defaultDevice;
	} else {
		return deviceName;
	}
}

std::string CoIpManagerConfig::getVideoQuality() const {
	return getStringKeyValue(VIDEO_QUALITY_KEY);
}
