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

#include "GaimIMConnect.h"

extern "C" {
#include "gaim/account.h"
#include "gaim/connection.h"
#include "gaim/core.h"
}

#include "GaimEnumIMProtocol.h"
#include <imwrapper/IMAccount.h>
#include <imwrapper/IMAccountParameters.h>

#include <util/Logger.h>

#define MAIL_NOTIFICATION_KEY			"check-mail"

#define YAHOO_IS_JAPAN_KEY				"yahoojp"
#define YAHOO_SERVER_KEY				"server"
#define YAHOO_JAPAN_SERVER_KEY			"serverjp"
#define YAHOO_PORT_KEY					"port"
#define YAHOO_XFER_HOST_KEY				"xfer_host"
#define YAHOO_JAPAN_XFER_HOST_KEY		"xferjp_host"
#define YAHOO_XFER_PORT_KEY				"xfer_port"
#define YAHOO_ROOM_LIST_LOCALE_KEY		"room_listlocale"
#define MSN_SERVER_KEY					"server"
#define MSN_PORT_KEY					"port"
#define MSN_USE_HTTP_KEY				"http_method"
#define OSCAR_SERVER_KEY				"server"
#define OSCAR_PORT_KEY					"port"
#define OSCAR_ENCODING_KEY				"encoding"
#define JABBER_SERVER_KEY				"server"
#define JABBER_PORT_KEY					"port"
#define JABBER_RESOURCE_KEY				"resource"
#define JABBER_USE_TLS_KEY				"use_tls"
#define JABBER_REQUIRE_TLS_KEY			"require_tls"
#define JABBER_USE_OLD_SSL_KEY			"old_ssl"
#define JABBER_AUTH_PLAIN_IN_CLEAR_KEY	"auth_plain_in_clear"
#define JABBER_CONNECTION_SERVER_KEY	"connect_server"

/*
int GetTabSize(char **tab)
{
	int i;

	if (!tab)
		return 0;

	for (i = 0; tab[i]; i++);

	return i;
}
*/

GaimIMConnect::GaimIMConnect(IMAccount & account)
	: IMConnect(account)
{
}

GaimIMConnect::~GaimIMConnect()
{
}

bool GaimIMConnect::equalsTo(std::string login, EnumIMProtocol::IMProtocol protocol)
{
	IMAccount imAccount(login, "", protocol);

	if (_imAccount == imAccount)
		return true;
	else
		return false;
}

void GaimIMConnect::AddMSNAccountParams(void *gaimAccount, IMAccountParameters &mParams)
{
	GaimAccount *gAccount = (GaimAccount *)gaimAccount;

	gaim_account_set_string(gAccount, MSN_SERVER_KEY, mParams.getMSNServer().c_str());
	gaim_account_set_int(gAccount, MSN_PORT_KEY, mParams.getMSNServerPort());
	gaim_account_set_bool(gAccount, MSN_USE_HTTP_KEY, mParams.isMSNHttpUsed());	
}

void GaimIMConnect::AddYahooAccountParams(void *gaimAccount, IMAccountParameters &mParams)
{
	GaimAccount *gAccount = (GaimAccount *)gaimAccount;
	
	gaim_account_set_bool(gAccount, YAHOO_IS_JAPAN_KEY, mParams.isYahooJapan());
	gaim_account_set_string(gAccount, YAHOO_SERVER_KEY, mParams.getYahooServer().c_str());
	gaim_account_set_string(gAccount, YAHOO_JAPAN_SERVER_KEY, mParams.getYahooJapanServer().c_str());
	gaim_account_set_int(gAccount, YAHOO_PORT_KEY, mParams.getYahooServerPort());
	gaim_account_set_string(gAccount, YAHOO_XFER_HOST_KEY, mParams.getYahooXferHost().c_str());
	gaim_account_set_string(gAccount, YAHOO_JAPAN_XFER_HOST_KEY, mParams.getYahooJapanXferHost().c_str());
	gaim_account_set_int(gAccount, YAHOO_XFER_PORT_KEY, mParams.getYahooXferPort());
	gaim_account_set_string(gAccount, YAHOO_ROOM_LIST_LOCALE_KEY, mParams.getYahooRoomListLocale().c_str());
}

void GaimIMConnect::AddOscarAccountParams(void *gaimAccount, IMAccountParameters &mParams)
{
	GaimAccount *gAccount = (GaimAccount *)gaimAccount;

	gaim_account_set_string(gAccount, OSCAR_SERVER_KEY, mParams.getOscarServer().c_str());
	gaim_account_set_int(gAccount, OSCAR_PORT_KEY, mParams.getOscarServerPort());
	gaim_account_set_string(gAccount, OSCAR_ENCODING_KEY, mParams.getOscarEncoding().c_str());
}

void GaimIMConnect::AddJabberAccountParams(void *gaimAccount, IMAccountParameters &mParams)
{
	GaimAccount *gAccount = (GaimAccount *)gaimAccount;

	gaim_account_set_string(gAccount, JABBER_SERVER_KEY, mParams.getJabberServer().c_str());
	gaim_account_set_int(gAccount, JABBER_PORT_KEY, mParams.getJabberServerPort());
	gaim_account_set_string(gAccount, JABBER_RESOURCE_KEY, mParams.getJabberResource().c_str());
	gaim_account_set_bool(gAccount, JABBER_USE_TLS_KEY, mParams.isJabberTLSUsed());
	gaim_account_set_bool(gAccount, JABBER_REQUIRE_TLS_KEY, mParams.isJabberTLSRequired());
	gaim_account_set_bool(gAccount, JABBER_USE_OLD_SSL_KEY, mParams.isJabberOldSSLUsed());
	gaim_account_set_bool(gAccount, JABBER_AUTH_PLAIN_IN_CLEAR_KEY, mParams.isJabberAuthPlainInClearUsed());
	gaim_account_set_string(gAccount, JABBER_CONNECTION_SERVER_KEY, mParams.getJabberConnectionServer().c_str());
}

void *GaimIMConnect::CreateAccount()
{
	GaimAccount	*gAccount = NULL;
	char *PrclId = (char *)GaimIMPrcl::GetPrclId(_imAccount.getProtocol());

	gAccount = gaim_account_new(_imAccount.getLogin().c_str(), PrclId);

	if (gAccount)
	{
		gaim_account_set_password(gAccount, _imAccount.getPassword().c_str());
		gaim_accounts_add(gAccount);
	}

	return gAccount;
}

void GaimIMConnect::AddAccountParams(void *gaimAccount)
{		
	GaimAccount *gAccount = (GaimAccount *)gaimAccount;
	IMAccountParameters &mParams = _imAccount.getIMAccountParameters();

	gaim_account_set_bool(gAccount, MAIL_NOTIFICATION_KEY, mParams.isMailNotified());

	switch (_imAccount.getProtocol())
	{
		case EnumIMProtocol::IMProtocolMSN:
			AddMSNAccountParams(gAccount, mParams);
			break;

		case EnumIMProtocol::IMProtocolYahoo:
			AddYahooAccountParams(gAccount, mParams);
			break;

		case EnumIMProtocol::IMProtocolAIMICQ:
			AddOscarAccountParams(gAccount, mParams);
			break;

		case EnumIMProtocol::IMProtocolJabber:
			AddJabberAccountParams(gAccount, mParams);
			break;

		default:
			break;
	}
}

void GaimIMConnect::connect()
{
	GaimAccount	*gAccount;

	gAccount = gaim_accounts_find(_imAccount.getLogin().c_str(),
								GaimIMPrcl::GetPrclId(_imAccount.getProtocol()));
	if (gAccount == NULL)
	{
		if (!(gAccount = (GaimAccount *) CreateAccount()))
			return;
	}

	AddAccountParams(gAccount);

	gaim_account_set_password(gAccount, _imAccount.getPassword().c_str());
	if (!gaim_account_get_enabled(gAccount, gaim_core_get_ui()))
		gaim_account_set_enabled(gAccount, gaim_core_get_ui(), TRUE);

	gaim_account_connect(gAccount);
}

void GaimIMConnect::disconnect()
{
	GaimAccount	*gAccount;

	gAccount = gaim_accounts_find(_imAccount.getLogin().c_str(),
								GaimIMPrcl::GetPrclId(_imAccount.getProtocol()));
	if (!gAccount)
		return;

	if (gaim_account_get_enabled(gAccount, gaim_core_get_ui()))
		gaim_account_set_enabled(gAccount, gaim_core_get_ui(), FALSE);
	gaim_account_disconnect(gAccount);
	
}
