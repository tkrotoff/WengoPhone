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

#ifndef OWCOIPMANAGERCONFIG_H
#define OWCOIPMANAGERCONFIG_H

#include <coipmanager/coipmanagerdll.h>

#include <networkdiscovery/EnumProxyAuthType.h>

#include <settings/AutomaticSettings.h>

/**
 * Configuration of CoIpManager.
 *
 * @ingroup CoIpManager
 * @author Philippe Bernery
 */
class CoIpManagerConfig : public AutomaticSettings {
public:

	COIPMANAGER_API CoIpManagerConfig();

	COIPMANAGER_API virtual ~CoIpManagerConfig();

	/** Path to profiles. */
	COIPMANAGER_API static const std::string CONFIG_PATH_KEY;
	COIPMANAGER_API std::string getConfigPath() const;
	COIPMANAGER_API void setConfigPath(const std::string & path);

	/** Paths to 'coip-plugins' directory. */
	COIPMANAGER_API static const std::string COIP_PLUGINS_PATH_KEY;
	COIPMANAGER_API StringList getCoIpPluginsPath() const;
	COIPMANAGER_API void addCoIpPluginsPath(const std::string & path);

	/** AutoSave profiles on change. */
	COIPMANAGER_API static const std::string AUTO_SAVE_KEY;
	COIPMANAGER_API bool autoSave() const;

	/**
	 * @name Http Proxy information
	 * @{
	 */

	/** Automatically detect proxy or get manual information?. */
	COIPMANAGER_API static const std::string HTTP_PROXY_AUTO_DETECT_KEY;
	COIPMANAGER_API bool autoDetectHttpProxy() const;

	/** Http proxy server. */
	COIPMANAGER_API static const std::string HTTP_PROXY_SERVER_KEY;
	COIPMANAGER_API std::string getHttpProxyServer() const; 

	/** Http proxy server port. */
	COIPMANAGER_API static const std::string HTTP_PROXY_SERVER_PORT_KEY;
	COIPMANAGER_API int getHttpProxyServerPort() const; 

	/** Http proxy login. */
	COIPMANAGER_API static const std::string HTTP_PROXY_LOGIN_KEY;
	COIPMANAGER_API std::string getHttpProxyLogin() const; 

	/** Http proxy password. */
	COIPMANAGER_API static const std::string HTTP_PROXY_PASSWORD_KEY;
	COIPMANAGER_API std::string getHttpProxyPassword() const; 

	/** Http proxy auth type. */
	COIPMANAGER_API static const std::string HTTP_PROXY_AUTH_TYPE_KEY;
	COIPMANAGER_API EnumProxyAuthType::ProxyAuthType getHttpProxyAuthType() const;

	/**
	 * @}
	 */

	/**
	 * @name Audio device information.
	 * @{
	 */

	/** Playback audio device name. */
	COIPMANAGER_API static const std::string AUDIO_OUTPUT_DEVICEID_KEY;
	COIPMANAGER_API StringList getAudioOutputDeviceId() const;

	/** Record audio device name. */
	COIPMANAGER_API static const std::string AUDIO_INPUT_DEVICEID_KEY;
	COIPMANAGER_API StringList getAudioInputDeviceId() const;

	/** Ringer audio device name. */
	COIPMANAGER_API static const std::string AUDIO_RINGER_DEVICEID_KEY;
	COIPMANAGER_API StringList getAudioRingerDeviceId() const;

	/**
	 * @}
	 */

	/**
	 * @name Video device information.
	 * @{
	 */

	COIPMANAGER_API static const std::string VIDEO_WEBCAM_DEVICE_KEY;
	COIPMANAGER_API std::string getVideoWebcamDevice() const;

	COIPMANAGER_API static const std::string VIDEO_QUALITY_KEY;
	COIPMANAGER_API std::string getVideoQuality() const;

	/**
	 * @}
	 */
};

#endif //OWCOIPMANAGERCONFIG_H
