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

#include <coipmanager_base/serializer/IMContactXMLSerializer.h>

#include <coipmanager_base/serializer/PeerXMLSerializer.h>
#include <coipmanager_base/serializer/SipPeerXMLSerializer.h>

#include <coipmanager_base/imcontact/GenericIMContact.h>
#include <coipmanager_base/imcontact/IMContact.h>
#include <coipmanager_base/imcontact/SipIMContact.h>
#include <coipmanager_base/imcontact/WengoIMContact.h>

#include <serializer/StringListXMLSerializer.h>
#include <util/Base64.h>
#include <util/Logger.h>
#include <util/SafeDelete.h>

#include <tinyxml.h>

IMContactXMLSerializer::IMContactXMLSerializer(IMContact & imContact)
	: _imContact(imContact) {
}

std::string IMContactXMLSerializer::serialize() {
	PeerXMLSerializer peerXMLSerializer(_imContact, "im");
	std::string result = peerXMLSerializer.serialize();

	result += ("<account>" + _imContact.getAccountId() + "</account>\n");

	StringList stringList = _imContact.getGroupList();
	StringListXMLSerializer stringListSerializer(stringList);
	result += ("<groups>" + stringListSerializer.serialize() + "</groups>\n");

	switch (_imContact.getAccountType()) {
	case EnumAccountType::AccountTypeAIM:
	case EnumAccountType::AccountTypeGTalk:
	case EnumAccountType::AccountTypeICQ:	
	case EnumAccountType::AccountTypeJabber:
	case EnumAccountType::AccountTypeMSN:
	case EnumAccountType::AccountTypeIAX:
	case EnumAccountType::AccountTypeYahoo:
	{
		break;
	}
	case EnumAccountType::AccountTypeSIP:
	case EnumAccountType::AccountTypeWengo:
	{
		SipPeerXMLSerializer sipPeerXMLSerializer(* (static_cast<SipIMContact *>(_imContact.getPrivateIMContact())));
		result += sipPeerXMLSerializer.serialize();
		break;
	}
	default:
		LOG_ERROR("unknown accountType");
		return false;
	}

	result += "</im>\n";

	return result;
}

bool IMContactXMLSerializer::unserialize(const std::string & data) {
	TiXmlDocument doc;

	doc.Parse(data.c_str());

	TiXmlHandle docHandle(&doc);
	TiXmlHandle im = docHandle.FirstChild("im");

	IIMContact * iIMContact = NULL;

	//Retrieving contactId
	TiXmlText * tinyContactId = im.FirstChild("id").FirstChild().Text();
	std::string contactId;
	if (tinyContactId) {
		contactId = tinyContactId->Value();
	}
	////

	// Retrieving accountType
	EnumAccountType::AccountType accountType;
	TiXmlElement * lastChildElt = im.Element();
	if (lastChildElt) {
		accountType = EnumAccountType::toAccountType(lastChildElt->Attribute("accountType"));
	} else {
		return false;
	}

	switch (accountType) {
	case EnumAccountType::AccountTypeAIM:
	case EnumAccountType::AccountTypeGTalk:
	case EnumAccountType::AccountTypeICQ:	
	case EnumAccountType::AccountTypeJabber:
	case EnumAccountType::AccountTypeMSN:
	case EnumAccountType::AccountTypeIAX:
	case EnumAccountType::AccountTypeYahoo:
	{
		iIMContact = new GenericIMContact(accountType, contactId);
		break;
	}
	case EnumAccountType::AccountTypeSIP:
	{
		iIMContact = new SipIMContact(contactId);
		SipPeerXMLSerializer sipPeerXMLSerializer(* (static_cast<SipIMContact *>(iIMContact)));
		sipPeerXMLSerializer.unserializeContent(im);
		break;
	}
	case EnumAccountType::AccountTypeWengo:
	{
		iIMContact = new WengoIMContact(contactId);
		SipPeerXMLSerializer sipPeerXMLSerializer(* (static_cast<SipIMContact *>(iIMContact)));
		sipPeerXMLSerializer.unserializeContent(im);
		break;
	}
	default:
		LOG_ERROR("unknown accountType");
		return false;
	}
	////

	//Retrieving associated account
	TiXmlText * accountId = im.FirstChild("account").FirstChild().Text();
	if (accountId) {
		iIMContact->setAccountId(accountId->Value());
	}
	/////

	// Retrieving groups
	TiXmlNode * groups = im.FirstChild("groups").Node();

	if (groups) {
		std::string nodeData;
		nodeData << *(groups->FirstChild());

		StringList stringList;
		StringListXMLSerializer stringListSerializer(stringList);
		stringListSerializer.unserialize(nodeData);

		for (StringList::const_iterator it = stringList.begin();
			it != stringList.end();
			++it) {
			_imContact.addToGroup(*it);
		}
	}
	////

	//Retrieving alias
	TiXmlText * alias = im.FirstChild("alias").FirstChild().Text();
	if (alias) {
		iIMContact->setAlias(alias->Value());
	}
	////

	//Retrieving icon
	TiXmlText * photo = im.FirstChild("photo").FirstChild().Text();
	if (photo) {
		OWPicture picture = OWPicture::pictureFromData(Base64::decode(photo->Value()));
		iIMContact->setIcon(picture);
	}
	////

	_imContact = IMContact(iIMContact);
	OWSAFE_DELETE(iIMContact);

	return true;
}
