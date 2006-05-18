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

#include <sound/AudioDevice.h>

#include <util/File.h>
#include <util/Path.h>
#include <util/Logger.h>
#include <cutil/global.h>

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

const std::string Config::ACTIVE_VOICE_MAIL_KEY = "voicemail.active";

const std::string Config::GENERAL_AUTOSTART_KEY = "general.autostart";
const std::string Config::GENERAL_CLICK_START_FREECALL_KEY = "general.click.start.freecall";
const std::string Config::GENERAL_CLICK_START_CHAT_KEY = "general.click.start.chat";
const std::string Config::GENERAL_CLICK_CALL_CELLPHONE_KEY = "general.click.call.cellphone";
const std::string Config::GENERAL_AWAY_TIMER_KEY = "general.away.timer";

const std::string Config::LANGUAGE_AUTODETECT_KEYVALUE = "detect";
const std::string Config::LANGUAGE_KEY = "language";

const std::string Config::NOTIFICATION_SHOW_TOASTER_ON_INCOMING_CALL_KEY = "notification.show.toaster.on.incoming.call";
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

const std::string Config::WENGO_SERVER_HOSTNAME_KEY = "wengo.server.hostname";
const std::string Config::WENGO_SMS_PATH_KEY = "wengo.sms.path";
const std::string Config::WENGO_SSO_PATH_KEY = "wengo.sso.path";
const std::string Config::WENGO_INFO_PATH_KEY = "wengo.info.path";
const std::string Config::WENGO_SUBSCRIBE_PATH_KEY = "wengo.subscribe.path";
const std::string Config::WENGO_SOFTUPDATE_PATH_KEY = "wengo.softupdate.path";
const std::string Config::WENGO_CIRPACK_PATH_KEY = "wengo.cirpack.path";
const std::string Config::WENGO_DIRECTORY_PATH_KEY = "wengo.directory.path";

const std::string Config::CONFIG_DIR_KEY = "config.dir";
const std::string Config::RESOURCES_DIR_KEY = "resources.dir";

const std::string Config::WENBOX_ENABLE_KEY = "wenbox.enable";
const std::string Config::IEACTIVEX_ENABLE_KEY = "ieactivex.enable";

Config::Config(const std::string & name)
	: AutomaticSettings() {

	static const std::string empty(String::null);
	static const StringList emptyStrList;
	std::string resourcesPath;
	std::string configPath;
	std::string pluginsPath;

	_name = name;

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
	CFBundleRef mainBundle = CFBundleGetMainBundle();
	if (mainBundle) {
		CFURLRef url = CFBundleCopyResourcesDirectoryURL(mainBundle);
		char resPath[1024];

		if (CFURLGetFileSystemRepresentation(url, true, (UInt8 *) resPath, sizeof(resPath))) {
			resourcesPath = (std::string(resPath) + File::getPathSeparator());
		}

		CFRelease(url);
	}
#endif
	_keyDefaultValueMap[RESOURCES_DIR_KEY] = resourcesPath;

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
	if (mainBundle) {
		CFURLRef url = CFBundleCopyPrivateFrameworksURL(mainBundle);
		char frameworkPath[1024];

		if (CFURLGetFileSystemRepresentation(url, true, (UInt8 *) frameworkPath, sizeof(frameworkPath))) {
			pluginsPath = (std::string(frameworkPath) + File::getPathSeparator()
				+ "phapi-plugins" + File::getPathSeparator());
		}

		CFRelease(url);
	}
#else
	pluginsPath = empty;
#endif
	_keyDefaultValueMap[CODEC_PLUGIN_PATH_KEY] = pluginsPath;

	_keyDefaultValueMap[AUDIO_OUTPUT_DEVICENAME_KEY] = AudioDevice::getDefaultPlaybackDevice();
	_keyDefaultValueMap[AUDIO_INPUT_DEVICENAME_KEY] = AudioDevice::getDefaultRecordDevice();
	_keyDefaultValueMap[AUDIO_RINGER_DEVICENAME_KEY] = AudioDevice::getDefaultPlaybackDevice();

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
	_keyDefaultValueMap[PROFILE_WIDTH] = 786;
	_keyDefaultValueMap[PROFILE_HEIGHT] = 758;
	_keyDefaultValueMap[PROFILE_POSX] = 100;
	_keyDefaultValueMap[PROFILE_POSY] = 100;

	_keyDefaultValueMap[CALL_FORWARD_MODE_KEY] = std::string("unauthorized");
	_keyDefaultValueMap[CALL_FORWARD_PHONENUMBER1_KEY] = empty;
	_keyDefaultValueMap[CALL_FORWARD_PHONENUMBER2_KEY] = empty;
	_keyDefaultValueMap[CALL_FORWARD_PHONENUMBER3_KEY] = empty;
	_keyDefaultValueMap[CALL_FORWARD_TOMOBILE_KEY] = false;
	_keyDefaultValueMap[ACTIVE_VOICE_MAIL_KEY] = false;

	_keyDefaultValueMap[GENERAL_AUTOSTART_KEY] = true;
	_keyDefaultValueMap[GENERAL_CLICK_START_FREECALL_KEY] = true;
	_keyDefaultValueMap[GENERAL_CLICK_START_CHAT_KEY] = false;
	_keyDefaultValueMap[GENERAL_CLICK_CALL_CELLPHONE_KEY] = true;
	_keyDefaultValueMap[GENERAL_AWAY_TIMER_KEY] = 2;

	_keyDefaultValueMap[LANGUAGE_KEY] = LANGUAGE_AUTODETECT_KEYVALUE;

	_keyDefaultValueMap[NOTIFICATION_SHOW_TOASTER_ON_INCOMING_CALL_KEY] = false;
	_keyDefaultValueMap[NOTIFICATION_SHOW_TOASTER_ON_CONTACT_ONLINE_KEY] = false;
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
}

Config::~Config() {
}

std::string Config::getName() const {
	return _name;
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

std::string Config::getAudioRingerDeviceName() const {
	std::string deviceName = getStringKeyValue(AUDIO_RINGER_DEVICENAME_KEY);
	if (deviceName.empty()) {
		return AudioDevice::getDefaultPlaybackDevice();
	} else {
		return deviceName;
	}
}

std::string Config::getAudioInputDeviceName() const {
	std::string deviceName = getStringKeyValue(AUDIO_INPUT_DEVICENAME_KEY);
	if (deviceName.empty()) {
		return AudioDevice::getDefaultRecordDevice();
	} else {
		return deviceName;
	}
}

std::string Config::getAudioOutputDeviceName() const {
	std::string deviceName = getStringKeyValue(AUDIO_OUTPUT_DEVICENAME_KEY);
	if (deviceName.empty()) {
		return AudioDevice::getDefaultPlaybackDevice();
	} else {
		return deviceName;
	}
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

bool Config::hasVoiceMail() const {
	return getBooleanKeyValue(ACTIVE_VOICE_MAIL_KEY);
}

bool Config::getGeneralAutoStart() const {
	return getBooleanKeyValue(GENERAL_AUTOSTART_KEY);
}

bool Config::getGeneralClickStartFreeCall() const {
	return getBooleanKeyValue(GENERAL_CLICK_START_FREECALL_KEY);
}

bool Config::getGeneralClickStartChat() const {
	return getBooleanKeyValue(GENERAL_CLICK_START_FREECALL_KEY);
}

bool Config::getGeneralClickCallCellPhone() const {
	return getBooleanKeyValue(GENERAL_CLICK_CALL_CELLPHONE_KEY);
}

int Config::getGeneralAwayTimer() const {
	return getIntegerKeyValue(GENERAL_AWAY_TIMER_KEY);
}

std::string Config::getLanguage() const {
	return getStringKeyValue(LANGUAGE_KEY);
}

bool Config::getNotificationShowToasterOnIncomingCall() const {
	return getBooleanKeyValue(NOTIFICATION_SHOW_TOASTER_ON_INCOMING_CALL_KEY);
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
	return getBooleanKeyValue(VIDEO_ENABLE_KEY);
}

std::string Config::getVideoWebcamDevice() const {
	return getStringKeyValue(VIDEO_WEBCAM_DEVICE_KEY);
}

int Config::getVideoQuality() const {
	return EnumVideoQuality::checkValue(getIntegerKeyValue(VIDEO_QUALITY_KEY));
}

bool Config::getWenboxEnable() const {
	return getBooleanKeyValue(WENBOX_ENABLE_KEY);
}

bool Config::getIEActiveX() const {
	return getBooleanKeyValue(IEACTIVEX_ENABLE_KEY);
}
