/*
 * WengoPhone, a voice over Internet phone
 * Copyright (C) 2004-2005  Wengo
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

#include "ContactParser.h"

#include "Contact.h"

#include <model/WengoPhoneLogger.h>
#include <model/WengoPhone.h>
#include <model/presence/PresenceHandler.h>

#include <imwrapper/IMAccountHandler.h>
#include <imwrapper/IMContact.h>

#include <tinyxml.h>

ContactParser::ContactParser(Contact & contact, const std::string & data) {
	TiXmlDocument doc;
	doc.Parse(data.c_str());

	TiXmlHandle docHandle(& doc);

	TiXmlHandle vCard = docHandle.FirstChild("vCard");

	TiXmlText * text = vCard.FirstChild("FN").FirstChild().Text();
	if (text) {
		contact.setFirstName(text->Value());
	}

	text = vCard.FirstChild("N").FirstChild().Text();
	if (text) {
		contact.setLastName(text->Value());
	}

	parseIMAccount(contact, vCard, "MSNID", EnumIMProtocol::IMProtocolMSN);
	parseIMAccount(contact, vCard, "YAHOOID", EnumIMProtocol::IMProtocolYahoo);
	parseIMAccount(contact, vCard, "AIMID", EnumIMProtocol::IMProtocolAIM);
	parseIMAccount(contact, vCard, "ICQID", EnumIMProtocol::IMProtocolICQ);
	parseIMAccount(contact, vCard, "JABBERID", EnumIMProtocol::IMProtocolJabber);

	text = vCard.FirstChild("WENGOID").FirstChild().Text();

	if (text) {
		//FIXME: crashes when IMAccountList is populated after initializing the ContactList
		List<IMAccount *> list = contact.getWengoPhone().getIMAccountHandler().getIMAccountsOfProtocol(EnumIMProtocol::IMProtocolSIPSIMPLE);
		
		IMContact imContact(*list[0], text->Value());
		contact.addIMContact(imContact);
		contact.setWengoPhoneId(text->Value());
	}
}

void ContactParser::parseIMAccount(Contact & contact, const TiXmlHandle & handle, const std::string & protocolId, EnumIMProtocol::IMProtocol protocol) {
	TiXmlNode * parentNode = handle.Node();
	TiXmlNode * node = parentNode->IterateChildren(protocolId, NULL);

	while (node) {
		TiXmlText * text = node->FirstChild()->ToText();
		if (text) {
			//TODO: find a way to load/save data
			//IMContact imContact(protocol, text->Value(), PresenceHandler::getInstance());
			//contact.addIMContact(imContact);
		}
		node = parentNode->IterateChildren(protocolId, node);
	}
}

