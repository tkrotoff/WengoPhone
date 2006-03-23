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

#include <model/profile/StreetAddressXMLSerializer.h>
#include <model/profile/StreetAddress.h>

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
	if (_contact._sex != EnumSex::SexUnknown) {
		result += ("<sex>" + EnumSex::toString(_contact._sex) + "</sex>\n");
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

	// Serializing organization
	/*
	result += "<organization>\n";

	result += "</organization>\n";
	*/
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

	if (!_contact._wengoPhoneNumber.empty()) {
		result += ("<tel type=\"wengo\">" + _contact._wengoPhoneNumber + "</tel>");
	}
	////

	// Serializing street address
	StreetAddressXMLSerializer streetSerializer(_contact._streetAddress);
	result += streetSerializer.serialize();
	////

	// Serializing emails
	if (!_contact._personalEmail.empty()) {
		result += ("<email type=\"home\">" + _contact._personalEmail + "</email>\n");
	}

	if (!_contact._workEmail.empty()) {
		result += ("<email type=\"work\">" + _contact._workEmail + "</email>\n");	
	}

	if (!_contact._otherEmail.empty()) {
		result += ("<email type=\"other\">" + _contact._otherEmail + "</email>\n");	
	}
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

	// Retrieving names
	TiXmlNode * name = wgCard.FirstChild("name").Node();
	if (name) {
		TiXmlNode * firstName = name->FirstChild("first");
		if (firstName) {
			_contact.setFirstName(firstName->FirstChild()->Value());
		}
		TiXmlNode * lastName = name->FirstChild("last");
		if (lastName) {
			_contact.setLastName(lastName->FirstChild()->Value());
		}
	}
	////

	// Retrieving sex
	TiXmlNode * sex = wgCard.FirstChild("sex").Node();
	if (sex) {
		_contact.setSex(EnumSex::toSex(sex->FirstChild()->Value()));
	}
	////

	// Retrieving URLs
	TiXmlNode * url = NULL;
	while ((url = wgCard.Node()->IterateChildren("url", url))) {
		TiXmlElement * urlElt = url->ToElement();
		string typeAttr = string(urlElt->Attribute("type"));
		if (typeAttr == "website") {
			_contact.setWebsite(url->FirstChild()->Value());
		}
	}
	////

	// Retrieving birthday
	TiXmlNode * birthday = wgCard.FirstChild("birthday").Node();
	if (birthday) {
		TiXmlElement * birthdayElt = birthday->FirstChild("date")->ToElement();
		string birthdayData;
		birthdayData << *birthdayElt;
		Date date;
		DateXMLSerializer dateSerializer(date);
		dateSerializer.unserialize(birthdayData);
		_contact.setBirthdate(date);
	}	
	/////

	// Retrieving organization
	////

	// Retrieving address
	TiXmlNode * address = wgCard.FirstChild("address").Node();
	if (address) {
		TiXmlElement* addressElt = address->ToElement();
		string addressData;
		addressData << *addressElt;
		StreetAddress streetAddress;
		StreetAddressXMLSerializer addressSerializer(streetAddress);
		addressSerializer.unserialize(addressData);
		_contact.setStreetAddress(streetAddress);
	}
	////

	// Retrieving phone numbers
	TiXmlNode * tel = NULL;
	while ((tel = wgCard.Node()->IterateChildren("tel", url))) {
		TiXmlElement * telElt = tel->ToElement();
		string typeAttr = string(telElt->Attribute("type"));
		if (typeAttr == "home") {
			_contact.setHomePhone(tel->FirstChild()->Value());
		} else if (typeAttr == "work") {
			_contact.setWorkPhone(tel->FirstChild()->Value());
		} else if (typeAttr == "cell") {
			_contact.setMobilePhone(tel->FirstChild()->Value());
		} else if (typeAttr == "wengo") {
			_contact.setWengoPhoneNumber(tel->FirstChild()->Value());
		}
	}
	////

	// Retrieving emails
	TiXmlNode * email = NULL;
	while ((email = wgCard.Node()->IterateChildren("email", url))) {
		TiXmlElement * emailElt = email->ToElement();
		string typeAttr = string(emailElt->Attribute("type"));
		if (typeAttr == "home") {
			_contact.setPersonalEmail(email->FirstChild()->Value());
		} else if (typeAttr == "work") {
			_contact.setWorkEmail(email->FirstChild()->Value());
		} else if (typeAttr == "other") {
			_contact.setOtherEmail(email->FirstChild()->Value());
		}
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
