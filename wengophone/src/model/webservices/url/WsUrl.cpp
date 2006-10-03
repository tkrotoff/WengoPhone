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

#include "WsUrl.h"

#include <WengoPhoneBuildId.h>

#include <model/config/ConfigManager.h>
#include <model/config/Config.h>
#include <model/account/wengo/WengoAccount.h>

#include <util/WebBrowser.h>
#include <util/Logger.h>

static const std::string URL_WENGO_ACCOUNTCREATION = "https://www.wengo.fr/public/public.php?page=subscribe_wengos";
static const std::string URL_WENGO_FORUM = "http://www.wengo.fr/public/public.php?page=forum";
static const std::string URL_WENGO_CALLOUT = "http://www.wengo.fr/public/public.php?page=product_callout";
static const std::string URL_WENGO_SMS = "http://www.wengo.fr/public/public.php?page=product_sms";
static const std::string URL_WENGO_VOICEMAIL = "http://www.wengo.fr/public/public.php?page=product_voicemail";
static const std::string URL_WENGO_SEARCH_EXT = "http://www.wengo.fr/public/public.php?page=main_smart_directory_ng";
static const std::string URL_WENGO_SEARCH_INT = "http://www.wengo.fr/public/public.php?page=smart_directory_ng";
static const std::string URL_WENGO_FAQ = "http://www.wengo.fr/public/public.php?page=wiki";
static const std::string URL_WENGO_ACCOUNT = "https://www.wengo.fr/auth/auth.php?page=homepage";
static const std::string URL_WENGO_BUYWENGOS = "https://www.wengo.fr/auth/auth.php?page=reload";
static const std::string URL_WENGO_DOWNLOAD = "http://www.wengo.fr/public/public.php?page=download";
static const std::string URL_WENGO_PHONENUMBER = "https://www.wengo.fr/auth/auth.php?page=display_account";

WengoAccount * WsUrl::_wengoAccount = NULL;

std::string WsUrl::getUrlWithoutAuth(const std::string & url) {
	Config & config = ConfigManager::getInstance().getCurrentConfig();
	std::string language = config.getLanguage();

	//Tune the url for Wengo
	std::string finalUrl = url;
	finalUrl += "&wl=" + std::string(WengoPhoneBuildId::SOFTPHONE_NAME);
	finalUrl += "&lang=" + language;

	LOG_DEBUG("url to open=" + finalUrl);
	return finalUrl;
}

std::string WsUrl::getUrlWithAuth(const std::string & url) {
	std::string finalUrl = url;

	if (_wengoAccount) {
		Config & config = ConfigManager::getInstance().getCurrentConfig();
		std::string language = config.getLanguage();

		//Tune the url for Wengo, with authentication
		finalUrl += "&wl=" + std::string(WengoPhoneBuildId::SOFTPHONE_NAME);
		finalUrl += "&lang=" + language;
		finalUrl += "&login=" + _wengoAccount->getWengoLogin();
		finalUrl += "&password=" + _wengoAccount->getWengoPassword();
		LOG_DEBUG("url to open=" + finalUrl);
	} else {
		LOG_FATAL("no WengoAccount set, this method should not be called");
	}

	return finalUrl;
}

void WsUrl::openWengoUrlWithoutAuth(const std::string & url) {
	WebBrowser::openUrl(getUrlWithoutAuth(url));
}

void WsUrl::openWengoUrlWithAuth(const std::string & url) {
	WebBrowser::openUrl(getUrlWithAuth(url));
}

std::string WsUrl::getWengoDirectoryUrl() {
	return getUrlWithoutAuth(URL_WENGO_SEARCH_INT);
}

void WsUrl::showWengoAccount() {
	openWengoUrlWithAuth(URL_WENGO_ACCOUNT);
}

void WsUrl::showWengoFAQ() {
	openWengoUrlWithoutAuth(URL_WENGO_FAQ);
}

void WsUrl::showWengoForum() {
	openWengoUrlWithoutAuth(URL_WENGO_FORUM);
}

void WsUrl::showWengoSmartDirectory() {
	openWengoUrlWithoutAuth(URL_WENGO_SEARCH_EXT);
}

void WsUrl::showWengoAccountCreation() {
	openWengoUrlWithoutAuth(URL_WENGO_ACCOUNTCREATION);
}

void WsUrl::showWengoCallOut() {
	openWengoUrlWithoutAuth(URL_WENGO_CALLOUT);
}

void WsUrl::showWengoSMS() {
	openWengoUrlWithoutAuth(URL_WENGO_SMS);
}

void WsUrl::showWengoVoiceMail() {
	openWengoUrlWithoutAuth(URL_WENGO_VOICEMAIL);
}

void WsUrl::showWengoBuyWengos() {
	openWengoUrlWithAuth(URL_WENGO_BUYWENGOS);
}

void WsUrl::showWengoDownload() {
	openWengoUrlWithAuth(URL_WENGO_DOWNLOAD);
}

void WsUrl::showWengoPhoneNumber() {
	openWengoUrlWithAuth(URL_WENGO_PHONENUMBER);
}
