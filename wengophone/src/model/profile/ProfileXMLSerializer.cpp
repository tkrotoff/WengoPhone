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

#include "ProfileXMLSerializer.h"

#include "Profile.h"

#include <model/profile/StreetAddressXMLSerializer.h>
#include <model/profile/StreetAddress.h>

#include <serializer/DateXMLSerializer.h>

#include <util/Base64.h>

#include <tinyxml.h>

using namespace std;

ProfileXMLSerializer::ProfileXMLSerializer(Profile & profile) 
: _profile(profile) {
}

string ProfileXMLSerializer::serialize() {
	string result;

	// Serializing Wengo ID
	if (!_profile._wengoPhoneId.empty()) {
		result += ("<wengoid>" + _profile._wengoPhoneId + "</wengoid>\n");
	}
	////

	// Serializing names
	result += "<name>\n";
	if (!_profile._firstName.empty()) {
		result += ("<first>" + _profile._firstName + "</first>\n");
	}
	if (!_profile._lastName.empty()) {
		result += ("<last>" + _profile._lastName + "</last>\n");
	}
	result += "</name>\n";
	////

	//Serializing sex
	if (_profile._sex != EnumSex::SexUnknown) {
		result += ("<sex>" + EnumSex::toString(_profile._sex) + "</sex>\n");
	}
	////

	// Serializing alias
	if (!_profile._alias.empty()) {
		result += ("<alias>" + _profile._alias + "</alias>");
	}
	////

	// Serializing urls
	if (!_profile._website.empty()) {
		result += ("<url type=\"website\">" + _profile._website + "</url>\n");
	}
	////

	// Serializing birthday
	result += "<birthday>\n";

	DateXMLSerializer serializer(_profile._birthdate);
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
	if (!_profile._mobilePhone.empty()) {
		result += ("<tel type=\"cell\">" + _profile._mobilePhone + "</tel>\n");
	}

	if (!_profile._workPhone.empty()) {
		result += ("<tel type=\"work\">" + _profile._workPhone + "</tel>\n");
	}

	if (!_profile._homePhone.empty()) {
		result += ("<tel type=\"home\">" + _profile._homePhone + "</tel>\n");
	}

	if (!_profile._wengoPhoneNumber.empty()) {
		result += ("<tel type=\"wengo\">" + _profile._wengoPhoneNumber + "</tel>");
	}
	////

	// Serializing street address
	StreetAddressXMLSerializer streetSerializer(_profile._streetAddress);
	result += streetSerializer.serialize();
	////

	// Serializing emails
	if (!_profile._personalEmail.empty()) {
		result += ("<email type=\"home\">" + _profile._personalEmail + "</email>\n");
	}

	if (!_profile._workEmail.empty()) {
		result += ("<email type=\"work\">" + _profile._workEmail + "</email>\n");	
	}

	if (!_profile._otherEmail.empty()) {
		result += ("<email type=\"other\">" + _profile._otherEmail + "</email>\n");	
	}
	////

	// Serializing photo
	if (!_profile._icon.getData().empty()) {
		result += ("<photo><![CDATA[" + Base64::encode(_profile._icon.getData()) + "]]></photo>");
	}
	////

	return result;
}

bool ProfileXMLSerializer::unserialize(const std::string & data) {
	return false;
}

bool ProfileXMLSerializer::unserializeContent(TiXmlHandle & rootElt) {
	// Retrieving Wengo ID
	TiXmlNode * wengoid = rootElt.FirstChild("wengoid").Node();
	if (wengoid) {
		_profile.setWengoPhoneId(wengoid->FirstChild()->Value());
	}
	////

	// Retrieving names
	TiXmlNode * name = rootElt.FirstChild("name").Node();
	if (name) {
		TiXmlNode * firstName = name->FirstChild("first");
		if (firstName) {
			_profile.setFirstName(firstName->FirstChild()->Value());
		}
		TiXmlNode * lastName = name->FirstChild("last");
		if (lastName) {
			_profile.setLastName(lastName->FirstChild()->Value());
		}
	}
	////

	// Retrieving alias
	TiXmlNode * alias = rootElt.FirstChild("alias").Node();
	if (alias) {
		_profile._alias = alias->FirstChild()->Value();
	}
	////

	// Retrieving sex
	TiXmlNode * sex = rootElt.FirstChild("sex").Node();
	if (sex) {
		_profile.setSex(EnumSex::toSex(sex->FirstChild()->Value()));
	}
	////

	// Retrieving URLs
	TiXmlNode * url = NULL;
	while ((url = rootElt.Node()->IterateChildren("url", url))) {
		TiXmlElement * urlElt = url->ToElement();
		string typeAttr = string(urlElt->Attribute("type"));
		if (typeAttr == "website") {
			_profile.setWebsite(url->FirstChild()->Value());
		}
	}
	////

	// Retrieving birthday
	TiXmlNode * birthday = rootElt.FirstChild("birthday").Node();
	if (birthday) {
		TiXmlElement * birthdayElt = birthday->FirstChild("date")->ToElement();
		string birthdayData;
		birthdayData << *birthdayElt;
		Date date;
		DateXMLSerializer dateSerializer(date);
		dateSerializer.unserialize(birthdayData);
		_profile.setBirthdate(date);
	}	
	/////

	// Retrieving organization
	////

	// Retrieving address
	TiXmlNode * address = rootElt.FirstChild("address").Node();
	if (address) {
		TiXmlElement* addressElt = address->ToElement();
		string addressData;
		addressData << *addressElt;
		StreetAddress streetAddress;
		StreetAddressXMLSerializer addressSerializer(streetAddress);
		addressSerializer.unserialize(addressData);
		_profile.setStreetAddress(streetAddress);
	}
	////

	// Retrieving phone numbers
	TiXmlNode * tel = NULL;
	while ((tel = rootElt.Node()->IterateChildren("tel", url))) {
		TiXmlElement * telElt = tel->ToElement();
		string typeAttr = string(telElt->Attribute("type"));
		if (typeAttr == "home") {
			_profile.setHomePhone(tel->FirstChild()->Value());
		} else if (typeAttr == "work") {
			_profile.setWorkPhone(tel->FirstChild()->Value());
		} else if (typeAttr == "cell") {
			_profile.setMobilePhone(tel->FirstChild()->Value());
		} else if (typeAttr == "wengo") {
			_profile.setWengoPhoneNumber(tel->FirstChild()->Value());
		}
	}
	////

	// Retrieving emails
	TiXmlNode * email = NULL;
	while ((email = rootElt.Node()->IterateChildren("email", url))) {
		TiXmlElement * emailElt = email->ToElement();
		string typeAttr = string(emailElt->Attribute("type"));
		if (typeAttr == "home") {
			_profile.setPersonalEmail(email->FirstChild()->Value());
		} else if (typeAttr == "work") {
			_profile.setWorkEmail(email->FirstChild()->Value());
		} else if (typeAttr == "other") {
			_profile.setOtherEmail(email->FirstChild()->Value());
		}
	}
	////

	// Retrieving icon
	TiXmlNode * photo = rootElt.FirstChild("photo").Node();
	if (photo) {
		Picture picture(Base64::decode(photo->FirstChild()->Value()));
		_profile.setIcon(picture);
	}
	////

	return true;
}
