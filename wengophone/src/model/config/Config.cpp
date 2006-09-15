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

#include <sipwrapper/EnumNatType.h>
#include <sipwrapper/EnumVideoQuality.h>

#include <webcam/WebcamDriver.h>

#include <sound/AudioDeviceManager.h>

#include <util/File.h>
#include <util/Path.h>
#include <util/Logger.h>
#include <cutil/global.h>

#ifdef OS_MACOSX
	#include <CoreFoundation/CoreFoundation.h>
#endif

const int Config::CONFIG_VERSION = 4;

const std::string Config::CONFIG_VERSION_KEY = "config.version";

const std::string Config::EXECUTABLE_NAME_KEY = "executable.name";
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
const std::string Config::AUDIO_OUTPUT_DEVICEID_KEY = "audio.output.deviceid";
const std::string Config::AUDIO_INPUT_DEVICEID_KEY = "audio.input.deviceid";
const std::string Config::AUDIO_RINGER_DEVICEID_KEY = "audio.ringer.deviceid";

const std::string Config::AUDIO_INCOMINGCALL_FILE_KEY = "audio.incomingcall.file";
const std::string Config::AUDIO_CALLCLOSED_FILE_KEY = "audio.callclosed.file";
const std::string Config::AUDIO_INCOMINGCHAT_FILE_KEY = "audio.incomingchat.file";
const std::string Config::AUDIO_IMACCOUNTCONNECTED_FILE_KEY = "audio.imaccountconnected.file";
const std::string Config::AUDIO_IMACCOUNTDISCONNECTED_FILE_KEY = "audio.imaccountdisconnected.file";
const std::string Config::AUDIO_CONTACTONLINE_FILE_KEY = "audio.contactonline.file";

const std::string Config::AUDIO_SMILEYS_DIR_KEY = "audio.smileys.dir";
const std::string Config::AUDIO_AEC_KEY = "audio.aec";
const std::string Config::AUDIO_HALFDUPLEX_KEY = "audio.halfduplex";

const std::string Config::PROFILE_LAST_USED_NAME_KEY = "profile.last_used_name";
const std::string Config::PROFILE_WIDTH = "profile.width";
const std::string Config::PROFILE_HEIGHT = "profile.height";
const std::string Config::PROFILE_POSX = "profile.posx";
const std::string Config::PROFILE_POSY = "profile.posy";

const std::string Config::CALL_FORWARD_MODE_KEY = "call.forward.mode";
const std::string Config::CALL_FORWARD_PHONENUMBER1_KEY = "call.forward.phonenumber1";
const std::string Config::CALL_FORWARD_PHONENUMBER2_KEY = "call.forward.phonenumber2";
const std::string Config::CALL_FORWARD_PHONENUMBER3_KEY = "call.forward.phonenumber3";
const std::string Config::CALL_FORWARD_TOMOBILE_KEY = "call.forward.tomobile";

const std::string Config::VOICE_MAIL_ACTIVE_KEY = "voicemail.active";

const std::string Config::GENERAL_AUTOSTART_KEY = "general.autostart";
const std::string Config::GENERAL_CLICK_START_FREECALL_KEY = "general.click.start.freecall";
const std::string Config::GENERAL_CLICK_START_CHAT_KEY = "general.click.start.chat";
const std::string Config::GENERAL_CLICK_CALL_CELLPHONE_KEY = "general.click.call.cellphone";
const std::string Config::GENERAL_AWAY_TIMER_KEY = "general.away.timer";
const std::string Config::GENERAL_SHOW_GROUPS_KEY = "general.show.groups";
const std::string Config::GENERAL_SHOW_OFFLINE_CONTACTS_KEY = "general.show.offline.contacts";

const std::string Config::LANGUAGE_AUTODETECT_KEYVALUE = "detect";
const std::string Config::LANGUAGE_KEY = "language";

const std::string Config::NOTIFICATION_SHOW_TOASTER_ON_INCOMING_CALL_KEY = "notification.show.toaster.on.incoming.call";
const std::string Config::NOTIFICATION_SHOW_TOASTER_ON_INCOMING_CHAT_KEY = "notification.show.toaster.on.incoming.chat";
const std::string Config::NOTIFICATION_SHOW_TOASTER_ON_CONTACT_ONLINE_KEY = "notification.show.toaster.on.contact";
const std::string Config::NOTIFICATION_DONOTDISTURB_NO_WINDOW_KEY = "notification.donotdisturb.nowindow";
const std::string Config::NOTIFICATION_DONOTDISTURB_NO_AUDIO_KEY = "notification.donotdisturb.noaudio";
const std::string Config::NOTIFICATION_AWAY_NO_WINDOW_KEY = "notification.away.nowindow";
const std::string Config::NOTIFICATION_AWAY_NO_AUDIO_KEY = "notification.away.noaudio";

const std::string Config::PRIVACY_ALLOW_CALL_FROM_ANYONE_KEY = "privacy.allow.call.from.anyone";
const std::string Config::PRIVACY_ALLOW_CALL_ONLY_FROM_CONTACT_LIST_KEY = "privacy.allow.call.only.from.contact.list";
const std::string Config::PRIVACY_ALLOW_CHAT_FROM_ANYONE_KEY = "privacy.allow.chat.from.anyone";
const std::string Config::PRIVACY_ALLOW_CHAT_ONLY_FROM_CONTACT_LIST_KEY = "privacy.allow.chat.only.from.contact.list";
const std::string Config::PRIVACY_SIGN_AS_INVISIBLE_KEY = "privacy.sign.as.invisible";

const std::string Config::VIDEO_ENABLE_KEY = "video.enable";
const std::string Config::VIDEO_WEBCAM_DEVICE_KEY = "video.webcam.device";
const std::string Config::VIDEO_QUALITY_KEY = "video.quality";
const std::string Config::VIDEO_ENABLE_XVIDEO = "video.xvideo.enable";
const std::string Config::VIDEO_ENABLE_FLIP = "video.flip.enable";

const std::string Config::WENGO_SERVER_HOSTNAME_KEY = "wengo.server.hostname";
const std::string Config::WENGO_SMS_PATH_KEY = "wengo.sms.path";
const std::string Config::WENGO_SSO_PATH_KEY = "wengo.sso.path";
const std::string Config::WENGO_INFO_PATH_KEY = "wengo.info.path";
const std::string Config::WENGO_SUBSCRIBE_PATH_KEY = "wengo.subscribe.path";
const std::string Config::WENGO_SOFTUPDATE_PATH_KEY = "wengo.softupdate.path";
const std::string Config::WENGO_CIRPACK_PATH_KEY = "wengo.cirpack.path";
const std::string Config::WENGO_DIRECTORY_PATH_KEY = "wengo.directory.path";

const std::string Config::CONFIG_DIR_KEY = "config.dir";
const std::string Config::LAST_CHAT_HISTORY_SAVE_DIR_KEY = "last.chat.history.save.dir";
const std::string Config::RESOURCES_DIR_KEY = "resources.dir";

const std::string Config::WENBOX_ENABLE_KEY = "wenbox.enable";
const std::string Config::IEACTIVEX_ENABLE_KEY = "ieactivex.enable";

const std::string Config::CMDLINE_BACKGROUND_MODE_ENABLE_KEY = "cmdline.background.mode.enable";
const std::string Config::CMDLINE_PLACECALL_KEY = "cmdline.placecall";

const std::string Config::CMDSERVER_AUTHORIZED_KEY = "cmdserver.authorized";

const std::string Config::FILETRANSFER_DOWNLOAD_FOLDER = "filetransfer.downloadfolder";

const std::string Config::LINUX_PREFERED_BROWSER = "linux.prefered.browser";

Config::Config(const std::string & name)
	: AutomaticSettings() {

	static const std::string empty(String::null);
	static const StringList emptyStrList;
	std::string resourcesPath;
	std::string configPath;
	std::string pluginsPath;

	_name = name;

	_keyDefaultValueMap[CONFIG_VERSION_KEY] = CONFIG_VERSION;
	// Forcing set of value to save it in config.xml
	set(CONFIG_VERSION_KEY, CONFIG_VERSION);

	//Default config path
#if defined(OS_MACOSX) || defined(OS_WINDOWS)
	configPath = Path::getConfigurationDirPath() + File::convertPathSeparators("WengoPhone/");
#else
	configPath = Path::getConfigurationDirPath() + File::convertPathSeparators(".wengophone/");
#endif
	File::createPath(configPath);
	_keyDefaultValueMap[CONFIG_DIR_KEY] = configPath;

	//Default resources path
#if defined(OS_WINDOWS)
	resourcesPath = Path::getApplicationDirPath();
#elif defined(OS_MACOSX)
	resourcesPath = Path::getApplicationResourcesDirPath();
#endif
	_keyDefaultValueMap[RESOURCES_DIR_KEY] = resourcesPath;
	_keyDefaultValueMap[EXECUTABLE_NAME_KEY] = empty;

	_keyDefaultValueMap[NETWORK_SSO_SSL_KEY] = true;
	_keyDefaultValueMap[NETWORK_NAT_TYPE_KEY] = EnumNatType::toString(EnumNatType::NatTypeSymmetric);
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

#if defined(OS_MACOSX)
	pluginsPath = Path::getApplicationPrivateFrameworksDirPath()
		+ "phapi-plugins" + File::getPathSeparator();
#else
	pluginsPath = empty;
#endif
	_keyDefaultValueMap[CODEC_PLUGIN_PATH_KEY] = pluginsPath;

	_keyDefaultValueMap[AUDIO_OUTPUT_DEVICEID_KEY] = AudioDeviceManager::getDefaultOutputDevice().getData();
	_keyDefaultValueMap[AUDIO_INPUT_DEVICEID_KEY] = AudioDeviceManager::getDefaultInputDevice().getData();
	_keyDefaultValueMap[AUDIO_RINGER_DEVICEID_KEY] = AudioDeviceManager::getDefaultOutputDevice().getData();

	_keyDefaultValueMap[AUDIO_INCOMINGCALL_FILE_KEY] = File::convertPathSeparators(resourcesPath + "sounds/ringin.wav");
	_keyDefaultValueMap[AUDIO_CALLCLOSED_FILE_KEY] = File::convertPathSeparators(resourcesPath + "sounds/callclosed.wav");
	_keyDefaultValueMap[AUDIO_INCOMINGCHAT_FILE_KEY] = File::convertPathSeparators(resourcesPath + "sounds/incomingchat.wav");
	_keyDefaultValueMap[AUDIO_IMACCOUNTCONNECTED_FILE_KEY] = File::convertPathSeparators(resourcesPath + "sounds/imaccountconnected.wav");
	_keyDefaultValueMap[AUDIO_IMACCOUNTDISCONNECTED_FILE_KEY] = File::convertPathSeparators(resourcesPath + "sounds/imaccountdisconnected.wav");
	_keyDefaultValueMap[AUDIO_CONTACTONLINE_FILE_KEY] = File::convertPathSeparators(resourcesPath + "sounds/contactonline.wav");

	_keyDefaultValueMap[AUDIO_SMILEYS_DIR_KEY] = File::convertPathSeparators(resourcesPath + "sounds/tones/");
	_keyDefaultValueMap[AUDIO_AEC_KEY] = true;
	_keyDefaultValueMap[AUDIO_HALFDUPLEX_KEY] = false;

	_keyDefaultValueMap[PROFILE_LAST_USED_NAME_KEY] = empty;
	_keyDefaultValueMap[PROFILE_WIDTH] = 340;
	_keyDefaultValueMap[PROFILE_HEIGHT] = 600;
	_keyDefaultValueMap[PROFILE_POSX] = 100;
	_keyDefaultValueMap[PROFILE_POSY] = 100;

	_keyDefaultValueMap[CALL_FORWARD_MODE_KEY] = std::string("unauthorized");
	_keyDefaultValueMap[CALL_FORWARD_PHONENUMBER1_KEY] = empty;
	_keyDefaultValueMap[CALL_FORWARD_PHONENUMBER2_KEY] = empty;
	_keyDefaultValueMap[CALL_FORWARD_PHONENUMBER3_KEY] = empty;
	_keyDefaultValueMap[CALL_FORWARD_TOMOBILE_KEY] = false;
	_keyDefaultValueMap[VOICE_MAIL_ACTIVE_KEY] = false;

	_keyDefaultValueMap[GENERAL_AUTOSTART_KEY] = true;
	_keyDefaultValueMap[GENERAL_CLICK_START_FREECALL_KEY] = true;
	_keyDefaultValueMap[GENERAL_CLICK_START_CHAT_KEY] = false;
	_keyDefaultValueMap[GENERAL_CLICK_CALL_CELLPHONE_KEY] = true;
	_keyDefaultValueMap[GENERAL_AWAY_TIMER_KEY] = 2;
	_keyDefaultValueMap[GENERAL_SHOW_GROUPS_KEY] = false;
	_keyDefaultValueMap[GENERAL_SHOW_OFFLINE_CONTACTS_KEY] = false;

	_keyDefaultValueMap[LANGUAGE_KEY] = LANGUAGE_AUTODETECT_KEYVALUE;

	_keyDefaultValueMap[NOTIFICATION_SHOW_TOASTER_ON_INCOMING_CALL_KEY] = true;
	_keyDefaultValueMap[NOTIFICATION_SHOW_TOASTER_ON_INCOMING_CHAT_KEY] = true;
	_keyDefaultValueMap[NOTIFICATION_SHOW_TOASTER_ON_CONTACT_ONLINE_KEY] = true;
	_keyDefaultValueMap[NOTIFICATION_DONOTDISTURB_NO_WINDOW_KEY] = false;
	_keyDefaultValueMap[NOTIFICATION_DONOTDISTURB_NO_AUDIO_KEY] = false;
	_keyDefaultValueMap[NOTIFICATION_AWAY_NO_WINDOW_KEY] = false;
	_keyDefaultValueMap[NOTIFICATION_AWAY_NO_AUDIO_KEY] = false;

	_keyDefaultValueMap[PRIVACY_ALLOW_CALL_FROM_ANYONE_KEY] = false;
	_keyDefaultValueMap[PRIVACY_ALLOW_CALL_ONLY_FROM_CONTACT_LIST_KEY] = false;
	_keyDefaultValueMap[PRIVACY_ALLOW_CHAT_FROM_ANYONE_KEY] = false;
	_keyDefaultValueMap[PRIVACY_ALLOW_CHAT_ONLY_FROM_CONTACT_LIST_KEY] = false;
	_keyDefaultValueMap[PRIVACY_SIGN_AS_INVISIBLE_KEY] = false;

	_keyDefaultValueMap[VIDEO_ENABLE_KEY] = true;
	_keyDefaultValueMap[VIDEO_WEBCAM_DEVICE_KEY] = WebcamDriver::getInstance()->getDefaultDevice();
	_keyDefaultValueMap[VIDEO_QUALITY_KEY] = (int) EnumVideoQuality::VideoQualityNormal;
	_keyDefaultValueMap[VIDEO_ENABLE_XVIDEO] = true;
	_keyDefaultValueMap[VIDEO_ENABLE_FLIP] = false;

	_keyDefaultValueMap[WENGO_SERVER_HOSTNAME_KEY] = std::string("ws.wengo.fr");
	_keyDefaultValueMap[WENGO_SMS_PATH_KEY] = std::string("/sms/sendsms.php");
	_keyDefaultValueMap[WENGO_SSO_PATH_KEY] = std::string("/softphone-sso/sso2.php");
	_keyDefaultValueMap[WENGO_INFO_PATH_KEY] = std::string("/info/info.php");
	_keyDefaultValueMap[WENGO_SUBSCRIBE_PATH_KEY] = std::string("/softphone-subscription/index.php");
	_keyDefaultValueMap[WENGO_SOFTUPDATE_PATH_KEY] = std::string("/softphone-version/version.php");
	_keyDefaultValueMap[WENGO_CIRPACK_PATH_KEY] = std::string("/cirpack/index.php");
	_keyDefaultValueMap[WENGO_DIRECTORY_PATH_KEY] = std::string("/directory/index.php");

	_keyDefaultValueMap[WENBOX_ENABLE_KEY] = true;
	_keyDefaultValueMap[IEACTIVEX_ENABLE_KEY] = true;

	_keyDefaultValueMap[CMDLINE_BACKGROUND_MODE_ENABLE_KEY] = false;
	_keyDefaultValueMap[CMDLINE_PLACECALL_KEY] = empty;

	_keyDefaultValueMap[CMDSERVER_AUTHORIZED_KEY] = empty;
	
	_keyDefaultValueMap[LAST_CHAT_HISTORY_SAVE_DIR_KEY] = Path::getHomeDirPath();

	_keyDefaultValueMap[FILETRANSFER_DOWNLOAD_FOLDER] = empty;

	_keyDefaultValueMap[LINUX_PREFERED_BROWSER] = empty;
}

Config::~Config() {
}

std::string Config::getName() const {
	return _name;
}

int Config::getConfigVersion() const {
	return getIntegerKeyValue(CONFIG_VERSION_KEY);
}

std::string Config::getExecutableName() const {
	return getStringKeyValue(EXECUTABLE_NAME_KEY);
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

std::string Config::getLastChatHistorySaveDir() const {
	return getStringKeyValue(LAST_CHAT_HISTORY_SAVE_DIR_KEY);
}

std::string Config::getResourcesDir() const {
	return getStringKeyValue(RESOURCES_DIR_KEY);
}

std::string Config::getAudioSmileysDir() const {
	return getStringKeyValue(AUDIO_SMILEYS_DIR_KEY);
}

std::string Config::getAudioIncomingCallFile() const {
	return getStringKeyValue(AUDIO_INCOMINGCALL_FILE_KEY);
}

std::string Config::getAudioCallClosedFile() const {
	return getStringKeyValue(AUDIO_CALLCLOSED_FILE_KEY);
}

std::string Config::getAudioIncomingChatFile() const {
	return getStringKeyValue(AUDIO_INCOMINGCHAT_FILE_KEY);
}

std::string Config::getAudioIMAccountConnectedFile() const {
	return getStringKeyValue(AUDIO_IMACCOUNTCONNECTED_FILE_KEY);
}

std::string Config::getAudioIMAccountDisconnectedFile() const {
	return getStringKeyValue(AUDIO_IMACCOUNTDISCONNECTED_FILE_KEY);
}

std::string Config::getAudioContactOnlineFile() const {
	return getStringKeyValue(AUDIO_CONTACTONLINE_FILE_KEY);
}

/**
 * Code factorization.
 *
 * @see Config::getAudioRingerDeviceName()
 * @see Config::getAudioInputDeviceName()
 * @see Config::getAudioOutputDeviceName()
 */
static StringList getProperAudioDeviceId(const StringList & deviceData,
	const std::list<AudioDevice> & deviceList, const StringList & defaultDevice) {

	StringList result;

	if (deviceData.empty()) {
		result = defaultDevice;
	} else {
		bool found = false;

		for (std::list<AudioDevice>::const_iterator it = deviceList.begin();
			it != deviceList.end();
			++it) {

			std::string tmp = (*it).getData().join("/");
			std::string tmp2 = deviceData.join("/");
			if ((*it).getData() == deviceData) {
				found = true;
			}
		}

		if (found) {
			result = deviceData;
		} else {
			result = defaultDevice;
		}
	}

	return result;
}

StringList Config::getAudioRingerDeviceId() const {
	return getProperAudioDeviceId(getStringListKeyValue(AUDIO_RINGER_DEVICEID_KEY),
		AudioDeviceManager::getOutputDeviceList(),
		AudioDeviceManager::getDefaultOutputDevice().getData());
}

StringList Config::getAudioInputDeviceId() const {
	return getProperAudioDeviceId(getStringListKeyValue(AUDIO_INPUT_DEVICEID_KEY),
		AudioDeviceManager::getInputDeviceList(),
		AudioDeviceManager::getDefaultInputDevice().getData());
}

StringList Config::getAudioOutputDeviceId() const {
	return getProperAudioDeviceId(getStringListKeyValue(AUDIO_OUTPUT_DEVICEID_KEY),
		AudioDeviceManager::getOutputDeviceList(),
		AudioDeviceManager::getDefaultOutputDevice().getData());
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

std::string Config::getWengoSubscribePath() const {
	return getStringKeyValue(WENGO_SUBSCRIBE_PATH_KEY);
}

std::string Config::getWengoInfoPath() const {
	return getStringKeyValue(WENGO_INFO_PATH_KEY);
}

std::string Config::getWengoSoftUpdatePath() const {
	return getStringKeyValue(WENGO_SOFTUPDATE_PATH_KEY);
}

std::string Config::getWengoCirpackPath() const {
	return getStringKeyValue(WENGO_CIRPACK_PATH_KEY);
}

std::string Config::getWengoDirectoryPath() const {
	return getStringKeyValue(WENGO_DIRECTORY_PATH_KEY);
}

std::string Config::getProfileLastUsedName() const {
	return getStringKeyValue(PROFILE_LAST_USED_NAME_KEY);
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

std::string Config::getCallForwardMode() const {
	return getStringKeyValue(CALL_FORWARD_MODE_KEY);
}

std::string Config::getCallForwardPhoneNumber1() const {
	return getStringKeyValue(CALL_FORWARD_PHONENUMBER1_KEY);
}

std::string Config::getCallForwardPhoneNumber2() const {
	return getStringKeyValue(CALL_FORWARD_PHONENUMBER2_KEY);
}

std::string Config::getCallForwardPhoneNumber3() const {
	return getStringKeyValue(CALL_FORWARD_PHONENUMBER3_KEY);
}

bool Config::getCallForwardToMobile() const {
	return getBooleanKeyValue(CALL_FORWARD_TOMOBILE_KEY);
}

bool Config::getVoiceMailActive() const {
	return getBooleanKeyValue(VOICE_MAIL_ACTIVE_KEY);
}

bool Config::getGeneralAutoStart() const {
	return getBooleanKeyValue(GENERAL_AUTOSTART_KEY);
}

bool Config::getGeneralClickStartFreeCall() const {
	return getBooleanKeyValue(GENERAL_CLICK_START_FREECALL_KEY);
}

bool Config::getGeneralClickStartChat() const {
	return getBooleanKeyValue(GENERAL_CLICK_START_CHAT_KEY);
}

bool Config::getGeneralClickCallCellPhone() const {
	return getBooleanKeyValue(GENERAL_CLICK_CALL_CELLPHONE_KEY);
}

int Config::getGeneralAwayTimer() const {
	return getIntegerKeyValue(GENERAL_AWAY_TIMER_KEY);
}

bool Config::getShowGroups() const {
    return getBooleanKeyValue(GENERAL_SHOW_GROUPS_KEY);
}

bool Config::getShowOfflineContacts() const {
    return getBooleanKeyValue(GENERAL_SHOW_OFFLINE_CONTACTS_KEY);
}

std::string Config::getLanguage() const {
	return getStringKeyValue(LANGUAGE_KEY);
}

bool Config::getNotificationShowToasterOnIncomingCall() const {
	return getBooleanKeyValue(NOTIFICATION_SHOW_TOASTER_ON_INCOMING_CALL_KEY);
}

bool Config::getNotificationShowToasterOnIncomingChat() const {
	return getBooleanKeyValue(NOTIFICATION_SHOW_TOASTER_ON_INCOMING_CHAT_KEY);
}

bool Config::getNotificationShowToasterOnContactOnline() const {
	return getBooleanKeyValue(NOTIFICATION_SHOW_TOASTER_ON_CONTACT_ONLINE_KEY);
}

bool Config::getNotificationDoNotDisturbNoWindow() const {
	return getBooleanKeyValue(NOTIFICATION_DONOTDISTURB_NO_WINDOW_KEY);
}

bool Config::getNotificationDoNotDisturbNoAudio() const {
	return getBooleanKeyValue(NOTIFICATION_DONOTDISTURB_NO_AUDIO_KEY);
}

bool Config::getNotificationAwayNoWindow() const {
	return getBooleanKeyValue(NOTIFICATION_AWAY_NO_WINDOW_KEY);
}

bool Config::getNotificationAwayNoAudio() const {
	return getBooleanKeyValue(NOTIFICATION_AWAY_NO_AUDIO_KEY);
}

bool Config::getPrivacyAllowCallFromAnyone() const {
	return getBooleanKeyValue(PRIVACY_ALLOW_CALL_FROM_ANYONE_KEY);
}

bool Config::getPrivacyAllowCallOnlyFromContactList() const {
	return getBooleanKeyValue(PRIVACY_ALLOW_CALL_ONLY_FROM_CONTACT_LIST_KEY);
}

bool Config::getPrivacyAllowChatsFromAnyone() const {
	return getBooleanKeyValue(PRIVACY_ALLOW_CHAT_FROM_ANYONE_KEY);
}

bool Config::getPrivacyAllowChatOnlyFromContactList() const {
	return getBooleanKeyValue(PRIVACY_ALLOW_CHAT_ONLY_FROM_CONTACT_LIST_KEY);
}

bool Config::getPrivacySignAsInvisible() const {
	return getBooleanKeyValue(PRIVACY_SIGN_AS_INVISIBLE_KEY);
}

bool Config::getVideoEnable() const {
	if (getVideoWebcamDevice().empty()) {
		return false;
	} else {
		return getBooleanKeyValue(VIDEO_ENABLE_KEY);
	}
}

std::string Config::getVideoWebcamDevice() const {
	WebcamDriver * webcam = WebcamDriver::getInstance();

	std::string deviceName = getStringKeyValue(VIDEO_WEBCAM_DEVICE_KEY);

	//FIXME see fixme DirectXWebcamDriver.h
	//std::string defaultDevice = webcam->getDefaultDevice();
	std::string defaultDevice = deviceName;

	if (defaultDevice == WEBCAM_NULL) {
		defaultDevice = String::null;
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

int Config::getVideoQuality() const {
	return EnumVideoQuality::checkValue(getIntegerKeyValue(VIDEO_QUALITY_KEY));
}

bool Config::getXVideoEnable() const {
	return getBooleanKeyValue(VIDEO_ENABLE_XVIDEO);
}

bool Config::getVideoFlipEnable() const {
	return getBooleanKeyValue(VIDEO_ENABLE_FLIP);
}

bool Config::getWenboxEnable() const {
	return getBooleanKeyValue(WENBOX_ENABLE_KEY);
}

bool Config::getIEActiveXEnable() const {
	return getBooleanKeyValue(IEACTIVEX_ENABLE_KEY);
}

std::string Config::getCmdLinePlaceCall() const {
	return getStringKeyValue(CMDLINE_PLACECALL_KEY);
}

bool Config::getCmdLineBackgroundModeEnable() const {
	return getBooleanKeyValue(CMDLINE_BACKGROUND_MODE_ENABLE_KEY);
}

std::string Config::getCmdServerAuthorized() const {
	return getStringKeyValue(CMDSERVER_AUTHORIZED_KEY);
}

std::string Config::getFileTransferDownloadFolder() const {
	return getStringKeyValue(FILETRANSFER_DOWNLOAD_FOLDER);
}

std::string Config::getLinuxPreferedBrowser() const {
	return getStringKeyValue(LINUX_PREFERED_BROWSER);
}
