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

#include "WsInfo.h"

#include <model/config/ConfigManager.h>
#include <model/config/Config.h>

#include <util/Logger.h>

#include <tinyxml.h>
#include <sstream>

const char * getValueFromKey(TiXmlElement * element, std::string key);

const std::string WsWengoInfo::WENGOSCOUNT_TAG = "contract.counter.wengos";
const std::string WsWengoInfo::SMSCOUNT_TAG = "contract.counter.sms";
const std::string WsWengoInfo::ACTIVEMAIL_TAG = "ucf.email.isactif";
const std::string WsWengoInfo::UNREADVOICEMAILCOUNT_TAG = "tph.mbox.unseencount";

WsWengoInfo::WsWengoInfo(WengoAccount & wengoAccount) : WengoWebService(wengoAccount) {
	//TODO: use the settings from Config
	Config & config = ConfigManager::getInstance().getCurrentConfig();

	_wengosCount = false;
	_smsCount = false;
	_activeMail = false;
	_unreadVoiceMail = false;

	//setup info web service
	setHostname(config.getWengoServerHostname());
	setGet(true);
	setHttps(false);
	setServicePath(config.getWengoWsInfoPath());
	setPort(80);
	setWengoAuthentication(true);
}

void WsWengoInfo::getWengosCount(bool wengosCount) {
	_wengosCount = wengosCount;
}

void WsWengoInfo::getSmsCount(bool smsCount) {
	_smsCount = smsCount;
}

void WsWengoInfo::getActiveMail(bool activeMail) {
	_activeMail = activeMail;
}

void WsWengoInfo::getUnreadVoiceMail(bool unreadVoiceMail) {
	_unreadVoiceMail = unreadVoiceMail;
}

int WsWengoInfo::execute() {
	
	//build the query
	std::string query = "query=";
	if(_wengosCount) {
		query += WENGOSCOUNT_TAG + "|";
	}
	if(_smsCount) {
		query += SMSCOUNT_TAG + "|";
	}
	if(_activeMail) {
		query += ACTIVEMAIL_TAG + "|";
	}
	if(_unreadVoiceMail) {
		query += UNREADVOICEMAILCOUNT_TAG + "|";
	}
	
	//remove the last pipe
	if(	query != "query=" ) {
		query = query.substr(0, query.length() - 2);
	}

	setParameters(query);
	
	return call(this);
}

void WsWengoInfo::answerReceived(std::string answer, int id) {
	
	TiXmlDocument document;
	document.Parse(answer.c_str());
	
	TiXmlElement * root = document.FirstChildElement("output");
	if ( root ) {
		
		//iterate over "o" element
		TiXmlElement * element = root->FirstChildElement("o");
		while ( element ) {
			
			//find the key
			const char * key = element->Attribute("k");
			if( key ) {
				
				if( std::string(key) == WENGOSCOUNT_TAG ) {
					float wengos  = 0.0;
					std::stringstream(getValueFromKey(element, WENGOSCOUNT_TAG)) >> wengos;
					wsInfoWengosEvent(*this, id, WsWengoInfoStatusOk, wengos);
					
				} else if( key == SMSCOUNT_TAG ) {
					int sms  = 0;
					std::stringstream(getValueFromKey(element, SMSCOUNT_TAG)) >> sms;
					wsInfoSmsCountEvent(*this, id, WsWengoInfoStatusOk, sms);
					
				} else if( key == ACTIVEMAIL_TAG ) {
					int activeMail = 0;
					std::stringstream(getValueFromKey(element, ACTIVEMAIL_TAG)) >> activeMail;
					wsInfoActiveMailEvent(*this, id, WsWengoInfoStatusOk, activeMail);
					
				} else if( key == UNREADVOICEMAILCOUNT_TAG ) {
					int voiceMail = 0;
					std::stringstream(getValueFromKey(element, UNREADVOICEMAILCOUNT_TAG)) >> voiceMail;
					wsInfoVoiceMailEvent(*this, id, WsWengoInfoStatusOk, voiceMail);
					
				}
			}
			element = element->NextSiblingElement("o");
		}
	}
}

const char * getValueFromKey(TiXmlElement * element, std::string key) {
	
	//find value
	TiXmlElement * elt = element->FirstChildElement("v");
	if( elt) {
		const char * type = elt->Attribute("t");
		
		//check for error
		if( ( type ) && (std::string(type) != "e" ) ) {

			//extract value text
			TiXmlText  * text = elt->FirstChild()->ToText();
			if( text ) {
				return text->Value();
			}
		}
	}
	return NULL;
}
