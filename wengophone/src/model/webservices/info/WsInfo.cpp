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

const char * getValueFromKey(TiXmlElement * element, const std::string key);

const std::string WsWengoInfo::WENGOSCOUNT_TAG = "contract.counter.wengos";
const std::string WsWengoInfo::SMSCOUNT_TAG = "contract.counter.sms";
const std::string WsWengoInfo::ACTIVEMAIL_TAG = "ucf.email.isactif";
const std::string WsWengoInfo::UNREADVOICEMAILCOUNT_TAG = "tph.mbox.unseencount";
const std::string WsWengoInfo::CALLFORWARD_TAG = "tph.callforward";
const std::string WsWengoInfo::PSTNNUMBER_TAG = "contract.option.pstnnum";
const std::string WsWengoInfo::CALLFORWARD_TOVOICEMAIL_ENABLE_TAG = "tph.callforward.cfb.enabled";
const std::string WsWengoInfo::CALLFORWARD_TOVOICEMAIL_DEST_TAG = "tph.callforward.cfb.destination";
const std::string WsWengoInfo::CALLFORWARD_TOPSTN_ENABLE_TAG = "tph.callforward.cffl.enabled";
const std::string WsWengoInfo::CALLFORWARD_TOPSTN_DEST1_TAG = "tph.callforward.cffl.destination1";
const std::string WsWengoInfo::CALLFORWARD_TOPSTN_DEST2_TAG = "tph.callforward.cffl.destination2";
const std::string WsWengoInfo::CALLFORWARD_TOPSTN_DEST3_TAG = "tph.callforward.cffl.destination3";

WsWengoInfo::WsWengoInfo(WengoAccount & wengoAccount) : WengoWebService(wengoAccount) {
	Config & config = ConfigManager::getInstance().getCurrentConfig();

	_wengosCount = false;
	_smsCount = false;
	_activeMail = false;
	_unreadVoiceMail = false;
	_callForward = false;
	_pstnNumber = false;

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

void WsWengoInfo::getCallForwardInfo(bool callForward) {
	_callForward = callForward;
}

void WsWengoInfo::getPstnNumber(bool pstnNumber) {
	_pstnNumber = pstnNumber;
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
	if(_pstnNumber) {
		query += PSTNNUMBER_TAG + "|";
	}
	if(_callForward) {
		query += CALLFORWARD_TAG + "|";
	}
	
	//remove the last pipe if any
	if(	query != "query=" ) {
		query = query.substr(0, query.length() - 1);
	}
	
	setParameters(query);
	
	return call(this);
}

void WsWengoInfo::answerReceived(std::string answer, int id) {
	
	const char * value = NULL;
	std::string voiceMailNumber = "";
	std::string dest1 = "";
	std::string dest2 = "";
	std::string dest3 = "";
	bool forward2VoiceMail = false;
	
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
				
				// wengos count
				if( std::string(key) == WENGOSCOUNT_TAG ) {
					float wengos  = 0.0;
					value = getValueFromKey(element, WENGOSCOUNT_TAG);
					if( value ) {
						std::stringstream ss(value);
						ss >> wengos;
						wsInfoWengosEvent(*this, id, WsWengoInfoStatusOk, wengos);
					}
					
				// sms count
				} else if( key == SMSCOUNT_TAG ) {
					int sms  = 0;
					value = getValueFromKey(element, SMSCOUNT_TAG);
					if( value ) {
						std::stringstream ss( value );
						ss >> sms;
						wsInfoSmsCountEvent(*this, id, WsWengoInfoStatusOk, sms);
					}
					
				// active mail
				} else if( key == ACTIVEMAIL_TAG ) {
					int activeMail = 0;
					value = getValueFromKey(element, ACTIVEMAIL_TAG);
					if( value ) {
						std::stringstream ss(value);
						ss >> activeMail;
						wsInfoActiveMailEvent(*this, id, WsWengoInfoStatusOk, activeMail);
					}
				
				// unread voice mail
				} else if( key == UNREADVOICEMAILCOUNT_TAG ) {
					int voiceMail = 0;
					value = getValueFromKey(element, UNREADVOICEMAILCOUNT_TAG);
					if( value ) {
						std::stringstream ss(value);
						ss >> voiceMail;
						wsInfoVoiceMailEvent(*this, id, WsWengoInfoStatusOk, voiceMail);
					}
				
				// call forward
				} else if( key == CALLFORWARD_TOVOICEMAIL_ENABLE_TAG ) {
					int enabled = 0;
					value = getValueFromKey(element, CALLFORWARD_TOVOICEMAIL_ENABLE_TAG);
					if( value ) {
						std::stringstream ss( value );
						ss >> enabled;
						if( enabled == 1 ) {
							forward2VoiceMail = true;
						}
					}
					
				// call forward
				} else if( key == CALLFORWARD_TOVOICEMAIL_DEST_TAG ) {
					value = getValueFromKey(element, CALLFORWARD_TOVOICEMAIL_DEST_TAG);
					if( value ) {
						voiceMailNumber = std::string(value);
					}
					
				// call forward
				} else if( key == CALLFORWARD_TOPSTN_ENABLE_TAG ) {
					int enabled = 0;
					value = getValueFromKey(element, CALLFORWARD_TOPSTN_ENABLE_TAG);
					if( value ) {
						std::stringstream ss(value);
						ss >> enabled;
						if( enabled == 1 ) {
							forward2VoiceMail = false;
						}
					}
					
				// call forward
				} else if( key == CALLFORWARD_TOPSTN_DEST1_TAG ) {
					value = getValueFromKey(element, CALLFORWARD_TOPSTN_DEST1_TAG);
					if( value ) {
						dest1 = std::string(value);
					}
					
				// call forward
				} else if( key == CALLFORWARD_TOPSTN_DEST2_TAG ) {
					value = getValueFromKey(element, CALLFORWARD_TOPSTN_DEST2_TAG);
					if( value ) {
						dest2 = std::string(value);
					}
					
				// call forward
				} else if( key == CALLFORWARD_TOPSTN_DEST3_TAG ) {
					value = getValueFromKey(element, CALLFORWARD_TOPSTN_DEST3_TAG);
					if( value ) {
						dest3 = std::string(value);
					}
				
				//pstn number
				} else if( key == PSTNNUMBER_TAG ) {
					value = getValueFromKey(element, PSTNNUMBER_TAG);
					if( value ) {
						wsInfoPtsnNumberEvent(*this, id, WsWengoInfoStatusOk, std::string(value));
					}
					
				}
			}
			element = element->NextSiblingElement("o");
		}
	}
	
	//emit call forward event
	if( _callForward ) {
		if( forward2VoiceMail ) {
			wsCallForwardInfoEvent(*this, id, WsWengoInfoStatusOk, forward2VoiceMail, voiceMailNumber, "", "");
		} else {
			wsCallForwardInfoEvent(*this, id, WsWengoInfoStatusOk, forward2VoiceMail, dest1, dest2, dest3);
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
