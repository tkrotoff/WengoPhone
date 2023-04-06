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

#include <coipmanager_base/serializer/ProfileXMLSerializer.h>

#include <coipmanager_base/profile/Profile.h>
#include <coipmanager_base/profile/StreetAddress.h>
#include <coipmanager_base/serializer/StreetAddressXMLSerializer.h>

#include <serializer/DateXMLSerializer.h>

#include <util/Base64.h>

#include <tinyxml.h>

ProfileXMLSerializer::ProfileXMLSerializer(Profile & profile)
	: _profile(profile) {
}

std::string ProfileXMLSerializer::serialize() {
	std::string result;

	// Serializing Wengo ID
	if (!_profile.getWengoPhoneId().empty()) {
		result += ("<wengoid>" + _profile.getWengoPhoneId() + "</wengoid>\n");
	}
	////

	// Serializing names
	result += "<name>\n";
	if (!_profile.getFirstName().empty()) {
		result += ("<first><![CDATA[" + _profile.getFirstName() + "]]></first>\n");
	}
	if (!_profile.getLastName().empty()) {
		result += ("<last><![CDATA[" + _profile.getLastName() + "]]></last>\n");
	}
	result += "</name>\n";
	////

	//Serializing sex
	if (_profile.getSex() != EnumSex::SexUnknown) {
		result += ("<sex>" + EnumSex::toString(_profile.getSex()) + "</sex>\n");
	}
	////

	// Serializing alias
	if (!_profile.getAlias().empty()) {
		result += ("<alias><![CDATA[" + _profile.getAlias() + "]]></alias>");
	}
	////

	// Serializing urls
	if (!_profile.getWebsite().empty()) {
		result += ("<url type=\"website\">" + _profile.getWebsite() + "</url>\n");
	}
	////

	// Serializing birthday
	result += "<birthday>\n";

	Date date = _profile.getBirthdate();
	DateXMLSerializer serializer(date);
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
	if (!_profile.getMobilePhone().empty()) {
		result += ("<tel type=\"cell\">" + _profile.getMobilePhone() + "</tel>\n");
	}

	if (!_profile.getWorkPhone().empty()) {
		result += ("<tel type=\"work\">" + _profile.getWorkPhone() + "</tel>\n");
	}

	if (!_profile.getHomePhone().empty()) {
		result += ("<tel type=\"home\">" + _profile.getHomePhone() + "</tel>\n");
	}

	if (!_profile.getWengoPhoneNumber().empty()) {
		result += ("<tel type=\"wengo\">" + _profile.getWengoPhoneNumber() + "</tel>");
	}
	////

	// Serializing street address
	StreetAddress streetAddress = _profile.getStreetAddress();
	StreetAddressXMLSerializer streetSerializer(streetAddress);
	result += streetSerializer.serialize();
	////

	// Serializing emails
	if (!_profile.getPersonalEmail().empty()) {
		result += ("<email type=\"home\">" + _profile.getPersonalEmail() + "</email>\n");
	}

	if (!_profile.getWorkEmail().empty()) {
		result += ("<email type=\"work\">" + _profile.getWorkEmail() + "</email>\n");
	}

	if (!_profile.getOtherEmail().empty()) {
		result += ("<email type=\"other\">" + _profile.getOtherEmail() + "</email>\n");
	}
	////

	// Serializing SMS signature
	if (!_profile.getSmsSignature().empty()) {
		result += ("<smssignature type=\"other\">" + _profile.getSmsSignature() + "</smssignature>\n");
	}
	////

	// Serializing photo
	if (_profile.getIcon().getData().length() > 0) {
		std::string data = std::string(_profile.getIcon().getData().constData(), _profile.getIcon().getData().length());
		result += ("<photo filename=\"" + _profile.getIcon().getFilename()
			+ "\"><![CDATA[" + Base64::encode(data) + "]]></photo>\n");
	}
	////

	// Serializing notes
	if (!_profile.getNotes().empty()) {
		result += ("<notes><![CDATA[" + _profile.getNotes() + "]]></notes>\n");
	}
	////

	return result;
}

bool ProfileXMLSerializer::unserialize(const std::string & data) {
	return false;
}

bool ProfileXMLSerializer::unserializeContent(TiXmlHandle & rootElt) {
	// Retrieving Wengo ID
	TiXmlNode * wengoPhoneId = rootElt.FirstChild("wengoid").FirstChild().Node();
	if (wengoPhoneId) {
		// Here we need to call setWengoPhoneId because each class inherited
		// from Profile handles the wengophone ID differently.
		_profile.setWengoPhoneId(wengoPhoneId->Value());
	}
	////

	// Retrieving names
	TiXmlNode * firstName = rootElt.FirstChild("name").FirstChild("first").FirstChild().Node();
	if (firstName) {
		_profile.setFirstName(firstName->Value());
	}
	TiXmlNode * lastName = rootElt.FirstChild("name").FirstChild("last").FirstChild().Node();
	if (lastName) {
		_profile.setLastName(lastName->Value());
	}
	////

	// Retrieving alias
	TiXmlNode * alias = rootElt.FirstChild("alias").FirstChild().Node();
	if (alias) {
		_profile.setAlias(alias->Value());
	}
	////

	// Retrieving SMS Signature
	TiXmlNode * smsSignature = rootElt.FirstChild("smssignature").FirstChild().Node();
	if (smsSignature) {
		_profile.setSmsSignature(smsSignature->Value());
	}
	////

	// Retrieving sex
	TiXmlNode * sex = rootElt.FirstChild("sex").FirstChild().Node();
	if (sex) {
		_profile.setSex(EnumSex::toSex(sex->Value()));
	}
	////

	// Retrieving URLs
	if (rootElt.Node()) {
		TiXmlNode * url = NULL;
		while ((url = rootElt.Node()->IterateChildren("url", url))) {
			TiXmlElement * urlElt = url->ToElement();
			std::string typeAttr = std::string(urlElt->Attribute("type"));
			if (typeAttr == "website") {
				TiXmlNode * website = url->FirstChild();
				if (website) {
					_profile.setWebsite(website->Value());
				}
			}
		}
	}
	////

	// Retrieving birthday
	TiXmlNode * birthday = rootElt.FirstChild("birthday").FirstChild("date").Node();
	if (birthday) {
		TiXmlElement * birthdayElt = birthday->ToElement();
		std::string birthdayData;
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
		TiXmlElement * addressElt = address->ToElement();
		std::string addressData;
		addressData << *addressElt;
		StreetAddress streetAddress;
		StreetAddressXMLSerializer addressSerializer(streetAddress);
		addressSerializer.unserialize(addressData);
		_profile.setStreetAddress(streetAddress);
	}
	////

	// Retrieving phone numbers
	TiXmlNode * tel = NULL;
	while ((rootElt.Node()) && (tel = rootElt.Node()->IterateChildren("tel", tel))) {
		TiXmlElement * telElt = tel->ToElement();
		std::string typeAttr = std::string(telElt->Attribute("type"));
		if (typeAttr == "home") {
			TiXmlNode * homePhone = tel->FirstChild();
			if (homePhone) {
				_profile.setHomePhone(homePhone->Value());
			}
		} else if (typeAttr == "work") {
			TiXmlNode * workPhone = tel->FirstChild();
			if (workPhone) {
				_profile.setWorkPhone(workPhone->Value());
			}
		} else if (typeAttr == "cell") {
			TiXmlNode * mobilePhone = tel->FirstChild();
			if (mobilePhone) {
				_profile.setMobilePhone(mobilePhone->Value());
			}
		} else if (typeAttr == "wengo") {
			TiXmlNode * wengoPhoneNumber = tel->FirstChild();
			if (wengoPhoneNumber) {
				_profile.setWengoPhoneNumber(wengoPhoneNumber->Value());
			}
		}
	}
	////

	// Retrieving emails
	TiXmlNode * email = NULL;
	while ((rootElt.Node()) && (email = rootElt.Node()->IterateChildren("email", email))) {
		TiXmlElement * emailElt = email->ToElement();
		std::string typeAttr = std::string(emailElt->Attribute("type"));
		if (typeAttr == "home") {
			TiXmlNode * personalEmail = email->FirstChild();
			if (personalEmail) {
				_profile.setPersonalEmail(personalEmail->Value());
			}
		} else if (typeAttr == "work") {
			TiXmlNode * workEmail = email->FirstChild();
			if (workEmail) {
				_profile.setWorkEmail(workEmail->Value());
			}
		} else if (typeAttr == "other") {
			TiXmlNode * otherEmail = email->FirstChild();
			if (otherEmail) {
				_profile.setOtherEmail(otherEmail->Value());
			}
		}
	}
	////

	// Retrieving icon
	TiXmlNode * photo = rootElt.FirstChild("photo").Node();
	if (photo) {
		TiXmlElement * photoElt = photo->ToElement();
		std::string filename;
		const char * filenameAttr = photoElt->Attribute("filename");
		if (filenameAttr) {
			filename = std::string(filenameAttr);
		}

		TiXmlNode * photoNode = photo->FirstChild();
		if (photoNode) {
			std::string data = Base64::decode(photoNode->Value());
			OWPicture picture = OWPicture::pictureFromData(QByteArray(data.c_str(), data.size()));
			picture.setFilename(filename);
			_profile.setIcon(picture);
		}
	}
	////

	// Retrieving notes
	TiXmlNode * notes = rootElt.FirstChild("notes").FirstChild().Node();
	if (notes) {
		_profile.setNotes(notes->Value());
	}
	////

	return true;
}
