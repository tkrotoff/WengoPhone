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

#include "CWengoPhone.h"

#include <model/wenbox/WenboxPlugin.h>
#include <model/account/wengo/WengoAccount.h>
#include <model/contactlist/Contact.h>
#include <model/connect/ConnectHandler.h>
#include <model/profile/UserProfile.h>
#include <model/phoneline/IPhoneLine.h>
#include <presentation/PFactory.h>
#include <presentation/PWengoPhone.h>
#include "phoneline/CPhoneLine.h"
#include "contactlist/CContactList.h"
#include "wenbox/CWenboxPlugin.h"
#include "WengoPhoneBuildId.h"
#include "connect/CConnectHandler.h"
#include <control/chat/CChatHandler.h>
#include <control/presence/CPresenceHandler.h>
#include <model/sms/Sms.h>
#include <control/sms/CSms.h>

#include <util/WebBrowser.h>
#include <util/StringList.h>
#include <util/Logger.h>

using namespace std;

const std::string CWengoPhone::URL_WENGO_ACCOUNTCREATION = "https://www.wengo.fr/public/public.php?page=subscribe_wengos";
const std::string CWengoPhone::URL_WENGO_FORUM = "http://www.wengo.fr/public/public.php?page=forum";
const std::string CWengoPhone::URL_WENGO_CALLOUT = "http://www.wengo.fr/public/public.php?page=product_callout";
const std::string CWengoPhone::URL_WENGO_SMS = "http://www.wengo.fr/public/public.php?page=product_sms";
const std::string CWengoPhone::URL_WENGO_VOICEMAIL = "http://www.wengo.fr/public/public.php?page=product_voicemail";
const std::string CWengoPhone::URL_WENGO_SEARCH_EXT = "http://www.wengo.fr/public/public.php?page=main_smart_directory";
const std::string CWengoPhone::URL_WENGO_SEARCH_INT = "http://www.wengo.fr/public/public.php?page=smart_directory";
const std::string CWengoPhone::URL_WENGO_FAQ = "http://www.wengo.fr/public/public.php?page=helpcenter";
const std::string CWengoPhone::URL_WENGO_ACCOUNT = "https://www.wengo.fr/auth/auth.php?page=homepage";
const std::string CWengoPhone::URL_WENGO_BUYWENGOS = "https://www.wengo.fr/auth/auth.php?page=reload";

CWengoPhone::CWengoPhone(WengoPhone & wengoPhone)
	: _wengoPhone(wengoPhone) {

	_pWengoPhone = PFactory::getFactory().createPresentationWengoPhone(*this);

	_wengoPhone.wenboxPluginCreatedEvent += boost::bind(&CWengoPhone::wenboxPluginCreatedEventHandler, this, _1, _2);
	_wengoPhone.initFinishedEvent += boost::bind(&CWengoPhone::initFinishedEventHandler, this, _1);
	_wengoPhone.getCurrentUserProfile().phoneLineCreatedEvent += boost::bind(&CWengoPhone::phoneLineCreatedEventHandler, this, _1, _2);
	_wengoPhone.getCurrentUserProfile().loginStateChangedEvent += loginStateChangedEvent;
	_wengoPhone.getCurrentUserProfile().networkDiscoveryStateChangedEvent += networkDiscoveryStateChangedEvent;
	_wengoPhone.getCurrentUserProfile().noAccountAvailableEvent += noAccountAvailableEvent;
	_wengoPhone.getCurrentUserProfile().smsCreatedEvent += boost::bind(&CWengoPhone::smsCreatedEventHandler, this, _1, _2);
	_wengoPhone.getCurrentUserProfile().proxyNeedsAuthenticationEvent += proxyNeedsAuthenticationEvent;
	_wengoPhone.getCurrentUserProfile().wrongProxyAuthenticationEvent += wrongProxyAuthenticationEvent;
	_wengoPhone.getCurrentUserProfile().newIMAccountAddedEvent += boost::bind(&CWengoPhone::newIMAccountAddedEventHandler, this, _1, _2);
}

void CWengoPhone::makeCall(const std::string & phoneNumber) {
	_wengoPhone.getCurrentUserProfile().makeCall(phoneNumber);
}

void CWengoPhone::addWengoAccount(const std::string & login, const std::string & password, bool autoLogin) {
	_wengoPhone.getCurrentUserProfile().addSipAccount(login, password, autoLogin);
}

void CWengoPhone::showWengoAccount() {
	openWengoUrlWithAuth(URL_WENGO_ACCOUNT);
}

void CWengoPhone::start() {
	_cContactList = new CContactList(_wengoPhone.getCurrentUserProfile().getContactList(), *this);
	LOG_DEBUG("CContactList created");

	_wengoPhone.start();
}

void CWengoPhone::terminate() {
	_wengoPhone.terminate();
}

void CWengoPhone::phoneLineCreatedEventHandler(UserProfile & sender, IPhoneLine & phoneLine) {
	CPhoneLine * cPhoneLine = new CPhoneLine(phoneLine, *this);

	LOG_DEBUG("CPhoneLine created");
}

void CWengoPhone::wenboxPluginCreatedEventHandler(WengoPhone & sender, WenboxPlugin & wenboxPlugin) {
	static CWenboxPlugin cWenboxPlugin(wenboxPlugin, *this);

	LOG_DEBUG("CWenboxPlugin created");
}

void CWengoPhone::initFinishedEventHandler(WengoPhone & sender) {
	static CConnectHandler cConnectHandler(sender.getCurrentUserProfile().getConnectHandler()); LOG_DEBUG("CConnectHandler created");
	static CPresenceHandler cPresenceHandler(sender.getCurrentUserProfile().getPresenceHandler()); LOG_DEBUG("CPresenceHandler created");
	static CChatHandler cChatHandler(sender.getCurrentUserProfile().getChatHandler()); LOG_DEBUG("CChatHandler created");
	LOG_DEBUG("WengoPhone::init() finished");
}

void CWengoPhone::smsCreatedEventHandler(UserProfile & sender, Sms & sms) {
	static CSms cSms(sms, *this);

	LOG_DEBUG("CSms created");
}

PhoneCall * CWengoPhone::getActivePhoneCall() const {
	IPhoneLine * phoneLine = _wengoPhone.getCurrentUserProfile().getActivePhoneLine();
	if (phoneLine) {
		return phoneLine->getActivePhoneCall();
	}
	return NULL;
}

void CWengoPhone::addIMAccount(const std::string & login, const std::string & password, EnumIMProtocol::IMProtocol protocol) {
	_wengoPhone.getCurrentUserProfile().addIMAccount(IMAccount(login, password, protocol));
}

void CWengoPhone::newIMAccountAddedEventHandler(UserProfile & sender, IMAccount & imAccount) {
	//_wengoPhone.getCurrentUserProfile().getConnectHandler().connect(imAccount);
}

void CWengoPhone::openWengoUrlWithoutAuth(std::string url) {
	//TODO: retrieve the language from the configuration
	static const std::string langCode = "fra";

	//tune the url for Wengo
	std::string finalUrl = url;
	finalUrl += "&wl=" + string(WengoPhoneBuildId::SOFTPHONE_NAME);
	finalUrl += "&lang=" + langCode;

	WebBrowser::openUrl(finalUrl);
	LOG_DEBUG("url opened: " + finalUrl);
}

void CWengoPhone::openWengoUrlWithAuth(std::string url) {
	static const std::string langCode = "fra";

	IPhoneLine * activePhoneLine = _wengoPhone.getCurrentUserProfile().getActivePhoneLine();
	if (activePhoneLine) {
		const SipAccount & account = activePhoneLine->getSipAccount();
		try {
			const WengoAccount & wengoAccount = dynamic_cast<const WengoAccount &>(account);

			//tune the url for Wengo, with authentication
			std::string finalUrl = url;
			finalUrl += "&wl=" + string(WengoPhoneBuildId::SOFTPHONE_NAME);
			finalUrl += "&lang=" + langCode;
			finalUrl += "&login=" + wengoAccount.getWengoLogin();
			finalUrl += "&password=" + wengoAccount.getWengoPassword();

			WebBrowser::openUrl(finalUrl);
			LOG_DEBUG("url opened: " + finalUrl);

		} catch (bad_cast) {
			LOG_DEBUG("Bad cast: from \"const SipAccount &\" to \"const WengoAccount &\"");
		}
	}
}

void CWengoPhone::showWengoFAQ() {
	openWengoUrlWithoutAuth(URL_WENGO_FAQ);
}

void CWengoPhone::showWengoForum() {
	openWengoUrlWithoutAuth(URL_WENGO_FORUM);
}

void CWengoPhone::showWengoSmartDirectory() {
	openWengoUrlWithoutAuth(URL_WENGO_SEARCH_EXT);
}

void CWengoPhone::showWengoAccountCreation() {
	openWengoUrlWithoutAuth(URL_WENGO_ACCOUNTCREATION);
}

void CWengoPhone::showWengoCallOut() {
	openWengoUrlWithoutAuth(URL_WENGO_CALLOUT);
}

void CWengoPhone::showWengoSMS() {
	openWengoUrlWithoutAuth(URL_WENGO_SMS);
}

void CWengoPhone::showWengoVoiceMail() {
	openWengoUrlWithoutAuth(URL_WENGO_VOICEMAIL);
}

void CWengoPhone::showWengoBuyWengos() {
	openWengoUrlWithAuth(URL_WENGO_BUYWENGOS);
}
