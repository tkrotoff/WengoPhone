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

#include "Config.h"

#include <model/wenbox/EnumWenboxStatus.h>

#include <sipwrapper/EnumNatType.h>
#include <sipwrapper/EnumVideoQuality.h>

#include <PhApiCodecList.h>

#include <webcam/WebcamDriver.h>

#include <sound/AudioDeviceManager.h>

#include <util/File.h>
#include <util/Path.h>
#include <util/Logger.h>
#include <cutil/global.h>

#ifdef OS_MACOSX
	#include <CoreFoundation/CoreFoundation.h>
#endif

using namespace std;

const int Config::CONFIG_VERSION = 8;

const string Config::CONFIG_VERSION_KEY = "config.version";

const string Config::EXECUTABLE_NAME_KEY = "executable.name";
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

const string Config::CODEC_PLUGIN_PATH_KEY = "codec.plugin.path";
const string Config::PHAPI_PLUGIN_PATH_KEY = "phapi.plugin.path";

const string Config::AUDIO_CODEC_LIST_KEY = "audio.codec.list";
const string Config::AUDIO_OUTPUT_DEVICEID_KEY = "audio.output.deviceid";
const string Config::AUDIO_INPUT_DEVICEID_KEY = "audio.input.deviceid";
const string Config::AUDIO_RINGER_DEVICEID_KEY = "audio.ringer.deviceid";
const string Config::AUDIO_ENABLE_RINGING_KEY = "audio.ringing.enable";

const string Config::AUDIO_INCOMINGCALL_FILE_KEY = "audio.incomingcall.file";
const string Config::AUDIO_CALLCLOSED_FILE_KEY = "audio.callclosed.file";
const string Config::AUDIO_INCOMINGCHAT_FILE_KEY = "audio.incomingchat.file";
const string Config::AUDIO_IMACCOUNTCONNECTED_FILE_KEY = "audio.imaccountconnected.file";
const string Config::AUDIO_IMACCOUNTDISCONNECTED_FILE_KEY = "audio.imaccountdisconnected.file";
const string Config::AUDIO_CONTACTONLINE_FILE_KEY = "audio.contactonline.file";

const string Config::AUDIO_SMILEYS_DIR_KEY = "audio.smileys.dir";
const string Config::AUDIO_AEC_KEY = "audio.aec";
const string Config::AUDIO_HALFDUPLEX_KEY = "audio.halfduplex";

const string Config::PROFILE_LAST_USED_NAME_KEY = "profile.last_used_name";
const string Config::PROFILE_WIDTH_KEY = "profile.width";
const string Config::PROFILE_HEIGHT_KEY = "profile.height";
const string Config::PROFILE_POSX_KEY = "profile.posx";
const string Config::PROFILE_POSY_KEY = "profile.posy";

const string Config::CALL_FORWARD_MODE_KEY = "call.forward.mode";
const string Config::CALL_FORWARD_PHONENUMBER1_KEY = "call.forward.phonenumber1";
const string Config::CALL_FORWARD_PHONENUMBER2_KEY = "call.forward.phonenumber2";
const string Config::CALL_FORWARD_PHONENUMBER3_KEY = "call.forward.phonenumber3";

const string Config::VOICE_MAIL_ACTIVE_KEY = "voicemail.active";

const string Config::GENERAL_AUTOSTART_KEY = "general.autostart";
const string Config::GENERAL_CLICK_START_FREECALL_KEY = "general.click.start.freecall";
const string Config::GENERAL_CLICK_START_CHAT_KEY = "general.click.start.chat";
const string Config::GENERAL_CLICK_CALL_CELLPHONE_KEY = "general.click.call.cellphone";
const string Config::GENERAL_AWAY_TIMER_KEY = "general.away.timer";
const string Config::GENERAL_SHOW_GROUPS_KEY = "general.show.groups";
const string Config::GENERAL_SHOW_OFFLINE_CONTACTS_KEY = "general.show.offline.contacts";

const string Config::LANGUAGE_AUTODETECT_KEYVALUE = "detect";
const string Config::LANGUAGE_KEY = "language";

const string Config::NOTIFICATION_SHOW_TOASTER_ON_INCOMING_CALL_KEY = "notification.show.toaster.on.incoming.call";
const string Config::NOTIFICATION_SHOW_TOASTER_ON_INCOMING_CHAT_KEY = "notification.show.toaster.on.incoming.chat";
const string Config::NOTIFICATION_SHOW_TOASTER_ON_CONTACT_ONLINE_KEY = "notification.show.toaster.on.contact";
const string Config::NOTIFICATION_DONOTDISTURB_NO_WINDOW_KEY = "notification.donotdisturb.nowindow";
const string Config::NOTIFICATION_DONOTDISTURB_NO_AUDIO_KEY = "notification.donotdisturb.noaudio";
const string Config::NOTIFICATION_AWAY_NO_WINDOW_KEY = "notification.away.nowindow";
const string Config::NOTIFICATION_AWAY_NO_AUDIO_KEY = "notification.away.noaudio";

const string Config::PRIVACY_ALLOW_CALL_FROM_ANYONE_KEY = "privacy.allow.call.from.anyone";
const string Config::PRIVACY_ALLOW_CALL_ONLY_FROM_CONTACT_LIST_KEY = "privacy.allow.call.only.from.contact.list";
const string Config::PRIVACY_ALLOW_CHAT_FROM_ANYONE_KEY = "privacy.allow.chat.from.anyone";
const string Config::PRIVACY_ALLOW_CHAT_ONLY_FROM_CONTACT_LIST_KEY = "privacy.allow.chat.only.from.contact.list";
const string Config::PRIVACY_SIGN_AS_INVISIBLE_KEY = "privacy.sign.as.invisible";

const string Config::VIDEO_ENABLE_KEY = "video.enable";
const string Config::VIDEO_WEBCAM_DEVICE_KEY = "video.webcam.device";
const string Config::VIDEO_QUALITY_KEY = "video.quality";
const string Config::VIDEO_ENABLE_XVIDEO_KEY = "video.xvideo.enable";
const string Config::VIDEO_ENABLE_FLIP_KEY = "video.flip.enable";

const string Config::WENGO_SERVER_HOSTNAME_KEY = "wengo.server.hostname";
const string Config::WENGO_SMS_PATH_KEY = "wengo.sms.path";
const string Config::WENGO_SSO_PATH_KEY = "wengo.sso.path";
const string Config::WENGO_INFO_PATH_KEY = "wengo.info.path";
const string Config::WENGO_SUBSCRIBE_PATH_KEY = "wengo.subscribe.path";
const string Config::WENGO_SOFTUPDATE_PATH_KEY = "wengo.softupdate.path";
const string Config::WENGO_CIRPACK_PATH_KEY = "wengo.cirpack.path";
const string Config::WENGO_DIRECTORY_PATH_KEY = "wengo.directory.path";

const string Config::CONFIG_DIR_KEY = "config.dir";
const string Config::LAST_CHAT_HISTORY_SAVE_DIR_KEY = "last.chat.history.save.dir";
const string Config::RESOURCES_DIR_KEY = "resources.dir";

const string Config::WENBOX_ENABLE_KEY = "wenbox.enable";
const string Config::WENBOX_AUDIO_OUTPUT_DEVICEID_KEY = "wenbox.audio.output.deviceid";
const string Config::WENBOX_AUDIO_INPUT_DEVICEID_KEY = "wenbox.audio.input.deviceid";
const string Config::WENBOX_AUDIO_RINGER_DEVICEID_KEY = "wenbox.audio.ringer.deviceid";

const string Config::IEACTIVEX_ENABLE_KEY = "ieactivex.enable";

const string Config::CMDLINE_BACKGROUND_MODE_ENABLE_KEY = "cmdline.background.mode.enable";
const string Config::CMDLINE_PLACECALL_KEY = "cmdline.placecall";

const string Config::CMDSERVER_AUTHORIZED_KEY = "cmdserver.authorized";

const string Config::FILETRANSFER_DOWNLOAD_FOLDER_KEY = "filetransfer.downloadfolder";
const string Config::FILETRANSFER_LASTUPLOADEDFILE_FOLDER_KEY = "filetransfer.lastuploadedfilefolder";

const string Config::LINUX_PREFERED_BROWSER_KEY = "linux.prefered.browser";

const string Config::CALL_ENCRYPTION_MODE_KEY = "call.encryption.mode";

Config::Config(const string & name)
	: AutomaticSettings() {

	static const string empty(String::null);
	static const StringList emptyStrList;
	string resourcesPath;
	string configPath;
	string pluginsPath;

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
#elif defined(OS_LINUX) and defined(OW_RESOURCEDIR)
	resourcesPath = OW_RESOURCEDIR + File::getPathSeparator();

	//Check if resourcesPath exist, if not then Path::getApplicationDirPath()
	if (!File::exists(resourcesPath)) {
		resourcesPath = Path::getApplicationDirPath();
	}
#endif

	_keyDefaultValueMap[RESOURCES_DIR_KEY] = resourcesPath;
	_keyDefaultValueMap[EXECUTABLE_NAME_KEY] = empty;

	_keyDefaultValueMap[NETWORK_SSO_SSL_KEY] = true;
	_keyDefaultValueMap[NETWORK_NAT_TYPE_KEY] = EnumNatType::toString(EnumNatType::NatTypeSymmetric);
	_keyDefaultValueMap[NETWORK_SIP_SERVER_KEY] = empty;
	_keyDefaultValueMap[NETWORK_SIP_LOCAL_PORT_KEY] = 5060;
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
#elif defined(OS_WINDOWS)
	pluginsPath = Path::getApplicationDirPath();
#else
	pluginsPath = empty;
#endif
	_keyDefaultValueMap[CODEC_PLUGIN_PATH_KEY] = pluginsPath;
	_keyDefaultValueMap[PHAPI_PLUGIN_PATH_KEY] = pluginsPath;

	StringList audiocodeclist;
	audiocodeclist +=	PhApiCodecList::AUDIO_CODEC_SPEEXWB;
#ifdef ENABLE_ILBC
	audiocodeclist +=	PhApiCodecList::AUDIO_CODEC_ILBC;
#endif
	audiocodeclist +=	PhApiCodecList::AUDIO_CODEC_AMRWB;
	audiocodeclist +=	PhApiCodecList::AUDIO_CODEC_PCMU;
	audiocodeclist +=	PhApiCodecList::AUDIO_CODEC_PCMA;
	audiocodeclist +=	PhApiCodecList::AUDIO_CODEC_AMRNB;
	audiocodeclist +=	PhApiCodecList::AUDIO_CODEC_GSM;
	_keyDefaultValueMap[AUDIO_CODEC_LIST_KEY] = audiocodeclist;

	_keyDefaultValueMap[AUDIO_OUTPUT_DEVICEID_KEY] = AudioDeviceManager::getInstance().getDefaultOutputDevice().getData();
	_keyDefaultValueMap[AUDIO_INPUT_DEVICEID_KEY] = AudioDeviceManager::getInstance().getDefaultInputDevice().getData();
	_keyDefaultValueMap[AUDIO_RINGER_DEVICEID_KEY] = AudioDeviceManager::getInstance().getDefaultOutputDevice().getData();
	_keyDefaultValueMap[AUDIO_ENABLE_RINGING_KEY] = true;

	_keyDefaultValueMap[AUDIO_INCOMINGCALL_FILE_KEY] = File::convertPathSeparators(resourcesPath + "sounds/ringin.wav");
	_keyDefaultValueMap[AUDIO_CALLCLOSED_FILE_KEY] = File::convertPathSeparators(resourcesPath + "sounds/callclosed.wav");
	_keyDefaultValueMap[AUDIO_INCOMINGCHAT_FILE_KEY] = empty;
	_keyDefaultValueMap[AUDIO_IMACCOUNTCONNECTED_FILE_KEY] = empty;
	_keyDefaultValueMap[AUDIO_IMACCOUNTDISCONNECTED_FILE_KEY] = empty;
	_keyDefaultValueMap[AUDIO_CONTACTONLINE_FILE_KEY] = empty;

	_keyDefaultValueMap[AUDIO_SMILEYS_DIR_KEY] = File::convertPathSeparators(resourcesPath + "sounds/tones/");
	_keyDefaultValueMap[AUDIO_AEC_KEY] = true;
	_keyDefaultValueMap[AUDIO_HALFDUPLEX_KEY] = false;

	_keyDefaultValueMap[PROFILE_LAST_USED_NAME_KEY] = empty;
	_keyDefaultValueMap[PROFILE_WIDTH_KEY] = 0;
	_keyDefaultValueMap[PROFILE_HEIGHT_KEY] = 600;
	_keyDefaultValueMap[PROFILE_POSX_KEY] = 100;
	_keyDefaultValueMap[PROFILE_POSY_KEY] = 100;

	_keyDefaultValueMap[CALL_FORWARD_MODE_KEY] = string("unauthorized");
	_keyDefaultValueMap[CALL_FORWARD_PHONENUMBER1_KEY] = empty;
	_keyDefaultValueMap[CALL_FORWARD_PHONENUMBER2_KEY] = empty;
	_keyDefaultValueMap[CALL_FORWARD_PHONENUMBER3_KEY] = empty;
	_keyDefaultValueMap[VOICE_MAIL_ACTIVE_KEY] = false;

	_keyDefaultValueMap[GENERAL_AUTOSTART_KEY] = true;
	_keyDefaultValueMap[GENERAL_CLICK_START_FREECALL_KEY] = true;
	_keyDefaultValueMap[GENERAL_CLICK_START_CHAT_KEY] = false;
	_keyDefaultValueMap[GENERAL_CLICK_CALL_CELLPHONE_KEY] = true;
	_keyDefaultValueMap[GENERAL_AWAY_TIMER_KEY] = 2;
	_keyDefaultValueMap[GENERAL_SHOW_GROUPS_KEY] = true;
	_keyDefaultValueMap[GENERAL_SHOW_OFFLINE_CONTACTS_KEY] = true;

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
	_keyDefaultValueMap[VIDEO_QUALITY_KEY] = EnumVideoQuality::toString(EnumVideoQuality::VideoQualityNormal);
	_keyDefaultValueMap[VIDEO_ENABLE_XVIDEO_KEY] = false;
	_keyDefaultValueMap[VIDEO_ENABLE_FLIP_KEY] = false;

	_keyDefaultValueMap[WENGO_SERVER_HOSTNAME_KEY] = string("ws.wengo.fr");
	_keyDefaultValueMap[WENGO_SMS_PATH_KEY] = string("/sms/sendsms.php");
	_keyDefaultValueMap[WENGO_SSO_PATH_KEY] = string("/softphone-sso/sso2.php");
	_keyDefaultValueMap[WENGO_INFO_PATH_KEY] = string("/info/info.php");
	_keyDefaultValueMap[WENGO_SUBSCRIBE_PATH_KEY] = string("/softphone-subscription/index.php");
	_keyDefaultValueMap[WENGO_SOFTUPDATE_PATH_KEY] = string("/softphone-version/version.php");
	_keyDefaultValueMap[WENGO_CIRPACK_PATH_KEY] = string("/cirpack/index.php");
	_keyDefaultValueMap[WENGO_DIRECTORY_PATH_KEY] = string("/directory/index.php");

	_keyDefaultValueMap[WENBOX_ENABLE_KEY] = EnumWenboxStatus::toString(EnumWenboxStatus::WenboxStatusNotConnected);
	_keyDefaultValueMap[WENBOX_AUDIO_OUTPUT_DEVICEID_KEY] = AudioDeviceManager::getInstance().getDefaultOutputDevice().getData();
	_keyDefaultValueMap[WENBOX_AUDIO_INPUT_DEVICEID_KEY] = AudioDeviceManager::getInstance().getDefaultInputDevice().getData();
	_keyDefaultValueMap[WENBOX_AUDIO_RINGER_DEVICEID_KEY] = AudioDeviceManager::getInstance().getDefaultOutputDevice().getData();

	_keyDefaultValueMap[IEACTIVEX_ENABLE_KEY] = true;

	_keyDefaultValueMap[CMDLINE_BACKGROUND_MODE_ENABLE_KEY] = false;
	_keyDefaultValueMap[CMDLINE_PLACECALL_KEY] = empty;

	_keyDefaultValueMap[CMDSERVER_AUTHORIZED_KEY] = empty;

	_keyDefaultValueMap[LAST_CHAT_HISTORY_SAVE_DIR_KEY] = Path::getHomeDirPath();

	_keyDefaultValueMap[FILETRANSFER_DOWNLOAD_FOLDER_KEY] = empty;
	_keyDefaultValueMap[FILETRANSFER_LASTUPLOADEDFILE_FOLDER_KEY] = empty;

	_keyDefaultValueMap[LINUX_PREFERED_BROWSER_KEY] = empty;

	_keyDefaultValueMap[CALL_ENCRYPTION_MODE_KEY] = false;
}

Config::~Config() {
}

string Config::getName() const {
	return _name;
}

int Config::getConfigVersion() const {
	return getIntegerKeyValue(CONFIG_VERSION_KEY);
}

string Config::getExecutableName() const {
	return getStringKeyValue(EXECUTABLE_NAME_KEY);
}

bool Config::getNetworkSSOSSL() const {
	return getBooleanKeyValue(NETWORK_SSO_SSL_KEY);
}

string Config::getNetworkNatType() const {
	return getStringKeyValue(NETWORK_NAT_TYPE_KEY);
}

int Config::getNetworkSipLocalPort() const {
	return getIntegerKeyValue(NETWORK_SIP_LOCAL_PORT_KEY);
}

string Config::getNetWorkSipServer() const {
	return getStringKeyValue(NETWORK_SIP_SERVER_KEY);
}

bool Config::getNetworkTunnelSSL() const {
	return getBooleanKeyValue(NETWORK_TUNNEL_SSL_KEY);
}

string Config::getNetworkTunnelServer() const {
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

string Config::getNetworkProxyServer() const {
	return getStringKeyValue(NETWORK_PROXY_SERVER_KEY);
}

int Config::getNetworkProxyPort() const {
	return getIntegerKeyValue(NETWORK_PROXY_PORT_KEY);
}

string Config::getNetworkProxyLogin() const {
	return getStringKeyValue(NETWORK_PROXY_LOGIN_KEY);
}

string Config::getNetworkProxyPassword() const {
	return getStringKeyValue(NETWORK_PROXY_PASSWORD_KEY);
}

string Config::getCodecPluginPath() const {
	return getStringKeyValue(CODEC_PLUGIN_PATH_KEY);
}

string Config::getPhApiPluginPath() const {
	return getStringKeyValue(PHAPI_PLUGIN_PATH_KEY);
}

StringList Config::getAudioCodecList() const {
	StringList res = getStringListKeyValue(AUDIO_CODEC_LIST_KEY);
	return res;
}

bool Config::getAudioAEC() const {
	return getBooleanKeyValue(AUDIO_AEC_KEY);
}

bool Config::getAudioHalfDuplex() const {
	return getBooleanKeyValue(AUDIO_HALFDUPLEX_KEY);
}

bool Config::getAudioRingingEnable() const {
	return getBooleanKeyValue(AUDIO_ENABLE_RINGING_KEY);
}

string Config::getConfigDir() const {
	return getStringKeyValue(CONFIG_DIR_KEY);
}

string Config::getLastChatHistorySaveDir() const {
	return getStringKeyValue(LAST_CHAT_HISTORY_SAVE_DIR_KEY);
}

string Config::getResourcesDir() const {
	return getStringKeyValue(RESOURCES_DIR_KEY);
}

string Config::getAudioSmileysDir() const {
	return getStringKeyValue(AUDIO_SMILEYS_DIR_KEY);
}

string Config::getAudioIncomingCallFile() const {
	return getStringKeyValue(AUDIO_INCOMINGCALL_FILE_KEY);
}

string Config::getAudioCallClosedFile() const {
	return getStringKeyValue(AUDIO_CALLCLOSED_FILE_KEY);
}

string Config::getAudioIncomingChatFile() const {
	return getStringKeyValue(AUDIO_INCOMINGCHAT_FILE_KEY);
}

string Config::getAudioIMAccountConnectedFile() const {
	return getStringKeyValue(AUDIO_IMACCOUNTCONNECTED_FILE_KEY);
}

string Config::getAudioIMAccountDisconnectedFile() const {
	return getStringKeyValue(AUDIO_IMACCOUNTDISCONNECTED_FILE_KEY);
}

string Config::getAudioContactOnlineFile() const {
	return getStringKeyValue(AUDIO_CONTACTONLINE_FILE_KEY);
}

/**
 * Code factorization.
 *
 * @see Config::getAudioRingerDeviceId()
 * @see Config::getAudioInputDeviceId()
 * @see Config::getAudioOutputDeviceId()
 * @param storedDeviceData the stored audio device
 * @param currentdeviceList the actual audio devices list
 * @param currentDefaultDevice the actual default device
 */
static StringList getProperAudioDeviceId(
	const StringList & storedDeviceData,
	const std::list<AudioDevice> & currentdeviceList,
	const StringList & currentDefaultDevice) {

	StringList result;

	// if no device stored use the default one
	if (storedDeviceData.empty()) {
		result = currentDefaultDevice;

	// else check if the stored device is currently available
	// do not exec this check under Linux because when devices
	// are busy they are not in currentdeviceList
	} else {
#ifndef OS_LINUX
		bool found = false;

		std::list<AudioDevice>::const_iterator it;
		for (it = currentdeviceList.begin(); it != currentdeviceList.end(); ++it) {

			if ((*it).getData() == storedDeviceData) {
				found = true;
			}
		}

		// if the stored device has been found we use it
		if (found) {
			result = storedDeviceData;
		} else {
			result = currentDefaultDevice;
		}
#else
		result = storedDeviceData;
#endif
	}
	return result;
}

StringList Config::getAudioOutputDeviceId() const {
	EnumWenboxStatus::WenboxStatus wenboxStatus =
		EnumWenboxStatus::toWenboxStatus(getWenboxEnable());
	if (wenboxStatus == EnumWenboxStatus::WenboxStatusEnable) {
		return getWenboxAudioOutputDeviceId();
	} else {
		return getProperAudioDeviceId(
			getStringListKeyValue(AUDIO_OUTPUT_DEVICEID_KEY),
			AudioDeviceManager::getInstance().getOutputDeviceList(),
			AudioDeviceManager::getInstance().getDefaultOutputDevice().getData()
		);
	}
}

StringList Config::getAudioInputDeviceId() const {
	EnumWenboxStatus::WenboxStatus wenboxStatus =
		EnumWenboxStatus::toWenboxStatus(getWenboxEnable());
	if (wenboxStatus == EnumWenboxStatus::WenboxStatusEnable) {
		return getWenboxAudioInputDeviceId();
	} else {
		return getProperAudioDeviceId(
			getStringListKeyValue(AUDIO_INPUT_DEVICEID_KEY),
			AudioDeviceManager::getInstance().getInputDeviceList(),
			AudioDeviceManager::getInstance().getDefaultInputDevice().getData()
		);
	}
}

StringList Config::getAudioRingerDeviceId() const {
	EnumWenboxStatus::WenboxStatus wenboxStatus =
		EnumWenboxStatus::toWenboxStatus(getWenboxEnable());
	if (wenboxStatus == EnumWenboxStatus::WenboxStatusEnable) {
		return getWenboxAudioRingerDeviceId();
	} else {
		return getProperAudioDeviceId(
			getStringListKeyValue(AUDIO_RINGER_DEVICEID_KEY),
			AudioDeviceManager::getInstance().getOutputDeviceList(),
			AudioDeviceManager::getInstance().getDefaultOutputDevice().getData()
		);
	}
}

string Config::getWengoServerHostname() const {
	return getStringKeyValue(WENGO_SERVER_HOSTNAME_KEY);
}

string Config::getWengoSSOPath() const {
	return getStringKeyValue(WENGO_SSO_PATH_KEY);
}

string Config::getWengoSMSPath() const {
	return getStringKeyValue(WENGO_SMS_PATH_KEY);
}

string Config::getWengoSubscribePath() const {
	return getStringKeyValue(WENGO_SUBSCRIBE_PATH_KEY);
}

string Config::getWengoInfoPath() const {
	return getStringKeyValue(WENGO_INFO_PATH_KEY);
}

string Config::getWengoSoftUpdatePath() const {
	return getStringKeyValue(WENGO_SOFTUPDATE_PATH_KEY);
}

string Config::getWengoCirpackPath() const {
	return getStringKeyValue(WENGO_CIRPACK_PATH_KEY);
}

string Config::getWengoDirectoryPath() const {
	return getStringKeyValue(WENGO_DIRECTORY_PATH_KEY);
}

string Config::getProfileLastUsedName() const {
	return getStringKeyValue(PROFILE_LAST_USED_NAME_KEY);
}

int Config::getProfileWidth() const {
	return getIntegerKeyValue(PROFILE_WIDTH_KEY);
}

int Config::getProfileHeight() const {
	return getIntegerKeyValue(PROFILE_HEIGHT_KEY);
}

int Config::getProfilePosX() const {
	return getIntegerKeyValue(PROFILE_POSX_KEY);
}

int Config::getProfilePoxY() const {
	return getIntegerKeyValue(PROFILE_POSY_KEY);
}

string Config::getCallForwardMode() const {
	return getStringKeyValue(CALL_FORWARD_MODE_KEY);
}

string Config::getCallForwardPhoneNumber1() const {
	return getStringKeyValue(CALL_FORWARD_PHONENUMBER1_KEY);
}

string Config::getCallForwardPhoneNumber2() const {
	return getStringKeyValue(CALL_FORWARD_PHONENUMBER2_KEY);
}

string Config::getCallForwardPhoneNumber3() const {
	return getStringKeyValue(CALL_FORWARD_PHONENUMBER3_KEY);
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

string Config::getLanguage() const {
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

string Config::getVideoWebcamDevice() const {
	WebcamDriver * webcam = WebcamDriver::getInstance();

	string deviceName = getStringKeyValue(VIDEO_WEBCAM_DEVICE_KEY);

	//FIXME see fixme DirectXWebcamDriver.h
	//string defaultDevice = webcam->getDefaultDevice();
	string defaultDevice = deviceName;

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

string Config::getVideoQuality() const {
	return getStringKeyValue(VIDEO_QUALITY_KEY);
}

bool Config::getXVideoEnable() const {
	return getBooleanKeyValue(VIDEO_ENABLE_XVIDEO_KEY);
}

bool Config::getVideoFlipEnable() const {
	return getBooleanKeyValue(VIDEO_ENABLE_FLIP_KEY);
}

string Config::getWenboxEnable() const {
	return getStringKeyValue(WENBOX_ENABLE_KEY);
}

StringList Config::getWenboxAudioOutputDeviceId() const {
	return getStringListKeyValue(WENBOX_AUDIO_OUTPUT_DEVICEID_KEY);
}

StringList Config::getWenboxAudioInputDeviceId() const {
	return getStringListKeyValue(WENBOX_AUDIO_INPUT_DEVICEID_KEY);
}

StringList Config::getWenboxAudioRingerDeviceId() const {
	return getStringListKeyValue(WENBOX_AUDIO_RINGER_DEVICEID_KEY);
}

bool Config::getIEActiveXEnable() const {
	return getBooleanKeyValue(IEACTIVEX_ENABLE_KEY);
}

string Config::getCmdLinePlaceCall() const {
	return getStringKeyValue(CMDLINE_PLACECALL_KEY);
}

bool Config::getCmdLineBackgroundModeEnable() const {
	return getBooleanKeyValue(CMDLINE_BACKGROUND_MODE_ENABLE_KEY);
}

string Config::getCmdServerAuthorized() const {
	return getStringKeyValue(CMDSERVER_AUTHORIZED_KEY);
}

string Config::getFileTransferDownloadFolder() const {
	return getStringKeyValue(FILETRANSFER_DOWNLOAD_FOLDER_KEY);
}

string Config::getLinuxPreferedBrowser() const {
	return getStringKeyValue(LINUX_PREFERED_BROWSER_KEY);
}

string Config::getLastUploadedFileFolder() const {
	return getStringKeyValue(FILETRANSFER_LASTUPLOADEDFILE_FOLDER_KEY);
}

bool Config::getCallEncryptionMode() const {
	return getBooleanKeyValue(CALL_ENCRYPTION_MODE_KEY);
}

void Config::fixWenboxConfigHack() {
	string configFilename = getConfigDir() + "config.xml";

	//Replace in config.xml
	FileReader iConfigFile(configFilename);
	if (iConfigFile.open()) {

		String data = iConfigFile.read();
		iConfigFile.close();

		data.replace(
			"<wenbox.enable><bool>0</bool></wenbox.enable>",
			"<wenbox.enable><string>WenboxStatusNotConnected</string></wenbox.enable>"
		);
		data.replace(
			"<wenbox.enable><bool>1</bool></wenbox.enable>",
			"<wenbox.enable><string>WenboxStatusEnable</string></wenbox.enable>"
		);

		FileWriter oConfigFile(configFilename);
		oConfigFile.write(data);
		oConfigFile.close();
	}
	////
}
