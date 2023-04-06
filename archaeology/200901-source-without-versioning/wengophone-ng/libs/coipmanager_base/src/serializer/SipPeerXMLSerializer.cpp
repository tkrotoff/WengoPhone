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

#include <coipmanager_base/serializer/SipPeerXMLSerializer.h>

#include <coipmanager_base/peer/SipPeer.h>

#include <util/Logger.h>

#include <tinyxml.h>

SipPeerXMLSerializer::SipPeerXMLSerializer(SipPeer & sipPeer) 
	: Serializable(),
	_sipPeer(sipPeer) {
}

std::string SipPeerXMLSerializer::serialize() {
	std::string result;

	result += "<identity>" + _sipPeer.getIdentity() + "</identity>\n";
	result += "<username>" + _sipPeer.getUsername() + "</username>\n";
	result += "<realm>" + _sipPeer.getRealm() + "</realm>\n";
	result += "<displayname>" + _sipPeer.getDisplayName() + "</displayname>\n";

	return result;
}

bool SipPeerXMLSerializer::unserializeContent(TiXmlHandle & rootElt) {
	// Retrieving identity
	TiXmlNode * identity = rootElt.FirstChild("identity").FirstChild().Node();
	if (identity) {
		_sipPeer.setIdentity(identity->Value());
	} else {
		LOG_ERROR("cannot get identity");
		return false;
	}
	////

	// Retrieving username
	TiXmlNode * username = rootElt.FirstChild("username").FirstChild().Node();
	if (username) {
		_sipPeer.setUsername(username->Value());
	}
	////

	// Retrieving realm
	TiXmlNode * realm = rootElt.FirstChild("realm").FirstChild().Node();
	if (realm) {
		_sipPeer.setRealm(realm->Value());
	} else {
		LOG_ERROR("cannot get realm");
		return false;
	}
	////

	// Retrieving displayname
	TiXmlNode * displayname = rootElt.FirstChild("displayname").FirstChild().Node();
	if (displayname) {
		_sipPeer.setDisplayName(displayname->Value());
	}
	////

	return true;
}

bool SipPeerXMLSerializer::unserialize(const std::string & data) {
	return false;
}
