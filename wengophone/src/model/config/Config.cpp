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

#include "Config.h"

#include <AudioDevice.h>

const std::string Config::NETWORK_KEY = "network";
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
const std::string Config::WENGO_SERVER_HOSTNAME_KEY = "wengo.server.hostname";
const std::string Config::WENGO_SMS_PATH_KEY = "wengo.sms.path";
const std::string Config::WENGO_SSO_PATH_KEY = "wengo.sso.path";

Config::Config(const std::string & name) {
	_name = name;
}

Config::~Config() {
}

std::string Config::getAudioOutputDeviceName() const {
	return get(AUDIO_OUTPUT_DEVICENAME_KEY, AudioDevice::getDefaultPlaybackDevice());
}

std::string Config::getAudioInputDeviceName() const {
	return get(AUDIO_INPUT_DEVICENAME_KEY, AudioDevice::getDefaultRecordDevice());
}

std::string Config::getAudioRingerDeviceName() const {
	return get(AUDIO_RINGER_DEVICENAME_KEY, AudioDevice::getDefaultPlaybackDevice());
}
