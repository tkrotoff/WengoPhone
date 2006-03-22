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

#include "StreetAddress.h"

#include <tinyxml.h>

using namespace std;

StreetAddressXMLSerializer::StreetAddressXMLSerializer(StreetAddress & streetAddress)
: _streetAddress(streetAddress) {
	
}

std::string StreetAddressXMLSerializer::serialize() {
	string result;
	
	result += "<address type=\"home\">\n";
	
	if (!_streetAddress._street.empty()) {
		result += ("<street>" + _streetAddress._street + "</street>\n");
	}
	
	if (!_streetAddress._city.empty()) {
		result += ("<locality>" + _streetAddress._city + "</locality>\n");
	}
	
	if (!_streetAddress._stateProvince.empty()) {
		result += ("<region>" + _streetAddress._stateProvince + "</region>\n");
	}
	
	if (!_streetAddress._zipCode.empty()) {
		result += ("<postcode>" + _streetAddress._zipCode + "</postcode>\n");
	}
	
	if (!_streetAddress._country.empty()) {
		result += ("<country>" + _streetAddress._country + "</country>\n");
	}
	
	result += "</address>\n";

	return result;
}

bool StreetAddressXMLSerializer::unserialize(const std::string & data) {
	TiXmlDocument doc;
	doc.Parse(data.c_str());
	/*
	TiXmlHandle docHandle(& doc);
	TiXmlNode * stringlist = docHandle.FirstChild("stringlist").Node();
	
	
	TiXmlNode * lastChild = NULL;
	while ((lastChild = stringlist->IterateChildren("elt", lastChild))) {
		_stringList += lastChild->FirstChild()->Value();
	}
	*/
	return true;	
}
