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

#include "ContactXMLSerializer.h"

#include "Contact.h"
#include "StreetAddressXMLSerializer.h"

#include <imwrapper/IMContactXMLSerializer.h>
#include <imwrapper/IMContactSet.h>

#include <serializer/DateXMLSerializer.h>

#include <tinyxml.h>

using namespace std;

ContactXMLSerializer::ContactXMLSerializer(Contact & contact, IMAccountHandler & imAccountHandler) 
: _contact(contact), _imAccountHandler(imAccountHandler) {
}

string ContactXMLSerializer::serialize() {
	string result;

	result += "<wgcard version=\"1.0\" xmlns=\"http://www.openwengo.org/wgcard/1.0\">\n";

	// Serializing Wengo ID
	if (!_contact._wengoPhoneId.empty()) {
		result += ("<wengoid>" + _contact._wengoPhoneId + "</wengoid>\n");
	}
	////

	// Serializing names
	result += "<name>\n";
	if (!_contact._firstName.empty()) {
		result += ("<first>" + _contact._firstName + "</first>\n");
	}
	if (!_contact._lastName.empty()) {
		result += ("<last>" + _contact._lastName + "</last>\n");
	}
	result += "</name>\n";
	////

	//Serializing sex
	if (_contact._sex != Contact::SexUnknown) {
		result += ("<sex>" + ((_contact._sex == Contact::SexFemale) ? string("female") : string("male")) + "</sex>\n");
	}
	////

	// Serializing alias
	////

	// Serializing urls
	if (!_contact._website.empty()) {
		result += ("<url type=\"website\">" + _contact._website + "</url>\n");
	}
	////

	// Serializing birthday
	result += "<birthday>\n";

	DateXMLSerializer serializer(_contact._birthdate);
	result += serializer.serialize();

	result += "</birthday>\n";
	////

	// Serializing street address
	StreetAddressXMLSerializer streetSerializer(_contact._streetAddress);
	result += streetSerializer.serialize();
	////

	// Serializing phone numbers
	if (!_contact._mobilePhone.empty()) {
		result += ("<tel type=\"cell\">" + _contact._mobilePhone + "</tel>\n");
	}

	if (!_contact._workPhone.empty()) {
		result += ("<tel type=\"work\">" + _contact._workPhone + "</tel>\n");
	}

	if (!_contact._homePhone.empty()) {
		result += ("<tel type=\"home\">" + _contact._homePhone + "</tel>\n");
	}
/*
	if (!_contact._mobilePhone.empty()) {
		result += ("<tel type=\"wengo\">" + _contact._wengoPhoneNumber + "</tel>");
	}
	*/
	////

	// Serializing IMContacts
	for (IMContactSet::const_iterator it = _contact._imContactSet.begin();
		it != _contact._imContactSet.end();
		++it) {
		IMContactXMLSerializer imContactSerializer((IMContact &)*it, _imAccountHandler);
		result += imContactSerializer.serialize();
	}
	////

	// Serializing groups
	result += "<groups>\n";
	for (Contact::ContactGroupSet::const_iterator it = _contact._contactGroupSet.begin();
		it != _contact._contactGroupSet.end();
		++it) {
		result += ("<group>" + (*it) + "</group>\n");
	}
	result += "</groups>\n";
	////

	result += "</wgcard>";

	return result;
}

bool ContactXMLSerializer::unserialize(const string & data) {
	TiXmlDocument doc;
	EnumIMProtocol imProtocol;

	doc.Parse(data.c_str());

	TiXmlHandle docHandle(&doc);
	TiXmlHandle wgCard = docHandle.FirstChild("wgcard");

	// Retrieving Wengo ID
	TiXmlNode * wengoid = wgCard.FirstChild("wengoid").Node();
	if (wengoid) {
		_contact.setWengoPhoneId(wengoid->FirstChild()->Value());
	}
	////

	// Retrieving IMContacts
	TiXmlNode * imLastChild = NULL;
	while ((imLastChild = wgCard.Node()->IterateChildren("im", imLastChild))) {
		string imData;
		//FIXME: constructing an IMContact tlike this is not very good
		IMAccount fakeAccount;
		IMContact imContact(fakeAccount, "");
		IMContactXMLSerializer imContactSerializer(imContact, _imAccountHandler);
		
		imData << *imLastChild;
		imContactSerializer.unserialize(imData);

		_contact.addIMContact(imContact);
	}	
	////

	//Retrieving Groups
	TiXmlNode * groups = wgCard.FirstChild("groups").Node();

	if (groups) {
		TiXmlNode * groupLastChild = NULL;
		while ((groupLastChild = groups->IterateChildren("group", groupLastChild))) {
			_contact.addToContactGroup(string(groupLastChild->FirstChild()->Value()));
		}
	}
	////

	return true;
}
