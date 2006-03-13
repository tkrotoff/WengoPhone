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

#include "Config.h"

#include <AudioDevice.h>
#include <File.h>
#include <Logger.h>

#include <global.h>

#ifdef OS_MACOSX
#include <CoreFoundation/CoreFoundation.h>
#endif

const std::string Config::NETWORK_SSO_SSL_KEY = "network.sso.ssl";
const std::string Config::NETWORK_NAT_TYPE_KEY = "network.nat.type";
const std::string Config::NETWORK_SIP_SERVER_KEY = "network.sip.server";
const std::string Config::NETWORK_SIP_LOCAL_PORT_KEY = "network.sip.localport";
const std::string Config::NETWORK_TUNNEL_NEEDED_KEY = "network.tunnel.needed";
const std::string Config::NETWORK_TUNNEL_SSL_KEY = "network.tunnel.ssl";
const std::string Config::NETWORK_TUNNEL_SERVER_KEY = "network.tunnel.server";
const std::string Config::NETWORK_TUNNEL_PORT_KEY = "network.tunnel.port";
const std::string Config::NETWORK_PROXY_DETECTED_KEY = "network.proxy.detected";
const std::string Config::NETWORK_PROXY_SERVER_KEY = "network.proxy.server";
const std::string Config::NETWORK_PROXY_PORT_KEY = "network.proxy.port";
const std::string Config::NETWORK_PROXY_LOGIN_KEY = "network.proxy.login";
const std::string Config::NETWORK_PROXY_PASSWORD_KEY = "network.proxy.password";

const std::string Config::CODEC_PLUGIN_PATH_KEY = "codec.plugin.path";
const std::string Config::AUDIO_OUTPUT_DEVICENAME_KEY = "audio.output.devicename";
const std::string Config::AUDIO_INPUT_DEVICENAME_KEY = "audio.input.devicename";
const std::string Config::AUDIO_RINGER_DEVICENAME_KEY = "audio.ringer.devicename";
const std::string Config::AUDIO_RINGING_FILE_KEY = "audio.ringing.file";
const std::string Config::AUDIO_CALLCLOSED_FILE_KEY = "audio.callclosed.file";
const std::string Config::AUDIO_SMILEYS_DIR_KEY = "audio.smileys.dir";
const std::string Config::AUDIO_AEC_KEY = "audio.aec";
const std::string Config::AUDIO_HALFDUPLEX_KEY = "audio.halfduplex";

const std::string Config::PROFILE_NICKNAME = "profile.nickname";
const std::string Config::PROFILE_FIRSTNAME = "profile.firstname";
const std::string Config::PROFILE_LASTNAME = "profile.lastname";
const std::string Config::PROFILE_BIRTHDATE = "profile.birthdate";
const std::string Config::PROFILE_CITY = "profile.city";
const std::string Config::PROFILE_ALTSIP = "profile.altsip";
const std::string Config::PROFILE_GENDER = "profile.gender";
const std::string Config::PROFILE_COUNTRY = "profile.country";
const std::string Config::PROFILE_STATE = "profile.state";
const std::string Config::PROFILE_IMPIC1 = "profile.impic1";
const std::string Config::PROFILE_IMPIC2 = "profile.impic2";
const std::string Config::PROFILE_IMPIC3 = "profile.impic3";
const std::string Config::PROFILE_IMPIC4 = "profile.impic4";
const std::string Config::PROFILE_IMPIC5 = "profile.impic5";
const std::string Config::PROFILE_IMPIC6 = "profile.impic6";
const std::string Config::PROFILE_IMPIC7 = "profile.impic7";
const std::string Config::PROFILE_IMPIC8 = "profile.impic8";
const std::string Config::PROFILE_IMPIC9 = "profile.impic9";
const std::string Config::PROFILE_IMEMAIL1 = "profile.impemail1";
const std::string Config::PROFILE_IMEMAIL2 = "profile.impemail2";
const std::string Config::PROFILE_IMEMAIL3 = "profile.impemail3";
const std::string Config::PROFILE_IMEMAIL4 = "profile.impemail4";
const std::string Config::PROFILE_IMEMAIL5 = "profile.impemail5";
const std::string Config::PROFILE_IMEMAIL6 = "profile.impemail6";
const std::string Config::PROFILE_IMEMAIL7 = "profile.impemail7";
const std::string Config::PROFILE_IMEMAIL8 = "profile.impemail8";
const std::string Config::PROFILE_IMEMAIL9 = "profile.impemail9";
const std::string Config::PROFILE_CELLPHONE = "profile.cellphone";
const std::string Config::PROFILE_WENGOPHONE = "profile.wengophone";
const std::string Config::PROFILE_HOMEPHONE = "profile.homephone";
const std::string Config::PROFILE_WORKPHONE = "profile.workphone";
const std::string Config::PROFILE_EMAIL = "profile.email";
const std::string Config::PROFILE_BLOG = "profile.blog";
const std::string Config::PROFILE_WEB = "profile.web";
const std::string Config::PROFILE_ABOUT = "profile.about";
const std::string Config::PROFILE_AVATAR = "profile.avatar"; 
const std::string Config::PROFILE_WIDTH = "profile.width";
const std::string Config::PROFILE_HEIGHT = "profile.height";
const std::string Config::PROFILE_POSX = "profile.posx";
const std::string Config::PROFILE_POSY = "profile.posy";

const std::string Config::WENGO_SERVER_HOSTNAME_KEY = "wengo.server.hostname";
const std::string Config::WENGO_SMS_PATH_KEY = "wengo.sms.path";
const std::string Config::WENGO_SSO_PATH_KEY = "wengo.sso.path";

const std::string Config::CONFIG_DIR_KEY = "config.dir";
const std::string Config::RESOURCES_DIR_KEY = "resources.dir";

Config::Config(const std::string & name) {
	static const std::string empty("");
	std::string resourcesPath;
	_name = name;

	_keyDefaultValueMap[CONFIG_DIR_KEY] = File::getApplicationDirPath();

#if defined(OS_WINDOWS)
	resourcesPath = File::getApplicationDirPath();
#elif defined(OS_MACOSX)
	CFBundleRef mainBundle = CFBundleGetMainBundle();
	if (mainBundle) {
		CFURLRef url = CFBundleCopyResourcesDirectoryURL(mainBundle);
		char applicationPath[1024];
	
		if (CFURLGetFileSystemRepresentation(url, true, (UInt8 *)applicationPath, sizeof(applicationPath))) {
			resourcesPath = (std::string(applicationPath) + File::getPathSeparator());
		}
	
		CFRelease(url);
	}
#endif
	_keyDefaultValueMap[RESOURCES_DIR_KEY] = resourcesPath;

	_keyDefaultValueMap[NETWORK_SSO_SSL_KEY] = true;
	_keyDefaultValueMap[NETWORK_NAT_TYPE_KEY] = std::string("NatTypeSymmetric");
	_keyDefaultValueMap[NETWORK_SIP_SERVER_KEY] = 5060;
	_keyDefaultValueMap[NETWORK_SIP_LOCAL_PORT_KEY] = empty;
	_keyDefaultValueMap[NETWORK_TUNNEL_NEEDED_KEY] = false;
	_keyDefaultValueMap[NETWORK_TUNNEL_SSL_KEY] = false;
	_keyDefaultValueMap[NETWORK_TUNNEL_SERVER_KEY] = empty;
	_keyDefaultValueMap[NETWORK_TUNNEL_PORT_KEY] = 80;
	_keyDefaultValueMap[NETWORK_PROXY_DETECTED_KEY] = false;
	_keyDefaultValueMap[NETWORK_PROXY_SERVER_KEY] = empty;
	_keyDefaultValueMap[NETWORK_PROXY_PORT_KEY] = 0;
	_keyDefaultValueMap[NETWORK_PROXY_LOGIN_KEY] = empty;
	_keyDefaultValueMap[NETWORK_PROXY_PASSWORD_KEY] = empty;

	_keyDefaultValueMap[CODEC_PLUGIN_PATH_KEY] = empty;
	_keyDefaultValueMap[AUDIO_OUTPUT_DEVICENAME_KEY] = AudioDevice::getDefaultPlaybackDevice();
	_keyDefaultValueMap[AUDIO_INPUT_DEVICENAME_KEY] = AudioDevice::getDefaultRecordDevice();
	_keyDefaultValueMap[AUDIO_RINGER_DEVICENAME_KEY] = AudioDevice::getDefaultPlaybackDevice();
	_keyDefaultValueMap[AUDIO_RINGING_FILE_KEY] = File::convertPathSeparators(resourcesPath + "sounds/ringin.wav");
	_keyDefaultValueMap[AUDIO_CALLCLOSED_FILE_KEY] = File::convertPathSeparators(resourcesPath + "sounds/callclosed.wav");
	_keyDefaultValueMap[AUDIO_SMILEYS_DIR_KEY] = File::convertPathSeparators(resourcesPath + "sounds/tones/");
	_keyDefaultValueMap[AUDIO_AEC_KEY] = false;
	_keyDefaultValueMap[AUDIO_HALFDUPLEX_KEY] = true;

	_keyDefaultValueMap[PROFILE_NICKNAME] = empty;
	_keyDefaultValueMap[PROFILE_FIRSTNAME] = empty;
	_keyDefaultValueMap[PROFILE_LASTNAME] = empty;
	_keyDefaultValueMap[PROFILE_BIRTHDATE] = empty;
	_keyDefaultValueMap[PROFILE_CITY] = empty;
	_keyDefaultValueMap[PROFILE_ALTSIP] = empty;
	_keyDefaultValueMap[PROFILE_GENDER] = empty;
	_keyDefaultValueMap[PROFILE_COUNTRY] = empty;
	_keyDefaultValueMap[PROFILE_STATE] = empty;
	_keyDefaultValueMap[PROFILE_IMPIC1] = empty;
	_keyDefaultValueMap[PROFILE_IMPIC2] = empty;
	_keyDefaultValueMap[PROFILE_IMPIC3] = empty;
	_keyDefaultValueMap[PROFILE_IMPIC4] = empty;
	_keyDefaultValueMap[PROFILE_IMPIC5] = empty;
	_keyDefaultValueMap[PROFILE_IMPIC6] = empty;
	_keyDefaultValueMap[PROFILE_IMPIC7] = empty;
	_keyDefaultValueMap[PROFILE_IMPIC8] = empty;
	_keyDefaultValueMap[PROFILE_IMPIC9] = empty;
	_keyDefaultValueMap[PROFILE_IMEMAIL1] = empty;
	_keyDefaultValueMap[PROFILE_IMEMAIL2] = empty;
	_keyDefaultValueMap[PROFILE_IMEMAIL3] = empty;
	_keyDefaultValueMap[PROFILE_IMEMAIL4] = empty;
	_keyDefaultValueMap[PROFILE_IMEMAIL5] = empty;
	_keyDefaultValueMap[PROFILE_IMEMAIL6] = empty;
	_keyDefaultValueMap[PROFILE_IMEMAIL7] = empty;
	_keyDefaultValueMap[PROFILE_IMEMAIL8] = empty;
	_keyDefaultValueMap[PROFILE_IMEMAIL9] = empty;
	_keyDefaultValueMap[PROFILE_CELLPHONE] = empty;
	_keyDefaultValueMap[PROFILE_WENGOPHONE] = empty;
	_keyDefaultValueMap[PROFILE_HOMEPHONE] = empty;
	_keyDefaultValueMap[PROFILE_WORKPHONE] = empty;
	_keyDefaultValueMap[PROFILE_EMAIL] = empty;
	_keyDefaultValueMap[PROFILE_BLOG] = empty;
	_keyDefaultValueMap[PROFILE_WEB] = empty;
	_keyDefaultValueMap[PROFILE_ABOUT] = empty;
	_keyDefaultValueMap[PROFILE_AVATAR] = empty;
	_keyDefaultValueMap[PROFILE_WIDTH] = 786;
	_keyDefaultValueMap[PROFILE_HEIGHT] = 758;
	_keyDefaultValueMap[PROFILE_POSX] = 100;
	_keyDefaultValueMap[PROFILE_POSY] = 100;
	
	_keyDefaultValueMap[WENGO_SERVER_HOSTNAME_KEY] = std::string("ws.wengo.fr");
	_keyDefaultValueMap[WENGO_SMS_PATH_KEY] = std::string("/sms/sendsms.php");
	_keyDefaultValueMap[WENGO_SSO_PATH_KEY] = std::string("/softphone-sso/sso.php");

}

Config::~Config() {
}

std::string Config::getName() const {
	return _name;
}

StringList Config::getAllKeys() const {
	StringList tmp;
	for (Keys::const_iterator it = _keyDefaultValueMap.begin(); it != _keyDefaultValueMap.end(); ++it) {
		tmp += it->first;
	}
	return tmp;
}

bool Config::getNetworkSSOSSL() const {
	return getBooleanKeyValue(NETWORK_SSO_SSL_KEY);
}

std::string Config::getNetworkNatType() const {
	return getStringKeyValue(NETWORK_NAT_TYPE_KEY);
}

int Config::getNetworkSipLocalPort() const {
	return getIntegerKeyValue(NETWORK_SIP_LOCAL_PORT_KEY);
}

std::string Config::getNetWorkSipServer() const {
	return getStringKeyValue(NETWORK_SIP_SERVER_KEY);
}

bool Config::getNetworkTunnelSSL() const {
	return getBooleanKeyValue(NETWORK_TUNNEL_SSL_KEY);
}

std::string Config::getNetworkTunnelServer() const {
	return getStringKeyValue(NETWORK_TUNNEL_SERVER_KEY);
}

bool Config::getNetWorkTunnelNeeded() const {
	return getBooleanKeyValue(NETWORK_TUNNEL_NEEDED_KEY);
}

int Config::getNetworkTunnelPort() const {
	return getIntegerKeyValue(NETWORK_TUNNEL_PORT_KEY);
}

bool Config::getNetworkProxyDetected() const {
	return getBooleanKeyValue(NETWORK_PROXY_DETECTED_KEY);
}

std::string Config::getNetworkProxyServer() const {
	return getStringKeyValue(NETWORK_PROXY_SERVER_KEY);
}

int Config::getNetworkProxyPort() const {
	return getIntegerKeyValue(NETWORK_PROXY_PORT_KEY);
}

std::string Config::getNetworkProxyLogin() const {
	return getStringKeyValue(NETWORK_PROXY_LOGIN_KEY);
}

std::string Config::getNetworkProxyPassword() const {
	return getStringKeyValue(NETWORK_PROXY_PASSWORD_KEY);
}

std::string Config::getCodecPluginPath() const {
	return getStringKeyValue(CODEC_PLUGIN_PATH_KEY);
}

bool Config::getAudioAEC() const {
	return getBooleanKeyValue(AUDIO_AEC_KEY);
}

bool Config::getAudioHalfDuplex() const {
	return getBooleanKeyValue(AUDIO_HALFDUPLEX_KEY);
}

std::string Config::getConfigDir() const {
	return getStringKeyValue(CONFIG_DIR_KEY);
}

std::string Config::getResourcesDir() const {
	return getStringKeyValue(RESOURCES_DIR_KEY);
}

std::string Config::getAudioSmileysDir() const {
	return getStringKeyValue(AUDIO_SMILEYS_DIR_KEY);
}

std::string Config::getAudioCallClosedFile() const {
	return getStringKeyValue(AUDIO_CALLCLOSED_FILE_KEY);
}

std::string Config::getAudioRingingFile() const {
	return getStringKeyValue(AUDIO_RINGING_FILE_KEY);
}

std::string Config::getAudioRingerDeviceName() const {
	return getStringKeyValue(AUDIO_RINGER_DEVICENAME_KEY);
}

std::string Config::getAudioInputDeviceName() const {
	return getStringKeyValue(AUDIO_INPUT_DEVICENAME_KEY);
}

std::string Config::getAudioOutputDeviceName() const {
	return getStringKeyValue(AUDIO_OUTPUT_DEVICENAME_KEY);
}

std::string Config::getWengoServerHostname() const {
	return getStringKeyValue(WENGO_SERVER_HOSTNAME_KEY);
}

std::string Config::getWengoSSOPath() const {
	return getStringKeyValue(WENGO_SSO_PATH_KEY);
}

std::string Config::getWengoSMSPath() const {
	return getStringKeyValue(WENGO_SMS_PATH_KEY);
}

std::string Config::getProfileNickName() const {
	return getStringKeyValue(PROFILE_NICKNAME);
}
	
std::string Config::getProfileFirstName() const {
	return getStringKeyValue(PROFILE_FIRSTNAME);
}
	
std::string Config::getProfileLastName() const {
	return getStringKeyValue(PROFILE_LASTNAME);
}

std::string Config::getProfileBirthDate() const {
	return getStringKeyValue(PROFILE_BIRTHDATE);
}
	
std::string Config::getProfileCity() const {
	return getStringKeyValue(PROFILE_CITY);
}
	
std::string Config::getProfileAltSip() const {
	return getStringKeyValue(PROFILE_ALTSIP);
}

std::string Config::getProfileGender() const {
	return getStringKeyValue(PROFILE_GENDER);
}

std::string Config::getProfileCountry() const {
	return getStringKeyValue(PROFILE_COUNTRY);
}

std::string Config::getProfileState() const {
	return getStringKeyValue(PROFILE_STATE);
}

std::string Config::getProfileIMPic1() const {
	return getStringKeyValue(PROFILE_IMPIC1);
}

std::string Config::getProfileIMPic2() const {
	return getStringKeyValue(PROFILE_IMPIC2);
}

std::string Config::getProfileIMPic3() const {
	return getStringKeyValue(PROFILE_IMPIC3);
}

std::string Config::getProfileIMPic4() const {
	return getStringKeyValue(PROFILE_IMPIC4);
}

std::string Config::getProfileIMPic5() const {
	return getStringKeyValue(PROFILE_IMPIC5);
}

std::string Config::getProfileIMPic6() const {
	return getStringKeyValue(PROFILE_IMPIC6);
}

std::string Config::getProfileIMPic7() const {
	return getStringKeyValue(PROFILE_IMPIC7);
}

std::string Config::getProfileIMPic8() const {
	return getStringKeyValue(PROFILE_IMPIC8);
}

std::string Config::getProfileIMPic9() const {
	return getStringKeyValue(PROFILE_IMPIC9);
}


std::string Config::getProfileIMEmail1()  const {
	return getStringKeyValue(PROFILE_IMEMAIL1);
}

std::string Config::getProfileIMEmail2()  const {
	return getStringKeyValue(PROFILE_IMEMAIL2);
}

std::string Config::getProfileIMEmail3()  const {
	return getStringKeyValue(PROFILE_IMEMAIL3);
}

std::string Config::getProfileIMEmail4()  const {
	return getStringKeyValue(PROFILE_IMEMAIL4);
}

std::string Config::getProfileIMEmail5()  const {
	return getStringKeyValue(PROFILE_IMEMAIL5);
}

std::string Config::getProfileIMEmail6()  const {
	return getStringKeyValue(PROFILE_IMEMAIL6);
}

std::string Config::getProfileIMEmail7()  const {
	return getStringKeyValue(PROFILE_IMEMAIL7);
}

std::string Config::getProfileIMEmail8()  const {
	return getStringKeyValue(PROFILE_IMEMAIL8);
}

std::string Config::getProfileIMEmail9()  const {
	return getStringKeyValue(PROFILE_IMEMAIL9);
}

std::string Config::getProfileCellPhone() const {
	return getStringKeyValue(PROFILE_CELLPHONE);
}

std::string Config::getProfileWengoPhone() const {
	return getStringKeyValue(PROFILE_WENGOPHONE);
}

std::string Config::getProfileHomePhone() const {
	return getStringKeyValue(PROFILE_HOMEPHONE);
}

std::string Config::getProfileWorkPhone() const {
	return getStringKeyValue(PROFILE_WORKPHONE);
}

std::string Config::getProfileEmail() const {
	return getStringKeyValue(PROFILE_EMAIL);
}

std::string Config::getProfileBlog() const {
	return getStringKeyValue(PROFILE_BLOG);
}

std::string Config::getProfileWeb() const {
	return getStringKeyValue(PROFILE_WEB);
}

std::string Config::getProfileAbout() const {
	return getStringKeyValue(PROFILE_ABOUT);
}

std::string Config::getProfileAvatar() const {
	return getStringKeyValue(PROFILE_AVATAR);
}

int Config::getProfileWidth() const {
	return getIntegerKeyValue(PROFILE_WIDTH);
}

int Config::getProfileHeight() const {
	return getIntegerKeyValue(PROFILE_HEIGHT);
}

int Config::getProfilePosX() const {
	return getIntegerKeyValue(PROFILE_POSX);
}

int Config::getProfilePoxY() const {
	return getIntegerKeyValue(PROFILE_POSY);
}

boost::any Config::getAny(const std::string & key) const {
	Keys::const_iterator it = _keyDefaultValueMap.find(key);
	if (it == _keyDefaultValueMap.end()) {
		LOG_FATAL("key=" + key + " not found, add it inside the Config constructor");
	}

	boost::any defaultValue = it->second;
	if (defaultValue.empty()) {
		LOG_FATAL("default value for key=" + key + " not defined, add it inside the Config constructor");
	}

	return Settings::getAny(key, defaultValue);
}

bool Config::getBooleanKeyValue(const std::string & key) const {
	boost::any value = getAny(key);
	if (!Settings::isBoolean(value)) {
		LOG_FATAL("value for key=" + key + " is not a boolean");
	}

	return boost::any_cast<bool>(value);
}

int Config::getIntegerKeyValue(const std::string & key) const {
	boost::any value = getAny(key);
	if (!Settings::isInteger(value)) {
		LOG_FATAL("value for key=" + key + " is not an integer");
	}

	return boost::any_cast<int>(value);
}

std::string Config::getStringKeyValue(const std::string & key) const {
	boost::any value = getAny(key);
	if (!Settings::isString(value)) {
		LOG_FATAL("value for key=" + key + " is not a string");
	}

	return boost::any_cast<std::string>(value);
}
