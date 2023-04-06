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

#include <coipmanager_base/serializer/UserProfileXMLSerializer.h>

#include <coipmanager_base/userprofile/UserProfile.h>

#include <tinyxml.h>

UserProfileXMLSerializer::UserProfileXMLSerializer(UserProfile & userProfile) 
	: ProfileXMLSerializer(userProfile), _userProfile(userProfile) {
}

std::string UserProfileXMLSerializer::serialize() {
	std::string result;

	result += "<userprofile>\n";

	result += "<userprofileName>" + _userProfile.getName() + "</userprofileName>\n";

	result += ProfileXMLSerializer::serialize();

	result += "</userprofile>\n";

	return result;
}

bool UserProfileXMLSerializer::unserialize(const std::string & data) {
	TiXmlDocument doc;

	doc.Parse(data.c_str());

	TiXmlHandle docHandle(&doc);
	TiXmlHandle userprofile = docHandle.FirstChild("userprofile");

	// Gets the UserProfile name
	TiXmlNode * userprofileName = userprofile.FirstChild("userprofileName").FirstChild().Node();
	if (userprofileName) {
		_userProfile.setName(userprofileName->Value());
	}

	return ProfileXMLSerializer::unserializeContent(userprofile);;
}
