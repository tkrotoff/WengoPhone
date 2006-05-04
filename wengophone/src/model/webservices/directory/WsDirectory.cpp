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

#include <model/config/ConfigManager.h>
#include <model/config/Config.h>
#include <model/profile/StreetAddress.h>

#include <tinyxml.h>
#include <util/Logger.h>
#include <stdio.h>

WsDirectory::WsDirectory(WengoAccount * wengoAccount) :
    WengoWebService(wengoAccount) {

    Config & config = ConfigManager::getInstance().getCurrentConfig();

	setHostname(config.getWengoServerHostname());
	setGet(true);
	setHttps(false);
	setServicePath(config.getWengoDirectoryPath());
	setPort(80);
	setWengoAuthentication(false);
}

void WsDirectory::searchEntry(const std::string & q, Criteria criteria) {
	
	std::string query = "q=" + q;

	switch( criteria ) {
		case lname:
			query += "&a=lname";
			break;
		case fname:
			query += "&a=fname";
			break;
		case city:
			query += "&a=city";
			break;
		case country:
			query += "&a=country";
			break;
		case alias:
			query += "&a=alias";
			break;
		case wengoid:
			query += "&a=wengoid";
			break;
		case none:
			break;
		default:
			LOG_FATAL("Unknown Criteria");
	}
	
	setParameters(query);
	call(this);
}

void WsDirectory::answerReceived(const std::string & answer, int requestId) {

	int wgcardCount = 0;
	TiXmlDocument doc;
	doc.Parse(answer.c_str());

	TiXmlHandle docHandle(& doc);
	TiXmlElement * root = docHandle.FirstChild("root").Element();

	if( !root ) {
		contactFoundEvent(*this, NULL, false);
		return;
	}

	TiXmlElement * wgcard = root->FirstChildElement("wgcard");
	while( wgcard ) {

		bool online = false;
		std::string wengoid = "";
		std::string sip = "";
		std::string fname = "";
		std::string lname = "";
		std::string city = "";
		std::string country = "";
		std::string wengoNumber = "";

		//TODO: extract the wgcard version

		//extract "wengoid"
		TiXmlElement * elt = wgcard->FirstChildElement("wengoid");
		if( elt && (elt->FirstChild()) ) {
			wengoid = std::string(elt->FirstChild()->ToText()->Value());
		}

		//extract "status"
		elt = wgcard->FirstChildElement("status");
		if( elt && (elt->FirstChild()) ) {
			if( std::string(elt->FirstChild()->ToText()->Value()) == "online" ) {
				online = true;
			}
		}

		//extract "sip"
		elt = wgcard->FirstChildElement("sip");
		if( elt && (elt->FirstChild()) ) {
			sip = std::string(elt->FirstChild()->ToText()->Value());
		}

		//extract "name" info
		elt = wgcard->FirstChildElement("name");
		if( elt ) {

			TiXmlElement * eltName = elt->FirstChildElement("last");
			if( eltName && (eltName->FirstChild()) ) {
				lname = std::string(eltName->FirstChild()->ToText()->Value());
			}

			eltName = elt->FirstChildElement("first");
			if( eltName && (eltName->FirstChild()) ) {
				fname = std::string(eltName->FirstChild()->ToText()->Value());
			}
		}

		//extract all "address" info
		elt = wgcard->FirstChildElement("address");
		while( elt ) {
			const char * type = elt->Attribute("type");
			if( type ) {
				if( std::string(type) == "home" ) {

					TiXmlElement * addElt = elt->FirstChildElement("country");
					if( addElt && (addElt->FirstChild()) ) {
						country = std::string(addElt->FirstChild()->ToText()->Value());
					}
	
					addElt = elt->FirstChildElement("locality");
					if( addElt && (addElt->FirstChild()) ) {
						city = std::string(addElt->FirstChild()->ToText()->Value());
					}
				} else if( std::string(type) == "work" ) {
				}
			}
			elt = elt->NextSiblingElement("address");
		}

		//extract phone numbers
		elt = wgcard->FirstChildElement("tel");
		while( elt ) {

			const char * prefered = elt->Attribute("prefered");
			if( prefered && (std::string(prefered) == "prefered")) {
				
			}

			const char * type = elt->Attribute("type");
			if( type ) {

				if( std::string(type) == "home" ) {
					if( elt && (elt->FirstChild()) ) {
					}
				} else if ( std::string(type) == "work") {
					if( elt && (elt->FirstChild()) ) {
					}
				} else if ( std::string(type) == "cell") {
					if( elt && (elt->FirstChild()) ) {
					}
				} else if ( std::string(type) == "wengo") {
					if( elt && (elt->FirstChild()) ) {
						wengoNumber = std::string(elt->FirstChild()->ToText()->Value());
					}
				}
			}

			elt = elt->NextSiblingElement("tel");
		}

		//create a Profile with extracted info
		Profile * profile = new Profile();
		StreetAddress address;
		address.setCity(city);
		address.setCountry(country);
		profile->setStreetAddress(address);
		profile->setWengoPhoneNumber(wengoNumber);
		profile->setFirstName(fname);
		profile->setLastName(lname);
		profile->setWengoPhoneId(wengoid);

		//emit the contactFoundEvent event
		contactFoundEvent(*this, profile, online);
		
		wgcardCount++;
		wgcard = wgcard->NextSiblingElement("wgcard");
	}
	
	if( wgcardCount == 0 ) {
		contactFoundEvent(*this, NULL, false);
	}
}
