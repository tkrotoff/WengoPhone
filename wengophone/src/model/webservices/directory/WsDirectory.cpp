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

#include "WsDirectory.h"

#include <model/contactlist/ContactProfile.h>
#include <model/profile/ProfileXMLSerializer.h>

#include <model/config/ConfigManager.h>
#include <model/config/Config.h>
#include <model/profile/StreetAddress.h>

#include <tinyxml.h>
#include <util/Logger.h>

#include <cstdio>

WsDirectory::WsDirectory(WengoAccount * wengoAccount)
	: WengoWebService(wengoAccount) {

	Config & config = ConfigManager::getInstance().getCurrentConfig();

	setHostname(config.getWengoServerHostname());
	setGet(true);
	setHttps(false);
	setServicePath(config.getWengoDirectoryPath());
	setPort(80);
	setWengoAuthentication(false);
}

void WsDirectory::searchEntry(const std::string & query, Criteria criteria) {

	std::string q = "q=" + query;

	switch (criteria) {
	case LastName:
		q += "&a=lname";
		break;
	case FirstName:
		q += "&a=fname";
		break;
	case City:
		q += "&a=city";
		break;
	case Country:
		q += "&a=country";
		break;
	case Alias:
		q += "&a=alias";
		break;
	case WengoId:
		q += "&a=wengoid";
		break;
	case None:
		break;
	default:
		LOG_FATAL("unknown criteria=" + String::fromNumber(criteria));
	}

	setParameters(q);
	call(this);
}

void WsDirectory::answerReceived(const std::string & answer, int requestId) {
	int wgCardCount = 0;
	TiXmlDocument doc;
	doc.Parse(answer.c_str());

	TiXmlHandle docHandle(&doc);
	TiXmlElement * root = docHandle.FirstChild("root").Element();

	if (!root) {
		contactFoundEvent(*this, NULL, false);
		return;
	}

	//Retrieving Contacts
	TiXmlNode * wgCard = NULL;
	while ((wgCard = root->IterateChildren("wgcard", wgCard))) {

		//Create a Contact with extracted info
		ContactProfile * contact = new ContactProfile();

		ProfileXMLSerializer serializer(*contact);
		TiXmlHandle h(wgCard);
		serializer.unserializeContent(h);
		wgCardCount++;

		bool online = false;
		//Extract "status"
		TiXmlElement * elt = wgCard->FirstChildElement("status");
		if (elt && (elt->FirstChild())) {
			if (std::string(elt->FirstChild()->ToText()->Value()) == "online") {
				online = true;
			}
		}

		//Emit the contactFoundEvent event
		contactFoundEvent(*this, contact, online);
	}

	if (wgCardCount == 0) {
		contactFoundEvent(*this, NULL, false);
	}
}
