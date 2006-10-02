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

#ifndef OWCONFIG_H
#define OWCONFIG_H

#include <settings/AutomaticSettings.h>

#include <util/String.h>
#include <util/StringList.h>

/**
 * Stores the configuration options of WengoPhone.
 *
 * Tries to make it impossible to make a mistake.
 *
 * @see Settings
 * @ingroup model
 * @author Tanguy Krotoff
 * @author Philippe Bernery
 */
class Config : public AutomaticSettings {
public:

	/**
	 * Last configuration version.
	 *
	 * Used to import config from previous version of Wengophone
	 * @see ConfigImporter for more information
	 *
	 * version 1: WengoPhone classic
	 * version 2: WengoPhone NG Beta1 to Beta2
	 *  - New save directory
	 *  - New save directory structure
	 * version 3: WengoPhone NG Beta3:
	 *  - multi-profile management
	 * version 4: WengoPhone NG Final:
	 *  - added a new type of protocol: Wengo
	 */
	static const int CONFIG_VERSION;

	static const std::string CONFIG_VERSION_KEY;
	int getConfigVersion() const;

	Config(const std::string & name);

	~Config();

	/**
	 * Gets the name associated with the Config.
	 *
	 * @return Config name
	 */
	std::string getName() const;

	/** Executable name (qtwengophone.exe). */
	static const std::string EXECUTABLE_NAME_KEY;
	std::string getExecutableName() const;

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
	static const std::string AUDIO_OUTPUT_DEVICEID_KEY;
	StringList getAudioOutputDeviceId() const;

	/** Record audio device name. */
	static const std::string AUDIO_INPUT_DEVICEID_KEY;
	StringList getAudioInputDeviceId() const;

	/** Ringer audio device name. */
	static const std::string AUDIO_RINGER_DEVICEID_KEY;
	StringList getAudioRingerDeviceId() const;

	/** Ringing/incoming call/ringtone audio file name. */
	static const std::string AUDIO_INCOMINGCALL_FILE_KEY;
	std::string getAudioIncomingCallFile() const;

	/** Call closed audio file name. */
	static const std::string AUDIO_CALLCLOSED_FILE_KEY;
	std::string getAudioCallClosedFile() const;

	/** Incoming chat audio file name. */
	static const std::string AUDIO_INCOMINGCHAT_FILE_KEY;
	std::string getAudioIncomingChatFile() const;

	/** IM account connected audio file name. */
	static const std::string AUDIO_IMACCOUNTCONNECTED_FILE_KEY;
	std::string getAudioIMAccountConnectedFile() const;

	/** IM account disconnected audio file name. */
	static const std::string AUDIO_IMACCOUNTDISCONNECTED_FILE_KEY;
	std::string getAudioIMAccountDisconnectedFile() const;

	/** Contact online audio file name. */
	static const std::string AUDIO_CONTACTONLINE_FILE_KEY;
	std::string getAudioContactOnlineFile() const;

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

	/** Wengo info web service path on the server. */
	static const std::string WENGO_INFO_PATH_KEY;
	std::string getWengoInfoPath() const;

	/** Wengo subscription web service path on the server. */
	static const std::string WENGO_SUBSCRIBE_PATH_KEY;
	std::string getWengoSubscribePath() const;

	/** WengoPhone update path on the Wengo server. */
	static const std::string WENGO_SOFTUPDATE_PATH_KEY;
	std::string getWengoSoftUpdatePath() const;

	/**
	 * Wengo cirpack web service path on the server.
	 * FIXME no cirpack information please!
	 */
	static const std::string WENGO_CIRPACK_PATH_KEY;
	std::string getWengoCirpackPath() const;

	/** Wengo directory web service path on the server. */
	static const std::string WENGO_DIRECTORY_PATH_KEY;
	std::string getWengoDirectoryPath() const;

	/** @} */

	/**
	 * @name Profile Methods
	 * @{
	 */

	/** Gets the name of the last used UserProfile. */
	static const std::string PROFILE_LAST_USED_NAME_KEY;
	std::string getProfileLastUsedName() const;

	/** Profile all widget enabled. */
	static const std::string PROFILE_ALL;
	bool getProfileAll() const;

	/** Profile window width. */
	static const std::string PROFILE_WIDTH;
	int getProfileWidth() const;

	/** Profile window height. */
	static const std::string PROFILE_HEIGHT;
	int getProfileHeight() const;

	/** Profile window position X. */
	static const std::string PROFILE_POSX;
	int getProfilePosX() const;

	/** Profile window position Y. */
	static const std::string PROFILE_POSY;
	int getProfilePoxY() const;

	/** @} */

	/**
	 * @name Call Forwarding Methods
	 * @{
	 */

	/** Call forward mode. */
	static const std::string CALL_FORWARD_MODE_KEY;
	std::string getCallForwardMode() const;

	/** Forward call phone number 1. */
	static const std::string CALL_FORWARD_PHONENUMBER1_KEY;
	std::string getCallForwardPhoneNumber1() const;

	/** Forward call phone number 2. */
	static const std::string CALL_FORWARD_PHONENUMBER2_KEY;
	std::string getCallForwardPhoneNumber2() const;

	/** Forward call phone number 3. */
	static const std::string CALL_FORWARD_PHONENUMBER3_KEY;
	std::string getCallForwardPhoneNumber3() const;

	/** Active voice mail. */
	static const std::string VOICE_MAIL_ACTIVE_KEY;
	bool getVoiceMailActive() const;

	/** @} */

	/**
	 * @name General Settings Methods
	 * @{
	 */

	/** Automatically start WengoPhone when computer starts. */
	static const std::string GENERAL_AUTOSTART_KEY;
	bool getGeneralAutoStart() const;

	/** Start a free call if applicable. */
	static const std::string GENERAL_CLICK_START_FREECALL_KEY;
	bool getGeneralClickStartFreeCall() const;

	/** Start a chat-only when applicable. */
	static const std::string GENERAL_CLICK_START_CHAT_KEY;
	bool getGeneralClickStartChat() const;

	/** Call cell phone or land line if contact not available. */
	static const std::string GENERAL_CLICK_CALL_CELLPHONE_KEY;
	bool getGeneralClickCallCellPhone() const;

	/** Show me as away when I am inactive for. */
	static const std::string GENERAL_AWAY_TIMER_KEY;
	static const int NO_AWAY_TIMER = -1;
	int getGeneralAwayTimer() const;

	/** Show / Hide groups in the contacts list. */
	static const std::string GENERAL_SHOW_GROUPS_KEY;
	bool getShowGroups() const;

	/** Show / Hide offline contacts in the contacts list. */
	static const std::string GENERAL_SHOW_OFFLINE_CONTACTS_KEY;
	bool getShowOfflineContacts() const;

	/** @} */

	/** ISO 639 code of the current language for translating WengoPhone. */
	static const std::string LANGUAGE_KEY;
	static const std::string LANGUAGE_AUTODETECT_KEYVALUE;
	std::string getLanguage() const;

	/**
	 * @name Notification Settings Methods
	 * @{
	 */

	/** Shows toaster for incoming calls. */
	static const std::string NOTIFICATION_SHOW_TOASTER_ON_INCOMING_CALL_KEY;
	bool getNotificationShowToasterOnIncomingCall() const;

	/** Shows toaster for incoming chats. */
	static const std::string NOTIFICATION_SHOW_TOASTER_ON_INCOMING_CHAT_KEY;
	bool getNotificationShowToasterOnIncomingChat() const;

	/** Shows toaster when a contact is turning online. */
	static const std::string NOTIFICATION_SHOW_TOASTER_ON_CONTACT_ONLINE_KEY;
	bool getNotificationShowToasterOnContactOnline() const;

	/** Do not show any toaster or window when in do not disturb mode. */
	static const std::string NOTIFICATION_DONOTDISTURB_NO_WINDOW_KEY;
	bool getNotificationDoNotDisturbNoWindow() const;

	/** Switch off audio notifications when in do not disturb mode mode. */
	static const std::string NOTIFICATION_DONOTDISTURB_NO_AUDIO_KEY;
	bool getNotificationDoNotDisturbNoAudio() const;

	/** Do not show any call toaster when in away mode. */
	static const std::string NOTIFICATION_AWAY_NO_WINDOW_KEY;
	bool getNotificationAwayNoWindow() const;

	/** Switch off audio notifications when in away mode. */
	static const std::string NOTIFICATION_AWAY_NO_AUDIO_KEY;
	bool getNotificationAwayNoAudio() const;

	/** @} */

	/**
	 * @name Privacy Methods
	 * @{
	 */

	/** Allow calls from: anyone. */
	static const std::string PRIVACY_ALLOW_CALL_FROM_ANYONE_KEY;
	bool getPrivacyAllowCallFromAnyone() const;

	/** Allow calls from: only people from my contact list. */
	static const std::string PRIVACY_ALLOW_CALL_ONLY_FROM_CONTACT_LIST_KEY;
	bool getPrivacyAllowCallOnlyFromContactList() const;

	/** Allow chats from: anyone. */
	static const std::string PRIVACY_ALLOW_CHAT_FROM_ANYONE_KEY;
	bool getPrivacyAllowChatsFromAnyone() const;

	/** Allow chats from: only people from my contact list. */
	static const std::string PRIVACY_ALLOW_CHAT_ONLY_FROM_CONTACT_LIST_KEY;
	bool getPrivacyAllowChatOnlyFromContactList() const;

	/** When I sign into Wengo: always sign in as invisible. */
	static const std::string PRIVACY_SIGN_AS_INVISIBLE_KEY;
	bool getPrivacySignAsInvisible() const;

	/** @} */

	/**
	 * @name Video Methods
	 * @{
	 */

	static const std::string VIDEO_ENABLE_KEY;
	bool getVideoEnable() const;

	static const std::string VIDEO_WEBCAM_DEVICE_KEY;
	std::string getVideoWebcamDevice() const;

	static const std::string VIDEO_QUALITY_KEY;
	int getVideoQuality() const;

	static const std::string VIDEO_ENABLE_XVIDEO;
	bool getXVideoEnable() const;

	static const std::string VIDEO_ENABLE_FLIP;
	bool getVideoFlipEnable() const;

	/** @} */

	/**
	 * @name Path Methods
	 * @{
	 */

	/** Location of configuration files on the hard disk. */
	static const std::string CONFIG_DIR_KEY;
	std::string getConfigDir() const;

	/** Last location where a chat history has benn saved */
	static const std::string LAST_CHAT_HISTORY_SAVE_DIR_KEY;
	std::string getLastChatHistorySaveDir() const;

	/**
	 * Resources directory.
	 *
	 * TODO move somewhere else since this is not dynamic and should not be changed.
	 */
	static const std::string RESOURCES_DIR_KEY;
	std::string getResourcesDir() const;

	/** @} */

	/**
	 * @name Wenbox Methods
	 * @{
	 */

	/** Wenbox support enable or not. */
	static const std::string WENBOX_ENABLE_KEY;
	bool getWenboxEnable() const;

	/** @} */

	/**
	 * @name Other Methods
	 * @{
	 */

	/** Enables Internet Explorer ActiveX support (relevant only under Windows). */
	static const std::string IEACTIVEX_ENABLE_KEY;
	bool getIEActiveXEnable() const;

	/**
	 * Returns the authorized server that can pass commands to the softphone.
	 * @see CommandServer
	 */
	static const std::string CMDSERVER_AUTHORIZED_KEY;
	std::string getCmdServerAuthorized() const;

	/** @} */

	/**
	 * @name Command Line Options
	 * @{
	 */

	/** Run the GUI in background mode. */
	static const std::string CMDLINE_BACKGROUND_MODE_ENABLE_KEY;
	bool getCmdLineBackgroundModeEnable() const;

	/** Place a call. */
	static const std::string CMDLINE_PLACECALL_KEY;
	std::string getCmdLinePlaceCall() const;

	/** @} */

	/**
	 * @name File Transfer Options
	 * @{
	 */

	/** download folder. */
	static const std::string FILETRANSFER_DOWNLOAD_FOLDER;
	std::string getFileTransferDownloadFolder() const;

	/** Path to the last uploaded file. */
	static const std::string FILETRANSFER_LASTUPLOADEDFILE_FOLDER;
	std::string getLastUploadedFileFolder() const;

	/** @} */

	/**
	 * @name Linux Options
	 * @{
	 */

	/** Prefered browser. */
	static const std::string LINUX_PREFERED_BROWSER;
	std::string getLinuxPreferedBrowser() const;

	/** @} */

private:

	/** Name of this configuration. */
	std::string _name;
};

#endif	//OWCONFIG_H
