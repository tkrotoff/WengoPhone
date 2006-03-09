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

const std::string Config::WENGO_SERVER_HOSTNAME_KEY = "wengo.server.hostname";
const std::string Config::WENGO_SMS_PATH_KEY = "wengo.sms.path";
const std::string Config::WENGO_SSO_PATH_KEY = "wengo.sso.path";

const std::string Config::CONFIG_DIR_KEY = "config.dir";


Config::Config(const std::string & name) {
	_name = name;

	_keyDefaultValueMap[NETWORK_SSO_SSL_KEY] = true;
	_keyDefaultValueMap[NETWORK_NAT_TYPE_KEY] = std::string("NatTypeSymmetric");
	_keyDefaultValueMap[NETWORK_SIP_SERVER_KEY] = 5060;
	_keyDefaultValueMap[NETWORK_SIP_LOCAL_PORT_KEY] = std::string("");
	_keyDefaultValueMap[NETWORK_TUNNEL_NEEDED_KEY] = false;
	_keyDefaultValueMap[NETWORK_TUNNEL_SSL_KEY] = false;
	_keyDefaultValueMap[NETWORK_TUNNEL_SERVER_KEY] = std::string("");
	_keyDefaultValueMap[NETWORK_TUNNEL_PORT_KEY] = 80;
	_keyDefaultValueMap[NETWORK_PROXY_DETECTED_KEY] = false;
	_keyDefaultValueMap[NETWORK_PROXY_SERVER_KEY] = std::string("");
	_keyDefaultValueMap[NETWORK_PROXY_PORT_KEY] = 0;
	_keyDefaultValueMap[NETWORK_PROXY_LOGIN_KEY] = std::string("");
	_keyDefaultValueMap[NETWORK_PROXY_PASSWORD_KEY] = std::string("");

	_keyDefaultValueMap[CODEC_PLUGIN_PATH_KEY] = std::string("");
	_keyDefaultValueMap[AUDIO_OUTPUT_DEVICENAME_KEY] = AudioDevice::getDefaultPlaybackDevice();
	_keyDefaultValueMap[AUDIO_INPUT_DEVICENAME_KEY] = AudioDevice::getDefaultRecordDevice();
	_keyDefaultValueMap[AUDIO_RINGER_DEVICENAME_KEY] = AudioDevice::getDefaultPlaybackDevice();
	_keyDefaultValueMap[AUDIO_RINGING_FILE_KEY] = File::convertPathSeparators(File::getApplicationDirPath() + "sounds/ringin.wav");
	_keyDefaultValueMap[AUDIO_CALLCLOSED_FILE_KEY] = File::convertPathSeparators(File::getApplicationDirPath() + "sounds/callclosed.wav");
	_keyDefaultValueMap[AUDIO_SMILEYS_DIR_KEY] = File::convertPathSeparators(File::getApplicationDirPath() + "sounds/tones/");
	_keyDefaultValueMap[AUDIO_AEC_KEY] = false;
	_keyDefaultValueMap[AUDIO_HALFDUPLEX_KEY] = true;

	_keyDefaultValueMap[WENGO_SERVER_HOSTNAME_KEY] = std::string("ws.wengo.fr");
	_keyDefaultValueMap[WENGO_SMS_PATH_KEY] = std::string("/sms/sendsms.php");
	_keyDefaultValueMap[WENGO_SSO_PATH_KEY] = std::string("/softphone-sso/sso.php");

	_keyDefaultValueMap[CONFIG_DIR_KEY] = File::getApplicationDirPath();
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
