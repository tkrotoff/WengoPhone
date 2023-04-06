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

#include <coipmanager_base/serializer/AccountXMLSerializer.h>

#include <coipmanager_base/account/Account.h>
#include <coipmanager_base/account/AIMAccount.h>
#include <coipmanager_base/account/GTalkAccount.h>
#include <coipmanager_base/account/IAXAccount.h>
#include <coipmanager_base/account/ICQAccount.h>
#include <coipmanager_base/account/JabberAccount.h>
#include <coipmanager_base/account/MSNAccount.h>
#include <coipmanager_base/account/WengoAccount.h>
#include <coipmanager_base/account/YahooAccount.h>
#include <coipmanager_base/serializer/OscarAccountXMLSerializer.h>
#include <coipmanager_base/serializer/WengoAccountXMLSerializer.h>

#include <util/Base64.h>
#include <util/Logger.h>
#include <util/SafeDelete.h>
#include <util/String.h>

#include <tinyxml.h>

AccountXMLSerializer::AccountXMLSerializer(Account & account)
	: _account(account) {
}

AccountXMLSerializer::~AccountXMLSerializer() {
}

std::string AccountXMLSerializer::serialize() {
	std::string result;

	result += "<account accountType=\"" + EnumAccountType::toString(_account.getAccountType()) + "\">\n";

	result += "<uuid>" + _account.getUUID() + "</uuid>\n";

	if (!(!_account.isPasswordSaved() && (_account.getAccountType() == EnumAccountType::AccountTypeWengo))) {
		// If accountType is Wengo and the password must not be saved,
		// we does'nt save the SIP login as it will
		// be retrieved via the SSO request
		result += ("<login>" + _account.getLogin() + "</login>\n");
	}

	if (_account.isPasswordSaved()) {
		result += ("<password>" + Base64::encode(_account.getPassword()) + "</password>\n");
		result += ("<connectionconfigurationset>" + String::fromBoolean(_account.isConnectionConfigurationSet()) + "</connectionconfigurationset>\n");
	}

	result += ("<alias>" + _account.getAlias() + "</alias>\n");
	result += ("<presence>" + EnumPresenceState::toString(_account.getPresenceState()) + "</presence>\n");

	std::string privateContent;
	switch (_account.getAccountType()) {
	case EnumAccountType::AccountTypeAIM:
	case EnumAccountType::AccountTypeICQ:
	{
		OscarAccountXMLSerializer serializer(*static_cast<OscarAccount *>(_account.getPrivateAccount()));
		privateContent = serializer.serialize();
		break;
	}

/*	case EnumAccountType::AccountTypeIAX:
		break;
	case EnumAccountType::AccountTypeJabber:
		break;
	case EnumAccountType::AccountTypeMSN:
		break;
*/

	case EnumAccountType::AccountTypeSIP:
	{
		SipAccountXMLSerializer serializer(*static_cast<SipAccount *>(_account.getPrivateAccount()));
		privateContent = serializer.serialize();
		break;
	}

	case EnumAccountType::AccountTypeWengo:
	{
		WengoAccountXMLSerializer serializer(*static_cast<WengoAccount *>(_account.getPrivateAccount()));
		privateContent = serializer.serialize();
		break;
	}

/*	case EnumAccountType::AccountTypeYahoo:
		break;
*/	

	default:
		// No specific task to do
		break;
	}

	result += privateContent;

	result += "</account>\n";

	return result;
}

bool AccountXMLSerializer::unserialize(const std::string & data) {
	TiXmlDocument doc;

	doc.Parse(data.c_str());

	TiXmlHandle docHandle(&doc);
	TiXmlHandle account = docHandle.FirstChild("account");

	IAccount *myAccount = NULL;

	// Retrieving accountType and accountType specific data
	// Retrieving accountType specific data has to be done before
	// everything else because WengoAccount::setWengoLogin and
	// WengoAccount::setWengoPassword reset data in WengoAccount
	EnumAccountType::AccountType accountType;
	TiXmlElement * lastChildElt = account.Element();
	if (lastChildElt) {
		accountType = EnumAccountType::toAccountType(lastChildElt->Attribute("accountType"));
		switch (accountType) {
		case EnumAccountType::AccountTypeAIM:
		{
			myAccount = new AIMAccount();
			OscarAccountXMLSerializer serializer(*static_cast<OscarAccount *>(myAccount));
			if (!serializer.unserialize(account)) {
				OWSAFE_DELETE(myAccount);
				return false;
			}
			break;
		}

		case EnumAccountType::AccountTypeGTalk:
		{
			myAccount = new GTalkAccount();
			break;
		}

		case EnumAccountType::AccountTypeIAX:
		{
			myAccount = new IAXAccount();
			break;
		}

		case EnumAccountType::AccountTypeICQ:
		{
			myAccount = new ICQAccount();
			OscarAccountXMLSerializer serializer(*static_cast<OscarAccount *>(myAccount));
			if (!serializer.unserialize(account)) {
				OWSAFE_DELETE(myAccount);
				return false;
			}
			break;
		}

		case EnumAccountType::AccountTypeJabber:
		{
			myAccount = new JabberAccount();
			break;
		}

		case EnumAccountType::AccountTypeMSN:
		{
			myAccount = new MSNAccount();
			break;
		}

		case EnumAccountType::AccountTypeSIP:
		{
			myAccount = new SipAccount();
			SipAccountXMLSerializer serializer(*static_cast<WengoAccount *>(myAccount));
			if (!serializer.unserialize(account)) {
				OWSAFE_DELETE(myAccount);
				return false;
			}
			break;
		}

		case EnumAccountType::AccountTypeWengo:
		{
			myAccount = new WengoAccount();
			WengoAccountXMLSerializer serializer(*static_cast<WengoAccount *>(myAccount));
			if (!serializer.unserialize(account)) {
				OWSAFE_DELETE(myAccount);
				return false;
			}
			break;
		}

		case EnumAccountType::AccountTypeYahoo:
		{
			myAccount = new YahooAccount();
			break;
		}

		default:
			LOG_FATAL("unknown accountType");
			break;
		}
	} else {
		return false;
	}

	//Retrieving login
	TiXmlText * login = account.FirstChild("login").FirstChild().Text();
	if (login) {
		myAccount->setLogin(login->Value());
	} else {
		if (accountType != EnumAccountType::AccountTypeWengo) {
			// Wengo Account could not have a fulled login field if the SSO
			// request has not been launched.
			OWSAFE_DELETE(myAccount);
			return false;
		}
	}

	//Retrieving password
	TiXmlText * password = account.FirstChild("password").FirstChild().Text();
	if (password) {
		myAccount->setPassword(Base64::decode(password->Value()));
		myAccount->setSavePassword(true);
	} else {
		myAccount->setSavePassword(false);	
	}

	//Retrieving alias
	TiXmlText * alias = account.FirstChild("alias").FirstChild().Text();
	if (alias) {
		myAccount->setAlias(alias->Value());
	}

	//Retireving Presence state
	TiXmlText * presence = account.FirstChild("presence").FirstChild().Text();
	if (presence) {
		myAccount->setPresenceState(EnumPresenceState::fromString(presence->Value()));
	}

	// Retrieving connection configuration set
	TiXmlNode * configSet = account.FirstChild("connectionconfigurationset").FirstChild().Node();
	if (configSet) {
		String sConfigSet = configSet->Value();
		myAccount->setConnectionConfigurationSet(sConfigSet.toBoolean());
	}
	////

	_account = Account(myAccount);
	OWSAFE_DELETE(myAccount);

	// Retrieving UUID
	TiXmlText * uuid = account.FirstChild("uuid").FirstChild().Text();
	if (uuid) {
		_account.setUUID(uuid->Value());
	} else {
		LOG_FATAL("no UUID found");
		return false;
	}
	////

	return true;
}
