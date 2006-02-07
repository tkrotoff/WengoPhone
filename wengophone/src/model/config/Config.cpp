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

using namespace std;

const string Config::NETWORK_KEY = "network";
const string Config::NETWORK_SSO_SSL_KEY = "network.sso.ssl";
const string Config::NETWORK_NAT_TYPE_KEY = "network.nat.type";
const string Config::NETWORK_SIP_SERVER_KEY = "network.sip.server";
const string Config::NETWORK_SIP_LOCAL_PORT_KEY = "network.sip.localport";
const string Config::NETWORK_TUNNEL_NEEDED_KEY = "network.tunnel.needed";
const string Config::NETWORK_TUNNEL_SSL_KEY = "network.tunnel.ssl";
const string Config::NETWORK_TUNNEL_SERVER_KEY = "network.tunnel.server";
const string Config::NETWORK_TUNNEL_PORT_KEY = "network.tunnel.port";
const string Config::NETWORK_PROXY_DETECTED_KEY = "network.proxy.detected";
const string Config::NETWORK_PROXY_SERVER_KEY = "network.proxy.server";
const string Config::NETWORK_PROXY_PORT_KEY = "network.proxy.port";
const string Config::NETWORK_PROXY_LOGIN_KEY = "network.proxy.login";
const string Config::NETWORK_PROXY_PASSWORD_KEY = "network.proxy.password";
const string Config::CODEC_PLUGIN_PATH = "codec.plugin.path";

Config::Config(const std::string & name) {
	_name = name;
}

Config::~Config() {
}
