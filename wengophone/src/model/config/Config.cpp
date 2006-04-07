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
const std::string Config::AUDIO_RINGING_FILE_KEY = "audio.ringing.file";
const std::string Config::AUDIO_PERSONAL_CONFIGURATION_KEY = "audio.personal.configuration";
const std::string Config::AUDIO_CALLCLOSED_FILE_KEY = "audio.callclosed.file";
const std::string Config::AUDIO_SMILEYS_DIR_KEY = "audio.smileys.dir";
const std::string Config::AUDIO_AEC_KEY = "audio.aec";
const std::string Config::AUDIO_HALFDUPLEX_KEY = "audio.halfduplex";

const std::string Config::PROFILE_WIDTH = "profile.width";
const std::string Config::PROFILE_HEIGHT = "profile.height";
const std::string Config::PROFILE_POSX = "profile.posx";
const std::string Config::PROFILE_POSY = "profile.posy";

const std::string Config::CALL_FORWARD_ALL_UNDELIVREDTOVM_KEY = "call.forward.all.undelivredtovm";
const std::string Config::CALL_FORWARD_ALL_UNDELIVREDTO_KEY = "call.forward.all.undelivredto";
const std::string Config::CALL_FORWARD_PHONENUMBER1_KEY = "call.forward.phonenumber1";
const std::string Config::CALL_FORWARD_PHONENUMBER2_KEY = "call.forward.phonenumber2";
const std::string Config::CALL_FORWARD_PHONENUMBER3_KEY = "call.forward.phonenumber3";
const std::string Config::CALL_FORWARD_TOMOBILE_KEY = "call.forward.tomobile";

const std::string Config::GENERAL_SETTINGS_AUTOSTART_WENGO_KEY = "general.settings.autostart.wengo";
const std::string Config::GENERAL_SETTINGS_CHECK_UPDATE_KEY = "general.settings.check.update";
const std::string Config::GENERAL_SETTINGS_START_FREECALL_KEY = "general.settings.start.freecall";
const std::string Config::GENERAL_SETTINGS_START_CHATONLY_KEY = "general.settings.start.chatonly";
const std::string Config::GENERAL_SETTINGS_CALL_CELLPHONE_KEY = "general.settings.call.cellphone";
const std::string Config::GENERAL_SETTINGS_SHOW_AWAY_KEY = "general.settings.show.away";
const std::string Config::GENERAL_SETTINGS_SHOW_NOTAVAILABLE_KEY = "general.settings.show.notavailable";
const std::string Config::GENERAL_SETTINGS_AWAY_TIMER_KEY = "general.settings.away.timer";
const std::string Config::GENERAL_SETTINGS_NOTAVAILABLE_TIMER_KEY = "general.settings.notavailable.timer";

const std::string Config::NOTIFICATION_SHOW_WINDOWONTOP_KEY = "notification.show.windowontop";
const std::string Config::NOTIFICATION_SHOW_BLINKINGWINDOW_KEY = "notification.show.blinkingwindow";
const std::string Config::NOTIFICATION_SHOW_TOASTER_ON_INCOMING_CALL_KEY = "notification.show.toaster.on.incoming.call";
const std::string Config::NOTIFICATION_SHOW_TOASTER_ON_CONTACT_ONLINE_KEY = "notification.show.toaster.on.contact";
const std::string Config::NOTIFICATION_SHOW_WINDOWPOPUP_AUTHORIZATION_KEY = "notification.show.windowpopup.authorization";
const std::string Config::NOTIFICATION_SHOW_NO_TOASTER_KEY = "notification.show.no.toaster";
const std::string Config::NOTIFICATION_SWITCH_OFF_AUDIO_NOFITICATION_KEY = "notification.switch.off.audio.notifiaction";
const std::string Config::NOTIFICATION_NOT_SHOW_AWAY_TOASTER_AND_CHAT_KEY = "notification.not.show.away.toaster.and.chat";
const std::string Config::NOTIFICATION_SWITCH_OFF_AWAY_AUDIO_KEY = "notification.switch.off.away";
const std::string Config::NOTIFICATION_INCOMING_CHAT_KEY = "notification.incoming.chat";
const std::string Config::NOTIFICATION_INCOMING_CALLS_KEY = "notification.incoming.calls";
const std::string Config::NOTIFICATION_WENGOPHONE_OPENS_KEY = "notification.wengophone.opens";
const std::string Config::NOTIFICATION_WENGOPHONE_CONNECTS_KEY = "notifiaction.wengophone.connects";
const std::string Config::NOTIFICATION_WENGOPHONE_NOT_SUCCED_CONNECT_KEY="notification.wengophone.not.succed.connect";
const std::string Config::NOTIFICATION_ALTERNATIVE_IMACCOUNT_KEY = "notification.alternative.imaccount";
const std::string Config::NOTIFICATION_CONTACT_TURNING_ONLINE_KEY = "notification.contact.turning.online";
const std::string Config::NOTIFICATION_AUTHORIZATION_KEY = "notification.authorization";
const std::string Config::NOTIFICATION_ERROR_MESSAGE_KEY = "notification.error.message";

const std::string Config::PRIVACY_ALLOW_CALL_FROM_ANYONE_KEY = "privacy.allow.call.from.anyone";
const std::string Config::PRIVACY_ALLOW_CALL_ONLY_FROM_CONTACT_LIST_KEY = "privacy.allow.call.only.from.contact.list";
const std::string Config::PRIVACY_ALLOW_CHAT_FROM_ANYONE_KEY = "privacy.allow.chat.from.anyone";
const std::string Config::PRIVACY_ALLOW_CHAT_ONLY_FROM_CONTACT_LIST_KEY = "privacy.allow.chat.only.from.contact.list";
const std::string Config::PRIVACY_ALLWAYS_SIGN_AS_INVISIBLE_KEY = "privacy.sign.as.invisible";

const std::string Config::VIDEO_ENABLE_KEY = "video.enable";
const std::string Config::VIDEO_WEBCAM_DEVICE_KEY = "video.webcam.device";
const std::string Config::VIDEO_QUALITY_KEY = "video.quality";

const std::string Config::WENGO_SERVER_HOSTNAME_KEY = "wengo.ws.server.hostname";
const std::string Config::WENGO_SMS_PATH_KEY = "wengo.ws.sms.path";
const std::string Config::WENGO_SSO_PATH_KEY = "wengo.ws.sso.path";
const std::string Config::WENGO_WS_INFO_PATH_KEY = "wengo.ws.info.path";
const std::string Config::WENGO_WS_SUBSCRIBE_PATH_KEY = "wengo.ws.subscribe.path";
const std::string Config::WENGO_SOFTUPDATE_PATH_KEY = "wengo.ws.softupdate.path";

const std::string Config::CONFIG_DIR_KEY = "config.dir";
const std::string Config::RESOURCES_DIR_KEY = "resources.dir";

const std::string Config::AVAILABLE_PROTOCOLS_KEY = "available.protocols";

Config::Config(const std::string & name)
	: AutomaticSettings() {

	static const std::string empty("");
	static const StringList emptyStrList;
	StringList defaultProtocols;

	defaultProtocols += "MSN";
	defaultProtocols += "AIM / ICQ";
	defaultProtocols += "Jabber";
	defaultProtocols += "Yahoo";

	std::string resourcesPath;
	std::string configPath;

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
		char applicationPath[1024];

		if (CFURLGetFileSystemRepresentation(url, true, (UInt8 *)applicationPath, sizeof(applicationPath))) {
			resourcesPath = (std::string(applicationPath) + File::getPathSeparator());
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

	_keyDefaultValueMap[CODEC_PLUGIN_PATH_KEY] = empty;
	_keyDefaultValueMap[AUDIO_OUTPUT_DEVICENAME_KEY] = AudioDevice::getDefaultPlaybackDevice();
	_keyDefaultValueMap[AUDIO_INPUT_DEVICENAME_KEY] = AudioDevice::getDefaultRecordDevice();
	_keyDefaultValueMap[AUDIO_RINGER_DEVICENAME_KEY] = AudioDevice::getDefaultPlaybackDevice();
	_keyDefaultValueMap[AUDIO_RINGING_FILE_KEY] = File::convertPathSeparators(resourcesPath + "sounds/ringin.wav");
	_keyDefaultValueMap[AUDIO_PERSONAL_CONFIGURATION_KEY] = false;
	_keyDefaultValueMap[AUDIO_CALLCLOSED_FILE_KEY] = File::convertPathSeparators(resourcesPath + "sounds/callclosed.wav");
	_keyDefaultValueMap[AUDIO_SMILEYS_DIR_KEY] = File::convertPathSeparators(resourcesPath + "sounds/tones/");
	_keyDefaultValueMap[AUDIO_AEC_KEY] = false;
	_keyDefaultValueMap[AUDIO_HALFDUPLEX_KEY] = true;

	_keyDefaultValueMap[PROFILE_WIDTH] = 786;
	_keyDefaultValueMap[PROFILE_HEIGHT] = 758;
	_keyDefaultValueMap[PROFILE_POSX] = 100;
	_keyDefaultValueMap[PROFILE_POSY] = 100;

	_keyDefaultValueMap[CALL_FORWARD_ALL_UNDELIVREDTOVM_KEY] = false;
	_keyDefaultValueMap[CALL_FORWARD_ALL_UNDELIVREDTO_KEY] = false;
	_keyDefaultValueMap[CALL_FORWARD_PHONENUMBER1_KEY] = empty;
	_keyDefaultValueMap[CALL_FORWARD_PHONENUMBER2_KEY] = empty;
	_keyDefaultValueMap[CALL_FORWARD_PHONENUMBER3_KEY] = empty;
	_keyDefaultValueMap[CALL_FORWARD_TOMOBILE_KEY] = false;

	_keyDefaultValueMap[GENERAL_SETTINGS_AUTOSTART_WENGO_KEY] = false;
	_keyDefaultValueMap[GENERAL_SETTINGS_CHECK_UPDATE_KEY] = false;
	_keyDefaultValueMap[GENERAL_SETTINGS_START_FREECALL_KEY] = false;
	_keyDefaultValueMap[GENERAL_SETTINGS_START_CHATONLY_KEY] = false;
	_keyDefaultValueMap[GENERAL_SETTINGS_CALL_CELLPHONE_KEY] = false;
	_keyDefaultValueMap[GENERAL_SETTINGS_SHOW_AWAY_KEY] = false;
	_keyDefaultValueMap[GENERAL_SETTINGS_SHOW_NOTAVAILABLE_KEY] = false;
	_keyDefaultValueMap[GENERAL_SETTINGS_AWAY_TIMER_KEY] = 0;
	_keyDefaultValueMap[GENERAL_SETTINGS_NOTAVAILABLE_TIMER_KEY] = 0;

	_keyDefaultValueMap[NOTIFICATION_SHOW_WINDOWONTOP_KEY] = false;
	_keyDefaultValueMap[NOTIFICATION_SHOW_BLINKINGWINDOW_KEY] = false;
	_keyDefaultValueMap[NOTIFICATION_SHOW_TOASTER_ON_INCOMING_CALL_KEY] = false;
	_keyDefaultValueMap[NOTIFICATION_SHOW_TOASTER_ON_CONTACT_ONLINE_KEY] = false;
	_keyDefaultValueMap[NOTIFICATION_SHOW_WINDOWPOPUP_AUTHORIZATION_KEY] = false;
	_keyDefaultValueMap[NOTIFICATION_SHOW_NO_TOASTER_KEY] = false;
	_keyDefaultValueMap[NOTIFICATION_SWITCH_OFF_AUDIO_NOFITICATION_KEY] = false;
	_keyDefaultValueMap[NOTIFICATION_NOT_SHOW_AWAY_TOASTER_AND_CHAT_KEY] = false;
	_keyDefaultValueMap[NOTIFICATION_SWITCH_OFF_AWAY_AUDIO_KEY] = false;
	_keyDefaultValueMap[NOTIFICATION_INCOMING_CHAT_KEY] = false;
	_keyDefaultValueMap[NOTIFICATION_INCOMING_CALLS_KEY] = false;
	_keyDefaultValueMap[NOTIFICATION_WENGOPHONE_OPENS_KEY] = false;
	_keyDefaultValueMap[NOTIFICATION_WENGOPHONE_CONNECTS_KEY] = false;
	_keyDefaultValueMap[NOTIFICATION_WENGOPHONE_NOT_SUCCED_CONNECT_KEY] = false;
	_keyDefaultValueMap[NOTIFICATION_ALTERNATIVE_IMACCOUNT_KEY] = false;
	_keyDefaultValueMap[NOTIFICATION_CONTACT_TURNING_ONLINE_KEY] = false;
	_keyDefaultValueMap[NOTIFICATION_AUTHORIZATION_KEY] = false;
	_keyDefaultValueMap[NOTIFICATION_ERROR_MESSAGE_KEY] = false;

	_keyDefaultValueMap[PRIVACY_ALLOW_CALL_FROM_ANYONE_KEY] = false;
	_keyDefaultValueMap[PRIVACY_ALLOW_CALL_ONLY_FROM_CONTACT_LIST_KEY] = false;
	_keyDefaultValueMap[PRIVACY_ALLOW_CHAT_FROM_ANYONE_KEY] = false;
	_keyDefaultValueMap[PRIVACY_ALLOW_CHAT_ONLY_FROM_CONTACT_LIST_KEY] = false;
	_keyDefaultValueMap[PRIVACY_ALLWAYS_SIGN_AS_INVISIBLE_KEY] = false;

	_keyDefaultValueMap[VIDEO_ENABLE_KEY] = false;
	_keyDefaultValueMap[VIDEO_WEBCAM_DEVICE_KEY] = empty;
	_keyDefaultValueMap[VIDEO_QUALITY_KEY] = empty;

	_keyDefaultValueMap[WENGO_SERVER_HOSTNAME_KEY] = std::string("ws.wengo.fr");
	_keyDefaultValueMap[WENGO_SMS_PATH_KEY] = std::string("/sms/sendsms.php");
	_keyDefaultValueMap[WENGO_SSO_PATH_KEY] = std::string("/softphone-sso/sso2.php");
	_keyDefaultValueMap[WENGO_WS_INFO_PATH_KEY] = std::string("/info/info.php");
	_keyDefaultValueMap[WENGO_WS_SUBSCRIBE_PATH_KEY] = std::string("/softphone-subscription/index.php");
	_keyDefaultValueMap[WENGO_SOFTUPDATE_PATH_KEY] = std::string("/softphone-version/version.php");

	_keyDefaultValueMap[AVAILABLE_PROTOCOLS_KEY] = defaultProtocols;
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

bool Config::getAudioPersonalConfiguration() const {
	return getBooleanKeyValue(AUDIO_PERSONAL_CONFIGURATION_KEY);
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
	return getStringKeyValue(WENGO_WS_SUBSCRIBE_PATH_KEY);
}

std::string Config::getWengoWsInfoPath() const {
	return getStringKeyValue(WENGO_WS_INFO_PATH_KEY);
}

std::string Config::getWengoSoftUpdatePath() const {
	return getStringKeyValue(WENGO_SOFTUPDATE_PATH_KEY);
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

bool Config::getCallForwardAllUndelivredToVm() const {
	return getBooleanKeyValue(CALL_FORWARD_ALL_UNDELIVREDTOVM_KEY);
}

bool Config::getCallForwardAllUndelivredTo() const {
	return getBooleanKeyValue(CALL_FORWARD_ALL_UNDELIVREDTO_KEY);
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

bool Config::getGeneralSettingsAutoStartWengo() const {
	return getBooleanKeyValue(GENERAL_SETTINGS_AUTOSTART_WENGO_KEY);
}

bool Config::getGeneralSettingsCheckUpdate() const {
	return getBooleanKeyValue(GENERAL_SETTINGS_CHECK_UPDATE_KEY);
}

bool Config::getGeneralSettingsStartFreeCall() const {
	return getBooleanKeyValue(GENERAL_SETTINGS_START_FREECALL_KEY);
}

bool Config::getGeneralSettingsStartChatOnly() const {
	return getBooleanKeyValue(GENERAL_SETTINGS_START_CHATONLY_KEY);
}

bool Config::getGeneralSettingsCallCellPhone() const {
	return getBooleanKeyValue(GENERAL_SETTINGS_CALL_CELLPHONE_KEY);
}

bool Config::getGeneralSettingsShowAway() const {
	return getBooleanKeyValue(GENERAL_SETTINGS_SHOW_AWAY_KEY);
}

bool Config::getGeneralSettingsShowNotAvailable() const {
	return getBooleanKeyValue(GENERAL_SETTINGS_SHOW_NOTAVAILABLE_KEY);
}

int Config::getGeneralSettingsGetAwayTimer() const {
	return getIntegerKeyValue(GENERAL_SETTINGS_AWAY_TIMER_KEY);
}

int Config::getGeneralSettingsNotAvailableTimer() const {
	return getIntegerKeyValue(GENERAL_SETTINGS_NOTAVAILABLE_TIMER_KEY);
}

bool Config::getNotificationShowWindowOnTop() const {
	return getBooleanKeyValue(NOTIFICATION_SHOW_WINDOWONTOP_KEY);
}

bool Config::getNotificationShowBlinkingWindow() const {
	return getBooleanKeyValue(NOTIFICATION_SHOW_BLINKINGWINDOW_KEY);
}

bool Config::getNotificationShowToasterOnIncomingCall() const {
	return getBooleanKeyValue(NOTIFICATION_SHOW_TOASTER_ON_INCOMING_CALL_KEY);
}

bool Config::getNotificationShowToasterOnContactOnline() const {
	return getBooleanKeyValue(NOTIFICATION_SHOW_TOASTER_ON_CONTACT_ONLINE_KEY);
}

bool Config::getNotificationShowWindowPopupAuthorization() const {
	return getBooleanKeyValue(NOTIFICATION_SHOW_WINDOWPOPUP_AUTHORIZATION_KEY);
}

bool Config::getNotificationShowNoToaster() const {
	return getBooleanKeyValue(NOTIFICATION_SHOW_NO_TOASTER_KEY);
}

bool Config::getNotificationSwithOffAudioNotification() const {
	return getBooleanKeyValue(NOTIFICATION_SWITCH_OFF_AUDIO_NOFITICATION_KEY);
}

bool Config::getNotificationNotShowAwayToasterAndChat() const {
	return getBooleanKeyValue(NOTIFICATION_NOT_SHOW_AWAY_TOASTER_AND_CHAT_KEY);
}

bool Config::getNotificationSwitchOffAwayAudio() const {
	return getBooleanKeyValue(NOTIFICATION_SWITCH_OFF_AWAY_AUDIO_KEY);
}

bool Config::getNotificationIncomingChat() const {
	return getBooleanKeyValue(NOTIFICATION_INCOMING_CHAT_KEY);
}

bool Config::getNotificationIncomingCalls() const {
	return getBooleanKeyValue(NOTIFICATION_INCOMING_CALLS_KEY);
}

bool Config::getNotificationWengophoneOpens() const {
	return getBooleanKeyValue(NOTIFICATION_WENGOPHONE_OPENS_KEY);
}

bool Config::getNotificationWengophoneConnects() const {
	return getBooleanKeyValue(NOTIFICATION_WENGOPHONE_CONNECTS_KEY);
}

bool Config::getNotificationWengophoneNotSuccedConnect() const {
	return getBooleanKeyValue(NOTIFICATION_WENGOPHONE_NOT_SUCCED_CONNECT_KEY);
}

bool Config::getNotificationAlternativeIMAccount() const {
	return getBooleanKeyValue(NOTIFICATION_ALTERNATIVE_IMACCOUNT_KEY);
}

bool Config::getNotificationContactTurningOnline() const {
	return getBooleanKeyValue(NOTIFICATION_CONTACT_TURNING_ONLINE_KEY);
}

bool Config::getNotificationAuthorization() const {
	return getBooleanKeyValue(NOTIFICATION_AUTHORIZATION_KEY);
}

bool Config::getNotificationErrorMessage() const {
	return getBooleanKeyValue(NOTIFICATION_ERROR_MESSAGE_KEY);
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

bool Config::getPrivacyAlwaysSigneAsInvisible() const {
	return getBooleanKeyValue(PRIVACY_ALLWAYS_SIGN_AS_INVISIBLE_KEY);
}

bool Config::getVideoEnable() const {
	return getBooleanKeyValue(VIDEO_ENABLE_KEY);
}

std::string Config::getVideoWebCamDevice() const {
	return getStringKeyValue(VIDEO_WEBCAM_DEVICE_KEY);
}

std::string Config::getVideoQuality() const {
	return getStringKeyValue(VIDEO_QUALITY_KEY);
}

StringList Config::getAvailableProtocols() const {
	return getStringListKeyValue(AVAILABLE_PROTOCOLS_KEY);
}
