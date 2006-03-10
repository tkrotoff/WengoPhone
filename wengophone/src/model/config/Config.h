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

/**
 * Stores the configuration options of WengoPhone.
 *
 * High component above Settings, tries to make it impossible to make a mistake.
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

	std::string getName() const;

	/**
	 * @see Settings::getAllKeys()
	 */
	StringList getAllKeys() const;

	/** True if SSL connection to SSO is available. */
	static const std::string NETWORK_SSO_SSL_KEY;
	bool getNetworkSSOSSL() const;

	/**
	 * Type of NAT.
	 *
	 * @see EnumNatType::NatType
	 */
	static const std::string NETWORK_NAT_TYPE_KEY;
	std::string getNetworkNatType() const;

	/** Client SIP Port to use. */
	static const std::string NETWORK_SIP_LOCAL_PORT_KEY;
	int getNetworkSipLocalPort() const;

	/** SIP server. */
	static const std::string NETWORK_SIP_SERVER_KEY;
	std::string getNetWorkSipServer() const;

	/** True if tunnel is SSL. */
	static const std::string NETWORK_TUNNEL_SSL_KEY;
	bool getNetworkTunnelSSL() const;

	/** Tunnel server. Empty if no tunnel is needed */
	static const std::string NETWORK_TUNNEL_SERVER_KEY;
	std::string getNetworkTunnelServer() const;

	/** True if HTTP tunnel needed. */
	static const std::string NETWORK_TUNNEL_NEEDED_KEY;
	bool getNetWorkTunnelNeeded() const;

	/** Tunnel server port. */
	static const std::string NETWORK_TUNNEL_PORT_KEY;
	int getNetworkTunnelPort() const;

	/** Is proxy detected ?. */
	static const std::string NETWORK_PROXY_DETECTED_KEY;
	bool getNetworkProxyDetected() const;

	/** Proxy server. Empty if no proxy */
	static const std::string NETWORK_PROXY_SERVER_KEY;
	std::string getNetworkProxyServer() const;

	/** Proxy server port. */
	static const std::string NETWORK_PROXY_PORT_KEY;
	int getNetworkProxyPort() const;

	/** Proxy login. Empty if not needed */
	static const std::string NETWORK_PROXY_LOGIN_KEY;
	std::string getNetworkProxyLogin() const;

	/** Proxy Password. Empty if not needed */
	static const std::string NETWORK_PROXY_PASSWORD_KEY;
	std::string getNetworkProxyPassword() const;

	/**
	 * @name Audio Methods
	 * @{
	 */

	/** Path to codecs. */
	static const std::string CODEC_PLUGIN_PATH_KEY;
	std::string getCodecPluginPath() const;

	/** Playback audio device name. */
	static const std::string AUDIO_OUTPUT_DEVICENAME_KEY;
	std::string getAudioOutputDeviceName() const;

	/** Record audio device name. */
	static const std::string AUDIO_INPUT_DEVICENAME_KEY;
	std::string getAudioInputDeviceName() const;

	/** Ringer audio device name. */
	static const std::string AUDIO_RINGER_DEVICENAME_KEY;
	std::string getAudioRingerDeviceName() const;

	/** Ringing audio file name. */
	static const std::string AUDIO_RINGING_FILE_KEY;
	std::string getAudioRingingFile() const;

	/** Call closed audio file name. */
	static const std::string AUDIO_CALLCLOSED_FILE_KEY;
	std::string getAudioCallClosedFile() const;

	/** Audio smileys directory. */
	static const std::string AUDIO_SMILEYS_DIR_KEY;
	std::string getAudioSmileysDir() const;

	/** AEC (echo canceller) enable or not. */
	static const std::string AUDIO_AEC_KEY;
	bool getAudioAEC() const;

	/** Half duplex mode enable or not. */
	static const std::string AUDIO_HALFDUPLEX_KEY;
	bool getAudioHalfDuplex() const;

	/** @} */


	/**
	 * @name Wengo Specific Methods
	 * @{
	 */

	/** Wengo server hostname. */
	static const std::string WENGO_SERVER_HOSTNAME_KEY;
	std::string getWengoServerHostname() const;

	/** Wengo SSO path on the server. */
	static const std::string WENGO_SSO_PATH_KEY;
	std::string getWengoSSOPath() const;

	/** Wengo SMS path on the server. */
	static const std::string WENGO_SMS_PATH_KEY;
	std::string getWengoSMSPath() const;

	/** @} */

	/** Location of configuration files on the hard disk. */
	static const std::string CONFIG_DIR_KEY;
	std::string getConfigDir() const;

	/**
	 * @see Settings::getAny()
	 */
	boost::any getAny(const std::string & key) const;

private:

	/**
	 * Makes it impossible to use get() directly.
	 *
	 * @see Settings::get()
	 */
	std::string get(const std::string &, const std::string &) const {}

	/**
	 * @see get()
	 */
	StringList get(const std::string &, const StringList &) const {}

	/**
	 * @see get()
	 */
	bool get(const std::string &, bool) const {}

	/**
	 * @see get()
	 */
	int get(const std::string &, int) const {}

	/**
	 * @see get()
	 */
	boost::any getAny(const std::string &, const boost::any &) const {}

	/**
	 * @see get()
	 */
	bool getBooleanKeyValue(const std::string & key) const;

	/**
	 * @see get()
	 */
	int getIntegerKeyValue(const std::string & key) const;

	/**
	 * @see get()
	 */
	std::string getStringKeyValue(const std::string & key) const;

	/** Name of this configuration. */
	std::string _name;

	/** Associates a key to a default value. */
	Keys _keyDefaultValueMap;
};

#endif	//CONFIG_H
