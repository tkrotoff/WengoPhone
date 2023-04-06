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
 * You should have receiveda copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "PurpleParamSetter.h"

#include <coipmanager_base/account/Account.h>
#include <coipmanager_base/account/GTalkAccount.h>
#include <coipmanager_base/account/JabberAccount.h>
#include <coipmanager_base/account/MSNAccount.h>
#include <coipmanager_base/account/OscarAccount.h>
#include <coipmanager_base/account/YahooAccount.h>

#include <purplewrapper/PurpleEnumIMProtocol.h>

#include <networkdiscovery/NetworkProxyDiscovery.h>

#include <util/Logger.h>

extern "C" {
#include <libpurple/connection.h>
#include <libpurple/core.h>
#include <libpurple/proxy.h>
}

static const char * PURPLE_MAIL_NOTIFICATION_KEY = "check-mail";

static const char * PURPLE_JABBER_SERVER_KEY = "server";
static const char * PURPLE_JABBER_PORT_KEY = "port";
static const char * PURPLE_JABBER_RESOURCE_KEY = "resource";
static const char * PURPLE_JABBER_USE_TLS_KEY = "use_tls";
static const char * PURPLE_JABBER_REQUIRE_TLS_KEY = "require_tls";
static const char * PURPLE_JABBER_USE_OLD_SSL_KEY = "old_ssl";
static const char * PURPLE_JABBER_AUTH_PLAIN_IN_CLEAR_KEY = "auth_plain_in_clear";
static const char * PURPLE_JABBER_CONNECTION_SERVER_KEY = "connect_server";
static const int PURPLE_JABBER_SERVER_PORT = 5222;

static const int PURPLE_GTALK_ALTERNATE_SERVER_PORT = 80;

static const char * PURPLE_MSN_SERVER_KEY = "server";
static const char * PURPLE_MSN_PORT_KEY = "port";
static const char * PURPLE_MSN_USE_HTTP_KEY = "http_method";
static const char * PURPLE_MSN_HTTP_SERVER_KEY = "http_method_server";

static const char * PURPLE_OSCAR_SERVER_KEY = "server";
static const char * PURPLE_OSCAR_PORT_KEY = "port";
static const char * PURPLE_OSCAR_ENCODING_KEY = "encoding";
static const int PURPLE_OSCAR_SERVER_PORT = 5190;
static const int PURPLE_OSCAR_ALTERNATE_SERVER_PORT = 443;

static const char * PURPLE_YAHOO_IS_JAPAN_KEY = "yahoojp";
static const char * PURPLE_YAHOO_SERVER_KEY = "server";
static const char * PURPLE_YAHOO_JAPAN_SERVER_KEY = "serverjp";
static const char * PURPLE_YAHOO_PORT_KEY = "port";
static const char * PURPLE_YAHOO_XFER_HOST_KEY = "xfer_host";
static const char * PURPLE_YAHOO_JAPAN_XFER_HOST_KEY = "xferjp_host";
static const char * PURPLE_YAHOO_XFER_PORT_KEY = "xfer_port";
static const char * PURPLE_YAHOO_ROOM_LIST_LOCALE_KEY = "room_listlocale";
static const int PURPLE_YAHOO_SERVER_PORT = 5050;
static const int PURPLE_YAHOO_ALTERNATE_SERVER_PORT = 23;

void PurpleParamSetter::setAccountParams(const NetworkProxy &networkProxy, Account & account, PurpleAccount * pAccount, bool alternateConnectionWay) {
	if (pAccount) {
		// Setting proxy
		PurpleProxyInfo * proxyInfo = purple_account_get_proxy_info(pAccount);
		if (!proxyInfo) {
			proxyInfo = purple_proxy_info_new();
		}

		if (!networkProxy.getServer().empty()) {
			purple_proxy_info_set_type(proxyInfo, PURPLE_PROXY_HTTP);
			purple_proxy_info_set_host(proxyInfo, networkProxy.getServer().c_str());
			purple_proxy_info_set_port(proxyInfo, networkProxy.getServerPort());
			purple_proxy_info_set_username(proxyInfo, networkProxy.getLogin().c_str());
			purple_proxy_info_set_password(proxyInfo, networkProxy.getPassword().c_str());
		} else {
			purple_proxy_info_set_type(proxyInfo, PURPLE_PROXY_NONE);
		}

		purple_account_set_proxy_info(pAccount, proxyInfo);
		////
	}

	switch (account.getAccountType()) {
	case EnumAccountType::AccountTypeAIM:
	case EnumAccountType::AccountTypeICQ:
		setOscarParams(account, pAccount, alternateConnectionWay);
		break;
	case EnumAccountType::AccountTypeGTalk:
		setGTalkParams(account, pAccount, alternateConnectionWay);
		break;
	case EnumAccountType::AccountTypeJabber:
		setJabberParams(account, pAccount, alternateConnectionWay);
		break;
	case EnumAccountType::AccountTypeMSN:
		setMSNParams(account, pAccount, alternateConnectionWay);
		break;
	case EnumAccountType::AccountTypeYahoo:
		setYahooParams(account, pAccount, alternateConnectionWay);
		break;
	default:
		LOG_FATAL("protocol not supported");
		break;
	}
}

void PurpleParamSetter::setGTalkParams(Account & account, PurpleAccount *pAccount, bool alternateConnectionWay) {
	setJabberParams(account, pAccount, alternateConnectionWay);

	GTalkAccount *gtalkAccount = static_cast<GTalkAccount *>(account.getPrivateAccount());

	alternateConnectionWay = true;

	if (alternateConnectionWay) {
		gtalkAccount->setServerPort(PURPLE_GTALK_ALTERNATE_SERVER_PORT);
	} else {
		gtalkAccount->setServerPort(PURPLE_JABBER_SERVER_PORT);
	}

	if (pAccount) {
		purple_account_set_int(pAccount, PURPLE_JABBER_PORT_KEY, gtalkAccount->getServerPort());
	}
}

void PurpleParamSetter::setJabberParams(Account & account, PurpleAccount *pAccount, bool alternateConnectionWay) {
	JabberAccount *jabberAccount = static_cast<JabberAccount *>(account.getPrivateAccount());

	if (pAccount) {
		purple_account_set_string(pAccount, PURPLE_JABBER_SERVER_KEY, jabberAccount->getServer().c_str());
		purple_account_set_int(pAccount, PURPLE_JABBER_PORT_KEY, jabberAccount->getServerPort());
		purple_account_set_string(pAccount, PURPLE_JABBER_RESOURCE_KEY, jabberAccount->getResource().c_str());
		purple_account_set_bool(pAccount, PURPLE_JABBER_USE_TLS_KEY, jabberAccount->isTLSUsed());
		purple_account_set_bool(pAccount, PURPLE_JABBER_REQUIRE_TLS_KEY, jabberAccount->isTLSRequired());
		purple_account_set_bool(pAccount, PURPLE_JABBER_USE_OLD_SSL_KEY, jabberAccount->isOldSSLUsed());
		purple_account_set_bool(pAccount, PURPLE_JABBER_AUTH_PLAIN_IN_CLEAR_KEY, jabberAccount->isAuthPlainInClearUsed());
		purple_account_set_string(pAccount, PURPLE_JABBER_CONNECTION_SERVER_KEY, jabberAccount->getConnectionServer().c_str());
	}
}

void PurpleParamSetter::setMSNParams(Account & account, PurpleAccount *pAccount, bool alternateConnectionWay) {
	MSNAccount *msnAccount = static_cast<MSNAccount *>(account.getPrivateAccount());

	alternateConnectionWay = true;

	if (alternateConnectionWay) {
		msnAccount->setHttpConnection(true);
	} else {
		msnAccount->setHttpConnection(false);
	}

	if (pAccount) {
		purple_account_set_bool(pAccount, PURPLE_MAIL_NOTIFICATION_KEY, msnAccount->isMailNotified());
		purple_account_set_string(pAccount, PURPLE_MSN_SERVER_KEY, msnAccount->getServer().c_str());
		purple_account_set_int(pAccount, PURPLE_MSN_PORT_KEY, msnAccount->getServerPort());
		purple_account_set_bool(pAccount, PURPLE_MSN_USE_HTTP_KEY, msnAccount->isHttpUsed());
		purple_account_set_string(pAccount, PURPLE_MSN_HTTP_SERVER_KEY, "gateway.messenger.hotmail.com");
	}
}

void PurpleParamSetter::setOscarParams(Account & account, PurpleAccount *pAccount, bool alternateConnectionWay) {
	OscarAccount *oscarAccount = static_cast<OscarAccount *>(account.getPrivateAccount());

	alternateConnectionWay = true;

	if (alternateConnectionWay) {
		oscarAccount->setServerPort(PURPLE_OSCAR_ALTERNATE_SERVER_PORT);
	} else {
		oscarAccount->setServerPort(PURPLE_OSCAR_SERVER_PORT);
	}

	if (pAccount) {
		purple_account_set_string(pAccount, PURPLE_OSCAR_SERVER_KEY, oscarAccount->getServer().c_str());
		purple_account_set_int(pAccount, PURPLE_OSCAR_PORT_KEY, oscarAccount->getServerPort());
		purple_account_set_string(pAccount, PURPLE_OSCAR_ENCODING_KEY, oscarAccount->getEncoding().c_str());
	}
}

void PurpleParamSetter::setYahooParams(Account &account, PurpleAccount *pAccount, bool alternateConnectionWay) {
	YahooAccount *yahooAccount = static_cast<YahooAccount *>(account.getPrivateAccount());

	alternateConnectionWay = true;

	if (alternateConnectionWay) {
		yahooAccount->setServerPort(PURPLE_YAHOO_ALTERNATE_SERVER_PORT);
	} else {
		yahooAccount->setServerPort(PURPLE_YAHOO_SERVER_PORT);
	}

	if (pAccount) {
		purple_account_set_bool(pAccount, PURPLE_YAHOO_IS_JAPAN_KEY, yahooAccount->isYahooJapan());
		purple_account_set_int(pAccount, PURPLE_YAHOO_PORT_KEY, yahooAccount->getServerPort());
		purple_account_set_int(pAccount, PURPLE_YAHOO_XFER_PORT_KEY, yahooAccount->getXferPort());
		purple_account_set_string(pAccount, PURPLE_YAHOO_ROOM_LIST_LOCALE_KEY, yahooAccount->getRoomListLocale().c_str());

		if (yahooAccount->isYahooJapan()) {
			purple_account_set_string(pAccount, PURPLE_YAHOO_JAPAN_SERVER_KEY, yahooAccount->getServer().c_str());
			purple_account_set_string(pAccount, PURPLE_YAHOO_JAPAN_XFER_HOST_KEY, yahooAccount->getXferHost().c_str());
		} else {
			purple_account_set_string(pAccount, PURPLE_YAHOO_SERVER_KEY, yahooAccount->getServer().c_str());
			purple_account_set_string(pAccount, PURPLE_YAHOO_XFER_HOST_KEY, yahooAccount->getXferHost().c_str());
		}
	}
}

bool PurpleParamSetter::isAlternateConnectionUsed(const Account & account) {
	bool result = false;

	switch (account.getAccountType()) {
	case EnumAccountType::AccountTypeAIM:
	case EnumAccountType::AccountTypeICQ: {
		OscarAccount *oscarAccount = static_cast<OscarAccount *>(account.getPrivateAccount());
		result = (oscarAccount->getServerPort() == PURPLE_OSCAR_ALTERNATE_SERVER_PORT);
		break;
		}
	case EnumAccountType::AccountTypeGTalk: {
		GTalkAccount *gtalkAccount = static_cast<GTalkAccount *>(account.getPrivateAccount());
		result = (gtalkAccount->getServerPort() == PURPLE_GTALK_ALTERNATE_SERVER_PORT);
		break;
		}
	case EnumAccountType::AccountTypeMSN: {
		MSNAccount *msnAccount = static_cast<MSNAccount *>(account.getPrivateAccount());
		result = (msnAccount->isHttpUsed());
		break;
		}
	case EnumAccountType::AccountTypeYahoo: {
		YahooAccount *yahooAccount = static_cast<YahooAccount *>(account.getPrivateAccount());
		result = (yahooAccount->getServerPort() == PURPLE_YAHOO_ALTERNATE_SERVER_PORT);
		break;
		}
	default: {
		break;
		}
	}

	return result;
}
