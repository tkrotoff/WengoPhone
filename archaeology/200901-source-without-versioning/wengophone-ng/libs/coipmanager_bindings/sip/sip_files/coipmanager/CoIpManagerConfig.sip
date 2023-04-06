class CoIpManagerConfig {
%TypeHeaderCode
#include <coipmanager/CoIpManagerConfig.h>
%End
public:
 CoIpManagerConfig();
 virtual ~CoIpManagerConfig();
 static const std::string CONFIG_PATH_KEY;
 std::string getConfigPath() const;
 void setConfigPath(const std::string & path);
 static const std::string COIP_PLUGINS_PATH_KEY;
 StringList getCoIpPluginsPath() const;
 void addCoIpPluginsPath(const std::string & path);
 static const std::string AUTO_SAVE_KEY;
 bool autoSave() const;
 static const std::string HTTP_PROXY_AUTO_DETECT_KEY;
 bool autoDetectHttpProxy() const;
 static const std::string HTTP_PROXY_SERVER_KEY;
 std::string getHttpProxyServer() const;
 static const std::string HTTP_PROXY_SERVER_PORT_KEY;
 int getHttpProxyServerPort() const;
 static const std::string HTTP_PROXY_LOGIN_KEY;
 std::string getHttpProxyLogin() const;
 static const std::string HTTP_PROXY_PASSWORD_KEY;
 std::string getHttpProxyPassword() const;
 static const std::string HTTP_PROXY_AUTH_TYPE_KEY;
 EnumProxyAuthType::ProxyAuthType getHttpProxyAuthType() const;
 static const std::string AUDIO_OUTPUT_DEVICEID_KEY;
 StringList getAudioOutputDeviceId() const;
 static const std::string AUDIO_INPUT_DEVICEID_KEY;
 StringList getAudioInputDeviceId() const;
 static const std::string AUDIO_RINGER_DEVICEID_KEY;
 StringList getAudioRingerDeviceId() const;
 static const std::string VIDEO_WEBCAM_DEVICE_KEY;
 std::string getVideoWebcamDevice() const;
 static const std::string VIDEO_QUALITY_KEY;
 std::string getVideoQuality() const;
};
