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

#include "StreetAddressXMLSerializer.h"

#include <StreetAddress.h>

StreetAddressXMLSerializer::StreetAddressXMLSerializer(StreetAddress & streetAddress)
: _streetAddress(streetAddress) {
	
}

std::string StreetAddressXMLSerializer::serialize() {
	string result;
	
	result += "<address type=\"home\">\n";
	
	if (!_street.empty()) {
		result += ("<street>" + _street + "</street>\n");
	}
	
	if (!_city.empty()) {
		result += ("<locality>" + _city + "</locality>\n");
	}
	
	if (!_stateProvince.empty()) {
		result += ("<region>" + _stateProvince + "</region>\n");
	}
	
	if (!_zipCode.empty()) {
		result += ("<postcode>" + _zipCode + "</postcode>\n");
	}
	
	if (!_country.empty()) {
		result += ("<country>" + _country + "</country>\n");
	}
	
	result += "</address>";

	return result;
}

bool StreetAddressXMLSerializer::unserialize(const std::string & data) {
	TiXmlDocument doc;
	doc.Parse(data.c_str());
	
	TiXmlHandle docHandle(& doc);
	TiXmlNode * stringlist = docHandle.FirstChild("stringlist").Node();
	
	TiXmlNode * lastChild = NULL;
	while ((lastChild = stringlist->IterateChildren("elt", lastChild))) {
		_stringList += lastChild->FirstChild()->Value();
	}
	
	return true;	
}
