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

#include <purplewrapper/PurpleEnumIMProtocol.h>

#include <util/Logger.h>

#include <cstring>

#define PRPL_MSN	"prpl-msn"
#define PRPL_YAHOO	"prpl-yahoo"
#define PRPL_OSCAR	"prpl-oscar"
#define PRPL_JABBER	"prpl-jabber"
#define PRPL_SIMPLE	"prpl-simple"
#define PRPL_UNK	"prpl-unknown"

const char * PurpleIMPrcl::getPrclId(EnumAccountType::AccountType accountType) {
	switch (accountType) {
	case EnumAccountType::AccountTypeGTalk:
		return PRPL_JABBER;

	case EnumAccountType::AccountTypeMSN:
		return PRPL_MSN;

	case EnumAccountType::AccountTypeYahoo:
		return PRPL_YAHOO;

	case EnumAccountType::AccountTypeAIM:
		return PRPL_OSCAR;

	case EnumAccountType::AccountTypeICQ:
		return PRPL_OSCAR;

	case EnumAccountType::AccountTypeJabber:
		return PRPL_JABBER;

	default:
		return PRPL_UNK;
	}
}

EnumAccountType::AccountType PurpleIMPrcl::getEnumIMProtocol(PurpleAccount * gAccount) {
	EnumAccountType::AccountType result = EnumAccountType::AccountTypeUnknown;
	const char *protocolId = purple_account_get_protocol_id(gAccount);

	if (!protocolId || *protocolId == '\0') {
		LOG_DEBUG("unknown purple accountType ID");
		result = EnumAccountType::AccountTypeUnknown;
	} else if (!strcmp(protocolId, PRPL_MSN)) {
		result = EnumAccountType::AccountTypeMSN;
	} else if (!strcmp(protocolId, PRPL_YAHOO)) {
		result = EnumAccountType::AccountTypeYahoo;
	} else if (!strcmp(protocolId, PRPL_OSCAR)) {
		// FIXME: should check if it is an AIM or an ICQ account
		result = EnumAccountType::AccountTypeAIM;
	} else if (!strcmp(protocolId, PRPL_JABBER)) {
		if (strstr(purple_account_get_username(gAccount), "@gmail.com")) {
			result = EnumAccountType::AccountTypeGTalk;
		} else {
			result = EnumAccountType::AccountTypeJabber;
		}
	} else {
		result = EnumAccountType::AccountTypeUnknown;
	}

	return result;
}
