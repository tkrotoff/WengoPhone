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

#ifndef CONFIG_H
#define CONFIG_H

#include <Settings.h>

#include <string>

/**
 * Stores the configuration options of WengoPhone.
 *
 * @see Settings
 * @ingroup model
 * @author Tanguy Krotoff
 * @author Philippe Bernery
 */
class Config : public Settings {
public:

	Config(const std::string & name);

	~Config();

	std::string getName() const {
		return _name;
	}

	/** String key. Network branch */
	static const std::string NETWORK_KEY;

	/** True if SSL connection to SSO is available. */
	static const std::string NETWORK_SSO_SSL_KEY;
	bool getNetworkSSOSSL() const {
		return get(NETWORK_SSO_SSL_KEY, true);
	}

	/**
	 * Type of NAT.
	 * Can be:
	 * - cone
	 * - restricted
	 * - portRestricted
	 * - sym
	 * - symfirewall
	 * - blocked
	 */
	static const std::string NETWORK_NAT_TYPE_KEY;
	std::string getNetworkNatType() const {
		return get(NETWORK_NAT_TYPE_KEY, std::string("auto"));
	}

	/** Client SIP Port to use. */
	static const std::string NETWORK_SIP_LOCAL_PORT_KEY;
	int getNetworkSipLocalPort() const {
		return get(NETWORK_SIP_LOCAL_PORT_KEY, 5060);
	}

	/** SIP server. */
	static const std::string NETWORK_SIP_SERVER_KEY;
	std::string getNetWorkSipServer() const {
		return get(NETWORK_SIP_SERVER_KEY, std::string(""));
	}

	/** True if tunnel is SSL. */
	static const std::string NETWORK_TUNNEL_SSL_KEY;
	bool getNetworkTunnelSSL() const {
		return get(NETWORK_TUNNEL_SSL_KEY, false);
	}

	/** Tunnel server. Empty if no tunnel is needed */
	static const std::string NETWORK_TUNNEL_SERVER_KEY;
	std::string getNetworkTunnelServer() const {
		return get(NETWORK_TUNNEL_SERVER_KEY, std::string(""));
	}

	/** True if HTTP tunnel needed. */
	static const std::string NETWORK_TUNNEL_NEEDED_KEY;
	bool getNetWorkTunnelNeeded() const {
		return get(NETWORK_TUNNEL_NEEDED_KEY, false);
	}

	/** Tunnel server port. */
	static const std::string NETWORK_TUNNEL_PORT_KEY;
	int getNetworkTunnelPort() const {
		return get(NETWORK_TUNNEL_PORT_KEY, 80);
	}

	/** Is proxy detected ?. */
	static const std::string NETWORK_PROXY_DETECTED_KEY;
	bool getNetworkProxyDetected() const {
		return get(NETWORK_PROXY_DETECTED_KEY, false);
	}

	/** Proxy server. Empty if no proxy */
	static const std::string NETWORK_PROXY_SERVER_KEY;
	std::string getNetworkProxyServer() const {
		return get(NETWORK_PROXY_SERVER_KEY, std::string(""));
	}

	/** Proxy server port. */
	static const std::string NETWORK_PROXY_PORT_KEY;
	int getNetworkProxyPort() const {
		return get(NETWORK_PROXY_PORT_KEY, 0);
	}

	/** Proxy login. Empty if not needed */
	static const std::string NETWORK_PROXY_LOGIN_KEY;
	std::string getNetworkProxyLogin() const {
		return get(NETWORK_PROXY_LOGIN_KEY, std::string(""));
	}

	/** Proxy Password. Empty if not needed */
	static const std::string NETWORK_PROXY_PASSWORD_KEY;
	std::string getNetworkProxyPassword() const {
		return get(NETWORK_PROXY_PASSWORD_KEY, std::string(""));
	}

	/** Path to codecs. */
	static const std::string CODEC_PLUGIN_PATH_KEY;
	std::string getCodecPluginPath() const {
		return get(CODEC_PLUGIN_PATH_KEY, std::string(""));
	}

	/** Playback audio device name. */
	static const std::string AUDIO_OUTPUT_DEVICENAME_KEY;
	std::string getAudioOutputDeviceName() const {
		return get(AUDIO_OUTPUT_DEVICENAME_KEY, std::string(""));
	}

	/** Record audio device name. */
	static const std::string AUDIO_INPUT_DEVICENAME_KEY;
	std::string getAudioInputDeviceName() const {
		return get(AUDIO_INPUT_DEVICENAME_KEY, std::string(""));
	}

	/** Ringing audio device name. */
	static const std::string AUDIO_RINGER_DEVICENAME_KEY;
	std::string getAudioRingerDeviceName() const {
		return get(AUDIO_RINGER_DEVICENAME_KEY, std::string(""));
	}

	/** Wengo server hostname. */
	static const std::string WENGO_SERVER_HOSTNAME_KEY;
	std::string getWengoServerHostname() const {
		return get(WENGO_SERVER_HOSTNAME_KEY, std::string("ws.wengo.fr"));
	}

	/** Wengo SSO path on the server. */
	static const std::string WENGO_SSO_PATH_KEY;
	std::string getWengoSSOPath() const {
		return get(WENGO_SERVER_HOSTNAME_KEY, std::string("/softphone-sso/sso.php"));
	}

	/** Wengo SMS path on the server. */
	static const std::string WENGO_SMS_PATH_KEY;
	std::string getWengoSMSPath() const {
		return get(WENGO_SMS_PATH_KEY, std::string("/sms/sendsms.php"));
	}

private:

	/**
	 * @see Settings::get()
	 */
	std::string get(const std::string & key, const std::string & defaultValue) const {
		return Settings::get(key, defaultValue);
	}

	/**
	 * @see get()
	 */
	bool get(const std::string & key, bool defaultValue) const {
		return Settings::get(key, defaultValue);
	}

	/**
	 * @see get()
	 */
	int get(const std::string & key, int defaultValue) const {
		return Settings::get(key, defaultValue);
	}

	std::string _name;
};

#endif	//CONFIG_H
