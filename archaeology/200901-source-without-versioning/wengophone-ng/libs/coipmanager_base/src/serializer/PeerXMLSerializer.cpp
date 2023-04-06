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

#include <coipmanager_base/serializer/PeerXMLSerializer.h>

#include <coipmanager_base/peer/Peer.h>

#include <util/Base64.h>

#include <tinyxml.h>

PeerXMLSerializer::PeerXMLSerializer(Peer & peer, const std::string & rootEltName)
	: _peer(peer) {
	_rootEltName = rootEltName;
}

std::string PeerXMLSerializer::serialize() {
	std::string result;

	result += ("<" + _rootEltName + " accountType=\""
		+ EnumAccountType::toString(_peer.getAccountType())
		+ "\">\n");

	result += ("<id>" + _peer.getPeerId() + "</id>\n");

	if (!_peer.getAlias().empty()) {
		result += ("<alias><![CDATA[" + _peer.getAlias() + "]]></alias>\n");
	}

	if (_peer.getIcon().getData().length() >  0) {
		std::string data = std::string(_peer.getIcon().getData().constData(), _peer.getIcon().getData().length());
		result += ("<photo><![CDATA[" + Base64::encode(data) + "]]></photo>");
	}

	return result;
}

bool PeerXMLSerializer::unserializeContent(TiXmlHandle & rootElt) {
	// Retrieving accountType
	TiXmlElement * lastChildElt = rootElt.Element();
	if (lastChildElt) {
		_peer.setAccountType(EnumAccountType::toAccountType(lastChildElt->Attribute("accountType")));
	} else {
		return false;
	}
	////

	//Retrieving contactId
	TiXmlText * peerId = rootElt.FirstChild("id").FirstChild().Text();
	if (peerId) {
		_peer.setPeerId(peerId->Value());
	} else {
		return false;
	}
	////

	//Retrieving alias
	TiXmlText * alias = rootElt.FirstChild("alias").FirstChild().Text();
	if (alias) {
		_peer.setAlias(alias->Value());
	}
	////

	//Retrieving icon
	TiXmlText * photo = rootElt.FirstChild("photo").FirstChild().Text();
	if (photo) {
		std::string data = Base64::decode(photo->Value());
		OWPicture picture = OWPicture::pictureFromData(QByteArray(data.c_str(), data.size()));
		_peer.setIcon(picture);
	}
	////

	return true;
}

bool PeerXMLSerializer::unserialize(const std::string & data) {
	return false;
}
