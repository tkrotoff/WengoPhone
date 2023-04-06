/*
 * WengoPhone, a voice over Internet phone
 * Copyright (C) 2004-2005  Wengo
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

#include <StringList.h>

#include <cctype>
#include <algorithm>
#include <sstream>
using namespace std;

const String String::null;

int String::toInteger() const {
	int tmp;
	stringstream ss(c_str());
	ss >> tmp;
	return tmp;
}

String String::toUpperCase() const {
	String tmp(c_str());
	transform(tmp.begin(), tmp.end(), tmp.begin(), (int (*)(int)) toupper);
	return tmp;
}

String String::toLowerCase() const {
	String tmp(c_str());
	transform(tmp.begin(), tmp.end(), tmp.begin(), (int (*)(int)) tolower);
	return tmp;
}

void String::replace(const std::string & before, const std::string & after, bool caseSensitive) {
	//Copy this + before to tmp + before2
	string tmp(c_str());
	String before2(before);

	if (!caseSensitive) {
		//Converts tmp + before2 in lower case
		tmp = toLowerCase();
		before2 = before2.toLowerCase();
	}

	//Searches on tmp + before2 rather than this + before
	string::size_type pos = 0;
	while ((pos = tmp.find(before2, pos)) != string::npos) {
		//Replaces on this + tmp
		string::replace(pos, before2.length(), after);
		tmp.replace(pos, before2.length(), after);
		pos = pos + after.length();
	}
}

void String::remove(const std::string & str) {
	replace(str, String::null);
}

std::string String::fromNumber(int number) {
	string tmp;
	stringstream ss;
	ss << number;
	ss >> tmp;
	return tmp;
}

StringList StringList::split(const std::string & str, const std::string & separator) {
	//Skip separator at beginning.
	size_type lastPos = str.find_first_not_of(separator, 0);

	//Find first "non-separator".
	size_type pos = str.find_first_of(separator, lastPos);

	StringList tokens;
	while (string::npos != pos || string::npos != lastPos) {

		//Found a token, add it to the vector.
		tokens.push_back(str.substr(lastPos, pos - lastPos));

		//Skip delimiters. Note the "not_of"
		lastPos = str.find_first_not_of(separator, pos);

		//Find next "non-delimiter"
		pos = str.find_first_of(separator, lastPos);
	}
	return tokens;
}

String StringList::operator[](unsigned int i) const {
	StringList::const_iterator it;
	unsigned int j = 0;
	for (it = begin(); it != end(); ++it) {
		if (j == i) {
			return *it;
		}
		j++;
	}

	return String::null;
}

void StringList::operator+=(const std::string & str) {
	push_back(str);
}

unsigned int StringList::contains(const std::string & str, bool caseSensitive) const {
	unsigned int result = 0;
	for (StringList::const_iterator it = begin(); it != end(); ++it) {
		String tmp1 = str;
		String tmp2 = *it;
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
