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

#include <util/StringList.h>

#include <util/String.h>
#include <util/Logger.h>

using namespace std;

StringList::StringList(const std::list<std::string> & strList) {
	for (std::list<string>::const_iterator it = strList.begin();
		it != strList.end(); ++it) {

		this->add(*it);
	}
}

StringList::StringList() {
}

StringList::operator std::list<std::string>() {
	std::list<std::string> strList;
	for (unsigned i = 0; i < size(); i++) {
		strList.push_back((*this)[i]);
	}
	return strList;
}

StringList StringList::split(const std::string & str, const std::string & separator) {
	//Skip separator at beginning.
	string::size_type lastPos = str.find_first_not_of(separator, 0);

	//Find first "non-separator".
	string::size_type pos = str.find_first_of(separator, lastPos);

	StringList tokens;
	while (string::npos != pos || string::npos != lastPos) {

		//Found a token, add it to the vector.
		tokens.add(str.substr(lastPos, pos - lastPos));

		//Skip delimiters. Note the "not_of"
		lastPos = str.find_first_not_of(separator, pos);

		//Find next "non-delimiter"
		pos = str.find_first_of(separator, lastPos);
	}
	return tokens;
}

std::string StringList::operator[](unsigned i) const {
	std::string tmp;
	try {
		tmp = get(i);
	} catch (OutOfRangeException & e) {
		LOG_DEBUG(e.what());
		return String::null;
	}
	return tmp;
}

unsigned StringList::contains(const std::string & str, bool caseSensitive) const {
	unsigned result = 0;
	for (unsigned i = 0; i < size(); i++) {
		String tmp1 = str;
		String tmp2 = get(i);
		if (!caseSensitive) {
			tmp1 = tmp1.toLowerCase();
			tmp2 = tmp2.toLowerCase();
		}

		if (tmp1 == tmp2) {
			++result;
		}
	}

	return result;
}

void StringList::sort(SortingOrder order) {
	switch(order) {
	case Ascendant:
		std::sort(_list.begin(), _list.end());
		break;

	case Descendant:
		std::sort(_list.begin(), _list.end(), StringCompareDescendant());
		break;

	default:
		LOG_FATAL("unknown sorting order");
	}
}
