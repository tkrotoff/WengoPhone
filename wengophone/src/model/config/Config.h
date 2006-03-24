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

#include <settings/Settings.h>

#include <util/String.h>
#include <util/StringList.h>

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

	/** Personal audio configuration. */
	static const std::string AUDIO_PERSONAL_CONFIGURATION_KEY;
	bool getAudioPersonalConfiguration() const;

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

	/**
	 * @name Profile Methods
	 * @{
	 */

	/** Profile nickname. */
	static const std::string PROFILE_NICKNAME;
	std::string getProfileNickName() const;

	/** Profile firstname. */
	static const std::string PROFILE_FIRSTNAME;
	std::string getProfileFirstName() const;

	/** Profile lastname. */
	static const std::string PROFILE_LASTNAME;
	std::string getProfileLastName() const;

	/** Profile birth date. */
	static const std::string PROFILE_BIRTHDATE;
	std::string getProfileBirthDate() const;

	/** Profile city. */
	static const std::string PROFILE_CITY;
	std::string getProfileCity() const;

	/** Profile gender. */
	static const std::string PROFILE_GENDER;
	std::string getProfileGender() const;

	/** Profile country. */
	static const std::string PROFILE_COUNTRY;
	std::string getProfileCountry() const;

	/** Profile state. */
	static const std::string PROFILE_STATE;
	std::string getProfileState() const;

	/** Profile IM picture 1. */
	static const std::string PROFILE_IMPIC1;
	std::string getProfileIMPic1() const;

	/** Profile IM picture 2. */
	static const std::string PROFILE_IMPIC2;
	std::string getProfileIMPic2() const;

	/** Profile IM picture 3. */
	static const std::string PROFILE_IMPIC3;
	std::string getProfileIMPic3() const;

	/** Profile IM picture 4. */
	static const std::string PROFILE_IMPIC4;
	std::string getProfileIMPic4() const;

	/** Profile IM picture 5. */
	static const std::string PROFILE_IMPIC5;
	std::string getProfileIMPic5() const;

	/** Profile IM picture 6. */
	static const std::string PROFILE_IMPIC6;
	std::string getProfileIMPic6() const;

	/** Profile IM picture 7. */
	static const std::string PROFILE_IMPIC7;
	std::string getProfileIMPic7() const;

	/** Profile IM picture 8. */
	static const std::string PROFILE_IMPIC8;
	std::string getProfileIMPic8() const;

	/** Profile IM picture 9. */
	static const std::string PROFILE_IMPIC9;
	std::string getProfileIMPic9() const;

	/** Profile email 1. */
	static const std::string PROFILE_IMEMAIL1;
	std::string getProfileIMEmail1() const;

	/** Profile email 2. */
	static const std::string PROFILE_IMEMAIL2;
	std::string getProfileIMEmail2() const;

	/** Profile email 3. */
	static const std::string PROFILE_IMEMAIL3;
	std::string getProfileIMEmail3() const;

	/** Profile email 4. */
	static const std::string PROFILE_IMEMAIL4;
	std::string getProfileIMEmail4() const;

	/** Profile email 5. */
	static const std::string PROFILE_IMEMAIL5;
	std::string getProfileIMEmail5() const;

	/** Profile email 6. */
	static const std::string PROFILE_IMEMAIL6;
	std::string getProfileIMEmail6() const;

	/** Profile email 7. */
	static const std::string PROFILE_IMEMAIL7;
	std::string getProfileIMEmail7() const;

	/** Profile email 8. */
	static const std::string PROFILE_IMEMAIL8;
	std::string getProfileIMEmail8() const;

	/** Profile email 9. */
	static const std::string PROFILE_IMEMAIL9;
	std::string getProfileIMEmail9() const;

	/** Profile cell phone. */
	static const std::string PROFILE_CELLPHONE;
	std::string getProfileCellPhone() const;

	/** Profile wengo phone. */
	static const std::string PROFILE_WENGOPHONE;
	std::string getProfileWengoPhone() const;

	/** Profile home phone. */
	static const std::string PROFILE_HOMEPHONE;
	std::string getProfileHomePhone() const;

	/** Profile work phone. */
	static const std::string PROFILE_WORKPHONE;
	std::string getProfileWorkPhone() const;

	/** Profile email. */
	static const std::string PROFILE_EMAIL;
	std::string getProfileEmail() const;

	/** Profile blog url. */
	static const std::string PROFILE_BLOG;
	std::string getProfileBlog() const;

	/** Profile web url. */
	static const std::string PROFILE_WEB;
	std::string getProfileWeb() const;

	/** Profile avatar file path. */
	static const std::string PROFILE_AVATAR;
	std::string getProfileAvatar() const;

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
	 * @name Call forwarding Methods
	 * @{
	 */

	/** Forward all undelivred call to vm */
	static const std::string CALL_FORWARD_ALL_UNDELIVREDTOVM_KEY;
	bool getCallForwardAllUndelivredToVm() const;

	/** Forward all undelivred call to : */
	static const std::string CALL_FORWARD_ALL_UNDELIVREDTO_KEY;
	bool getCallForwardAllUndelivredTo() const;

	/** Forward call phone number 1. */
	static const std::string CALL_FORWARD_PHONENUMBER1_KEY;
	std::string getCallForwardPhoneNumber1() const;

	/** Forward call phone number 2. */
	static const std::string CALL_FORWARD_PHONENUMBER2_KEY;
	std::string getCallForwardPhoneNumber2() const;

	/** Forward call phone number 3. */
	static const std::string CALL_FORWARD_PHONENUMBER3_KEY;
	std::string getCallForwardPhoneNumber3() const;

	/** Forward call to mobile when status is set to mobile. */
	static const std::string CALL_FORWARD_TOMOBILE_KEY;
	bool getCallForwardToMobile() const;

	/**
	 * @}
	 */

	/**
	 * @name General settings Methods
	 * @{
	 */

	/** Automatically start Wengo. */
	static const std::string GENERAL_SETTINGS_AUTOSTART_WENGO_KEY;
	bool getGeneralSettingsAutoStartWengo() const;

	/** Check for updates automatically. */
	static const std::string GENERAL_SETTINGS_CHECK_UPDATE_KEY;
	bool getGeneralSettingsCheckUpdate() const;

	/** Start a free call if applicable */
	static const std::string GENERAL_SETTINGS_START_FREECALL_KEY;
	bool getGeneralSettingsStartFreeCall() const;

	/** Start a chat-only when applicable */
	static const std::string GENERAL_SETTINGS_START_CHATONLY_KEY;
	bool getGeneralSettingsStartChatOnly() const;

	/** Call cell phone or land line if contact not available. */
	static const std::string GENERAL_SETTINGS_CALL_CELLPHONE_KEY;
	bool getGeneralSettingsCallCellPhone() const;

	/** Show me as away when I am inactive for. */
	static const std::string GENERAL_SETTINGS_SHOW_AWAY_KEY;
	bool getGeneralSettingsShowAway() const;

	/** Show me as not available when I am inactive for. */
	static const std::string GENERAL_SETTINGS_SHOW_NOTAVAILABLE_KEY;
	bool getGeneralSettingsShowNotAvailable() const;


	static const std::string GENERAL_SETTINGS_AWAY_TIMER_KEY;
	int getGeneralSettingsGetAwayTimer() const;


	static const std::string GENERAL_SETTINGS_NOTAVAILABLE_TIMER_KEY;
	int getGeneralSettingsNotAvailableTimer() const;

	/**
	 * @}
	 */

	/**
	 * @name Notification settings Methods
	 * @{
	 */

	/** Show related window on top of other applications for incoming calls & incoming chats */
	static const std::string NOTIFICATION_SHOW_WINDOWONTOP_KEY;
	bool getNotificationShowWindowOnTop() const;

	/** Show minimized blinking window in taskbar for incoming calls & incoming chats */
	static const std::string NOTIFICATION_SHOW_BLINKINGWINDOW_KEY;
	bool getNotificationShowBlinkingWindow() const;

	/** Show toaster only for incomin calls & incoming chats */
	static const std::string NOTIFICATION_SHOW_TOASTER_ON_INCOMING_CALL_KEY;
	bool getNotificationShowToasterOnIncomingCall() const;

	/** Show toaster when contact is turning online */
	static const std::string NOTIFICATION_SHOW_TOASTER_ON_CONTACT_ONLINE_KEY;
	bool getNotificationShowToasterOnContactOnline() const;

	/** Show window pop-up for authorizations */
	static const std::string NOTIFICATION_SHOW_WINDOWPOPUP_AUTHORIZATION_KEY;
	bool getNotificationShowWindowPopupAuthorization() const;

	/** Do not show any toaster or window */
	static const std::string NOTIFICATION_SHOW_NO_TOASTER_KEY;
	bool getNotificationShowNoToaster() const;

	/** Switch off audio notifications */
	static const std::string NOTIFICATION_SWITCH_OFF_AUDIO_NOFITICATION_KEY;
	bool getNotificationSwithOffAudioNotification() const;

	/** Do not show any call toaster or chat window */
	static const std::string NOTIFICATION_NOT_SHOW_AWAY_TOASTER_AND_CHAT_KEY;
	bool getNotificationNotShowAwayToasterAndChat() const;

	/** Switch off audio notifications */
	static const std::string NOTIFICATION_SWITCH_OFF_AWAY_AUDIO_KEY;
	bool getNotificationSwitchOffAwayAudio() const;

	/** Incoming chat */
	static const std::string NOTIFICATION_INCOMING_CHAT_KEY;
	bool getNotificationIncomingChat() const;

	/** Incoming calls */
	static const std::string NOTIFICATION_INCOMING_CALLS_KEY;
	bool getNotificationIncomingCalls() const;

	/** Wengophone opens */
	static const std::string NOTIFICATION_WENGOPHONE_OPENS_KEY;
	bool getNotificationWengophoneOpens() const;

	/** Wengophone connects */
	static const std::string NOTIFICATION_WENGOPHONE_CONNECTS_KEY;
	bool getNotificationWengophoneConnects() const;

	/** Wengophone doesn't succeed to connect */
	static const std::string NOTIFICATION_WENGOPHONE_NOT_SUCCED_CONNECT_KEY;
	bool getNotificationWengophoneNotSuccedConnect() const;

	/** Alternative IM account connects */
	static const std::string NOTIFICATION_ALTERNATIVE_IMACCOUNT_KEY;
	bool getNotificationAlternativeIMAccount() const;

	/** Contact is turning online */
	static const std::string NOTIFICATION_CONTACT_TURNING_ONLINE_KEY;
	bool getNotificationContactTurningOnline() const;

	/** Authorization (contact, file transfer...) */
	static const std::string NOTIFICATION_AUTHORIZATION_KEY;
	bool getNotificationAuthorization() const;

	/** Error message */
	static const std::string NOTIFICATION_ERROR_MESSAGE_KEY;
	bool getNotificationErrorMessage() const;

	/**
	 * @}
	 */

	/**
	 * @name PrivacySettings Methods
	 * @{
	 */


	/** Allow calls from : anyone */
	static const std::string PRIVACY_ALLOW_CALL_FROM_ANYONE_KEY;
	bool getPrivacyAllowCallFromAnyone() const;

	/** Allow calls from : only people from my contact list */
	static const std::string PRIVACY_ALLOW_CALL_ONLY_FROM_CONTACT_LIST_KEY;
	bool getPrivacyAllowCallOnlyFromContactList() const;

	/** Allow chats from : anyone */
	static const std::string PRIVACY_ALLOW_CHAT_FROM_ANYONE_KEY;
	bool getPrivacyAllowChatsFromAnyone() const;

	/** Allow chats from : only people from my contact list */
	static const std::string PRIVACY_ALLOW_CHAT_ONLY_FROM_CONTACT_LIST_KEY;
	bool getPrivacyAllowChatOnlyFromContactList() const;

	/** When I sign into Wengo : Always sign in as invisible */
	static const std::string PRIVACY_ALLWAYS_SIGN_AS_INVISIBLE_KEY;
	bool getPrivacyAlwaysSigneAsInvisible() const;

	/**
	 * @}
	 */

	/**
	 * @name PrivacySettings Methods
	 * @{
	 */

	static const std::string VIDEO_ENABLE_KEY;
	bool getVideoEnable() const;

	static const std::string VIDEO_WEBCAM_DEVICE_KEY;
	std::string getVideoWebCamDevice() const;

	static const std::string VIDEO_QUALITY_KEY;
	std::string getVideoQuality() const;

	/**
	 * @}
	 */

	/**
	 * @name Path Methods
	 * @{
	 */

	/** Location of configuration files on the hard disk. */
	static const std::string CONFIG_DIR_KEY;
	std::string getConfigDir() const;

	/** Resources directory. */
	static const std::string RESOURCES_DIR_KEY;
	std::string getResourcesDir() const;

	/**
	 * @}
	 */

	/**
	 * @name Protocols Methods
	 * @{
	 */

	static const std::string AVAILABLE_PROTOCOLS_KEY;
	StringList getAvailableProtocols() const;

	/**
	 * @}
	 */

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
	std::string get(const std::string &, const std::string &) const {
		return String::null;
	}

	/**
	 * @see get()
	 */
	StringList get(const std::string &, const StringList &) const {
		static const StringList empty;
		return empty;
	}

	/**
	 * @see get()
	 */
	bool get(const std::string &, bool) const {
		return false;
	}

	/**
	 * @see get()
	 */
	int get(const std::string &, int) const {
		return 0;
	}

	/**
	 * @see get()
	 */
	boost::any getAny(const std::string &, const boost::any &) const {
		static const boost::any empty;
		return empty;
	}

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

	/**
	 * @see get()
	 */
	StringList Config::getStringListKeyValue(const std::string & key) const;

	/** Name of this configuration. */
	std::string _name;

	/** Associates a key to a default value. */
	Keys _keyDefaultValueMap;
};

#endif	//CONFIG_H
